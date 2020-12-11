#include "cjson/cJSON.h"
#include "read_dht11.h"

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