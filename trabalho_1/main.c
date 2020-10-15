#include "LCD/lcd.c"
#include "utils.c"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

pthread_mutex_t mutex1, mutex2, mutex3, mutex4, mutex5;
pthread_cond_t condition1, condition2, condition3, condition4, condition5;

int csv_counter = 0;
int run_read_bme_thread = 0;
int run_read_uart_thread = 0; 
int run_write_csv_thread = 0;
int run_output_temp_thread = 0;
int run_keep_temp_thread = 0;
volatile sig_atomic_t run = 1; 

void stopWhile(int signum){
    run = 0;
}

void* read_uart_thread(void* args){
	pthread_mutex_lock(&mutex1);
	while(!run_read_uart_thread){
		pthread_cond_wait(&condition1, &mutex1);
		get_uart_temperatures(args);
		run_read_uart_thread = 0;
	}
	pthread_mutex_unlock(&mutex1);
}

void* read_bme_thread(void* args){
	pthread_mutex_lock(&mutex2);
	while(!run_read_bme_thread){
		pthread_cond_wait(&condition2, &mutex2);
		get_external_temperature(args);
		run_read_bme_thread = 0;
	}
	pthread_mutex_unlock(&mutex2);
}

void* write_on_CSV_thread(void* args){
	pthread_mutex_lock(&mutex3);
	while(!run_write_csv_thread){
		pthread_cond_wait(&condition3, &mutex3);
		if(csv_counter == 4){
			write_on_CSV(args);
			csv_counter = 0;
		}
		run_write_csv_thread = 0;
	}	
	pthread_mutex_unlock(&mutex3);
}

void* keep_temperature_thread(void* args){
	pthread_mutex_lock(&mutex4);
	while(!run_keep_temp_thread){
		pthread_cond_wait(&condition4, &mutex4);
		keep_temperature(args);
		run_keep_temp_thread = 0;
	}
	pthread_mutex_unlock(&mutex4);
}

void* output_temperatures_thread(void* args){
	pthread_mutex_lock(&mutex5);
	while(!run_output_temp_thread){
		pthread_cond_wait(&condition5, &mutex5);
		output_temperatures(args);
		run_output_temp_thread = 0;
	}
	pthread_mutex_unlock(&mutex5);
}
void sig_handler(int signum) {
    pthread_mutex_lock(&mutex1);

    if(run_read_uart_thread == 0){ 
        run_read_uart_thread = 1;
        pthread_cond_signal(&condition1);
    }   
    pthread_mutex_unlock(&mutex1);

    pthread_mutex_lock(&mutex2);
    if(run_read_bme_thread == 0){ 
        run_read_bme_thread = 1;
        pthread_cond_signal(&condition2);
    }   
    pthread_mutex_unlock(&mutex2);

    pthread_mutex_lock(&mutex3);
    if(run_write_csv_thread == 0){ 
        run_write_csv_thread = 1;
		csv_counter++;
        pthread_cond_signal(&condition3);
    }   
    pthread_mutex_unlock(&mutex3);

    pthread_mutex_lock(&mutex4);
    if(run_keep_temp_thread == 0){ 
        run_keep_temp_thread = 1;
        pthread_cond_signal(&condition4);
    }   
    pthread_mutex_unlock(&mutex4);

    pthread_mutex_lock(&mutex5);
    if(run_output_temp_thread == 0){ 
        run_output_temp_thread = 1;
        pthread_cond_signal(&condition5);
    } 
    pthread_mutex_unlock(&mutex5);

	ualarm(5e5, 5e5);
}


void main(int argc, char ** argv)
{

	struct bme280 *T_BME280 = malloc(sizeof(struct bme280));
	struct uart *T_UART = malloc(sizeof(struct uart));
	struct temp_control *T_CONTROL = malloc(sizeof(struct temp_control)); 

	init_bme280_attr(T_BME280);
	init_uart_attr(T_UART);

	T_CONTROL->UART = T_UART;
	T_CONTROL->BME280 = T_BME280;
	T_CONTROL->coolerIsOn = 0;
	T_CONTROL->resistorIsOn = 0;
	T_CONTROL->hysteresis = 4.0;

	resetCoolerAndResistor();
	setup_LCD();

	menu(T_UART, T_CONTROL);

	signal(SIGALRM, sig_handler);
	signal(SIGINT, stopWhile);
    ualarm(5e5, 5e5);

	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);
	pthread_mutex_init(&mutex4, NULL);
	pthread_mutex_init(&mutex5, NULL);
	pthread_cond_init (&condition1, NULL);
	pthread_cond_init (&condition2, NULL);
	pthread_cond_init (&condition3, NULL);
	pthread_cond_init (&condition4, NULL);
	pthread_cond_init (&condition5, NULL);



	pthread_t threads[5];
	pthread_create(&threads[0], NULL, read_bme_thread, (void*)T_BME280);
	pthread_create(&threads[1], NULL, read_uart_thread, (void*)T_UART);
	pthread_create(&threads[2], NULL, write_on_CSV_thread, (void*)T_CONTROL);
	pthread_create(&threads[3], NULL, keep_temperature_thread, (void*)T_CONTROL);
	pthread_create(&threads[4], NULL, output_temperatures_thread, (void*)T_CONTROL);
	
	
	while(run){
	}
	resetCoolerAndResistor();

	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	pthread_mutex_destroy(&mutex4);
	pthread_mutex_destroy(&mutex5);

	pthread_cond_destroy(&condition1);
	pthread_cond_destroy(&condition2);
	pthread_cond_destroy(&condition3);
	pthread_cond_destroy(&condition4);
	pthread_cond_destroy(&condition5);

    pthread_exit(NULL);
}
