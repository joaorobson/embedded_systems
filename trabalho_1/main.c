#include "LCD/lcd.c"
#include "utils.c"
#include <pthread.h>

pthread_mutex_t lock;

void main()
{
//if (pthread_mutex_init(&lock, NULL) != 0) { 
//        printf("\n mutex init has failed\n"); 
//        return 1; 
//    } 
//  
//	pthread_t threads[3];
//	strcpy(TI->device, "/dev/i2c-1");
//	TE->command = 0xA1;
//	TR->command = 0xA2;
//	pthread_create(&(threads[0]), NULL, get_temperature, (void*)TI);
//	pthread_join(threads[0], NULL);
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

	strcpy(TI->sensor, "BME280");
	strcpy(TI->device, "/dev/i2c-1");
	TE->command = 0xA1;
	strcpy(TE->sensor, "LM35");
	strcpy(TE->device, "/dev/serial0");
	TR->command = 0xA2;
	strcpy(TR->sensor, "LM35");
	strcpy(TR->device, "/dev/serial0");
	
	//printf("%f\n", TI->temp);
	//printf("%f\n", TE->temp);
	//printf("%f\n", TR->temp);

	while(1){
		get_temperature((void*)TI);
		get_temperature((void*)TE);
		get_temperature((void*)TR);
		printf("T\n\nI: %f, TE: %f, TR: %f\n", TI->temperature, TE->temperature, TR->temperature);

		char* line1text = (char*)malloc(16*sizeof(char));
		char* line2text = (char*)malloc(16*sizeof(char));
		sprintf(line1text, "TI:%.2f TE:%.2f", TI->temperature, TE->temperature); 
		sprintf(line2text, "TR: %.2f", TR->temperature); 
		
		writeOnLCD(line1text, line2text);
		writeOnCSV(TI->temperature, TE->temperature, TR->temperature);
	}


}
