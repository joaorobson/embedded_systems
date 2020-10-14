#include "LCD/lcd.c"
#include "utils.c"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t needs_to_write_on_csv = false;
volatile int csv_write_counter = 0;

pthread_mutex_t mutex;


volatile sig_atomic_t stop; 
void
stopWhile(int signum)
{
    stop = 1;
}

void sig_handler(int signum){
	needs_to_write_on_csv = true;
        //get_external_temperature((void*) T_BME280);
		//pthread_create(&(threads[0]), NULL, get_external_temperature, (void*)T_BME280);
		//pthread_create(&(threads[1]), NULL, get_uart_temperatures, (void*)T_UART);
		//pthread_join(threads[0], NULL);
}


void main()
{
//if (pthread_mutex_init(&lock, NULL) != 0) { 
//        printf("\n mutex init has failed\n"); 
//        return 1; 
//    } 
//  
	pthread_t threads[3];

//	pthread_mutex_lock(&lock); 
//	pthread_create(&(threads[1]), NULL, get_uart_temperature, (void*)TE);
//	pthread_join(threads[1], NULL);
//    pthread_mutex_unlock(&lock); 
//
//	pthread_mutex_lock(&lock); 
//
//	pthread_create(&(threads[2]), NULL, get_uart_temperature, (void*)TR);
//	pthread_join(threads[2], NULL);
//    pthread_mutex_unlock(&lock); 
	//pthread_create(&(threads[1]), NULL, get_uart_temperature, 0xA1);
	//pthread_create(&(threads[2]), NULL, get_uart_temperature, 0xA2);
	struct bme280 *T_BME280 = malloc(sizeof(struct bme280));
	struct uart *T_UART = malloc(sizeof(struct uart));
	struct temp_control *T_CONTROL = malloc(sizeof(struct temp_control)); 

	T_CONTROL->coolerIsOn = 0;
	T_CONTROL->resistorIsOn = 0;
	T_CONTROL->hysteresis = 4.0;

	resetCoolerAndResistor();
	setupLCD();

	//TR->temperature = 49.0;

	signal(SIGINT, stopWhile);
	signal(SIGALRM,sig_handler);
	alarm(2);

	//pthread_mutex_init(&mutex, NULL);
	//TR->mutex = mutex;
	//TI->mutex = mutex;
	init_bme280_attr(T_BME280);
	init_uart_attr(T_UART);
	T_CONTROL->UART = *T_UART;

	while(!stop){
        //get_external_temperature((void*) T_BME280);
		pthread_create(&(threads[0]), NULL, get_external_temperature, (void*)T_BME280);
		pthread_create(&(threads[1]), NULL, get_uart_temperatures, (void*)T_UART);
		pthread_join(threads[0], NULL);
		pthread_join(threads[1], NULL);
		//pthread_join(threads[2], NULL);
		//get_temperature((void*)TI);
		//get_temperature((void*)TR);

		menu(T_BME280, T_UART);

		if(needs_to_write_on_csv){
			write_on_CSV(T_UART->TI, T_BME280->temperature, T_UART->TR);
			needs_to_write_on_csv = false;
			alarm(2);  // Scheduled alarm after 2 seconds
		}

		char* line1text = (char*)malloc(16*sizeof(char));
		char* line2text = (char*)malloc(16*sizeof(char));
		sprintf(line1text, "TI:%.2fTE:%.2f", T_UART->TI, T_BME280->temperature); 
		sprintf(line2text, "TR: %.2f", T_UART->TR); 
		
		writeOnLCD(line1text, line2text);
		keep_temperature(T_CONTROL);
	}
	//pthread_mutex_destroy(&mutex);
	resetCoolerAndResistor();
}
