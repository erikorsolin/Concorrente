#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia o resultado no vetor c. Assume-se que todos os ponteiros (a, b, e c)
// tenham tamanho size.
void avaliar(double* a, double* b, double* c, int size);

// Estrutura que será usada pela thread
typedef struct {
    double* a;
    double* b;
    double* c;
    int indice_inicial;
    int indice_final;
} info;

void* calculo(void *arg) {
    info infos_ = *(info *)arg;  // Converte o argumento arg de um ponteiro genérico (void *) para um ponteiro do tipo dados
    double *a = infos_.a;
    double *b = infos_.b; 
    double *c = infos_.c; 
    for (int i = infos_.indice_inicial; i <= infos_.indice_final; i++) {
        c[i] = a[i] + b[i];
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Gera um resultado diferente a cada execução do programa
    // Se **para fins de teste** quiser gerar sempre o mesmo valor
    // descomente o srand(0)
    srand(time(NULL)); //valores diferentes
    //srand(0);        //sempre mesmo valor

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n",
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    //Cria vetor do resultado 
    double* c = malloc(a_size*sizeof(double));

    // Caso o número de threads seja maior que o número de cálculos a serem feitos
    // devemos reduzir o número de threads para igualar ao número de cálculos
    if (n_threads > a_size) {
        n_threads = a_size;
    }

    pthread_t threads[n_threads];

    info info_[n_threads];
    int qtd_calculos = (a_size+(n_threads-1)) / n_threads;  // Calcula o número de cálculos que cada thread deve realizar arredondando pra cima
    int indice_inicial = 0;
    int indice_final = -1;

    for (int i = 0; i < n_threads; i++) {
        indice_inicial = indice_final + 1;
        indice_final = indice_inicial + qtd_calculos - 1;
        info_[i].c = c;
        info_[i].a = a;
        info_[i].b = b;
        info_[i].indice_inicial = indice_inicial;
        if (indice_final > a_size - 1) {            // Põe limite ao elemento final de forma a não passar do índice final
            info_[i].indice_final = a_size - 1;     //
        } else {                                    //
            info_[i].indice_final = indice_final;   //
        }                                           //
    }
    


    // Cria n_threads threads informando como argumento de entrada até onde ela deve realizar os cálculos.
    for (int i = 0; i < n_threads; ++i) {
        pthread_create(&threads[i], NULL, calculo, (void *)&info_[i]);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < n_threads; ++i)
        pthread_join(threads[i], NULL);

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, c, a_size);
    

    //Importante: libera memória
    free(a);
    free(b);
    free(c);

    return 0;
}
