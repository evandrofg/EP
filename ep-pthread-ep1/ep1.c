/* EP1 MAC438 Programação Concorrente
 * Evandro Fernandes Giovanini */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#define PISTA   160
#define BOXE    10


typedef struct {
    int num;    /* numero do piloto */
    int v;      /* velocidade */
    int pos;    /* posicao do piloto na pista 0 e 159 */
    int pista;  /* em qual lado da pista o piloto está, 0 ou 1 */
    int volta;  /* volta atual do piloto */
    int class;  /* classificacao do piloto na corrida */
    int pontos; /* pontuação do piloto no campeonato */
} Piloto;
typedef struct Piloto *piloto;

typedef struct {
    int num;    /* numero da equipe */
    int pontos; /* pontuação da equipe */
} Equipe;

int n;  /* número de voltas */
int m;  /* número de equipes */
int k;  /* porcentagem de redução de velocidade */
char V; /* velocidade constante ou nao */

int *boxe;
int **pista;

/* vetor que armazena pilotos e equipes participantes */
Piloto *pilotos;
Equipe *equipes;

sem_t semfim[100]; /* semáforo para controlar o fim de cada thread */
pthread_mutex_t mutexpos; /* mutex para travar mudanças na pista */

void init(){
    int i;

    pilotos = malloc ( ((m*2) + 1) * sizeof(Piloto));

    equipes = malloc (m * sizeof(Equipe));

    /* Cada posição pista vale -1 (livre), -2 (não é pista), ou
     *  0-2*m (número do piloto ocupando aquele trecho).
     *
     * A pista tem dois lados, representados por pista[0] e pista[1].
     * pista[0] é sempre válido, pista[1] pode ser pista (-1) ou não (-2)
     * Inicialmente vamos assumir que sempre vale -2, e vamos ler os trechos
     * onde é válido (-1) ao ler a entrada do usuário.
     */
    pista = malloc (2 * sizeof(int*));
    pista[0] = malloc (PISTA * sizeof(int));
    pista[1] = malloc (PISTA * sizeof(int));
    for (i = 0; i < PISTA; i++){
        pista[0][i] = -1;
        pista[1][i] = -2;
    }

    /*boxe = malloc(BOXE * sizeof(int));*/
}

void clean(){
    int i;
    for (i = 0; i < m*2; i++){
        sem_destroy(&semfim[i]);
    }
    pthread_mutex_destroy(&mutexpos);
    free(boxe);
    free(pista[0]);
    free(pista[1]);
    free(pista);
}

void cria_pilotos(){
    int i;
    /* Cria os pilotos e os posiciona no grid */
    for (i = 0; i < 2*m; i++){
        pilotos[i].v = 50;
        pilotos[i].num = i;
        pilotos[i].pos = i/2;
        pilotos[i].pista = i%2;
        pilotos[i].volta = 0;
        pilotos[i].class = 2*m - i;
        pilotos[i].pontos = 0;
        pista[pilotos[i].pista][pilotos[i].pos] = pilotos[i].num;
        equipes[i/2].num = pilotos[i].num/2;
    }
}

/* Altera a classificação entre dois pilotos, usado em ultrapassagens */
void troca_class(int i, int j){
    int temp;
    //printf("%d ultrapassou %d\n", i, j);
    temp = pilotos[i].class;
    pilotos[i].class = pilotos[j].class;
    pilotos[j].class = temp;
}

