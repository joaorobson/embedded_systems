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
#define CS_PORT 10010
#define DS_PORT 10110
#define CS_HOST "192.168.0.53"
#define DS_HOST "192.168.0.52"

#define SA struct sockaddr


pthread_mutex_t bme280_mutex, sensors_mutex, send_mutex;
pthread_cond_t bme280_cond, sensors_cond, send_cond;

volatile sig_atomic_t run = 1;
volatile sig_atomic_t read_bme280 = 0,
                      read_sensors = 0,
                      run_sending = 0,
                      sending_counter = 0,
                      bme_counter = 0;


void stopWhile(int signum){
    run = 0;
}

void* get_sensors_states_thread(void* _args){
    pthread_mutex_lock(&sensors_mutex);
    while(!read_sensors && run){
        pthread_cond_wait(&sensors_cond, &sensors_mutex);
        struct distr_server *server= (struct distr_server*) _args;

        get_sensors_states(server->GPIO);
        uint8_t level_change = check_level_changing(server->GPIO);
        if(server->alarm != level_change){
            server->alarm = level_change;

            char *message = (char*) malloc(300*sizeof(char));
            sprintf(message, "{\"Alert\": %u}", server->alarm);

            send(server->sending_socket, message, strlen(message), 0 );

            printf("%s\n", message);
        }

        read_sensors = 0;
    }
    pthread_mutex_unlock(&sensors_mutex);
    pthread_exit( NULL );
}

void* sending_thread(void* _args){
    pthread_mutex_lock(&send_mutex);
    while(!run_sending && run){
        pthread_cond_wait(&send_cond, &send_mutex);
        if(sending_counter == 5){
            struct distr_server *server = (struct distr_server*) _args;

            char *message = (char*) malloc(300*sizeof(char));
            message = data_to_JSON(server);

            send(server->sending_socket, message, strlen(message), 0 );

            printf("%s\n", message);
            sending_counter = 0;
        }
        run_sending = 0;
        sending_counter++;
    }
    pthread_mutex_unlock(&send_mutex);
    pthread_exit( NULL );
}

void* receiving_thread(void* _args){
	struct distr_server *server = (struct distr_server*) _args;

	while(1){
		char buffer[1024] = {0};
		int data = recv(server->receiving_socket, buffer, 1024, 0);
		if(data == 0){
			exit(0);
		}
		char device[10];
		sscanf(buffer, "{\"Device\": \"%[^\"}]\"}", device);
		switch_device_state(device, server->GPIO);
		printf("%s\n", buffer);
	}
}

void* get_temp_and_hum_thread(void* _args){
    pthread_mutex_lock(&bme280_mutex);
    while(!read_bme280 && run){
        pthread_cond_wait(&bme280_cond, &bme280_mutex);
        if(bme_counter == 5){
            struct distr_server *server = (struct distr_server*) _args;
            get_temperature_and_humidity((void*)server->BME280);
            bme_counter = 0;
        }
        read_bme280 = 0;
        bme_counter++;

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

        pthread_mutex_lock(&send_mutex);
        if(run_sending == 0){
            run_sending = 1;
            pthread_cond_signal(&send_cond);
        }
        pthread_mutex_unlock(&send_mutex);

        pthread_mutex_lock(&sensors_mutex);
        if(read_sensors == 0){
            read_sensors = 1;
            pthread_cond_signal(&sensors_cond);
        }
        pthread_mutex_unlock(&sensors_mutex);

        ualarm(2e5, 2e5);
    }
    if(signum == SIGINT){
        run = 0;
    }
}

int config_sender(struct distr_server *server)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(CS_HOST);
    serv_addr.sin_port = htons(CS_PORT);

    if(inet_pton(AF_INET, CS_HOST, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed. Trying again... \n");
    }
    server->sending_socket = sock;
    return 0;
}

int config_receiver(struct distr_server *server){
	int sock, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((sock = socket(AF_INET, SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
		return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
		return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //inet_addr(DS_HOST);
    address.sin_port = htons(DS_PORT);

    if (bind(sock, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
		return -1;
    }
    if (listen(sock, 3) < 0)
    {
        perror("listen");
		return -1;
    }

    if ((new_socket = accept(sock, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
		return -1;
    }
    server->receiving_socket = new_socket;
	printf("Distributed server receiving requests\n");
	return 0;
}

int main(int argc, char ** argv)
{
    struct bme280 *BME280 = malloc(sizeof(struct bme280));
    struct distr_server *server = malloc(sizeof(struct distr_server));
    struct gpio *GPIO = malloc(sizeof(struct gpio));

    int receiver_status = config_receiver(server);
    if(receiver_status == -1){
        exit(1);
    }

    int sender_status = config_sender(server);
    if(sender_status == -1){
        exit(1);
	}

    server->BME280 = BME280;
    server->GPIO = GPIO;
    server->alarm = 0;
    init_bme280_attr(BME280);
    set_sensors_mode();

    signal(SIGALRM, sig_handler);
    signal(SIGINT, sig_handler);
    ualarm(2e5, 2e5);

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
    pthread_create(&threads[3], NULL, receiving_thread, (void*)server);

    while(run){}

    pthread_mutex_destroy(&bme280_mutex);
    pthread_mutex_destroy(&send_mutex);
    pthread_mutex_destroy(&sensors_mutex);

    pthread_cond_destroy(&bme280_cond);
    pthread_cond_destroy(&send_cond);
    pthread_cond_destroy(&sensors_cond);

    close(server->sending_socket);
    close(server->receiving_socket);
    bcm2835_close();

    return 0;
}
