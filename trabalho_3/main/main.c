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

TaskHandle_t suspend_LED_loop;

int led_status = 0;

void do_HTTPRequest(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY))
    {
      eTaskState led_loop_state = eTaskGetState(suspend_LED_loop);
      if(led_loop_state != eSuspended){
        ESP_LOGI("Main Task", "Suspendendo LED piscando em loop"); 
        vTaskSuspend(suspend_LED_loop);
        xSemaphoreGive(turn_led_on_semaphore);
      }
      ESP_LOGI("Main Task", "Realiza HTTP Request"); 
      get_weather_forecast();
      vTaskDelay(3e5 / portTICK_PERIOD_MS);

      xSemaphoreGive(wifi_connection_semaphore);
    }
    else{
      vTaskResume(suspend_LED_loop);
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

void LED_blink_loop(void* params){
  while(true){
    ESP_LOGI("Secondary Task", "Pisca LED em loop"); 
    blink_LED(led_status);
    vTaskDelay( 1000 / portTICK_PERIOD_MS);
  }
}

void do_LED_blink(void* params){
  while(true){
    if(xSemaphoreTake(blink_led_semaphore, portMAX_DELAY)){
      ESP_LOGI("Secondary Task", "Pisca LED"); 
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

    wifi_connection_semaphore = xSemaphoreCreateBinary();
    turn_led_on_semaphore = xSemaphoreCreateBinary();
    blink_led_semaphore = xSemaphoreCreateBinary();
   
    xTaskCreate(&LED_blink_loop, "Piscando LED sem parar...", 2048, NULL, 2, &suspend_LED_loop);
   
    LED_start();
    wifi_start();

    xTaskCreate(&do_HTTPRequest, "Processa HTTP", 4096, NULL, 2, NULL);
    xTaskCreate(&do_LED_blink, "Piscar LED", 2048, NULL, 2, NULL);
    xTaskCreate(&turn_LED_on, "Ligar LED", 2048, NULL, 2, NULL);
}
