CFLAGS = -Wall -g -lpthread

LIBS = irc.c

all:	server	

server:	server.c irc.c irc.h
	gcc ${CFLAGS} -o server server.c ${LIBS}

clean:
	rm -f ${PROGS} ${CLEANFILES}
