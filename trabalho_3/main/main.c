#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "http_client.h"
#include "led.h"

xSemaphoreHandle wifi_connection_semaphore;
xSemaphoreHandle blink_led_semaphore;
xSemaphoreHandle turn_led_on_semaphore;

int led_status = 0;

void do_HTTPRequest(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Realiza HTTP Request"); 
      xSemaphoreGive(turn_led_on_semaphore);
      get_weather_forecast();
      xSemaphoreGive(wifi_connection_semaphore);
      vTaskDelay(3e4 / portTICK_PERIOD_MS);
    }
  }
}

void turn_LED_on(void* params){
  while(true){
    if(xSemaphoreTake(turn_led_on_semaphore, portMAX_DELAY)){
      ESP_LOGI("Secondary Task", "Liga LED"); 
      led_status = 1;
      gpio_set_level(LED, led_status);
    }
  }
}

void do_LED_blink(void* params){
  while(true){
    if(xSemaphoreTake(blink_led_semaphore, portMAX_DELAY)){
      blink_LED(led_status);
    }
  }

}

void app_main(void)
{
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    LED_start();

    wifi_connection_semaphore = xSemaphoreCreateBinary();
    turn_led_on_semaphore = xSemaphoreCreateBinary();
    blink_led_semaphore = xSemaphoreCreateBinary();

    wifi_start();

    xTaskCreate(&do_HTTPRequest, "Processa HTTP", 4096, NULL, 2, NULL);
    xTaskCreate(&do_LED_blink, "Piscar LED", 2048, NULL, 2, NULL);
    xTaskCreate(&turn_LED_on, "Ligar LED", 2048, NULL, 2, NULL);
}
