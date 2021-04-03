#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#define SECS 10

static int end = 0; 

/* manejador : rutina de tratamiento de la señal SIGUSR1 . */
void manejador_usr(int sig) {
    /* ANYADIDO PORQUE SI NO IMPRIME MUY RAPIDO */
    
    return;
}
void manejador_fin(int sig){
    end = 1;
}

int main(int argc, char **argv){
    int NUM_PROC = 0;
    pid_t pid=0;
    pid_t principal;
    long ciclo = 0;
    struct sigaction act;
    sigset_t esperar;

    /* INICIO APARTADO A */
    if(argc != 2){
        printf("Formato erroneo, por favor introduzca algo del estilo\n./conc_cycle <Entero>\n");
        return -1;
    }

    NUM_PROC = atoi(argv[1]);

    act.sa_handler = manejador_usr;
    /* CAMBIADO PARA EL APARTADO D */
    sigfillset(&(act.sa_mask)); 
    act.sa_flags = 0;

    /* APARTADO D INICIO*/
    sigfillset(&esperar);
    sigdelset(&esperar, SIGINT);
    sigdelset(&esperar, SIGTERM);
    sigdelset(&esperar, SIGUSR1);
    sigdelset(&esperar, SIGALRM);

    if (sigprocmask(SIG_BLOCK, &(act.sa_mask), NULL) < 0) {
        perror("sigprocmask");
        return -1;
    }  
    /* APARTADO D FIN */

    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    principal=getpid();

    for(int i = 1; i < NUM_PROC; i++ ){
        if(pid==0){
            pid = fork();
            if(pid<0){
                perror("fork");
                return -1;
            }
        }
    }
    /* FIN APARTADO A */

    
    
    /* PRIMERA PARTE APARTADO C*/
    act.sa_handler = manejador_fin;
    
    if(getpid()==principal){
        if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
        }
        if (sigaction(SIGALRM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
        }

    }else{
        if (sigaction(SIGTERM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
        }
    }
    /*  FIN PRIMERA PARTE C */

    if(getpid() == principal){
        if (alarm(SECS)) {
        fprintf(stderr, "Existe una alarma previa establecida\n");
        }
    }
    
    /* INICIO APARTADO B  los if(end) SON DEL APARTADO C*/
    while(1){
        if(pid != 0){
            if(end){
                if(getpid()==principal){
                    kill(pid, SIGTERM);
                    wait(NULL);
                    break;
                }else{
                    kill(pid, SIGTERM);
                    wait(NULL);
                    exit(EXIT_SUCCESS);
                }
            }else{
              
                kill(pid, SIGUSR1);
                printf("Ciclo: %ld, PID: %d\n", ciclo, getpid()); 
                fflush(stdout);             
                
            }
        }else{
            if(end){
                exit(EXIT_SUCCESS);
            }else{
                kill(principal, SIGUSR1);
                printf("Ciclo: %ld, PID: %d\n", ciclo, getpid());
                fflush(stdout);
                
            
            }
        }
        ciclo++;
        /* CAMBIADO PARA EL APARTADO D */
        sigsuspend(&esperar);
        
    }
    /* FIN APARTADO B */
  

}


