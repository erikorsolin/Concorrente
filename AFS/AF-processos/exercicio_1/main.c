#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>


//       (pai)      
//         |        
//    +----+----+
//    |         |   
// filho_1   filho_2


// ~~~ printfs  ~~~
// pai (ao criar filho): "Processo pai criou %d\n"
//    pai (ao terminar): "Processo pai finalizado!\n"
//  filhos (ao iniciar): "Processo filho %d criado\n"
// Obs:
// - pai deve esperar pelos filhos antes de terminar!


// Função para o processo filho do processo principal
void processoFilho() {
    pid_t pid = getpid();
    printf("Processo filho %d criado\n", pid);
}



int main(int argc, char** argv) {

    // cria o primeiro processo filho
    pid_t pid1 = fork(); 
    if (pid1  > 0){
        printf("Processo pai criou %d\n", pid1);
    }
    
    if (pid1 == -1) {
        perror("Erro ao criar o primeiro filho");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Este é o primeiro filho 1
        processoFilho();
        exit(EXIT_SUCCESS);
    }

    // Limpar o buffer de saída antes de criar o segundo filho
    fflush(stdout);

    // cria o segundo processo filho
    pid_t pid2 = fork(); 
    if (pid2 > 0) {
        printf("Processo pai criou %d\n", pid2);
    }
    
    if (pid2 == -1) {
        perror("Erro ao criar o segundo filho");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Este é o segundo filho
        processoFilho();
        exit(EXIT_SUCCESS);
    }

    // O processo pai espera pelos filhos
    int status;
    waitpid(pid1, &status, 0); // Espera pelo primeiro filho
    waitpid(pid2, &status, 0); // Espera pelo segundo filho

    printf("Processo pai finalizado!\n");   
    return 0;
}
