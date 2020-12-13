#include "cjson/cJSON.h"
#include "read_dht11.h"
#include "mqtt.h"
#include "esp_system.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_1 2
#define BOTAO_1 0

extern xQueueHandle interruption_queue;


char* get_room_name(char* json_str){
    const cJSON* room_name = NULL;

    cJSON* json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    }

    room_name = cJSON_GetObjectItemCaseSensitive(json, "room");
    if (cJSON_IsString(room_name) && (room_name->valuestring != NULL))
    {
        return room_name->valuestring;
    }

    return "";

}

char* mount_dht11_JSON(struct dht11_data* data){

    char* json = NULL;
    cJSON* temp = NULL;
    cJSON* hum = NULL;

    cJSON* monitor = cJSON_CreateObject();

    temp = cJSON_CreateNumber(data->temp);

    cJSON_AddItemToObject(monitor, "temperature", temp);

    hum = cJSON_CreateNumber(data->hum);

    cJSON_AddItemToObject(monitor, "humidity", hum);

    json = cJSON_PrintUnformatted(monitor);

    return json;
}

char* get_esp_init_topic(){
    char* topic = (char*) malloc(sizeof(char)*100);
    uint8_t mac_addr[6] = {0};

    esp_efuse_mac_get_default(mac_addr);

    sprintf(topic, 
            "fse2020/%s/dispositivos/%x:%x:%x:%x:%x:%x", 
            CONFIG_MATRICULA, 
            mac_addr[0],
            mac_addr[1],
            mac_addr[2],
            mac_addr[3],
            mac_addr[4],
            mac_addr[5]);

    return topic;
}

char* get_room_topic(){
    char* topic = (char*) malloc(sizeof(char)*100);
    sprintf(topic, "fse2020/%s/+", CONFIG_MATRICULA);

    return topic;
}

void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(interruption_queue, &pino, NULL);
}

void init_button(){
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


    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);
    initialize_dht11();
}