#include "led.h"
#include "esp_event.h"

void LED_start(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void blink_LED(){

    gpio_set_level(LED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(LED, 1);

}