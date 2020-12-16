
#ifndef LED_H
#define LED_H

#include "driver/gpio.h"

#define LED 2

void LED_start();
void handle_LED_state(int state);


#endif