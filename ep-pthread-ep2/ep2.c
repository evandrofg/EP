/*
EP2 de MAC438
Evandro Fernandes Giovanini nUSP 5173890 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <gmp.h>

#include <unistd.h> // detecta numero de threads (Unix)

int pri;
char seg;
mpf_t ter;
int ter_len;
char qua;
bool continua = true;

int nthreads;
unsigned long n;
unsigned long cont;
mpf_t euler;
mpf_t euler_ant;

pthread_barrier_t barr1;
pthread_barrier_t barr2;
pthread_mutex_t mutex_e;


void calcula_termo(mpf_t result, mpz_t fatorial){
    mpf_set_z(result, fatorial);
    mpf_ui_div(result, 1.0, result);
}


void *Passo (void *arg){
    int rc;
    unsigned long i = 0;
    unsigned long j, last;
    unsigned long tid;
    tid = (unsigned long)arg;

    mpz_t fatorial;
    mpf_t termo;
    mpf_t delta;
    mpz_init_set_ui(fatorial, 1);
    mpf_init(termo);
    mpf_init(delta);

    last = 1;

    while (i < 2000){

        rc = pthread_barrier_wait(&barr1);
        if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) exit(-1);
        
        cont++;
        if (qua == 'd')
            printf("iteração %ld, thread %ld na barreira\n", i/nthreads, tid);

        /*
        calcula o fatorial do termo atual a partir do valor encontrado
        na iteração anterior
        */
        for (j = last+1; j <= tid + i; j++){
            mpz_mul_ui(fatorial, fatorial, j);
            last = j;
        }

        calcula_termo(termo, fatorial);                

        pthread_mutex_lock(&mutex_e);
        
        mpf_add(euler, euler, termo);

/*        if (qua == 'd'){
            gmp_printf("valor parcial de Euler %.*Ff \n", ter_len, euler);
            gmp_printf("termo %.*Ff\n", ter_len, termo);
        }
        if (seg == 'm'){
            if( mpf_cmp(termo, ter) <= 0){
            continua = false;}
        }
        if (seg == 'f' && i > 0){
            mpf_sub(delta, euler, euler_ant);
            if( mpf_cmp(delta,ter) < 0){
                continua = false;
            } 
        }*/

        mpf_set(euler_ant, euler);
        pthread_mutex_unlock(&mutex_e);

        i = i + nthreads;
        rc = pthread_barrier_wait(&barr2);
        if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) exit(-1);
    }
    mpz_clear(fatorial);
    mpf_clear(termo);
    mpf_clear(delta);
    pthread_exit(NULL);
}


void initMT(){
    int rc;
    unsigned long i;
    pthread_t threads[nthreads];

    printf("Usando %d threads\n", nthreads);  
    cont = 0;

    /* inicia barreiras */
    if(pthread_barrier_init(&barr1, NULL, nthreads)) exit (-1);
    if(pthread_barrier_init(&barr2, NULL, nthreads)) exit (-1);
    for(i = 0; i < nthreads; i++){
        rc = pthread_create(&threads[i], NULL, Passo, (void *)i);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for(i = 0; i < nthreads; i++)
    {
        if(pthread_join(threads[i], NULL))
        {
            printf("Could not join thread %lu\n", i);
            exit (-1);
        }
    }
    gmp_printf("As threads se encontraram %ld vezes.\n", cont/nthreads);
    gmp_printf("Euler: %.*Ff\n", ter_len, euler);
    pthread_exit(NULL);
}


void initST (){
    unsigned long i = 0;
    unsigned long j, last;
    unsigned long tid;
    tid = 0;
    last = 0;
    nthreads = 1;

    mpz_t fatorial;
    mpf_t termo;
    mpf_t delta;
    mpz_init_set_ui(fatorial, 1);
    mpf_init(termo);
    mpf_init(delta);

    while (i < 2000){
        
        cont++;

        /*
        calcula o fatorial do termo atual a partir do valor encontrado
        na iteração anterior
        */
        for (j = last+1; j <= tid + i; j++){
            mpz_mul_ui(fatorial, fatorial, j);
            last = j;
        }

        calcula_termo(termo, fatorial);                
        
        mpf_add(euler, euler, termo);
            
        /*if (seg == 'm'){
            if( mpf_cmp(termo, ter) <= 0){
            continua = false;}
        }
        if (seg == 'f' && i > 0){
            mpf_sub(delta, euler, euler_ant);
            if( mpf_cmp(delta,ter) < 0){
                continua = false;
            } 
        }*/

        mpf_set(euler_ant, euler);

        i = i + nthreads;
    }
    gmp_printf("Foram encontrados %ld termos.\n", i);
    gmp_printf("Euler: %.*Ff\n", ter_len, euler);
    mpz_clear(fatorial);
    mpf_clear(termo);
    mpf_clear(delta);
}


int main(int argc, char * argv[]){
    pri = atoi(argv[1]);
    seg = argv[2][0];

    mpf_init_set_str(ter, argv[3], 10);
    ter_len = strlen(argv[3]);
    
    if (argc >= 5)
        qua = argv[4][0];

    if (pri == 0){
        // valor padrão para não-Linux
        nthreads = 2;
        // descobre o número de cores disponíveis (Linux)
#ifdef _SC_NPROCESSORS_ONLN
        nthreads = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    }
    else
        nthreads = pri;

    mpf_set_default_prec(ter_len*ter_len);
    mpf_init_set_ui(euler,0);
    mpf_init_set_ui(euler_ant, -1);
    mpf_sub(euler_ant, euler_ant, ter); // garante que a primeira comparação não para
    
    if (qua == 's')
        initST();
    else
        initMT();
    
    mpf_clear(euler_ant);
    mpf_clear(euler);
    mpf_clear(ter);
    
    return 0;
}
