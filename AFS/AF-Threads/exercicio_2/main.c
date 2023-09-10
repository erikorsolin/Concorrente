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


void* calculo(void *arg) {
    dados *dados1 = (dados *)arg;  // Converte o argumento arg de um ponteiro genérico (void *) para um ponteiro do tipo dados
    int n_threads = (*dados1).n_threads; // Acessa o campo n_threads da struct dados
    int size_a = (*dados1).size_a; // Acessa o campo size_a da struct dados
    double *a = (*dados1).a;
    double *b = (*dados1).b; 
    double *c = (*dados1).c; 

    int elementos_por_thread = size_a / n_threads; // Calcula a quantidade de elementos que cada thread deve processar
    int inicio = elementos_por_thread * dados1->thread_id;  // Calcula o índice inicial do vetor que a thread deve processar
    int fim = (dados1->thread_id == n_threads - 1) ? size_a : inicio + elementos_por_thread; // Calcula o índice final do vetor que a thread deve processar

    for (int i = inicio; i < fim; i++) {
        c[i] = a[i] + b[i];
    }

    pthread_exit(NULL);
}


typedef struct {
    double size_a;
    int n_threads;
    double *a;
    double *b;
    double *c;
    int thread_id;
} dados;


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
    
    // Cria vetor de dados para passar para as threads
    dados dados1;
    dados1.size_a = a_size;
    dados1.n_threads = n_threads;
    dados1.a = a;
    dados1.b = b;
    dados1.c = c;



    // Calcula com uma thread só. Programador original só deixou a leitura 
    // do argumento e fugiu pro caribe. É essa computação que você precisa 
    // paralelizar
    pthread_t threads[n_threads];


// criando threads
    if (n_threads > a_size){
        for (int i = 0; i < a_size; ++i){
        // caso n_threads seja maior que o tamanho de a, a qtd de threads criadas será size a
        pthread_create(&threads[i], NULL, calculo,  (void *)&dados1);

        }

    } else {
        for (int i = 0; i < n_threads; ++i){
        pthread_create(&threads[i], NULL, calculo, (void *)&dados1);

        }

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
