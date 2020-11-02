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
 
int switch_lamp_state(int lamp_number, uint8_t state){
	if((lamp_number < 1) || (lamp_number > 4)){
		return 1;
	}
    else if(!bcm2835_init()){
        return 1;
	}

	switch(lamp_number){
		case 1:
			bcm2835_gpio_fsel(LAMP1_PIN, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(LAMP1_PIN, state);
			bcm2835_close();
			return 0;
		case 2:
			bcm2835_gpio_fsel(LAMP2_PIN, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(LAMP2_PIN, state);
			bcm2835_close();
			return 0;
		case 3:
			bcm2835_gpio_fsel(LAMP3_PIN, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(LAMP3_PIN, state);
			bcm2835_close();
			return 0;
		case 4:
			bcm2835_gpio_fsel(LAMP4_PIN, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(LAMP4_PIN, state);
			bcm2835_close();
			return 0;
		default:
			return 0;
	}
}

void set_sensors_mode(){
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


//int main(int argc, char **argv)
//{
//    // If you call this, it will not actually access the GPIO
//    // Use for testing
////    bcm2835_set_debug(1);
// 
//    if (!bcm2835_init())
//        return 1;
// 
//    // Set the pin to be an output
//    bcm2835_gpio_fsel(AIR_C1_PIN, BCM2835_GPIO_FSEL_OUTP);
// 
//    // Blink
//        // Turn it on
//	bcm2835_gpio_write(AIR_C1_PIN, HIGH);
//	bcm2835_delay(2999);
//	bcm2835_gpio_write(AIR_C1_PIN, LOW);
//	bcm2835_delay(2999);
//
//    bcm2835_close();
//    return 0;
//    // Set RPI pin P1-15 to be an input
//    bcm2835_gpio_fsel(LAMP1_PIN, BCM2835_GPIO_FSEL_INPT);
//    bcm2835_gpio_fsel(LAMP2_PIN, BCM2835_GPIO_FSEL_INPT);
//    //bcm2835_gpio_fsel(LAMP3_PIN, BCM2835_GPIO_FSEL_INPT);
//    //  with a pullup
//    bcm2835_gpio_set_pud(LAMP1_PIN, BCM2835_GPIO_PUD_UP);
//    bcm2835_gpio_set_pud(LAMP2_PIN, BCM2835_GPIO_PUD_UP);
//    //bcm2835_gpio_set_pud(LAMP3_PIN, BCM2835_GPIO_PUD_UP);
// 
//    // Blink
//    while (1)
//    {
//        // Read some data
//        uint8_t value1 = bcm2835_gpio_lev(LAMP1_PIN);
//        uint8_t value2 = bcm2835_gpio_lev(LAMP2_PIN);
//        uint8_t value3 = bcm2835_gpio_lev(LAMP3_PIN);
//        printf("read from pin 15: %d %d %d\n", value1, value2, value3);
//        
//        // wait a bit
//		sleep(1);
//    }
// 
//    bcm2835_close();
//    return 0;
//}
// 
