#include "cjson/cJSON.h"
#include "read_dht11.h"
#include "mqtt.h"
#include "esp_system.h"

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