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
//#include "gpio.h"

#define FLT_EXPO_SIZE 5
#define MAX 80 
#define PORT 10010
#define SA struct sockaddr 


pthread_mutex_t bme280_mutex, sensors_mutex, send_mutex;
pthread_cond_t bme280_cond, sensors_cond, send_cond;

volatile sig_atomic_t run = 1; 
volatile sig_atomic_t read_bme280 = 0,
					  read_sensors = 0,
					  run_sending = 0; 


void stopWhile(int signum){
    run = 0;
}

void* get_sensors_states_thread(void* _args){
	pthread_mutex_lock(&sensors_mutex);
	while(!read_sensors){
		pthread_cond_wait(&sensors_cond, &sensors_mutex);
		struct distr_server *server = (struct distr_server*) _args;
		get_sensors_states(server->GPIO);

		read_sensors = 0;
	}
	pthread_mutex_unlock(&sensors_mutex);
	pthread_exit( NULL );
}

void* sending_thread(void* _args){
	pthread_mutex_lock(&send_mutex);
	while(!run_sending){
		pthread_cond_wait(&send_cond, &send_mutex);
		struct distr_server *server = (struct distr_server*) _args;

		char *message = (char*) malloc(200*sizeof(char));
		char temp_buf[20];
		char hum_buf[20];
		gcvt(server->BME280->temperature, 7,  temp_buf);
		gcvt(server->BME280->humidity, 7,  hum_buf);
		sprintf(message, "{\"Temp\": %s, \"Hum\": %s, lamp1: %u}", temp_buf, hum_buf, server->GPIO->lamp1);

		char buffer[1024] = {0}; 
		send(server->socket_n, message, strlen(message), 0 ); 
		int valread = read(server->socket_n, buffer, 1024); 
		if(valread == 0){
			exit(0);
		}
		printf("%s\n",buffer );
		run_sending = 0;
	}
	pthread_mutex_unlock(&send_mutex);
	pthread_exit( NULL );
}

void* get_temp_and_hum_thread(void* _args){
	pthread_mutex_lock(&bme280_mutex);
	while(!read_bme280){
		pthread_cond_wait(&bme280_cond, &bme280_mutex);

		struct distr_server *server = (struct distr_server*) _args;
		get_temperature_and_humidity((void*)server->BME280);

		read_bme280 = 0;
	}
	pthread_mutex_unlock(&bme280_mutex);
	pthread_exit( NULL );
}

void sig_handler(int signum) {
    if(signum == SIGALRM){
        //pthread_mutex_lock(&bme280_mutex);
        pthread_mutex_lock(&send_mutex);
        if(read_bme280 == 0){ 
            read_bme280 = 1;
            pthread_cond_signal(&bme280_cond);
        }
        //pthread_mutex_unlock(&bme280_mutex);
        pthread_mutex_unlock(&send_mutex);

        pthread_mutex_lock(&send_mutex);
        if(run_sending == 0){ 
            run_sending = 1;
            pthread_cond_signal(&send_cond);
        }
        pthread_mutex_unlock(&send_mutex);

        //pthread_mutex_lock(&sensors_mutex);
        pthread_mutex_lock(&send_mutex);
        if(read_sensors == 0){ 
            read_sensors = 1;
            pthread_cond_signal(&sensors_cond);
        }
        pthread_mutex_unlock(&send_mutex);
        //pthread_mutex_unlock(&sensors_mutex);
        alarm(1);
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

  
int config_server(struct distr_server *server) 
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
	return 0;
} 


int main(int argc, char ** argv)
{
	struct bme280 *BME280 = malloc(sizeof(struct bme280));
	struct distr_server *server = malloc(sizeof(struct distr_server));
	struct gpio *GPIO = malloc(sizeof(struct gpio));

	int server_state = config_server(server);
	if(server_state == -1){
		exit(1);
	}

	server->BME280 = BME280;
	server->GPIO = GPIO;
	init_bme280_attr(BME280);	

	signal(SIGALRM, sig_handler);
	signal(SIGINT, stopWhile);
	alarm(1);

	pthread_mutex_init(&bme280_mutex, NULL);
	pthread_mutex_init(&send_mutex, NULL);
	pthread_mutex_init(&sensors_mutex, NULL);

	pthread_cond_init (&bme280_cond, NULL);
	pthread_cond_init (&send_cond, NULL);
	pthread_cond_init (&sensors_cond, NULL);

	pthread_t threads[3];
	pthread_create(&threads[0], NULL, sending_thread, (void*)server);
	pthread_create(&threads[1], NULL, get_temp_and_hum_thread, (void*)server);
	pthread_create(&threads[2], NULL, get_sensors_states_thread, (void*)server);

	while(run){}

	
	pthread_mutex_destroy(&bme280_mutex);
	pthread_mutex_destroy(&send_mutex);
	pthread_mutex_destroy(&sensors_mutex);

	pthread_cond_destroy(&bme280_cond); 
	pthread_cond_destroy(&send_cond); 
	pthread_cond_destroy(&sensors_cond); 

    close(server->socket_n); 
    return 0;
}

