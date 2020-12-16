#include "led.h"
#include "esp_event.h"
#include "driver/gpio.h"


void LED_start(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void handle_LED_state(int state){
    gpio_set_level(LED, state);
}