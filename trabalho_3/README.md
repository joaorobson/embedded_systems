# Trabalho 3 - Fundamentos de Sistemas Embarcados

## Como executar o projeto

Primeiramente, configure as informações do WiFi e
das chaves das API's executando ```idf.py menuconfig```.
O seguinte menu será mostrado:

![menu](https://raw.githubusercontent.com/joaorobson/embedded_systems/master/trabalho_3/menu.png)

Para executar o código na placa, basta rodar os seguintes comandos padrão na raiz:

* ```idf.py build```;
* ```idf.py -p device flash monitor```

onde device é a porta serial onde a placa está conectada.
