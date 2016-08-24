#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define MEDIAS  10
#define LAGO    11 

/* variáveis definidas no enunciado */
int larg;
int alt;
int L;
int H;
double T;
double v;
double e;
int Niter;
double P;
int s;
int numproc;

/* variáveis usadas no programa */
int **matriz;
double **lago;  // guarda cores
double **lagoM; // guarda médias
double **lagoQ; // guarda quadrados para calcular desvio padrão

/* variáveis globais */
double hmax;
double pmax;
double xScale, yScale;

/* ondas */
typedef struct onda{
    int center_x;
    int center_y;
    int passo;
} Onda;

Onda *vetor_onda;
int count_onda;

void desenha_ponto_no_lago(int x, int y, double color, int tipo){
    if (x > 0 && y > 0 && x < alt && y < larg){
        if (tipo == LAGO)
            lago[x][y] = color;
        if (tipo == MEDIAS){
            lagoM[x][y] += color;
            lagoQ[x][y] += color*color;
        }
    }
}

/* Desenha circunferência da onda no lago.
 * Retirado de Wikipedia
 * https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 */
void desenha_no_lago(int x0, int y0, int radius, double color, int tipo)
{
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

    while( y <= x ){
        desenha_ponto_no_lago( x + x0,  y + y0, color, tipo); // Octant 1
        desenha_ponto_no_lago( y + x0,  x + y0, color, tipo); // Octant 2
        desenha_ponto_no_lago(-x + x0,  y + y0, color, tipo); // Octant 4
        desenha_ponto_no_lago(-y + x0,  x + y0, color, tipo); // Octant 3
        desenha_ponto_no_lago(-x + x0, -y + y0, color, tipo); // Octant 5
        desenha_ponto_no_lago(-y + x0, -x + y0, color, tipo); // Octant 6
        desenha_ponto_no_lago( x + x0, -y + y0, color, tipo); // Octant 8
        desenha_ponto_no_lago( y + x0, -x + y0, color, tipo); // Octant 7
        y++;
        if (decisionOver2<=0){
            decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
        }
        else{
            x--;
            decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
        }
    }
}

int print_def(){
    printf("Lago: (%d,%d)\n", larg, alt);
    printf("Matriz: (%d,%d)\n", L, H);
    printf("Tempo: %f\n", T);
    printf("Velocidade da onda: %f\n", v);
    printf("Limiar de altura: %f\n", e);
    printf("Número de iterações: %d\n", Niter);
    printf("Probabilidade de surgir gota: %f\n", P);
    printf("Semente: %d\n", s);
    return 0;
}

/* Calcula o max da fórmula do delta. */
double delta_max(){
    double delta_h, delta_p;
    delta_h = hmax;
    delta_p = -pmax;

    if (delta_h > delta_p)
        return delta_h;
    return delta_p;
}

/* Mapeia coordenadas de ponto do lago para ponto na imagem */
void mapping(int i, int j, int *xi, int *xj){
    double tmp_xi, tmp_xj;
    int tmpi_xi, tmpi_xj;

    tmp_xi = i * xScale;
    tmp_xj = j * yScale;

    tmpi_xi = (int) tmp_xi;
    if (tmpi_xi == L)
      tmpi_xi--;
    tmpi_xj = (int) tmp_xj;
    if (tmpi_xj == H)
      tmpi_xj--;

    *xi = tmpi_xi;
    *xj = tmpi_xj;
}

/* Para cada ponto do lago, determina cor na matriz de imagem */
void img_output(){
    int i, j;
    int xi, xj;
    double delta;
    double tmp;

    /* delta deve dividir cada ponto, então pegaremos seu inverso
     * e faremos a multiplicação em cada ponto */
    delta = 255.0 / delta_max();
    for (i = 0; i < H; i++)
        #pragma omp parallel for
        for (j = 0; j < L; j++){
            mapping(i, j, &xi, &xj);
            tmp = delta * lago[xi][xj];
            matriz[i][j] = (int)tmp;
        }
}

void img_write_output(char *filename){
    FILE *fp;
    char *img_file;
    char size[1024];
    int i, j;

    img_file = malloc(1024*sizeof(char));
    strcpy(img_file, filename);
    strcat(img_file, ".ppm");

    fp = fopen(img_file, "w");

    fputs("P3\n", fp);
    sprintf(size, "%d %d\n255\n", L, H);
    fputs(size, fp);

    for (i = 0; i < H; i++)
        for (j = 0; j < L; j++){
            if (matriz[i][j] >= 0){
                sprintf(size, "%d\n", matriz[i][j]);
                fputs("0\n", fp); // R
                fputs("0\n", fp); // G
                fputs(size, fp); // B
            }
            else{
                sprintf(size, "%d\n", -1 * matriz[i][j]);
                fputs(size, fp); // R
                fputs("0\n", fp); // G
                fputs("0\n", fp); // B
            }
        }
}

void medias_output(){
    int i, j;

    for (i = 0; i < alt; i++)
        #pragma omp parallel for
        for (j = 0; j < larg; j++){
            lagoM[i][j] = lagoM[i][j] / (double)Niter;
            lagoQ[i][j] = (lagoQ[i][j] / (double)Niter) - lagoM[i][j] * lagoM[i][j];
            lagoQ[i][j] = sqrt(lagoQ[i][j]);
        }
}

