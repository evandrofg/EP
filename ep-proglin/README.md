 Evandro Fernandes Giovanini
 EP de MAC315 - Programação Linear
 =================================

  Este programa implementa o algoritmo simplex em redes para cálculo do menor
caminho. São dados quatro problemas de exemplo em arquivos .dat. A
especificação do programa está no enunciado, em pdf.


USO
===

Para compilar, use 'make'. Para rodar o programa:

  $ ./ep problema.dat

Será impresso na tela o caminho mínimo e seu custo.


CÓDIGO
======

ep.cpp - codigo fonte principal, com a função main e o simplex para redes
GRAPHutils.[h,c] - funções para manipulação de grafos

As funções de grafos foram criadas no curso de MAC328, copiei elas para usar
neste EP, com pequenas adaptações para compilar no g++ ao invés do gcc.

Para compilar o código basta executar o comando 'make'. Eu testei no Fedora 20
com o gcc 4.8.3 na minha máquina pessoal e no Debian 7.5 com o gcc 4.7.2 da
Rede Linux.

Para rodar o EP basta chamar './ep problema.dat', onde o argumento é o arquivo
de entrada.

