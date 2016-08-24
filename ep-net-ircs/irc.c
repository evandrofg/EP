/* irc.c - funções que implementam os comandos do protocolo IRC 
   Evandro Fernandes Giovanini 
 */

/* Usamos três estruturas especiais para armazenar dados (definidas em irc.h):
   User - lista ligada de usuários, cada célula contém sockfd e o nickname
   Channel_users - lista ligada de usuários. Cada canal tem a sua lista. 
   Channel - estrutura que contem o nome do canal e sua Channel_users.
   
   Estas estruturas são inicializadas em na função main, em server.c.
   
   As funções deste arquivos iniciadas com "_" fazem manuseio destas listas. 
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "irc.h"

#define MAXLINE        4096

void
_Channel_users_add(int sock, Channel_users *p){
	Channel_users *new;
	new = malloc(sizeof(Channel_users));
	new->sockfd = sock;
	new->prox = p->prox;
	p->prox = new;
}


void
_Channel_users_remove (int sock, Channel_users *ch)
{
   Channel_users *p, *q;
   p = ch;
   q = ch->prox;
   while (q != NULL && q->sockfd != sock) {
      p = q;
      q = q->prox;
   }
   if (q != NULL) {
      p->prox = q->prox;
      free (q);
   }
}


Channel_users* 
_Channel_users_search_by_sock (int sock, Channel_users *ch)
{
	Channel_users *p;

	p = ch->prox;
	while (p != NULL && sock != p->sockfd){
		p = p->prox;
	}
  	return p;
}


void
_User_insere (int sock, char *name, User *p)
{
   User *nova;
   nova = malloc (sizeof (User));
   nova->sockfd = sock;
   //printf("nick tamanho: %d\n", strlen(name));
   nova->nickname = malloc(strlen(name)*sizeof(char));
   strcpy (nova->nickname, name);
   nova->prox = p->prox;
   p->prox = nova;
}


User*
_User_search_by_name (char *nome, User *ini)
{
	User *p;

	p = ini->prox;
	while (p != NULL && strcmp(nome, p->nickname) != 0){
	//printf("Nao é %s (%d vs %d)\n", p->nickname, strlen(p->nickname), strlen(nome));
		p = p->prox;
	}
  	return p;
}


User *
_User_search_by_sock (int sock, User *ini)
{
	User *p;

	p = ini->prox;
	while (p != NULL && sock != p->sockfd){
		p = p->prox;
	}
  	return p;
}

void
_User_search_and_remove (int sock, User *ini)
{
   User *p, *q;
   p = ini;
   q = ini->prox;
   while (q != NULL && q->sockfd != sock) {
      p = q;
      q = q->prox;
   }
   if (q != NULL) {
      p->prox = q->prox;
      free (q);
   }
}


int macdata(int connfd){
	char saida[MAXLINE + 1];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(saida, sizeof(saida)-1, "%d/%m/%Y\n", t);
	printf("[Cliente no socket %d recebeu MACDATA]: %s", connfd, saida);
	write(connfd, saida, strlen(saida));
	memset(saida,0,strlen(saida));
	return 0;
}


int machora(int connfd){
	char saida[MAXLINE + 1];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(saida, sizeof(saida)-1, "%H:%M:%S-%Z\n", t);
	printf("[Cliente no socket %d recebeu MACHORA]: %s", connfd, saida);
	write(connfd, saida, strlen(saida));
	memset(saida,0,strlen(saida));
	return 0;
}


int quit(int connfd, char *recvline){
	printf("[Cliente no socket %d saiu]: %s", connfd, recvline + strlen("QUIT :"));
    _User_search_and_remove(connfd, ini);
    _Channel_users_remove (connfd, ch1u);
    _Channel_users_remove (connfd, ch2u);
	if (close(connfd) < 0){
		perror("Erro ao fechar conexão.\n");
	}
	return 0;
}


int user(int connfd, char *recvline){
	printf("Comando USER\n");
	return 0;
}


int ping(int connfd, char *recvline){

  	char bufprefixed[MAXLINE + 128];
  	User *u;
  	
  	/* prepara primeira parte da resposta */
  	u = _User_search_by_sock(connfd, ini);
 	strcpy (bufprefixed, ":");
  	strcat (bufprefixed, u->nickname);
  	strcat (bufprefixed, " ");

	/* muda I por O. PING->PONG */
  	recvline[1] = 79; 
	
	printf("[PING no socket %d]: %s", connfd, recvline);
  	strcat (bufprefixed, recvline);
	write(connfd, bufprefixed, strlen(bufprefixed));
	return 0;
}


