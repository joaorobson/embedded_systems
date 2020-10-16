# Trabalho 1 - Fundamentos de Sistemas Embarcados

## Como executar o projeto

* Para compilar o projeto, basta executar ```make compile``` na raiz;
* Para rodar o projeto, basta execurar ```make run``` na raiz;
* Para limpar o arquivo csv e apagar o binário gerado, basta executar ```make clean``` na raiz.

## Limitações conhecidas

* O comando ```ctrl + c``` não está finalizando a execução do projeto, apesar da chamada da função signal
que captura o ```SIGINT``` na main. Uma hipótese é que a utilização de threads com execução condicionada ao
sinal de alarm interfere na captura do sinal de interrupção;
* O terminal não permite a entrada da temperatura de referência e da histere a qualquer momento, pois a utilização de
threads para input/output não funcionou utilizando C e bibliotecas padrão. Para realizar isso, o uso de outra biblioteca
(ncurses, por exemplo) provavelmente resolveria.
