#include "dht11.h"



void initialize_dht11(){
    DHT11_init(GPIO_NUM_4);
}

void read_dht11(){
    printf("Temperature is %d \n", DHT11_read().temperature);
    printf("Humidity is %d\n", DHT11_read().humidity);
    printf("Status code is %d\n", DHT11_read().status);
}