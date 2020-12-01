# Trabalho 3 - Fundamentos de Sistemas Embarcados

## Como executar o projeto

Primeiramente, é necessário possuir o ambiente de desenvolvimento da ESP32 configurado como
explicado [aqui](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/). 

Após clonar o projeto, execute ```. path_to_esp-idf/export.sh``` na raiz dele.

Após isso, configure as informações do WiFi e
das chaves das API's executando ```idf.py menuconfig```.
O seguinte menu será mostrado:

![menu](https://raw.githubusercontent.com/joaorobson/embedded_systems/master/trabalho_3/menu.png)

Para executar o código na placa, basta rodar os seguintes comandos padrão na raiz:

* ```idf.py build```;
* ```idf.py -p device flash monitor```

onde device é a porta serial onde a placa está conectada.
