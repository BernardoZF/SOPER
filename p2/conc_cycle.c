#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int got_signal = 0;

/* manejador : rutina de tratamiento de la señal SIGINT . */
void manejador(int sig) {
    got_signal = 1;
}

int main(int argc, char **argv){
    int NUM_PROC = 0;
    pid_t pid=0;
    pid_t principal;
    long ciclo = 0;

    /* INICIO APARTADO A */
    if(argc != 2){
        printf("Formato erroneo, por favor introduzca algo del estilo\n./conc_cycle <Entero>\n");
        return -1;
    }

    NUM_PROC = atoi(argv[1]);


    struct sigaction act;

    act.sa_handler = manejador;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    principal=getpid();
    printf("%d\n", principal);
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
    
    /* INICIO APARTADO B */
    while(1){
        if(pid != 0){
            if(got_signal){
                printf("%d\n", getpid());
                if(getpid()==principal){
                    printf("soy el programa principal\n");
                }
                kill(pid, SIGINT);
                sleep(9999);
            }
        }else{
            if(got_signal){
                printf("%d\n", getpid());
                kill(principal, SIGUSR1);
                sleep(9999);
            }
        }
        
        
    }
  

}



/*for(i = 1; i< NUM_PROC; i++){
    if(pid==0){
        pid = fork;
    }
}
while(1){
    if(hijo){
        kill(SIGUSR1, hijo);
    }
    else{
        kill(SIGUSR, principal);
    }
    if(got_signal) {
		  got_signal = 0;
		  printf("Ciclo %ld pid %ld.\n", ciclo, pid);
          ciclo++;
		}        
    sleep(9999);
    
}*/