void medias_write_output(char *filename){
    FILE *fp;
    char *medias_file;
    char size[1024];
    int i, j;

    medias_file = malloc(1024*sizeof(char));
    strcpy(medias_file, filename);
    strcat(medias_file, "-medias.txt");

    fp = fopen(medias_file, "w");

    for (i = 0; i < alt; i++)
        for (j = 0; j < larg; j++){
            sprintf(size, "%12.7f %12.7f %12.7f %12.7f\n", (double)i, (double)j, lagoM[i][j], lagoQ[i][j]);
            fputs(size, fp);
        }
}

int gerar_onda(){
    if (rand() % 100 > P)
      return 0;

    int x, y;

    x = rand() % larg;
    y = rand() % alt;

    Onda onda;
    onda.center_x = x;
    onda.center_y = y;
    onda.passo = 0;
    vetor_onda[count_onda] = onda;
    count_onda++;
    return 1;
}

/* Altura é dada por três fatores: A * B * C 
 * B e C são dados pela função exponencial
 * exp(expoente_B) e exp(expoente_C) respectivamente
 */
double calcula_altura(double raio, double tempo){
    double altura, produto_a, produto_b, produto_c;
    double expoente_b, expoente_c;

    produto_a = raio - v * tempo;

    expoente_b = produto_a * produto_a * -1.0;

    produto_b = exp(expoente_b);

    expoente_c = -1.0 * tempo;
    expoente_c = expoente_c / 10.0;

    produto_c = exp(expoente_c);
    altura = produto_a * produto_b * produto_c;

    if (altura > hmax)
        hmax = altura;
    if (altura < pmax)
        pmax = altura;
    return altura;
}

/* propaga ondas em uma iteração
 */
int propaga(){
    int i, j;
    double alt, raio, tempo;

    for (i = 0; i < count_onda; i++){
        vetor_onda[i].passo++;
        for (j = 0; j <= vetor_onda[i].passo; j++){
            raio = v * T / Niter + j * v * T / Niter;
            tempo = vetor_onda[i].passo * T / Niter;
            alt = calcula_altura(raio, tempo);
            if (alt > e || alt < -e)
                desenha_no_lago(vetor_onda[i].center_x, vetor_onda[i].center_y, j*v*T/Niter, alt, MEDIAS);
        }
    }

    return 0;
}

void do_work(char*filename){
    int i, j;
    double alt, raio, tempo;

    for (i = 0; i < Niter; i++){
        propaga();
        gerar_onda();
    }

    /* desenha ondas da imagem final */
    for (i = 0; i < count_onda; i++)
        for (j = vetor_onda[i].passo; j >= 0; j--){ // cada aro da onda
            raio = v * T / Niter + j * v * T / Niter;
            tempo = vetor_onda[i].passo * T / Niter;
            alt = calcula_altura(raio, tempo);
            if (alt > e || alt < -e)
                desenha_no_lago(vetor_onda[i].center_x, vetor_onda[i].center_y, j*v*T/Niter, alt, LAGO);
        }

    img_output();
    img_write_output(filename);
    medias_output();
    medias_write_output(filename);
}

int main(int argc, char** argv){
    int i;
    if (argc == 3){
        numproc = atoi(argv[2]);
    }
    else if (argc == 2){
        numproc = 1;
    }
    else if (argc < 2){
        printf("ERRO: faltaram os argumentos.\n");
        return 1;
    }

    FILE *fp;
    char *line = NULL;
    char *line_aux = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    read = getline(&line, &len, fp);
    line_aux = strtok(line, " (),.-");
    larg = atoi(line_aux);
    line_aux = strtok(NULL, " (),.-");
    alt = atoi(line_aux);

    read = getline(&line, &len, fp);
    line_aux = strtok(line, " (),.-");
    L = atoi(line_aux);
    line_aux = strtok(NULL, " (),.-");
    H = atoi(line_aux);

    read = getline(&line, &len, fp);
    T = atof(line);

    read = getline(&line, &len, fp);
    v = atof(line);

    read = getline(&line, &len, fp);
    e = atof(line);

    read = getline(&line, &len, fp);
    Niter = atoi(line);

    read = getline(&line, &len, fp);
    P = atof(line);

    read = getline(&line, &len, fp);
    s = atoi(line);

    if (!read)
        printf("warning\n");

    fclose(fp);
    if (line)
        free(line);

    //print_def();

    /* inicializa globais */
    matriz = malloc(H * sizeof(int*));
    for (i = 0; i < H; i++)
        matriz[i] = malloc (L*sizeof(int));


    lago = malloc(alt * sizeof(double*));
    for (i = 0; i < alt; i++)
        lago[i] = malloc (larg*sizeof(double));

    lagoM = malloc(alt * sizeof(double*)); 
    for (i = 0; i < alt; i++)
        lagoM[i] = malloc (larg*sizeof(double));

    lagoQ = malloc(alt * sizeof(double*)); 
    for (i = 0; i < alt; i++)
        lagoQ[i] = malloc (larg*sizeof(double));

    srand(s);
    
    xScale = alt / H;
    yScale = larg / L;

    vetor_onda = malloc(Niter*sizeof(Onda));
    count_onda = 0;

    do_work(argv[1]);
    printf("%d\n", count_onda);
    return 0;
}
