#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "miner.h"
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <errno.h>

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
    int fd_shm_net;
    struct sigaction act;
    int fd[2];
    int pipe_status;
    pid_t father;
    NetData * nd;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(Block)};

    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    /* Se arma la señal SIGALRM. */
    act.sa_handler = manejador_SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0)
    {
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
        mqd_t mq;

        if ((mq = mq_open(Q_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
        {
            perror("mq_open");
            wait(NULL);
            exit(EXIT_FAILURE);
        }
        mq_unlink(Q_NAME);

        fd_shm_net = shm_open(SHM_NAME_NET, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        /* Si hay error */
        if (fd_shm_net == -1)
        { /*Compruebo si el error es porque existe */
            if (errno == EEXIST)
            { /* Si es porque existe entonces no lo creo y solo lo abro */
                fd_shm_net = shm_open(SHM_NAME_NET, O_RDWR, 0);
                if (fd_shm_net == -1)
                {
                    perror("Error opening the shared memory segment");
                    exit(EXIT_FAILURE);
                }
            }
            /* Errores creando el archivo */
            else
            {
                mq_close(mq);
                perror("shm_open");
                exit(EXIT_FAILURE);
                exit(EXIT_FAILURE);
            }
        }
        /* Este archivo crea el archivo compartido por lo tanto tiene que truncar el archivo */
        else
        {
            if (ftruncate(fd_shm_net, sizeof(NetData)) == -1)
            {
                mq_close(mq);
                perror("ftruncate");
                shm_unlink(SHM_NAME_NET);
                exit(EXIT_FAILURE);
            }
        }
        /* Resize of the memory segment. */

        /* Mapping of the memory segment. */
        nd = (NetData *)mmap(NULL, sizeof(NetData), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm_net, 0);
        close(fd_shm_net);
        if (nd == MAP_FAILED)
        {
            mq_close(mq);
            perror("mmap");
            shm_unlink(SHM_NAME_NET);
            exit(EXIT_FAILURE);
        }

        nd->monitor_pid = getpid();

        close(fd[0]);
        /* Codigo padre */
        Block blocks[10];
        int head = 0;
        int rear = 0;
        int flag = 0;
        int pos = 0;
        //unsigned int prio = 2;

        /* SE RECIBA UN NUEVO BLOQUE */
        Block b;
        while (1)
        {
            ssize_t n_bytes = 0;
            if (mq_receive(mq, (char *)&b, sizeof(b), NULL) == -1 && !end)
            {
                printf("Error recibiendo el mensaje");
                return -1;
            }
            /* COMPROBAMOS SI NO ESTA */
            //print_blocks(&b, 1);
            flag = 0;
            for (int i = 0; i < 10; i++)
            {
                if (b.id == blocks[i].id)
                {
                    flag = 1;
                    pos = i;
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
                blocks[head] = b;

                n_bytes = write(fd[1], (void *)&blocks[head], sizeof(blocks[head]));
                if (n_bytes < 0)
                {
                    perror("write");
                    wait(NULL);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (b.solution == blocks[pos].solution && b.target == blocks[pos].target)
                {
                    printf("Verified block %d with solution %ld for target %ld\n", b.id, b.solution, b.target);
                }
                else
                {
                    printf("Error in block %d with solution %ld for target %ld\n", b.id, b.solution, b.target);
                }
            }
            if (end)
            {
                /* LIBERAR TODA LA MEMORIA */
                close(fd[1]);
                mq_close(mq);
                wait(NULL);
                exit(EXIT_SUCCESS);
            }
        }
    }
    else
    {
        int turn = 0;
        Block *b = NULL;
        Block *next;
        ssize_t n_bytes = 0;
        close(fd[1]);

        act.sa_handler = manejador_SIGALRM;
        if (sigaction(SIGALRM, &act, NULL) < 0)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        do
        {
            if (alarm(5))
            {
            }

            next = (Block *)malloc(sizeof(Block));

            n_bytes = read(fd[0], (void *)next, sizeof(*next));
            if (n_bytes == -1 && !end && !got_alarm)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (b)
            {
                b->next = next;
                next->prev = b;
            }
            b = next;

            if (end)
            {
                /* Liberar memoria */
                blocks_free(b);
                close(fd[0]);
                exit(EXIT_SUCCESS);
            }
            if (got_alarm)
            {
                /* Falta obtener el numero de wallets que hay que no se como implementarlo */
                /* Ademas de todo lo de recepcion de bloques por tuberia */
                print_blocks(b, MAX_MINERS);

                got_alarm = 0;
            }
            turn++;
            sleep(5);
        } while (n_bytes != 0);
    }
}