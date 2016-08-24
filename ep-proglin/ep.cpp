/*  
 * Evandro Fernandes Giovanini  nUSP 5173890 efgiovanini@gmail.com
 * EP de MAC315
 * 
 * As funções de grafo foram reaproveitadas da disciplina 328.
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "GRAPHutils.h"

#include <iostream>
#include <fstream>
#include <vector>

#define maxV 1000

using namespace std;
using std::vector;



// parâmetros do programa
int N;
int no_origem;
int no_destino;
int produto;

// estruturas de dados usadas pelo simplex
std::vector< std::vector<int> > custos; // matrix estática, guarda todos os custos
std::vector< std::vector<int> > basicas; // arestas na arvore
std::vector< std::vector<int> > naobasicas; // arestas fora da arvore
std::vector< int> parcial; // custos parciais
std::vector< int> pai; // para cada vertice, guarda o vertice que leva a ele com menor custo

Digraph grafo;
int custoMax;

/* Funções imprime_* foram usadas para depuração */
int imprime_custos(){
    int i, j, k;
    printf("custos:\n");
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (custos[i][j] != -1)
                printf("%2d ", custos[i][j]);
            else
                printf(" * ");
        }
        printf("\n");
    }
}

int imprime_basicas(){
    int i, j, k;
    printf("básicas:\n");
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (basicas[i][j] != -1)
                printf("%2d ", basicas[i][j]);
            else
                printf(" * ");
        }
        printf("\n");
    }

    printf("\nnão básicas:\n");
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (naobasicas[i][j] != -1)
                printf("%2d ", naobasicas[i][j]);
            else
                printf(" * ");
        }
        printf("\n");
    }
}

void imprime_parcial(){
    int i;
    printf("custos parciais:\n");
    for (i = 0; i < N; i++)
        printf("%d ", parcial[i]); 
    printf("\n");
    printf("pai\n ");
    for (i = 0; i < N; i++)
        printf("%d ", pai[i]); 
    printf("\n");
    
}

/* imprime o menor caminho da origem até o vértice v encontrado pelo simplex */
void imprime_caminho(int v){
    if (pai[v] == no_origem)
        printf("%d  %d ", pai[v], v);
    else{
            imprime_caminho(pai[v]);
            printf(" %d ", v);
        }
}

int simplex(){
    int i, j;
    int status = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++){
            if (naobasicas[i][j] != 0){
                if (parcial[j] > parcial[i] + custos[i][j]){
                    // entra uma nao basica, sai a aresta anterior
                    basicas[pai[j]][j] = 0;
                    parcial[j] = parcial[i] + custos[i][j];
                    pai[j] = i; 
                    status = 1;
                }
            }
        }
    return status;
}

int main(int argc, char* argv[]){
    ifstream arq;
    int i, j, k;
    custoMax = 0;

    if (argc <= 1){
        printf("Argumento inválido. Use ./ep <arquivo de entrada>\n");
        printf("Por exemplo: ./ep problema.dat\n");
        return 0;
    }

    arq.open(argv[1]);
    if ( !arq.good() ){
        cout << "Erro ao abrir arquivo " << argv[1] << "\n";
        return -1;
    }

    arq >> N;
    arq >> no_origem;
    arq >> no_destino;
    arq >> produto;

    grafo = DIGRAPHinit (N);

    custos.resize(N);
    basicas.resize(N);
    naobasicas.resize(N);
    parcial.resize(N);
    pai.resize(N);
    for (i = 0; i < N; i++){
        custos[i].resize(N);
        basicas[i].resize(N);
        naobasicas[i].resize(N);
    }
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++){
            custos[i][j] = -1;
            basicas[i][j] = 0;
            naobasicas[i][j] = 0;
        }

    while (arq.good () == true){
        arq >> i;
        arq >> j;
        arq >> k;
        custos[i][j] = k;
        custoMax += k;
        DIGRAPHinsertA (grafo, i, j, k);
    }

    arq.close();
    for (i = 0; i < N; i++){
        parcial[i] = custoMax + 1;
    }
    parcial[no_origem] = 0;


    /* coloca as arestas dadas que não estão na árvore inicial na matrix naobasicas */
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++){
            if (basicas[i][j] == 0 && custos[i][j] != -1){
                naobasicas[i][j] = 1;
            }
        }

    /* agora temos uma arvore representada em basicas.
       as arestas que nao estao nessa arvore em naobasicas.
       vamos chamar o simplex para inserir as arestas otimas na arvore. */
    
/* Depuracao
    printf("ANTES DO SIMPLEX\n");
    imprime_basicas();

    imprime_parcial();
*/
    // roda o simplex até ele não mudar mais o grafo
    i = 1;
    while (i)
        i = simplex();

/* Depuração
    printf("DEPOIS DO SIMPLEX\n");
    imprime_basicas();

    imprime_parcial();
*/

    /* Depois de executado o simplex, mostrar o resultado */
    if (parcial[no_destino] == custoMax + 1)
        printf("Não existe caminho.\n");
    else
    {
        printf("caminho mínimo: \n");
        imprime_caminho(no_destino); printf("\n");

        printf("custo mínimo: ");
        printf("%d\n", parcial[no_destino]);
    }

}
