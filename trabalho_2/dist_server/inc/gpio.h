#ifndef GPIO_H_
#define GPIO_H_

#include <bcm2835.h>
#include <stdio.h>
#include <time.h> 

#define LAMP1_PIN RPI_V2_GPIO_P1_11
#define LAMP2_PIN RPI_V2_GPIO_P1_12
#define LAMP3_PIN RPI_V2_GPIO_P1_13
#define LAMP4_PIN RPI_V2_GPIO_P1_15
#define AIR_C1_PIN RPI_V2_GPIO_P1_16
#define AIR_C2_PIN RPI_V2_GPIO_P1_18
#define PRES_SENS1_PIN RPI_V2_GPIO_P1_22
#define PRES_SENS2_PIN RPI_V2_GPIO_P1_37
#define OPEN_SENS1_PIN RPI_V2_GPIO_P1_29
#define OPEN_SENS2_PIN RPI_V2_GPIO_P1_31
#define OPEN_SENS3_PIN RPI_V2_GPIO_P1_32
#define OPEN_SENS4_PIN RPI_V2_GPIO_P1_36
#define OPEN_SENS5_PIN RPI_V2_GPIO_P1_38
#define OPEN_SENS6_PIN RPI_V2_GPIO_P1_40

struct gpio {
	uint8_t lamp1;
	uint8_t lamp2;
	uint8_t lamp3;
	uint8_t lamp4;
	uint8_t air_c1;
	uint8_t air_c2;
	uint8_t pres_sens1;
	uint8_t pres_sens2;
	uint8_t open_sens1;
	uint8_t open_sens2;
	uint8_t open_sens3;
	uint8_t open_sens4;
	uint8_t open_sens5;
	uint8_t open_sens6;
};
 
void switch_device_state(char* device_name, struct gpio* GPIO);

uint8_t check_level_changing(struct gpio* GPIO);

uint8_t set_sensors_mode();

void get_sensors_states(struct gpio* GPIO);

#endif
