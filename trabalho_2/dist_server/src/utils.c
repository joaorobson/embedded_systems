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

void init_bme280_attr(struct bme280 *BME280){
	strcpy(BME280->device, "/dev/i2c-1");
	BME280->dev = set_bme280_configs(BME280->device);
}
