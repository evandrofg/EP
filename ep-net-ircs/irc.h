/* irc.h - funções que implementam os comandos do protocolo IRC 
   Evandro Fernandes Giovanini
 */

struct usr {
      int sockfd;
      char * nickname;
      struct usr *prox;
   };
typedef struct usr User;  // célula

struct usr_lst {
	int sockfd;
	struct usr_lst *prox;
};
typedef struct usr_lst Channel_users;

struct channel {
	char *name;
	Channel_users *list;
};
typedef struct channel Channel;


User *ini;
Channel ch1;
Channel_users *ch1u;
Channel ch2;
Channel_users *ch2u;


int Tcp_listen(const char *, const char *, socklen_t *);

int macdata(int connfd);

int machora(int connfd);

int welcome (char *nickname, int connfd);

int quit(int connfd, char *recvline);

int nick(int connfd, char *recvline);

int join(int sockfd, char *buf);

int part(int sockfd, char *buf);

int list(int sockfd, char *buf);

int privmsg(int connfd, char *recvline);

int user(int connfd, char *recvline);

int ping(int connfd, char *recvline);
