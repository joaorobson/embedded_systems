#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

char* read_nvs(char* key)
{
    // Inicia o acesso à partição padrão nvs
    ESP_ERROR_CHECK(nvs_flash_init());

    size_t required_size;
    nvs_handle default_partition_handle;
    
    // Abre o acesso à partição nvs
    esp_err_t res_nvs = nvs_open("storage", NVS_READONLY, &default_partition_handle);
    
    esp_err_t res = nvs_get_str(default_partition_handle, key, NULL, &required_size);

    char* value = malloc(required_size);

    if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: storage, não encontrado");
    }
    else
    {
        res = nvs_get_str(default_partition_handle, key, value, &required_size);

        switch (res)
        {
        case ESP_OK:
            printf("Valor armazenado: %s\n", value);
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE("NVS", "Valor não encontrado");
            return "-1";
        default:
            ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            return "-1";
            break;
        }

        nvs_close(default_partition_handle);
    }
    return value;
}

void write_on_nvs(char* key, char* value)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    nvs_handle default_partition_handle;

    esp_err_t res_nvs = nvs_open("storage", NVS_READWRITE, &default_partition_handle);
    
    if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }
    esp_err_t res = nvs_set_str(default_partition_handle, key, value);
    printf("Valor escrito: %s\n", value);
    if(res != ESP_OK)
    {
        ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
    }
    nvs_commit(default_partition_handle);
    nvs_close(default_partition_handle);
}