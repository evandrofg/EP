/* Tarefa 1 de MAC328 - 28/Fev/2013
 * Evandro Fernandes Giovanini nUSP 5173890 
 * 
 * Arquivo GRAPHutils.h: headers de funções padrão de manipulação de grafos.
 * Site do curso: http://www.ime.usp.br/~pf/algoritmos_para_grafos
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#define Vertex int

#define Edge Arc

#define EDGE ARC

#define graph digraph
#define Graph Digraph

#define GRAPHinit DIGRAPHinit
#define GRAPHshow DIGRAPHshow

typedef struct { 
	Vertex v; 
	Vertex w; 
} Arc;

Arc ARC (Vertex v, Vertex w);

struct digraph
{
	int V; 
	int A; 
	int **adj; 
};

typedef struct digraph *Digraph;

Digraph DIGRAPHinit (int V);

int **MATRIXint (int r, int c, int val) ;

void DIGRAPHinsertA (Digraph G, Vertex v, Vertex w, int cst);


void DIGRAPHremoveA (Digraph G, Vertex v, Vertex w);

void DIGRAPHshow (Digraph G);

void GRAPHinsertE( Graph G, Vertex v, Vertex w);


