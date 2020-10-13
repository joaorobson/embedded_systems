#include <time.h>
#include "BME280/bme280.c"
#include "BME280/read_data.c"
#include "UART/read_data.c"
#include "BCM2835/control_temperature.c"


struct sensors_temperature{
	float temperature;
	unsigned char command;
	char device[20];
	char sensor[20];
};

void* get_temperature(void* args){
	struct sensors_temperature *temp_args = (struct sensors_temperature*) args;
	
	if(strcmp(temp_args->sensor, "BME280") == 0){
			get_bme280_temperature(&temp_args->temperature,
                                   temp_args->device);
	}
	else if(strcmp(temp_args->sensor, "LM35") == 0){
			get_lm35_temperature(&temp_args->temperature,
								 temp_args->device,
                                 temp_args->command);
	}
}

char* get_datetime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *datetime = (char*)malloc(30 * sizeof(char));

    sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return datetime;
}


void writeOnCSV(float TI, float TE, float TR)
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


void initSensors(struct sensors_temperature *TE,
                  struct sensors_temperature *TI,
                  struct sensors_temperature *TR){

	strcpy(TE->sensor, "BME280");
	strcpy(TE->device, "/dev/i2c-1");
	TI->command = 0xA1;
	strcpy(TI->sensor, "LM35");
	strcpy(TI->device, "/dev/serial0");
	TR->command = 0xA2;
	strcpy(TR->sensor, "LM35");
	strcpy(TR->device, "/dev/serial0");
	
}

void menu(struct sensors_temperature *TE,
          struct sensors_temperature *TI,
          struct sensors_temperature *TR){
    system("@cls||clear");
    printf("\n\nTI: %f, TE: %f, TR: %f\n", TI->temperature, TE->temperature, TR->temperature);
    printf("Selecione uma opção:\n1 - Definir temperatura de referência\n2 - Sair\n");
}

void resetCoolerAndResistor(){
	controlTemperature("COOLER", "OFF");
	controlTemperature("RESISTOR", "OFF");

}
void keepTemperature(float hysteresis,
					 struct sensors_temperature *TI,
					 struct sensors_temperature *TR,
					 int* coolerIsOn,
					 int* resistorIsOn)
{
	float upperLimit = TR->temperature + (hysteresis/2.0);
	float lowerLimit = TR->temperature - (hysteresis/2.0);

	if(TI->temperature > upperLimit){
		if(*coolerIsOn == 0){
			controlTemperature("COOLER", "ON");
			*coolerIsOn = 1;
		}
		if(*resistorIsOn == 1){
			controlTemperature("RESISTOR", "OFF");
			*resistorIsOn = 0;
		}
	}
	else if(TI->temperature < lowerLimit){
		if(*coolerIsOn == 1){
			controlTemperature("COOLER", "OFF");
			*coolerIsOn = 0;
		}
		if(*resistorIsOn == 0){
			controlTemperature("RESISTOR", "ON");
			*resistorIsOn = 1;
		}
	}
}
