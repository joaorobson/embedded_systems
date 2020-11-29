#include "http_client.h"

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

#define TAG "HTTP"

char chunked_response[500];
char response[500];
int chunked_res_len = 0;


/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *encode_str(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}


esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            printf("%.*s", evt->data_len, (char *)evt->data);
            strcpy(response, (char *)evt->data);
            response[evt->data_len] = '\0';
        }
        else
        {
            strcat(chunked_response, (char *)evt->data);
            chunked_res_len += evt->data_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void get_location(struct location* ips_loc){
    cJSON* country_code = NULL;
    cJSON* city = NULL;
    cJSON* region_code = NULL;
    cJSON* monitor_json = cJSON_Parse(chunked_response);

    printf("+++++++++++++ %s\n", chunked_response);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }
    city = cJSON_GetObjectItemCaseSensitive(monitor_json, "city");
    if (cJSON_IsString(city) && (city->valuestring != NULL))
    {
        strcpy(ips_loc->city, city->valuestring);
    }

    country_code = cJSON_GetObjectItemCaseSensitive(monitor_json, "country_code");
    if (cJSON_IsString(country_code) && (country_code->valuestring != NULL))
    {
        strcpy(ips_loc->country_code, country_code->valuestring);
    }

    region_code = cJSON_GetObjectItemCaseSensitive(monitor_json, "region_code");
    if (cJSON_IsString(region_code) && (region_code->valuestring != NULL))
    {
        strcpy(ips_loc->region_code, region_code->valuestring);
    }
}


char* get_openw_url(struct location* ips_loc)
{
    char *openw_url = (char *)malloc(300 * sizeof(char));
    sprintf(openw_url, 
            OPENW_URL, 
            encode_str(ips_loc->city), 
            ips_loc->region_code,
            ips_loc->country_code);

    return openw_url;
}

void get_weather()
{
    struct location* ips_loc = malloc(sizeof(struct location));

    http_request(IPSTACK_URL);
    
    get_location(ips_loc);

    http_request(get_openw_url(ips_loc));
    printf("===============> %s\n", response);
}

void http_request(char *req_url)
{
    esp_http_client_config_t config = {
        .url = req_url,
        .event_handler = _http_event_handle,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }

    esp_http_client_cleanup(client);
}