#include "dht11.h"
#include "read_dht11.h"

void initialize_dht11(){
    DHT11_init(GPIO_NUM_4);
}

void read_dht11(struct dht11_data* data){
    data->temp = DHT11_read().temperature;
    data->hum = DHT11_read().humidity;

    printf("Temperature is %lf \n", data->temp);
    printf("Humidity is %lf\n", data->hum);
    printf("Status code is %d\n", DHT11_read().status);
}