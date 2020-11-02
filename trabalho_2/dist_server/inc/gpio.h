#ifndef GPIO_H_
#define GPIO_H_

#include <bcm2835.h>
#include <stdio.h>
#include <time.h> 
// Input on RPi pin GPIO 15
#define LAMP1_PIN RPI_GPIO_P1_11
#define LAMP2_PIN RPI_GPIO_P1_12
#define LAMP3_PIN RPI_GPIO_P1_13
#define LAMP4_PIN RPI_GPIO_P1_15
#define AIR_C1_PIN RPI_GPIO_P1_16
#define AIR_C2_PIN RPI_GPIO_P1_18
#define PRES_SENS1_PIN RPI_GPIO_P1_22
#define PRES_SENS2_PIN RPI_GPIO_P1_37
#define OPEN_SENS1_PIN RPI_GPIO_P1_29
#define OPEN_SENS2_PIN RPI_GPIO_P1_31
#define OPEN_SENS3_PIN RPI_GPIO_P1_32
#define OPEN_SENS4_PIN RPI_GPIO_P1_36
#define OPEN_SENS5_PIN RPI_GPIO_P1_38
#define OPEN_SENS6_PIN RPI_GPIO_P1_40

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
 
int switch_lamp_state(int lamp_number, uint8_t state);

void get_sensors_states(struct gpio* GPIO);


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

#endif
