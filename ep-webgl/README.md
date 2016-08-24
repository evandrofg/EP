MAC 420/5744 – Introdução à Computação Gráfica – Exercício-Programa 1

Evandro Fernandes Giovanini
Marcello Souza


    1. Introdução

    Este exercício programa de MAC420 carrega as peças para exibir uma partida de Xadrez em 3D, usando WebGL.

A implementação está em app.js, os shaders em index.html e a inicialização deles em initShaders.js.


    2. Como Rodar

    Para visualizar este programa é preciso um servidor Web local. Isso é necessário por questões de segurança dos navegadores atuais, que não carregam arquivos OBJ locais como pedido pelo código.

Uma maneira simples de fazer isso é rodar o seguinte comando no diretório deste projeto:

$ python3 -m http.server 8080

Com isso, basta acessar http://localhost:8080 no navegador.


    3. Sobre o programa

    O programa cria um tabuleiro usando 64 cubos, que são especificados pelo modelo cubo.obj. Também carregamos as peças usando os arquivos OBJ fornecidos, e temos assim o estado inicial de uma partida de xadrez.

Dois modos de visualização diferentes são fornecidos, e o usuário pode alternar entre eles clicando nos botões "Perspectiva" (padrão) e "Ortográfica". A cena sempre é mantida centralizada no viewport.

O programa respeita o redimensionamento da janela, e o canvas ocupa todo o espaço do browser, como pedido.

O controle da câmera com quatérnios também foi implementado. Basta clicar no canvas com o botão esquerdo e mover o mouse para iniciá-lo.

Finalmente, o zoom pode ser feito clicando com o botão direito ou o botão do meio do mouse e movendo-o verticalmente. Alguns navegadores não liberam o botão direito (pelo menos sem fazer gambiarras), por isso essa operação também foi implementada para o botão do meio do mouse.


