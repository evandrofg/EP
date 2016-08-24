/* Tarefa 1 de MAC328 - 28/Fev/2013
 * Evandro Fernandes Giovanini nUSP 5173890 
 * 
 * Arquivo GRAPHutils.c: implementação de funções padrão de manipulação de grafos.
 * Site do curso: http://www.ime.usp.br/~pf/algoritmos_para_grafos
 * 
 */

#include "GRAPHutils.h"


/* A função ARC recebe dois vértices v e w e devolve um arco com ponta inicial v e ponta final w. */
Arc ARC (Vertex v, Vertex w)
{
	Arc e;
	e.v = v;
	e.w = w;
	return e;
}

/* Esta função devolve (o endereço de) um novo digrafo com vértices 0,..,V-1 e nenhum arco. */
Digraph DIGRAPHinit( int V)
{ 
	Digraph G = static_cast<Digraph>(malloc( sizeof *G));
	G->V = V; 
	G->A = 0;
	G->adj = MATRIXint( V, V, 0);
	return G;
}

/* A função abaixo aloca uma matriz com linhas 0..r-1 e colunas 0..c-1. Cada elemento da matriz recebe valor val. */
int **MATRIXint( int r, int c, int val)
{
	Vertex i, j;
	int **m = static_cast<int**>(malloc( r * sizeof (int *)));
	for (i = 0; i < r; i++)
		m[i] = static_cast<int*>(malloc( c * sizeof (int)));
	for (i = 0; i < r; i++)
		for (j = 0; j < c; j++)
		m[i][j] = val;
	return m;
}

/* Esta função insere um arco v-w no digrafo G. Se v == w ou o digrafo já tem arco v-w, a função não faz nada.  É claro que v e w não podem ser negativos e devem ser menores que G->V. */
void DIGRAPHinsertA( Digraph G, Vertex v, Vertex w, int cst) 
{ 
	if (v != w && G->adj[v][w] == 0) {
		G->adj[v][w] = cst; 
		G->A++;
	}
}

/* Esta função remove do digrafo G o arco que tem ponta inicial v e ponta final w.  Se não existe tal arco, a função nada faz. */
void DIGRAPHremoveA( Digraph G, Vertex v, Vertex w)
{ 
	if (G->adj[v][w] > 0) {
		G->adj[v][w] = 0;
		G->A--;
	}
}

/* Para cada vértice v do digrafo G, esta função imprime, em uma linha, todos os vértices adjacentes a v. */
void DIGRAPHshow( Digraph G)
{ 
	Vertex v, w; 
	for (v = 0; v < G->V; v++) {
		printf( "%2d:", v);
		for (w = 0; w < G->V; w++)
			if (G->adj[v][w] > 0) 
			printf( " %2d (%2d)", w, G->adj[v][w]);
		printf( "\n");
	}
}

