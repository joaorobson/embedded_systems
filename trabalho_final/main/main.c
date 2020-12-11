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

#define LED_1 2
#define BOTAO_1 0

xQueueHandle interruption_queue;

xSemaphoreHandle wifi_connection_semaphore;
xSemaphoreHandle mqtt_connection_semaphore;

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



static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(interruption_queue, &pino, NULL);
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
            publish_message(get_esp_init_topic(), "oi");

      //publish_message("sensores/temperatura", mount_dht11_JSON(data));
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

      // Configuração dos pinos dos LEDs 
  gpio_pad_select_gpio(LED_1);   
  // Configura os pinos dos LEDs como Output
  gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);  

  // Configuração do pino do Botão
  gpio_pad_select_gpio(BOTAO_1);
  // Configura o pino do Botão como Entrada
  gpio_set_direction(BOTAO_1, GPIO_MODE_INPUT);
  // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
  gpio_pulldown_en(BOTAO_1);
  // Desabilita o resistor de Pull-up por segurança.
  gpio_pullup_dis(BOTAO_1);

  // Configura pino para interrupção
  gpio_set_intr_type(BOTAO_1, GPIO_INTR_POSEDGE);

  interruption_queue = xQueueCreate(10, sizeof(int));

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);
    initialize_dht11();
    wifi_start();

    xTaskCreate(&init_mqtt, "Inicializa MQTT", 2048, NULL, 2, NULL);
    xTaskCreate(&trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);
        xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
