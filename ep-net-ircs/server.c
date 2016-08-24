/* Mini servidor de IRC. 
   Evandro Fernandes Giovanini
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
#define LISTENQ        1


/* copiado do UNP */
int
tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int				listenfd, n;
	const int		on = 1;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		printf("tcp_listen error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;		/* error, try next one */

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		close(listenfd);	/* bind error, close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)	/* errno from final socket() or bind() */
		printf("tcp_listen error for %s, %s", host, serv);

	listen(listenfd, LISTENQ);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;	/* return size of protocol address */

	freeaddrinfo(ressave);

	return(listenfd);
}
/* end tcp_listen */

int
Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	return(tcp_listen(host, serv, addrlenp));
}

/* loop que processa requisições dos clientes
 * cada cliente tem sua thread que fica rodando isso
 */
void proc_commands(int sockfd)
{
        ssize_t         n;
        char            buf[MAXLINE];
        printf("[Nova conexão iniciada no socket]: %d\n", sockfd);

again:
        while ( (n = read(sockfd, buf, MAXLINE)) > 0){

			if (strncmp(buf, "QUIT", strlen("QUIT")) == 0 )
				quit(sockfd, buf);
			else if (strncmp(buf, "MACDATA", strlen("MACDATA")) == 0 )
				macdata(sockfd);
			else if (strncmp(buf, "MACHORA", strlen("MACHORA")) == 0 )
				machora(sockfd);
			else if (strncmp(buf, "NICK", strlen("NICK")) == 0 ){
				nick(sockfd, buf);
			}
			else if (strncmp(buf, "USER", strlen("USER")) == 0 ){
				user(sockfd, buf);
			}
			else if (strncmp(buf, "PING", strlen("PING")) == 0 ){
				ping(sockfd, buf);
			}
			else if (strncmp(buf, "JOIN", strlen("JOIN")) == 0){
				join(sockfd, buf);
			}
			else if (strncmp(buf, "PART", strlen("PART")) == 0){
				part(sockfd, buf);
			}
			else if (strncmp(buf, "LIST", strlen("LIST")) == 0){
				list(sockfd, buf);
			}
			else if (strncmp(buf, "PRIVMSG", strlen("PRIVMSG")) == 0){
				printf("[PRIVMSG do socket %d]\n", sockfd);
				privmsg(sockfd, buf);
			}
			else{
				printf("[echo no socket %d]: %s", sockfd, buf);
				write(sockfd, buf, strlen(buf));
			}
			memset(buf,0,strlen(buf));

		}

		if (n < 0 && errno == EINTR)
			goto again;
}

static void *doit(void *arg)
{
	pthread_detach(pthread_self());
	proc_commands((intptr_t) arg);	/* same function as before */
	close((intptr_t) arg);		/* done with connected socket */
	return(NULL);
}

int
main(int argc, char **argv)
{
	intptr_t		listenfd, connfd;
	pthread_t		tid;
	socklen_t		addrlen, len;
	struct sockaddr	*cliaddr;

	/* inicializa lista de conexoes/nicks */
	ini = malloc (sizeof(User));
	ini->prox = NULL;
  
	/* inicializa os dois canais */
	ch1u = malloc(sizeof(Channel_users));
	ch1u->prox = NULL;
	ch1.name = malloc (512*sizeof(char));
	ch1.list = ch1u;
	ch1.name = "#irchacks";
  
	ch2u = malloc(sizeof(Channel_users));
	ch2u->prox = NULL;
	ch2.name = malloc (512*sizeof(char));
	ch2.list = ch2u;
	ch2.name = "#testes";
	
	printf("Bem vindo ao mini servidor de IRC. \nOs canais disponíveis são #irchacks e #testes.\n");


    if (argc == 1)
	  	listenfd = Tcp_listen(NULL, "6667", &addrlen);
	else if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else
		perror("uso: server <porta>"); //err_quit

	cliaddr = malloc(addrlen);

	for ( ; ; ) {
		len = addrlen;
		connfd = accept(listenfd, cliaddr, &len);
		pthread_create(&tid, NULL, &doit, (void *) connfd);
	}
}


