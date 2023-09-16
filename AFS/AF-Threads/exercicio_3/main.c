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


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

typedef struct {
    double* a;
    double* b;
    int indice_inicial;
    int indice_final;
    double resultado;
} info;

void* thread(void* arg) {
    info *info_ = (info *)arg;
    for(int i = info_->indice_inicial; i <= info_->indice_final; i++) {
        info_->resultado += info_->a[i] * info_->b[i];
    }
    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

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

    //Calcula produto escalar. Paralelize essa parte
    double result = 0;

    // Caso o número de threads seja maior que o número de cálculos a serem feitos
    // devemos reduzir o número de threads para igualar ao número de cálculos
    if (n_threads > a_size) {
        n_threads = a_size;
    }
    
    pthread_t threads[n_threads];

    info infos[n_threads];
    int qtd_calculos = (a_size+(n_threads-1)) / n_threads;  // Calcula o número de cálculos que cada thread deve realizar arredondando pra cima
    int indice_inicial = 0;
    int indice_final = -1;

    for (int i = 0; i < n_threads; i++) {
        indice_inicial = indice_final + 1;
        indice_final = indice_inicial + qtd_calculos - 1;
        infos[i].a = a;
        infos[i].b = b;
        infos[i].indice_inicial = indice_inicial;
        if (indice_final > a_size - 1) {            // Põe limite ao elemento final de forma a não passar do índice final
            infos[i].indice_final = a_size - 1;     //
        } else {                                    //
            infos[i].indice_final = indice_final;   //
        }                                           //
    }

    // Cria n_threads threads informando como argumento de entrada até onde ela deve realizar os cálculos.
    for (int i = 0; i < n_threads; ++i) {
        pthread_create(&threads[i], NULL, thread, (void *)&infos[i]);
    }

    for (int i = 0; i < n_threads; ++i){
        pthread_join(threads[i], NULL);
        result += infos[i].resultado;
        }

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}
