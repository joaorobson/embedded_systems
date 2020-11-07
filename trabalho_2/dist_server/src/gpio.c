// input.c
//
// Example program for bcm2835 library
// Reads and prints the state of an input pin
//
// After installing bcm2835, you can build this 
// with something like:
// gcc -o input input.c -l bcm2835
// sudo ./input
//
// Or you can test it before installing with:
// gcc -o input -I ../../src ../../src/bcm2835.c input.c
// sudo ./input
//
// Author: Mike McCauley
// Copyright (C) 2011 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $
#include <bcm2835.h>
#include "gpio.h"
#include "utils.h"
#include <string.h>
 
void switch_device_state(char* device_name, struct gpio* GPIO){
    uint8_t state;
    if(strcmp(device_name, "Lamp1") == 0){
        state = 1 - GPIO->lamp1;
        bcm2835_gpio_fsel(LAMP1_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(LAMP1_PIN, state);
    }
    else if(strcmp(device_name, "Lamp2") == 0){
        state = 1 - GPIO->lamp2;
        bcm2835_gpio_fsel(LAMP2_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(LAMP2_PIN, state);
    }
    else if(strcmp(device_name, "Lamp3") == 0){
        state = 1 - GPIO->lamp3;
        bcm2835_gpio_fsel(LAMP3_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(LAMP3_PIN, state);
    }
    else if(strcmp(device_name, "Lamp4") == 0){
        state = 1 - GPIO->lamp4;
        bcm2835_gpio_fsel(LAMP4_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(LAMP4_PIN, state);
	}
    else if(strcmp(device_name, "AirC1") == 0){
        state = 1 - GPIO->air_c1;
        bcm2835_gpio_fsel(AIR_C1_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(AIR_C1_PIN, state);
	}
    else if(strcmp(device_name, "AirC2") == 0){
        state = 1 - GPIO->air_c2;
        bcm2835_gpio_fsel(AIR_C2_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(AIR_C2_PIN, state);
	}
}

uint8_t check_level_changing(struct gpio* GPIO){
    if(GPIO->pres_sens1 || GPIO->pres_sens2 ||
       GPIO->open_sens1 || GPIO->open_sens2 ||
       GPIO->open_sens3 || GPIO->open_sens4 ||
       GPIO->open_sens5 || GPIO->open_sens6){
            return 1;
       }
       return 0;
}

uint8_t set_sensors_mode(){
    if (!bcm2835_init()){
        return 1;
	}

	bcm2835_gpio_fsel(LAMP1_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(LAMP2_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(LAMP3_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(LAMP4_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(AIR_C1_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(AIR_C2_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(PRES_SENS1_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(PRES_SENS2_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS1_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS2_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS3_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS4_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS5_PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(OPEN_SENS6_PIN, BCM2835_GPIO_FSEL_INPT);
	return 0;
}

void get_sensors_states(struct gpio* GPIO){

	GPIO->lamp1 = bcm2835_gpio_lev(LAMP1_PIN);
	GPIO->lamp2 = bcm2835_gpio_lev(LAMP2_PIN);
	GPIO->lamp3 = bcm2835_gpio_lev(LAMP3_PIN);
	GPIO->lamp4 = bcm2835_gpio_lev(LAMP4_PIN);
	GPIO->air_c1 = bcm2835_gpio_lev(AIR_C1_PIN);
	GPIO->air_c2 = bcm2835_gpio_lev(AIR_C2_PIN);
	GPIO->pres_sens1 = bcm2835_gpio_lev(PRES_SENS1_PIN);
	GPIO->pres_sens2 = bcm2835_gpio_lev(PRES_SENS2_PIN);
	GPIO->open_sens1 = bcm2835_gpio_lev(OPEN_SENS1_PIN);
	GPIO->open_sens2 = bcm2835_gpio_lev(OPEN_SENS2_PIN);
	GPIO->open_sens3 = bcm2835_gpio_lev(OPEN_SENS3_PIN);
	GPIO->open_sens4 = bcm2835_gpio_lev(OPEN_SENS4_PIN);
	GPIO->open_sens5 = bcm2835_gpio_lev(OPEN_SENS5_PIN);
	GPIO->open_sens6 = bcm2835_gpio_lev(OPEN_SENS6_PIN);
}
