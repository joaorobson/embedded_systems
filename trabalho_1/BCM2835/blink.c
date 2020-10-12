// blink.c
//
// Example program for bcm2835 library
// Blinks a pin on an off every 0.5 secs
//
// After installing bcm2835, you can build this 
// with something like:
// gcc -o blink blink.c -l bcm2835
// sudo ./blink
//
// Or you can test it before installing with:
// gcc -o blink -I ../../src ../../src/bcm2835.c blink.c
// sudo ./blink
//
// Author: Mike McCauley
// Copyright (C) 2011 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $
 
#include <bcm2835.h>
#include <stdio.h>
 
#define RESISTOR_PIN RPI_GPIO_P1_16
#define COOLER_PIN RPI_GPIO_P1_18

 
void turnCoolerOn(){
	bcm2835_gpio_write(COOLER_PIN, LOW);
}

void turnCoolerOff(){
	bcm2835_gpio_write(COOLER_PIN, HIGH);
}


void turnResistorOn(){
	bcm2835_gpio_write(RESISTOR_PIN, LOW);
}

void turnResistorOff(){
	bcm2835_gpio_write(RESISTOR_PIN, HIGH);
}

//uint8_t deviceIsOn(char* device){
//	uint8_t value;
//    bcm2835_gpio_fsel(COOLER_PIN,  BCM2835_GPIO_FSEL_INPT);
//    bcm2835_gpio_fsel(RESISTOR_PIN, BCM2835_GPIO_FSEL_OUTP);
//	if(strcmp(device, "COOLER") == 0){
//		value = bcm2835_gpio_lev(COOLER_PIN);
//	}
//	else if(strcmp(device, "RESISTOR") == 0){
//		value = bcm2835_gpio_lev(RESISTOR_PIN);
//	}
//    bcm2835_close();
//	return value;
//}

int controlTemperature(char *device, char* action)
{
    if (!bcm2835_init())
      return 1;
 
	// Set the pins to be outputs
    bcm2835_gpio_fsel(COOLER_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESISTOR_PIN, BCM2835_GPIO_FSEL_OUTP);
 
	if(strcmp(device, "COOLER") == 0){
		if(strcmp(action, "ON") == 0){
			turnCoolerOn();
		}
		else if(strcmp(action, "OFF") == 0){
			turnCoolerOff();
		}
	}
	else if(strcmp(device, "RESISTOR") == 0){
		if(strcmp(action, "ON") == 0){
			turnResistorOn();
		}
		else if(strcmp(action, "OFF") == 0){
			turnResistorOff();
		}
	}

	bcm2835_delay(500);
    bcm2835_close();
    return 0;
}
