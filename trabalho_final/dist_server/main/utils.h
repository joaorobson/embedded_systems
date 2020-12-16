#ifndef UTILS_H
#define UTILS_H
#include "read_dht11.h"
#include "cJSON.h"

char* mount_dht11_JSON(char* key, struct dht11_data* data);
char* get_esp_init_topic();
char* get_room_topic(char* room_name, char* topic_info);
char* get_room_name(cJSON* json);
char* process_data(char* json_str);
void init_button();
int check_key(cJSON* json, char* key);
char* init_room_name(char* room_name);
char* mount_MAC_JSON();
void IRAM_ATTR gpio_isr_handler(void *args);
#endif