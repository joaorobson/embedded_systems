#ifndef READ_DHT11_H
#define READ_DHT11_H

struct dht11_data {
    double temp;
    double hum;
    int state;
};


void initialize_dht11();
void read_dht11(struct dht11_data* data);

#endif