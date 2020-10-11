#include "BME280/bme280.c"
#include "BME280/linux_userspace.c"
#include "LM35/main.c"
#include "LCD/lcd.c"
#include "utils.c"
#include <pthread.h>
pthread_mutex_t lock;

void main()
{
if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
  
	pthread_t threads[3];
	struct t *TE = malloc(sizeof(struct t));
	struct temps *TI = malloc(sizeof(struct temps));
	struct t *TR = malloc(sizeof(struct t));
	strcpy(TI->device, "/dev/i2c-1");
	TE->command = 0xA1;
	TR->command = 0xA2;
	pthread_create(&(threads[0]), NULL, get_temperature, (void*)TI);
	pthread_join(threads[0], NULL);
	pthread_mutex_lock(&lock); 
	pthread_create(&(threads[1]), NULL, get_uart_temperature, (void*)TE);
	pthread_join(threads[1], NULL);
    pthread_mutex_unlock(&lock); 

	pthread_mutex_lock(&lock); 

	pthread_create(&(threads[2]), NULL, get_uart_temperature, (void*)TR);
	pthread_join(threads[2], NULL);
    pthread_mutex_unlock(&lock); 
	//pthread_create(&(threads[1]), NULL, get_uart_temperature, 0xA1);
	//pthread_create(&(threads[2]), NULL, get_uart_temperature, 0xA2);
	printf("%f\n", TI->temp);
	printf("%f\n", TE->temp);
	printf("%f\n", TR->temp);

	//while(1){
	////float TE = get_temperature("/dev/i2c-1");
	////float TI = get_uart_temperature(0xA1);
	////float TR = get_uart_temperature(0xA2);
	//printf("TI: %f, TE: %f, TR: %f\n", TI, TE, TR);

	//char* line1text = (char*)malloc(16*sizeof(char));
	//char* line2text = (char*)malloc(16*sizeof(char));
	//sprintf(line1text, "TI:%.2f TE:%.2f", TI, TE); 
	//sprintf(line2text, "TR: %.2f", TR); 
	//
	//writeOnLCD(line1text, line2text);
	//writeOnCSV(TI, TE, TR);
	//}


}
