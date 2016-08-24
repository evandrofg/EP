MAC0438 Programação Concorrente – EP1
Evandro Fernandes Giovanini

1. Introdução

O programa simula uma corrida de Fórmula 1, e após completada ele exibe a
classificação de pilotos e equipes no campeonato. O programa também gera
aleatoriamente o resultado de outras 9 corridas, para completar uma temporada de
10 provas no total.

O programa funciona da seguinte forma: cada piloto é uma thread, e todas são
disparadas simultaneamente. O escalonador do SO é responsável por decidir quem
vai ser executado primeiro, e assim os pilotos vão completando suas voltas na
pista.

O programa usa um mutex para manter a consistência na leitura e alteração da
pista e usa semáforos para controlar quando cada thread termina a execução de
sua simulação.

O estado de cada piloto é exibido sempre que ele completa uma volta.

Na pasta do programa existem dois carquivos de entrada diferentes, entrada1.txt e
entrada2.txt.


2. Instruções

Para compilar o programa, rode o comando make. Isso irá criar o executável ep1,
que exige um arquivo de entrada como seu único argumento válido. O formato do
arquivo de entrada é descrito no enunciado do EP.
