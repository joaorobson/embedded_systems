#ifndef UTILS_H
#define UTILS_H
#include "read_dht11.h"

char* mount_dht11_JSON(struct dht11_data* data);
char* get_esp_init_topic();
char* get_room_topic();
char* get_room_name(char* json_str);
void init_button();
void IRAM_ATTR gpio_isr_handler(void *args);
#endif