int join(int sockfd, char *buf){
	Channel ch;
	Channel_users *clist;
	User *dest, *u;
	char *msg;
	char channelname[10];
	msg = malloc(2*MAXLINE*sizeof(char));
	strcpy(msg, buf + 5);
	
	if (strncmp(msg, "#irchacks", strlen("#irchacks")) == 0){
		ch = ch1;
		clist = ch1u;
		strcpy(channelname, "#irchacks");
	}
	else if (strncmp(msg, "#testes", strlen("#testes")) == 0){
		ch = ch2;
		clist = ch2u;
		strcpy(channelname, "#testes");
	}
	else{
		printf("Canal não existe.\n");
		ch = ch1;
		clist = ch1u;
	}
	
	_Channel_users_add(sockfd, ch.list);
	
	printf("[Cliente no socket %d entrou na sala ]: %s\n", sockfd, ch.name);

	/* dá resposta ao cliente para indicar sucesso */
	dest = _User_search_by_sock(sockfd, ini);
	strcpy(msg, ":");
	strcat(msg, dest->nickname);
	strcat(msg, " ");
	strcat(msg, buf);
	write(sockfd, msg, strlen(msg));
	
	strcpy(msg, ":server 331 ");
	strcat(msg, dest->nickname);
	strcat(msg, " ");
	strcat(msg, ch.name);
	strcat(msg, " :No topic is set\n");
	write(sockfd, msg, strlen(msg));

	while (clist != NULL){
		if (clist->sockfd > 0){
			//printf("Enviando para %d\n", clist->sockfd);
			strcpy(msg, ":server 353 ");
			strcat(msg, dest->nickname);
			strcat(msg, " = ");
			strcat(msg, channelname);
			strcat(msg, " :");
			u = _User_search_by_sock(clist->sockfd, ini);
			strcat(msg, u->nickname);
			strcat(msg, "\n");
			write(sockfd, msg, strlen(msg));
		}
			clist = clist->prox;
	}
	
	strcpy(msg, ":server 366 ");
	strcat(msg, dest->nickname);
	strcat(msg, " ");
	strcat(msg, ch.name);
	strcat(msg, " :End of /NAMES list\n");
	write(sockfd, msg, strlen(msg));
	
	free(msg);
	return 0;
}


int part(int sockfd, char *buf){
	Channel ch;
	Channel_users *clist;
	char *partall;
	User *u;
	partall = malloc(MAXLINE*sizeof(char));

	char *msg;
	msg = malloc(2*MAXLINE*sizeof(char));
	strcpy(msg, buf + 5);
	
	if (strncmp(msg, "#irchacks", strlen("#irchacks")) == 0){
		ch = ch1;
		clist = ch1u;
	}
	else if (strncmp(msg, "#testes", strlen("#testes")) == 0){
		ch = ch2;
		clist = ch2u;
	}

	// se pediu para sair das duas salas usar um truque:
	// -sai de ch1 nessa chamada
	// -faz chamada nova para sair da ch2
	else if (strncmp(msg, "all", strlen("all")) == 0){
		ch = ch1;
		clist = ch1u; 
		strcpy(partall, "PART #testes");
		part(sockfd, partall); 
	}

	/* remove usuario da lista */
	_Channel_users_add(sockfd, ch.list);
	printf("[Cliente no socket %d saiu da sala ]: %s\n", sockfd, ch.name);

	u = _User_search_by_sock(sockfd, ini);
	/* manda mensagem para clientes */
	strcpy(msg, ":");
	strcat(msg, u->nickname);
	strcat(msg, " ");
	strcat(msg, buf);
	
	write(sockfd, msg, strlen(msg));
	
	while (clist != NULL){
		if (clist->sockfd > 0 && clist->sockfd != sockfd){
			printf("Enviando para %d\n", clist->sockfd);
			write(clist->sockfd, msg, strlen(msg));
		}
			clist = clist->prox;
	}
	free(msg);
	free(partall);
	return 0;
}


int list(int sockfd, char *buf){
	char *name;
	name = malloc(MAXLINE*sizeof(char));
	
	strcpy (name, ch1.name);
	strcat (name, "\n");
	write(sockfd, name, strlen(name));
	
	strcpy (name, ch2.name);
	strcat (name, "\n");
	write(sockfd, name, strlen(name));
	
	write(sockfd, "End of LIST\n", strlen("End of LIST\n"));
	free(name);
	return 0;
}


