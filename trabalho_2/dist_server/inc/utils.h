#ifndef UTILS_H_
#define UTILS_H_

#include <time.h>
#include <pthread.h>
#include "bme280.h"
#include "gpio.h"

struct read_temperatures {
    float TI;
    float TR;
    float TE;
};

struct bme280 {
	float temperature;
	float humidity;
	char device[20];
    struct bme280_dev dev;
    struct identifier id;
};

struct distr_server {
    struct bme280 *BME280;
    struct gpio *GPIO;
    int receiving_socket;
    int sending_socket;
    uint8_t alarm;
};

char* data_to_JSON(struct distr_server* server);

void* get_temperature_and_humidity(void* args);

void init_bme280_attr(struct bme280 *BME280);

#endif
