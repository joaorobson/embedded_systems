#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

char* data_to_JSON(struct distr_server* server){
    char *message = (char*) malloc(300*sizeof(char));
    sprintf(message, "{\"Temp\": %.5f, \"Hum\": %.5f, "
                     "\"Lamp1\": %u, \"Lamp2\": %u, "
                     "\"Lamp3\": %u, \"Lamp4\": %u, "
                     "\"AirC1\": %u, \"AirC2\": %u, "
                     "\"PresSens1\": %u, \"PresSens2\": %u, "
                     "\"OpenSens1\": %u, \"OpenSens2\": %u, "
                     "\"OpenSens3\": %u, \"OpenSens4\": %u, "
                     "\"OpenSens5\": %u, \"OpenSens6\": %u}",
                     server->BME280->temperature, 
                     server->BME280->humidity, 
                     server->GPIO->lamp1,
                     server->GPIO->lamp2,
                     server->GPIO->lamp3,
                     server->GPIO->lamp4,
                     server->GPIO->air_c1,
                     server->GPIO->air_c2,
                     server->GPIO->pres_sens1,
                     server->GPIO->pres_sens2,
                     server->GPIO->open_sens1,
                     server->GPIO->open_sens2,
                     server->GPIO->open_sens3,
                     server->GPIO->open_sens4,
                     server->GPIO->open_sens5,
                     server->GPIO->open_sens6);

    return message;
}
void* get_temperature_and_humidity(void* args){
	struct bme280 *temp_args = (struct bme280*) args;
    read_bme280_data(&temp_args->temperature,
                     &temp_args->humidity,
                     temp_args->device,
                     temp_args->id,
					 temp_args->dev);
    return NULL;
}

//void* get_uart_temperatures(void* args){
//	struct uart *temp_args = (struct uart*) args;
//    get_uart_temperature(&temp_args->TI,
//                         temp_args->device,
//                         temp_args->get_TI_command);
//
//    if(temp_args->read_TR){
//        get_uart_temperature(&temp_args->TR,
//                             temp_args->device,
//                             temp_args->get_TR_command);
//    }
//}
//
//char* get_datetime()
//{
//    time_t t = time(NULL);
//    struct tm tm = *localtime(&t);
//    char *datetime = (char*)malloc(30 * sizeof(char));
//
//    sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
//
//    return datetime;
//}
//
//
//void write_on_CSV(void* args)
//{
//	struct temp_control *temp_args = (struct temp_control*) args;
//    FILE *fp;
//    fp = fopen("sensors_data.csv", "a+");
//
//    // Check if file has already a header
//    fseek(fp, 0, SEEK_END);
//    int size = ftell(fp);
//
//    if (size == 0) {
//        fprintf(fp,"Data/Hora, Temp Int, Temp Ext, Temp Ref");
//    }
//
//    fprintf(fp, "\n%s, %.2lf, %.2lf, %.2lf", get_datetime(), temp_args->UART->TI, temp_args->BME280->temperature, temp_args->UART->TR);
//    fclose(fp);
//
//}
//
//void write_on_console(void* args){
//	struct temp_control *temp_args = (struct temp_control*) args;
//
//    printf("\n\nTI: %f, TE: %f, TR: %f\n", temp_args->UART->TI, temp_args->BME280->temperature, temp_args->UART->TR);
//}
//
//void write_on_LCD(void* args){
//	struct temp_control *temp_args = (struct temp_control*) args;
//    char* line1text = (char*)malloc(16*sizeof(char));
//    char* line2text = (char*)malloc(16*sizeof(char));
//    sprintf(line1text, "TI:%.2fTE:%.2f", temp_args->UART->TI, temp_args->BME280->temperature); 
//    sprintf(line2text, "TR: %.2f", temp_args->UART->TR); 
//		
//	LCD(line1text, line2text);
//}
//
//void output_temperatures(void *args){
//    write_on_LCD(args);
//    write_on_console(args);
//}
//
void init_bme280_attr(struct bme280 *BME280){
	strcpy(BME280->device, "/dev/i2c-1");
	BME280->dev = set_bme280_configs(BME280->device);
}
//
//void init_uart_attr(struct uart *T_UART){
//
//	T_UART->get_TI_command = 0xA1;
//	T_UART->get_TR_command = 0xA2;
//	strcpy(T_UART->device, "/dev/serial0");
//	
//}
//
//
//void resetCoolerAndResistor(){
//	controlTemperature("COOLER", "OFF");
//	controlTemperature("RESISTOR", "OFF");
//}
//
//void menu(struct uart *T_UART, struct temp_control *T_CONTROL){
//    int option = 0;
//    printf("Deseja inserir temperatura de referência?\n1 - S\n2 - N\n");
//    scanf("%d", &option);
//    printf("%d\n", option);
//    if(option == 1){
//        printf("Insira a TR:\n");
//        scanf("%f", &T_UART->TR);
//        T_UART->read_TR = 0;
//    }
//    else if(option == 2){
//        T_UART->read_TR = 1;
//    }
//    else{
//        printf("Valor inválido!!\n");
//        exit(1);
//    }
//    option = 0;
//    printf("Deseja inserir a histerese?\n1 - S\n2 - N\n");
//    scanf("%d", &option);
//    if(option == 1){
//        printf("Insira a TR:\n");
//        scanf("%f", &T_CONTROL->hysteresis);
//    }
//    else if(option == 2){
//        T_CONTROL->hysteresis = 4.0;
//    }
//    else{
//        printf("Valor inválido!!\n");
//        exit(1);
//    }
//}
//
//void keep_temperature(struct temp_control *TC)
//{
//	float upperLimit = TC->UART->TR + (TC->hysteresis/2.0);
//	float lowerLimit = TC->UART->TR - (TC->hysteresis/2.0);
//	if(TC->UART->TI > upperLimit){
//		if(TC->coolerIsOn == 0){
//			controlTemperature("COOLER", "ON");
//			TC->coolerIsOn = 1;
//		}
//		if(TC->resistorIsOn == 1){
//			controlTemperature("RESISTOR", "OFF");
//			TC->resistorIsOn = 0;
//		}
//	}
//	else if(TC->UART->TI < lowerLimit){
//		if(TC->coolerIsOn == 1){
//			controlTemperature("COOLER", "OFF");
//			TC->coolerIsOn = 0;
//		}
//		if(TC->resistorIsOn == 0){
//			controlTemperature("RESISTOR", "ON");
//			TC->resistorIsOn = 1;
//		}
//	}
//}