int privmsg_user (char *remetente, char *recvline){
	User *dest;
	char *dest_nick;
	char *msg;

	dest_nick = malloc (MAXLINE*sizeof(char));
	msg = malloc(MAXLINE*sizeof(char));
	
  	// copia entrada para uma string que vamos editar, já eliminando "PRIVMSG "
	strcpy(msg, recvline + 8);

	// descobre o destinatário da mensagem (canal ou usuário)
	msg = strtok(msg, " :"); // pega o nick
	strcpy(dest_nick, msg);

	dest = _User_search_by_name(dest_nick, ini);
	if (dest == NULL){
		printf("Usuário não encontrado.\n");
	}
	else{
		printf("Recipiente tem socket %d\n", dest->sockfd);
		strcpy(msg, ":");
		strcat(msg, remetente);
		strcat(msg, " ");
		strcat(msg, recvline);
		printf("Enviar mensagem: %s\n", msg);
		write(dest->sockfd, msg, strlen(msg));
	}
	free(msg);
	free(dest_nick);
	return 0;

}


int privmsg_channel(int sockfd, char *remetente, char *recvline){
	Channel_users *clist;
	char *msg;
	
	msg = malloc(MAXLINE*sizeof(char));
	
	strcpy(msg, ":");
	strcat(msg, remetente);
	strcat(msg, " ");
	strcat(msg, recvline);

	if (strncmp(recvline+8, "#irchacks", strlen("#irchacks")) == 0)
		clist = ch1u;
	else if (strncmp(recvline+8, "#testes", strlen("#testes")) == 0)
		clist = ch2u;
	
	while (clist != NULL){
		if (clist->sockfd > 0 && clist->sockfd != sockfd){
			printf("Enviando para %d\n", clist->sockfd);
			write(clist->sockfd, msg, strlen(msg));
		}
			clist = clist->prox;
	}
	
	free(msg);
	
	return 0;
}


int privmsg(int connfd, char *recvline){

    User *sender;
	char remetente[9];
	
	sender = _User_search_by_sock(connfd, ini);
	strcpy (remetente, sender->nickname);

  	if (recvline[8] == '#')
		privmsg_channel (connfd, remetente, recvline);
  	else
		privmsg_user (remetente, recvline);
	
	return 0;
}

char * fix_chars_in_nick(char *str){
  int i = 0;
  int j = 0;
  char *c;
  c = malloc(MAXLINE*sizeof(char));

  while (str[i] != '\0'){
    if (str[i] > 59 && str[i] < 173){
      c[j] = str[i];
      j++;
    }
	else
	  break;
    i++;
  }
  c[j] = '\0';
  memset(str,0,strlen(str));
  strcpy(str, c);
  free (c);
  return str;
}


int nick(int connfd, char *recvline){

  	char nick[MAXLINE];

  	strcpy(nick, recvline);
	strcpy(nick, nick + 5);
    fix_chars_in_nick(nick); // tira caracters invalidos do nick

	_User_search_and_remove(connfd, ini);
	_User_insere(connfd, nick, ini);
	printf("[Cliente no socket %d definiu o NICK]: %s", connfd, nick);
    printf("\n\n"); /* sem isso o programa para. Não me pergunte o porquê. */
    
	welcome(nick, connfd);
	
	return 0;
}


int welcome(char *nickname, int connfd){
	char msg[MAXLINE];
	
	strcpy(msg, ":server 001 ");
	strcat(msg, nickname);
	strcat(msg, " :Bem vindo ao servidor.\n");
	write(connfd, msg, strlen(msg));
	
	strcpy(msg, ":server 004 ");
	strcat(msg, nickname);
	strcat(msg, " server\n");
	write(connfd, msg, strlen(msg));
	
	strcpy(msg, ":server 375 ");
	strcat(msg, nickname);
	strcat(msg, " :- Mensagem do Dia -\n");
	write(connfd, msg, strlen(msg));
	
	strcpy(msg, ":server 372 ");
	strcat(msg, nickname);
	strcat(msg, " :- Bem vindo. Os canais #irchacks e #testes estão disponíveis.\n");
	write(connfd, msg, strlen(msg));
	
	strcpy(msg, ":server 376 ");
	strcat(msg, nickname);
	strcat(msg, " :End of /MOTD command.\n");
	write(connfd, msg, strlen(msg));
	
	return 0;
	
}

