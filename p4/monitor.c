#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "miner.h"

static int got_alarm = 0;
static int end = 0;

void manejador_SIGALRM(int sig)
{
    got_alarm = 1;
}

void manejador_SIGINT(int sig)
{
    end = 1;
}

int main(int argc, char **argv)
{
    struct sigaction act;
    int fd[2];
    int pipe_status;
    pid_t father;

    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    /* Se arma la señal SIGALRM. */
     act.sa_handler = manejador_SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pipe_status = pipe(fd);
    if (pipe_status == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    father = fork();
    if (father < 0)
    {
        close(fd[0]);
        close(fd[1]);
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (father)
    {
        close(fd[0]);
        /* Codigo padre */
        Block blocks[10];
        int head = 0;
        int rear = 0;
        int flag;

        /* SE RECIBA UN NUEVO BLOQUE */
        Block *b;
        /* COMPROBAMOS SI NO ESTA */
        for (int i = 0, flag = 0; i < 10; i++)
        {
            if (b->id == blocks[i].id)
            {
                flag = 1;
            }
        }
        /* SI NO ESTA SE ANYADE (POR IMPLEMENTAR) */
        /* Y ACTUALIZAMOS LOS MARCADORES */
        if (!flag)
        {
            head = (head + 1) % 10;
            if (head == rear)
            {
                rear = (rear + 1) % 10;
            }
        }
        if(end){
            /* LIBERAR TODA LA MEMORIA */
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        close(fd[1]);

        act.sa_handler = manejador_SIGALRM;
        if (sigaction(SIGALRM, &act, NULL) < 0)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            if (alarm(5))
            {
            }
            if (got_alarm)
            {
                /* Falta obtener el numero de wallets que hay que no se cmo implementarlo */
                /* Ademas de todo lo de recepcion de bloques por tuberia */
                print_blocks(NULL, 10);

                got_alarm = 0;
            }
            if(end){
                /* Liberar memoria */
                exit(EXIT_SUCCESS);
            }
        }
    }
}