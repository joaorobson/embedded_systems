#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <float.h>
#include "utils.h"

#define FLT_EXPO_SIZE 5
#define MAX 80 
#define PORT 10110
#define SA struct sockaddr 


pthread_mutex_t bme280_mutex;
pthread_cond_t bme280_cond;

volatile sig_atomic_t run = 1; 
volatile sig_atomic_t read_bme280  = 0; 

void stopWhile(int signum){
    run = 0;
}

void* get_ex_temperature(void* _args){
	struct distr_server *server = (struct distr_server*) _args;
	get_external_temperature((void*)server->BME280);

	char *message = (char*) malloc(200*sizeof(char));///"Hello from client"; 
	char str_temp[20];
	gcvt(server->BME280->temperature, 7,  str_temp);
	sprintf(message, "{Temp: %s", str_temp);

	char buffer[1024] = {0}; 
	send(server->socket_n, message, strlen(message), 0 ); 
    int valread = read(server->socket_n, buffer, 1024); 
    printf("%s\n",buffer );
	return NULL;
}

void* read_bme_thread(void* args){
	pthread_mutex_lock(&bme280_mutex);
	while(!read_bme280){
		pthread_cond_wait(&bme280_cond, &bme280_mutex);
		get_ex_temperature(args);
		read_bme280 = 0;
	}
	pthread_mutex_unlock(&bme280_mutex);
	pthread_exit( NULL );
}

void sig_handler(int signum) {
    if(signum == SIGALRM){
        pthread_mutex_lock(&bme280_mutex);
        if(read_bme280 == 0){ 
            read_bme280 = 1;
            pthread_cond_signal(&bme280_cond);
        }   
        pthread_mutex_unlock(&bme280_mutex);
        alarm(3);
    }
    if(signum == SIGINT){
			run = 0;
			exit(0);
    }
}

char* struct_to_JSON(struct bme280 *BME280){
    char* json_str = (char*) malloc(200*sizeof(char));
    sprintf(json_str, "{\"Temp:\" %f", 42.0);

    return json_str;
}

  
void config_server(struct distr_server *server) 
{ 
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	server->socket_n = sock;
} 


int main(int argc, char ** argv)
{
	struct bme280 *BME280 = malloc(sizeof(struct bme280));
	struct distr_server *server = malloc(sizeof(struct distr_server));
	config_server(server);
	server->BME280 = BME280;
	init_bme280_attr(BME280);	

	signal(SIGALRM, sig_handler);
	signal(SIGINT, stopWhile);
	alarm(1);

	pthread_mutex_init(&bme280_mutex, NULL);
	pthread_cond_init (&bme280_cond, NULL);
	pthread_t threads[2];
	pthread_create(&threads[0], NULL, read_bme_thread, (void*)server);
	//pthread_create(&threads[1], NULL, client, (void*)BME280);
    // close the socket 
		while(run){}
    close(server->socket_n); 
    return 0;
}

