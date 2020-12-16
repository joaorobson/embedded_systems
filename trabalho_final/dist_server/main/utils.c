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

#include "storage.h"
#include "led.h"

#define BOTAO_1 0

extern xQueueHandle interruption_queue;
extern char room[50];

int get_LED_state(cJSON* json){
    const cJSON* led_state = NULL;

    led_state = cJSON_GetObjectItemCaseSensitive(json, "led");

    if (cJSON_IsNumber(led_state) && (led_state->valueint != NULL))
    {
        return led_state->valueint;
    }
    return 0;
}


int check_key(cJSON* json, char* key){
    const cJSON* value = NULL;

    value = cJSON_GetObjectItemCaseSensitive(json, key);

    if(value){
        return 1;
    }
    return 0;
}

char* get_room_name(cJSON* json){

    const cJSON* room_name = NULL;

    room_name = cJSON_GetObjectItemCaseSensitive(json, "room");

    if (cJSON_IsString(room_name) && (room_name->valuestring != NULL))
    {
        // Chave "room" existe no JSON
        write_on_nvs("room_name", room_name->valuestring);
        return room_name->valuestring;
    }
    return "";
}

char* init_room_name(char* room_name){
    strcpy(room_name, read_nvs("room_name"));

    if(strlen(room_name) != 0){
        return room_name;
    }
    
    strcpy(room_name, "");

    return room_name;
}

void process_data(char* json_str){

    cJSON* json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    }


    if(check_key(json, "room")){
        strcpy(room, get_room_name(json));
    }

    else if(check_key(json, "led")){
        handle_LED_state(get_LED_state(json));
    }
}   



char* mount_JSON(char* key, struct dht11_data* data){

    char* json = NULL;
    cJSON* value = NULL;

    cJSON* monitor = cJSON_CreateObject();

    if(strcmp(key, "temperature") == 0){
        value = cJSON_CreateNumber(data->temp);
    }
    else if(strcmp(key, "humidity") == 0){
        value = cJSON_CreateNumber(data->hum);
    }
    else if(strcmp(key, "state") == 0){
        value = cJSON_CreateNumber(data->state);
    }
    else{
        return "{}";
    }
    cJSON_AddItemToObject(monitor, key, value);
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

char* get_room_topic(char* room_name, char* topic_info){
    char* topic = (char*) malloc(sizeof(char)*100);
    sprintf(topic, 
            "fse2020/%s/%s/%s", 
            CONFIG_MATRICULA,
            room_name,
            topic_info);

    return topic;
}

void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(interruption_queue, &pino, NULL);
}

void init_button(){
 
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