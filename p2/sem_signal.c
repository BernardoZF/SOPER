#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_NAME "/example_sem"
static volatile sig_atomic_t got_signal = 0;

void manejador(int sig) {
	got_signal = 1;
    return;
}

void imprimir_semaforo(sem_t *sem) {
	int sval;
	if (sem_getvalue(sem, &sval) == -1) {
		perror("sem_getvalue");
		sem_unlink(SEM_NAME);
		exit(EXIT_FAILURE);
	}
	printf("Valor del semáforo: %d\n", sval);
	fflush(stdout);
}

int main(void) {
	sem_t *sem = NULL;
    struct sigaction act;
	if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}

    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    /* Se arma la señal SIGINT. */
    act.sa_handler = manejador;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    imprimir_semaforo(sem);
	printf("Entrando en espera (PID=%d)\n", getpid());
	while(sem_wait(sem)){
		if(got_signal){
			got_signal = 0;
			printf("SEÑAL RECIBIDA\n");
			imprimir_semaforo(sem);
		}
	}
    printf("Fin de la espera\n");
	sem_unlink(SEM_NAME);
}
