#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.c"

pthread_mutex_t bme280_mutex;
pthread_cond_t bme280_cond;

volatile sig_atomic_t run = 1; 
volatile sig_atomic_t read_bme280  = 0; 

void stopWhile(int signum){
    run = 0;
}

void* read_bme_thread(void* args){
	pthread_mutex_lock(&bme280_mutex);
	while(!read_bme280){
		pthread_cond_wait(&bme280_cond, &bme280_mutex);
		get_external_temperature(args);
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
        alarm(1);
    }
    if(signum == SIGINT){
        run = 0;
    }
}

int main(int argc, char ** argv)
{
	struct bme280 *BME280 = malloc(sizeof(struct bme280));


	init_bme280_attr(BME280);
	signal(SIGALRM, sig_handler);
	signal(SIGINT, stopWhile);
	alarm(1);

	pthread_mutex_init(&bme280_mutex, NULL);
	pthread_cond_init (&bme280_cond, NULL);
	pthread_t threads[1];
	pthread_create(&threads[0], NULL, read_bme_thread, (void*)BME280);

	while(run){
	}
    return 0;
}
