# Trabalho Final - Fundamentos de Sistemas Embarcados

## Como executar o projeto

### ESP32

Primeiramente, é necessário possuir o ambiente de desenvolvimento da ESP32 configurado como
explicado [aqui](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/). 

Após clonar o projeto, execute ```. path_to_esp-idf/export.sh``` na raiz dele.

Para executá-lo, é necessário a inclusão da biblioteca responsável pela leitura dos dados
do sensor DHT-11. Para isso, clone esse [repositório](https://github.com/0nism/esp32-DHT11)
nas pasta ```[path]/esp/esp-idf/components/``` e adicione o arquivo CMakeLists.txt na
dentro do diretório clonado, na raiz, com o seguinte conteúdo:

```
idf_component_register(SRCS "dht11.c"
                       INCLUDE_DIRS "include")
```

Após isso, para executar o código na placa, basta rodar os seguintes comandos padrão na raiz:

* ```idf.py build```;
* ```idf.py -p device flash monitor```

onde device é a porta serial onde a placa está conectada.
