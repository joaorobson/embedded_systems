#include "led.h"
#include "esp_event.h"

void LED_start(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void blink_LED(int led_status){
    gpio_set_level(LED, !led_status);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    gpio_set_level(LED, led_status);
    vTaskDelay(200 / portTICK_PERIOD_MS);

}