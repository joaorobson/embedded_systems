# Trabalho Final - Fundamentos de Sistemas Embarcados

## Como executar o projeto

### Python - Servidor central

Primeiramente, é necessário instalar as dependências do projeto na Raspberry Pi. Para isso, execute dentro de ```central_server```:

```pip3 install -r requirements.py```

Após isso, basta executar:

```python3 main.py```

### Front-end - Interface gráfica

Para executar o Front-end do projeto, basta executar dentro do diretório ```frontend``` o
comando ```docker-compose up --build```. A aplicação estará disponível na porta 3000, como mostrado abaixo:

![image](https://github.com/joaorobson/embedded_systems/blob/master/trabalho_final/interface.png)

### ESP32 - Servidor distribuído

Primeiramente, é necessário possuir o ambiente de desenvolvimento da ESP32 configurado como
explicado [aqui](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/). 

Após clonar o projeto, execute ```. path_to_esp-idf/export.sh``` dentro da pasta ```dist_server```.

Para executá-lo, é necessário a inclusão da biblioteca responsável pela leitura dos dados
do sensor DHT-11. Para isso, clone esse [repositório](https://github.com/0nism/esp32-DHT11)
nas pasta ```[path]/esp/esp-idf/components/``` e adicione o arquivo CMakeLists.txt
dentro do diretório clonado, na raiz, com o seguinte conteúdo:

```
idf_component_register(SRCS "dht11.c"
                       INCLUDE_DIRS "include")
```

Após isso, para executar o código na placa, basta rodar os seguintes comandos padrão na raiz:

* ```idf.py build```;
* ```idf.py -p device flash monitor```

onde device é a porta serial onde a placa está conectada.



