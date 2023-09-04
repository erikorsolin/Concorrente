#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//  principal (ao finalizar): "Processo principal %d finalizado\n"
//  filhos e netos (ao finalizar): "Processo %d finalizado\n"
//  filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar


void ProcessoFilho() {
    pid_t pid = getpid();
    pid_t pid_pai = getppid();
    printf("Processo %d, filho de %d\n", pid, pid_pai);
}

void ProcessoNeto() {
    pid_t pid = getpid();
    pid_t pid_pai = getppid();
    printf("Processo %d, filho de %d\n", pid, pid_pai);
}

int main(int argc, char** argv) {
    // Cria o primeiro processo filho
    pid_t pid1 = fork();

    if (pid1 == -1) {
        perror("Erro ao criar o primeiro filho");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Este é o primeiro filho 1
        ProcessoFilho();

        // Criar três netos
        for (int i = 0; i < 3; i++) {
            fflush(stdout);
            pid_t neto_pid = fork();
            if (neto_pid == -1) {
                perror("Erro ao criar neto");
                exit(EXIT_FAILURE);
            } else if (neto_pid == 0) {
                // Este é um neto
                ProcessoNeto();
                sleep(5); // Espera 5 segundos antes de terminar
                printf("Processo %d finalizado\n", getpid());
                exit(EXIT_SUCCESS);
            }
        }

        wait(NULL); // Espera pelos netos
        wait(NULL); // Espera pelos netos
        wait(NULL); // Espera pelos netos

        printf("Processo %d finalizado\n", getpid());
        exit(EXIT_SUCCESS);
    }

    // Limpar o buffer de saída antes de criar o segundo filho
    fflush(stdout);

    // Cria o segundo processo filho
    pid_t pid2 = fork();

    if (pid2 == -1) {
        perror("Erro ao criar o segundo filho");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Este é o segundo filho
        ProcessoFilho();

        // Criar três netos
        for (int i = 0; i < 3; i++) {
            fflush(stdout); 
            pid_t neto_pid = fork();
            if (neto_pid == -1) {
                perror("Erro ao criar neto");
                exit(EXIT_FAILURE);
            } else if (neto_pid == 0) {
                // Este é um neto
                ProcessoNeto();
                sleep(5); // Espera 5 segundos antes de terminar
                printf("Processo %d finalizado\n", getpid());
                exit(EXIT_SUCCESS);
            }
        }

        wait(NULL); // Espera pelos netos
        wait(NULL); // Espera pelos netos
        wait(NULL); // Espera pelos netos

        printf("Processo %d finalizado\n", getpid());
        exit(EXIT_SUCCESS);
    }

    // O processo pai espera pelos filhos
    wait(NULL);
    wait(NULL);

    printf("Processo principal %d finalizado\n", getpid());
    return 0;
}