/* thread de cada piloto */
void *PilotoThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    Piloto *p = &pilotos[tid];

    int ctrl;
    int r;
    int pos_atual;
    int pos_next;
    int pos_nextplus;
    int pos_final;

    while (1){
        pthread_mutex_lock(&mutexpos);
        ctrl = 0;

        pos_atual = p->pos % 160;
        pos_next = (pos_atual + 1) % 160;
        pos_nextplus = (pos_atual + 2) % 160;
        pos_final = pos_atual;

        /* tenta avançar dois trechos
         * (pode ter no máximo um piloto na frente -> ultrapassagem) */
        if (p->v == 50 && ctrl == 0){
            if (!(pista[0][pos_next] >= 0 && pista[1][pos_next] >= 0)){
                if (pista[0][pos_nextplus] == -1)
                {
                    pista[0][pos_nextplus] = p->num;
                    pista[p->pista][pos_atual] = -1;
                    p->pos = pos_nextplus;
                    pos_final = pos_nextplus;
                    p->pista = 0;
                    /* ultrapassou alguem?*/
                    if (pista[0][pos_next] >= 0){
                        if (pilotos[pista[0][pos_next]].class > p->class){
                            troca_class(p->num, pista[0][pos_next]);
                        }
                    }
                    else if (pista[1][pos_next] >= 0){
                        if (pilotos[pista[1][pos_next]].class > p->class){
                            troca_class(p->num, pista[1][pos_next]);
                        }
                    }
                    ctrl = 1;
                }
                else if (pista[1][pos_nextplus] == -1)
                {
                    pista[1][pos_nextplus] = p->num;
                    pista[p->pista][pos_atual] = -1;
                    p->pos = pos_nextplus;
                    pos_final = pos_nextplus;
                    p->pista = 1;
                    if (pista[0][pos_next] >= 0){
                        if (pilotos[pista[0][pos_next]].class > p->class){
                            troca_class(p->num, pista[0][pos_next]);
                        }
                    }
                    else if (pista[1][pos_next] >= 0){
                        if (pilotos[pista[1][pos_next]].class > p->class){
                            troca_class(p->num, pista[1][pos_next]);
                        }
                    }
                    ctrl = 1;
                }
            }

        }
        /* tenta avançar uma posição */
        if (ctrl == 0){
            // primeiro, tenta na pista 0
            if (pista[0][pos_next] == -1){
                pista[0][pos_next] = p->num;
                pista[p->pista][pos_atual] = -1;
                p->pos = pos_next;
                p->pista = 0;
                pos_final = pos_next;
                ctrl = 1;
            }
            // depois, tenta do outro lado, na pista 1
            else if (pista[1][pos_next] == -1){
                pista[1][pos_next] = p->num;
                pista[p->pista][pos_atual] = -1;
                p->pos = pos_next;
                p->pista = 1;
                pos_final = pos_next;
                ctrl = 1;
            }
        }
        if (pos_final < pos_atual){
            pilotos[tid].volta++;
            printf("Piloto %d está na volta %d, posição %d na corrida.\n", p->num, p-> volta, p->class);
            /* Vamos diminuir a velocidade?  */
            if (V == 'A')
                if (p->volta == n-10){
                    r = rand() % 100;
                    if (r < k){
                        p->v = 25;
                        printf("%d diminuiu a velocidade.\n", p->num);
                    }
                }
        }
        /* acabou a corrida? sai da pista para nao atrapalhar os outros */
        if (p->volta >= n){
            pista[p->pista][p->pos] = -1;
            pthread_mutex_unlock(&mutexpos);
            sem_post(&semfim[p->num]);
            pthread_exit(NULL);
        }

        pthread_mutex_unlock(&mutexpos);

        sleep(1);
    }
}

/* swap e randomize são funções usadas por simula_corridas() */
void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize(int *aux, int n){
    int i, j;

    for (i = 0; i < n; i++){
        j = rand() % (n - i + 1);
        swap(&aux[i], &aux[j]);
    }

}

void simula_corridas(){
    int *aux;
    int i;

    aux = malloc(2*m*sizeof(int));
    for (i = 0; i < 2*m; i++)
        aux[i] = i;

    /* Vamos simular 10 corridas */
    for (i = 0; i < 10; i++){
        randomize(aux, 2*m);
        pilotos[aux[0]].pontos += 25;
        pilotos[aux[1]].pontos += 18;
        pilotos[aux[2]].pontos += 15;
        pilotos[aux[3]].pontos += 12;
        pilotos[aux[4]].pontos += 10;
        pilotos[aux[5]].pontos += 8;
        pilotos[aux[6]].pontos += 6;
        pilotos[aux[7]].pontos += 4;
        pilotos[aux[8]].pontos += 2;
        pilotos[aux[9]].pontos += 1;

        equipes[aux[0]/2].pontos += 25;
        equipes[aux[1]/2].pontos += 18;
        equipes[aux[2]/2].pontos += 15;
        equipes[aux[3]/2].pontos += 12;
        equipes[aux[4]/2].pontos += 10;
        equipes[aux[5]/2].pontos += 8;
        equipes[aux[6]/2].pontos += 6;
        equipes[aux[7]/2].pontos += 4;
        equipes[aux[8]/2].pontos += 2;
        equipes[aux[9]/2].pontos += 1;
    }
}

/* Funções de comparação usadas no qsort
 * http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_15.html
 */
int comp_pilotos (const void * a, const void * b){
    Piloto *aa = (Piloto*)a;
    Piloto *bb = (Piloto*)b;

    if (aa->class < bb->class)
        return -1;
    if (aa->class >= bb->class)
        return 1;
    return 0;
}

