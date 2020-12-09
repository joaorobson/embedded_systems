#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "read_dht11.h"
#include "wifi.h"

xSemaphoreHandle temp_and_hum_semaphore;

void read_temp_and_hum(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(temp_and_hum_semaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Lê temperatura e pressão"); 
      read_dht11();
      vTaskDelay(3e4 / portTICK_PERIOD_MS);

      xSemaphoreGive(temp_and_hum_semaphore);
    }
  }
}



void app_main()
{
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    temp_and_hum_semaphore = xSemaphoreCreateBinary();
    
    initialize_dht11();
    wifi_start();

    xTaskCreate(&read_temp_and_hum, "Ler Temperatura/Umidade", 2048, NULL, 2, NULL);

}

