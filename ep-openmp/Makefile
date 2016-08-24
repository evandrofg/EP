CFLAGS = -Wall  -lm -fopenmp -O3

ondas:	ondas.c
	gcc ${CFLAGS} ondas.c -o ondas 

run: ondas
	./ondas in2.txt
	xdg-open in2.txt.ppm

clean:
	rm -f ondas