int comp_pilotos_pontos (const void * a, const void * b){
    Piloto *aa = (Piloto*)a;
    Piloto *bb = (Piloto*)b;

    if (aa->pontos > bb->pontos)
        return -1;
    if (aa->pontos < bb->pontos)
        return 1;
    return 0;
}

int comp_equipes_pontos  (const void * a, const void * b){
    Equipe *aa = (Equipe*)a;
    Equipe *bb = (Equipe*)b;

    if (aa->pontos > bb->pontos)
        return -1;
    if (aa->pontos <= bb->pontos)
        return 1;
    return 0;
}

/* soma os pontos ganhos a pilotos e equipes apos a corrida */
void pontuacao(){
    int i;

    /* ordena o vetor de acordo com resultado da corrida */
    qsort(pilotos, 2*m, sizeof(Piloto), comp_pilotos);

    /* soma pontos aos pilotos */
    if (m >= 1){
        pilotos[0].pontos += 25;
        pilotos[1].pontos += 18;
        equipes[pilotos[0].num/2].pontos += 25;
        equipes[pilotos[1].num/2].pontos += 18;
    }
    if (m >= 2){
        pilotos[2].pontos += 15;
        pilotos[3].pontos += 12;
        equipes[pilotos[2].num/2].pontos += 15;
        equipes[pilotos[3].num/2].pontos += 12;
    }
    if (m >= 3){
        pilotos[4].pontos += 10;
        pilotos[5].pontos += 8;
        equipes[pilotos[4].num/2].pontos += 10;
        equipes[pilotos[5].num/2].pontos += 8;
    }
    if (m >= 4){
        pilotos[6].pontos += 6;
        pilotos[7].pontos += 4;
        equipes[pilotos[6].num/2].pontos += 6;
        equipes[pilotos[7].num/2].pontos += 4;
    }
    if (m >= 5){
        pilotos[8].pontos += 2;
        pilotos[9].pontos += 1;
        equipes[pilotos[8].num/2].pontos += 2;
        equipes[pilotos[9].num/2].pontos += 1;
    }


    /* ordena pilotos e equipes de acordo com a pontuacao no campeonato */
    qsort(pilotos, 2*m, sizeof(Piloto), comp_pilotos_pontos);
    qsort(equipes, m, sizeof(Equipe), comp_equipes_pontos);


    printf("Classificação de Pilotos: \n");
    for (i = 0; i < 2*m; i++)
        printf("%2d. (%2d pontos)  Piloto %2d da Equipe %2d\n", i+1, pilotos[i].pontos, pilotos[i].num, (pilotos[i].num / 2));

    printf("\n Classificação de Equipes: \n");
    for (i = 0; i < m; i++)
        printf("%2d. (%2d pontos)  Equipe %2d\n", i+1, equipes[i].pontos, equipes[i].num);
}


int main (int argc, char *argv[])
{
    int i, ini, fim, rc;
    FILE *in;

    printf("Bem vindo ao EP1 de MAC438\n");
    /* lê a entrada */
    if (argc > 2){
        fprintf(stderr, "Erro: Formato de entrada: ./a.out.\n");
        return -1;
    }
    in = fopen(argv[1], "r");
    if (in == NULL){
        fprintf(stderr, "Erro: Arquivo '%s' não pode ser lido.\n", argv[1]);
        return -1;
    }
    fscanf(in, "%d", &n);
    fscanf(in, "%d", &m);
    fscanf(in, "%1c", &V);
    do V = fgetc(in); while(V == '\n');
    if (V == 'A')
        fscanf(in, "%d", &k);
    init();
    while (fscanf(in, "%d", &ini) != EOF){
        fscanf(in, "%d", &fim);
        for (i = ini; i <= fim; i++)
            pista[1][i] = -1;
    }

    /* Inicializa e prepara a simulação */
    cria_pilotos();
    srand(time(NULL));
    simula_corridas();


    pthread_t threads[2*m];

    for (i = 0; i < m*2; i++)
        sem_init(&semfim[i], 0, 0);
    printf("Foi dada a largada...\n");
    for(i=0; i < 2*m; i++){
        rc = pthread_create(&threads[i], NULL, PilotoThread, (void *)(intptr_t)i);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Terminou a corrida? */
    for (i = 0; i < 2*m; i++){
        sem_wait(&semfim[i]);
        if (pilotos[i].v == 25) printf("%d\n", i);
    }

    pontuacao();
    clean();
    pthread_exit(NULL);
}
