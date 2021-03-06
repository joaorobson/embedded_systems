#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H


#define IPSTACK_KEY CONFIG_IPSTACK_KEY
#define IPSTACK_FIELDS "?fields=country_code,city,region_code&access_key="
#define IPSTACK_HOST "http://api.ipstack.com/check"
#define IPSTACK_URL IPSTACK_HOST IPSTACK_FIELDS IPSTACK_KEY

#define OPENW_KEY CONFIG_OPENW_KEY

#define OPENW_FIELDS "q=%s,%s,%s&units=metric&appid="
#define OPENW_HOST "http://api.openweathermap.org/data/2.5/weather?" 
#define OPENW_URL OPENW_HOST OPENW_FIELDS OPENW_KEY

#define LED 2

struct location{
    char country_code[10];
    char city[50];
    char region_code[10];
};

char to_hex(char code);
char *encode_str(char *str);
char* get_ipstack_url();
void get_weather_forecast();
char* get_openw_url();
void http_request();

#endif
