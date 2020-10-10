#include "BME280/bme280.c"
#include "BME280/linux_userspace.c"
#include "LM35/main.c"
#include "LCD/lcd.c"
#include "utils.c"

void main()
{
	while(1){
	float TE = get_temperature("/dev/i2c-1");
	float TI = get_uart_temperature(0xA1);
	float TR = get_uart_temperature(0xA2);
	printf("TI: %f, TE: %f, TR: %f\n", TI, TE, TR);

	char* line1text = (char*)malloc(16*sizeof(char));
	char* line2text = (char*)malloc(16*sizeof(char));
	sprintf(line1text, "TI:%.2f TE:%.2f", TI, TE); 
	sprintf(line2text, "TR: %.2f", TR); 
	
	writeOnLCD(line1text, line2text);
	writeOnCSV(TI, TE, TR);
	}


}
