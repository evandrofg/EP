CFLAGS= -O2 `pkg-config glib-2.0 --cflags --libs`
CC=gcc

all : menu-shell 

menu-shell : filemanager.o
gui : filemanager.o

clean: 
	rm -rf *.o *~ menu-shell gui

