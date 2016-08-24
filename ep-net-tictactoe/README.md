
  Jogo da velha em rede implementado em Python 3.

  Este programa foi testado no Linux, com python3-3.4.3-5.fc23.x86_64.
Para executar corretamente, você precisa do binário "python3" no seu PATH.


RODANDO O SERVIDOR
------------------
  O binário 'server' aceita como argumento a porta a ser usada para escutar conexões:
 ./server 6667

  O mesmo número de porta será usado para TCP e UDP. Caso não seja especificada uma porta, o servidor irá escutar na porta 8000.

  Para encerrar o servidor, digite Control + C.

RODANDO O CLIENTE
-----------------
  O cliente possui dois argumentos, protocolo e porta:
 ./client --protocol=UDP --port=8000

  O padrão é usar o protocolo UDP na porta 8000.
  Note que você deve usar a mesma porta no cliente e servidor.

  Para encerrar o cliente, use o comando QUIT.

ORGANIZAÇÃO DO CÓDIGO
---------------------
server - código fonte do servidor.
client - código fonte do cliente.

 Arquivos adicionais:
halloffame.db - banco de dados com pontuação dos jogadores
userpass.db - banco de dados com nomes e senhas
server.log - arquivo de log gerado pelo servidor

server.crt - certificado SSL
server.key - chave privada do servidor

slides.pdf - slides para apresentação do protocolo


PROTOCOLO DO JOGO DA VELHA
--------------------------
  Os seguintes comandos são suportados. Eles devem ser digitados no prompt do cliente.

  Para criar uma conta e fazer login:
CREATE nomeDeUsuario pass1234
LOGIN nomeDeUsuario pass1234

  Para listar usuários conectados:
LISTUSERS

  Para ver a pontuação dos usuários:
HALLOFFAME

  Para convidar um usuário a jogar uma partida:
JOIN usuarioConvidado

  Para aceitar um convite de outro usuário:
JOINACCEPT usuarioQueConvidou

  Para enviar uma jogada na partida em andamento:
PLAY NumeroDaJogada
(As posições possíveis vão de 0 a 8, indicadas no tabuleiro).

  Para abandonar uma partida (com derrota):
FORFEIT

  Para obter o estado do jogo após desconexão:
REQBOARD

  Para sair do servidor:
LOGOUT

  Para finalizar o cliente:
QUIT

  Para o heartbeat o servidor envia mensagens do tipo "PING numero" e o cliente responde com "PONG numero". Note que o cliente faz isso automaticamente, sem interação do usuário.


CRÉDITOS
--------
Autor: Evandro Fernandes Giovanini

