# Trabalho 2 - Fundamentos de Sistemas Embarcados

## Como executar o projeto

O projeto consiste em dois servidores (central e distribuído) cada um escrito em uma linguagem (Python e C, respectivamente).
Para executá-los, basta executar os seguintes comandos:

* Em um terminal, no diretório ```central_server```, execute ```python3 main.py```;
* Em outro terminal, no diretório ```dist_server```, execute ```make all``` e ```./bin/bin```.

Após isso, a transmissão de dados via conexão TCP/IP entre os sockets dos servidores iniciará.

No terminal do servidor central, um menu como o seguinte será mostrado:

![menu](https://github.com/joaorobson/embedded_systems/blob/master/trabalho_2/menu.png)

Por meio dele, o estado das portas e janelas e dos dispositivos (lâmpadas e ar condicionados) pode ser monitorado,
assim como nele podem ser configurados a temperatura do ar condicionado e o estado (desligado(a)/ligado(a)) dos dispositivos.
No caso de haver alguma porta ou janela aberta, é mostrado um alerta também. 

No terminal do servidor distribuído, as informações enviadas e recebidas em formato JSON são exibidas, como é mostrado na figura abaixo:

![menu](https://github.com/joaorobson/embedded_systems/blob/master/trabalho_2/log_dist_server.png)
