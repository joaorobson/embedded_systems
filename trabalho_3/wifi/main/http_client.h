#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H


#define IPSTACK_KEY CONFIG_IPSTACK_KEY
#define IP "177.235.87.78"
#define IPSTACK_FIELDS "?fields=country_code,city,region_code&access_key="
#define IPSTACK_HOST "http://api.ipstack.com/"
#define IPSTACK_URL IPSTACK_HOST IP IPSTACK_FIELDS IPSTACK_KEY

#define OPENW_KEY CONFIG_OPENW_KEY

#define OPENW_FIELDS "q=%s,%s,%s&units=metric&appid="
#define OPENW_HOST "http://api.openweathermap.org/data/2.5/weather?" 
#define OPENW_URL OPENW_HOST OPENW_FIELDS OPENW_KEY

struct location{
    char country_code[10];
    char city[50];
    char region_code[10];
};


char* get_ipstack_url();
void get_weather();
char* get_openw_url();
void http_request();

#endif
