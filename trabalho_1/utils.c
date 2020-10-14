#include <time.h>
#include <pthread.h>
#include "BME280/bme280.c"
#include "BME280/read_data.c"
#include "UART/read_data.c"
#include "BCM2835/control_temperature.c"


struct uart {
	float TI;
	float TR;
	unsigned char get_TI_command;
	unsigned char get_TR_command;
	char device[20];
};

struct temp_control {
    float hysteresis;
    int coolerIsOn;
    int resistorIsOn;
    struct uart UART;
};
struct bme280 {
	float temperature;
	char device[20];
    struct bme280_dev dev;
    struct identifier id;
};

void* get_external_temperature(void* args){
	struct bme280 *temp_args = (struct bme280*) args;
    get_bme280_temperature(&temp_args->temperature,
                           temp_args->device,
                           temp_args->id,
						   temp_args->dev);
}

void* get_uart_temperatures(void* args){
	struct uart *temp_args = (struct uart*) args;
    get_uart_temperature(&temp_args->TI,
                         temp_args->device,
                         temp_args->get_TI_command);

    get_uart_temperature(&temp_args->TR,
                         temp_args->device,
                         temp_args->get_TR_command);
}

char* get_datetime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *datetime = (char*)malloc(30 * sizeof(char));

    sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return datetime;
}


void write_on_CSV(float TI, float TE, float TR)
{
    FILE *fp;
    fp = fopen("sensors_data.csv", "a+");

    // Check if file has already a header
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    if (size == 0) {
        fprintf(fp,"Data/Hora, Temp Int, Temp Ext, Temp Ref");
    }

    fprintf(fp, "\n%s, %.2lf, %.2lf, %.2lf", get_datetime(), TI, TE, TR);
    fclose(fp);

}

void init_bme280_attr(struct bme280 *BME280){
	strcpy(BME280->device, "/dev/i2c-1");
    //BME280->id = open_device(BME280->id, BME280->device);
    //BME280->dev = malloc(sizeof(struct bme280_dev));
	BME280->dev = set_bme280_configs(BME280->device);

}

void init_uart_attr(struct uart *T_UART){

	T_UART->get_TI_command = 0xA1;
	T_UART->get_TR_command = 0xA2;
	strcpy(T_UART->device, "/dev/serial0");
	
}

void menu(struct bme280 *TE,
          struct uart *T_UART){
    system("clear");
    printf("\n\nTI: %f, TE: %f, TR: %f\n", T_UART->TI, TE->temperature, T_UART->TR);
    printf("Selecione uma opção:\n1 - Definir temperatura de referência\n2 - Sair\n");
}

void resetCoolerAndResistor(){
	controlTemperature("COOLER", "OFF");
	controlTemperature("RESISTOR", "OFF");

}
void keep_temperature(struct temp_control *TC)
{
	float upperLimit = TC->UART.TR + (TC->hysteresis/2.0);
	float lowerLimit = TC->UART.TR - (TC->hysteresis/2.0);

	if(TC->UART.TI > upperLimit){
		if(TC->coolerIsOn == 0){
			controlTemperature("COOLER", "ON");
			TC->coolerIsOn = 1;
		}
		if(TC->resistorIsOn == 1){
			controlTemperature("RESISTOR", "OFF");
			TC->resistorIsOn = 0;
		}
	}
	else if(TC->UART.TI < lowerLimit){
		if(TC->coolerIsOn == 1){
			controlTemperature("COOLER", "OFF");
			TC->coolerIsOn = 0;
		}
		if(TC->resistorIsOn == 0){
			controlTemperature("RESISTOR", "ON");
			TC->resistorIsOn = 1;
		}
	}
}
