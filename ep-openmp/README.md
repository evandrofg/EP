
 Ondas irreais

  Este programa gera ondas irreais e simula seu comportamento num dado intervalo
de tempo. A ideia do exercício é aproveitar o paralelismo usando OpenMP.
  O enunciado está em Ondas.pdf, porém especificações adicionais foram passadas
em sala de aula.

PROGRAMA
--------

    Implementado em C, o código fonte está em ondas.c.


COMPILANDO
----------

    Para compilar, use o comando 'make'.


RODANDO
-------

    Para rodar o programa:

    ./ondas in.txt

    No pacote estão incluídos dois arquivos de entrada: in.txt e in2.txt.

    O primeiro contém os valores descritos no enunciado, o segundo é uma pequena
modificação para rodar em tempo menor, útil no desenvolvimento.

    A saída do programa são dois arquivos: uma imagem PPM, com o nome
$entrada.ppm, e um arquivo com as médias, com o nome $entrada-medias.txt.

    Também estão incluídos os dois arquivos usados na competição da turma:
comp.txt, comp3.txt.
    Todos os programas entregues rodaram esses dois arquivos, e este programa
foi um dos vencedores, rodando em menor tempo. :)

CRÉDITOS
--------

Autor: Evandro Fernandes Giovanini


