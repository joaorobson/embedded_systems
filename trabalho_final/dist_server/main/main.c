#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "read_dht11.h"
#include "wifi.h"
#include "mqtt.h"
#include "utils.h"
#include "read_dht11.h"
#include "storage.h"

xQueueHandle interruption_queue;

xSemaphoreHandle wifi_connection_semaphore;
xSemaphoreHandle mqtt_connection_semaphore;

char room[50];



void init_mqtt(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Inicializa MQTT"); 
      mqtt_start();
      
      char is_registered[10];
      strcpy(is_registered, read_nvs("is_registered"));
      if(strcmp(is_registered, "s") != 0){
        publish_message(get_esp_init_topic(), "{}");
        write_on_nvs("is_registered", "s");
      }
    }
  }
}


void handle_button_interruption(void *params)
{
  int pin;
  int device_state = 0;
  struct dht11_data* data = (struct dht11_data*) params;

  while(true)
  {
    if(xQueueReceive(interruption_queue, &pin, portMAX_DELAY))
    {
      // De-bouncing
      int state = gpio_get_level(pin);
      if(state == 1)
      {
        gpio_isr_handler_remove(pin);
        while(gpio_get_level(pin) == state)
        {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        device_state = 1 - device_state;
        data->state = device_state;
        printf("Estado atual: %d\n", device_state);

        char* topic = (char*)malloc(100*sizeof(char));
        sprintf(topic, "fse2020/150154003/%s/estado", room);
        publish_message(topic, mount_JSON("state", data));

        // Habilitar novamente a interrupção
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_isr_handler_add(pin, gpio_isr_handler, (void *) pin);
      }

    }
  }
}

void handle_mqtt_messages(void * params)
{
  if(xSemaphoreTake(mqtt_connection_semaphore, portMAX_DELAY))
  {
    while(true)
    {
      struct dht11_data* data = (struct dht11_data*) params;

      read_dht11(data);
      if(strcmp(room, "") != 0){
        char* topic = (char*)malloc(100*sizeof(char));
        sprintf(topic, "fse2020/150154003/%s/temperatura", room);
        publish_message(topic, mount_JSON("temperature", data));
        
        sprintf(topic, "fse2020/150154003/%s/umidade", room);
        publish_message(topic, mount_JSON("humidity", data));

        sprintf(topic, "fse2020/150154003/%s/estado", room);
        publish_message(topic, mount_JSON("state", data));

      }

      vTaskDelay(3000 / portTICK_PERIOD_MS);
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

    wifi_connection_semaphore = xSemaphoreCreateBinary();
    mqtt_connection_semaphore = xSemaphoreCreateBinary();
    interruption_queue = xQueueCreate(10, sizeof(int));

    init_button();
    wifi_start();
    
    struct dht11_data* data = malloc(sizeof(struct dht11_data));
    data->state = 0;
    
    xTaskCreate(&init_mqtt, "Inicializa MQTT", 2048, NULL, 2, NULL);
    xTaskCreate(&handle_button_interruption, "TrataBotao", 2048, (void*) data, 1, NULL);
    xTaskCreate(&handle_mqtt_messages, "Comunicação com Broker", 4096, (void*) data, 1, NULL);
}
