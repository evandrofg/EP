Exercício-Programa de Criptografia, algoritmo K128


	Este programa foi desenvolvido como um exercício de criptografia. Ele
permite encriptografar e descriptografar arquivos usando uma senha. A
criptografia é feita com o algoritmo K128.


MODO DE USO
-----------

    As seguintes opções são aceitas:

    -c              indica que a operação será criptografar
    -d              indica que a operação será descriptografar
    -i <arquivo>    arquivo de entrada
    -o <arquivo>    arquivo de saída
    -p <senha>      senha da criptografia
    -a              indica que o arquivo original será apagado

    Uma operação (-c ou -d) deve obrigatoriamente ser escolhida.



COMO COMPILAR
-------------

    Rode o comando 'make'. O executável 'ep' será gerado.

    O programa foi testado em um notebook Intel Core i5-2520M, rodando
Fedora Workstation 22 de 64 bits, com:

    Linux 4.0.3-300.fc22.x86_64
    gcc version 5.1.1 20150422 (Red Hat 5.1.1-1) (GCC)
    GNU Make 4.0


CÓDIGO FONTE
------------

    O programa está escrito em C. A organização do código:

    ep.c: função principal do programa, lê os argumentos da linha de comando e
chama as funções correspondentes para fazer o serviço.

    crypto.{c,h}: uma interface para criptografia, recebe o arquivo e argumentos
e chama o algoritmo K128, fazendo CBC.

    K128.{c,h}: implementação do algoritmo K128 pelo enunciado do EP.


TESTES
------

    O diretório tests/ contém um arquivo de texto e uma música Ogg Vorbis.

    Os arquivos originais são Entra.txt e Entra.ogg. Criptografados são Sai.txt
e Sai.ogg, e as versões descriptografadas são EntraD.txt e EntraD.ogg.

    O script test-all faz a criptografia e descriptografia de cada arquivo de
teste e imprime o hash do arquivo original e descriptografado para comparação.
Para rodar os testes use o comando './test-all'.


CRÉDITOS
--------
Autor: Evandro Fernandes Giovanini
