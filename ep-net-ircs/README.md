
Mini servidor de IRC implementado em linguagem C.

Este programa foi testado no Linux.


COMPILAR
--------
Execute o comando 'make' e será gerado o arquivo binário 'server'.


RODANDO O SERVIDOR
------------------
O binário 'server' aceita como argumento a porta a ser usada para escutar conexões:
 ./server 6667

Caso não seja especificada uma porta, o servidor irá escutar na porta 6667.


ORGANIZAÇÃO DO CÓDIGO
---------------------
server.c - função main.
	Coloca o programa em loop esperando conexões de clientes, e comandos, e processa os comandos. Cria uma POSIX thread para cada conexão.

irc.c - funções que implementam os comandos IRC suportados pelo servidor.

O cabeçalho do arquivo irc.c explica as estruturas usadas para armazenar dados, e é fundamental entendê-las para compreender o código.


CRÉDITOS
--------
Autor: Evandro Fernandes Giovanini


