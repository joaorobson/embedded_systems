#include "LCD/lcd.c"
#include "utils.c"
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex;

volatile sig_atomic_t stop; 
void
stopWhile(int signum)
{
    stop = 1;
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
	struct sensors_temperature *TE = malloc(sizeof(struct sensors_temperature));
	struct sensors_temperature *TI = malloc(sizeof(struct sensors_temperature));
	struct sensors_temperature *TR = malloc(sizeof(struct sensors_temperature));

	int coolerIsOn, resistorIsOn;
	coolerIsOn = 0;
	resistorIsOn = 0;
	float hysteresis = 4.0;

	initSensors(TE, TI, TR);
	resetCoolerAndResistor();
	setupLCD();

	//TR->temperature = 49.0;

	signal(SIGINT, stopWhile);

	pthread_mutex_init(&mutex, NULL);
	TR->mutex = mutex;
	TI->mutex = mutex;

	while(!stop){
		//get_temperature((void*)TE);
		pthread_create(&(threads[0]), NULL, get_temperature, (void*)TE);
		pthread_create(&(threads[1]), NULL, get_temperature, (void*)TR);
		pthread_create(&(threads[2]), NULL, get_temperature, (void*)TI);
		pthread_join(threads[0], NULL);
		pthread_join(threads[1], NULL);
		pthread_join(threads[2], NULL);
		//get_temperature((void*)TI);
		//get_temperature((void*)TR);

		menu(TE, TI, TR);

		usleep(500000);

		writeOnCSV(TI->temperature, TE->temperature, TR->temperature);


		char* line1text = (char*)malloc(16*sizeof(char));
		char* line2text = (char*)malloc(16*sizeof(char));
		sprintf(line1text, "TI:%.2fTE:%.2f", TI->temperature, TE->temperature); 
		sprintf(line2text, "TR: %.2f", TR->temperature); 
		
		writeOnLCD(line1text, line2text);
		keepTemperature(hysteresis, TI, TR, &coolerIsOn, &resistorIsOn);
	}
	pthread_mutex_destroy(&mutex);
	resetCoolerAndResistor();
}
