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
      publish_message(get_esp_init_topic(), "oi");
    }
  }
}


void trataInterrupcaoBotao(void *params)
{
  int pino;
  int contador = 0;

  while(true)
  {
    if(xQueueReceive(interruption_queue, &pino, portMAX_DELAY))
    {
      // De-bouncing
      int estado = gpio_get_level(pino);
      if(estado == 1)
      {
        gpio_isr_handler_remove(pino);
        while(gpio_get_level(pino) == estado)
        {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        contador++;
        printf("Os botões foram acionados %d vezes. Botão: %d\n", contador, pino);

        // Habilitar novamente a interrupção
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
      }

    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  struct dht11_data* data = malloc(sizeof(struct dht11_data));
  if(xSemaphoreTake(mqtt_connection_semaphore, portMAX_DELAY))
  {
    while(true)
    {
      read_dht11(data);
      if(strcmp(room, "") != 0){
        char topic[100];
        sprintf(topic, "fse2020/150154003/%s/temperatura", room);
        publish_message(topic, mount_dht11_JSON(data));
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

    xTaskCreate(&init_mqtt, "Inicializa MQTT", 2048, NULL, 2, NULL);
    xTaskCreate(&trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
