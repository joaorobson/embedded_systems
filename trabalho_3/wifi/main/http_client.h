#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#define IP "177.235.87.78"
#define IPSTACK_KEY "654e69afeb7ca79009619cacb54b8ef1"
#define IPSTACK_FIELDS "fields=country_code,city,region_code"
#define IPSTACK_URL "http://api.ipstack.com/" IP "?access_key=" IPSTACK_KEY "&" IPSTACK_FIELDS

#define OPENW_KEY "e7cb4b283c9ecd32924f0466949ff6ba"
#define OPENW_FIELDS "q=%s,%s,%s&units=metric&appid="

#define OPENW_URL "http://api.openweathermap.org/data/2.5/weather?" OPENW_FIELDS OPENW_KEY


void get_weather();
char* get_openw_url();
void http_request();

#endif
