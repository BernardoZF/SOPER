#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "miner.h"
#include <fcntl.h>
#include <mqueue.h>

#define PRIME 99997669
#define MAX_LENGTH 256

long int solution = 0;
int sol_found = 0;
static int end = 0;

void manejador_SIGINT(int sig)
{
    end = 1;
}

typedef struct worker_data
{
    long int target;
    long int to_check;
    int cycles;
} Wd;

void *work_mine(void *arg)
{
    Wd *wd;
    wd = arg;

    if (!end)
    {
        while (sol_found == 0)
        {
            if (wd->target == simple_hash(wd->to_check))
            {
                solution = wd->to_check;
                sol_found = 1;
            }
            else
            {
                wd->to_check = (wd->to_check + 1) % PRIME;
            }
        }
    }

    return NULL;
}

Block *create_new_block(Block *prev)
{
    Block *b = NULL;
    if (!prev)
    {
        b = (Block *)malloc(sizeof(Block));
        if (!b)
        {
            return NULL;
        }
        b->wallets[0] = 1;
        b->target = 1;
        b->solution = -1;
        b->id = 1;
        b->is_valid = 0;
        b->prev = NULL;
        b->next = NULL;

        return b;
    }

    b = (Block *)malloc(sizeof(Block));
    if (!b)
    {
        printf("error en la reserva de memoria del bloque\n");
        return NULL;
    }
    b->wallets[0] = prev->wallets[0] + 1;
    b->target = prev->solution;
    b->solution = -1;
    b->id = prev->id + 1;
    b->is_valid = 0;
    b->prev = NULL;
    b->next = NULL;

    return b;
}

int sol_found_dependancies(Block ** b, FILE *pf, pthread_t *workers, Wd *workers_data, mqd_t * mq)
{
    Block * next;
    unsigned int prio = 2;
    /* actualizo la solucion */
    (*b)->solution = solution;
    (*b)->is_valid = 1;
    /* Creo el siguiente bloque */
    next = create_new_block(*b);
    if (!next)
    {
        printf("error en la reserva de memoria del bloque");
        fclose(pf);
        free(workers);
        free(workers_data);
        return -1;
    }
    /* Establezco el siguiente bloque */
    print_blocks_to_file(*b, 1, pf);
    if(mq_send(*mq, (char *)*b, sizeof(**b), 1) == -1 && !end) {
        printf("Error en el enviado del bloque \n");
        return -1;
    }

    /*b->next = next;*/
    /* Y apunto al siguiente bloque*/
    *b = next;

    sol_found = 0;
    return 0;
}

int main(int argc, char **argv)
{

    int n_workers;
    int n_cycles;
    pthread_t *workers = NULL;
    Wd *workers_data = NULL;
    Block *b;
    struct sigaction act;
    FILE *pf = NULL;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(Block)};
    char filename[MAX_LENGTH];
    sprintf(filename, "%d", getpid());

    mqd_t mq;

    if ((mq = mq_open(Q_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    /* FALTA MQ_CLOSE EN CADA CONTROL DE ERRORES */

    /* Se arma la señal SIGALRM. */
    act.sa_handler = manejador_SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (argc != 3)
    {
        printf("Parametros de entrada erroneos\n");
        return -1;
    }

    pf = fopen(filename, "w");
    if (!pf)
    {
        printf("Error abriendo archivo log del minero");
        return -1;
    }

    n_workers = atoi(argv[1]);
    if (n_workers < 1)
    {
        printf("Numero de hilos menor que uno por favor introduzca un numero valido \n");
        fclose(pf);
        return -1;
    }
    else if (n_workers > MAX_WORKERS)
    {
        printf("Numero de hilos mayor que el maximo favor introduzca un numero valido \n");
        fclose(pf);
        return -1;
    }

    n_cycles = atoi(argv[2]);

    workers = malloc(sizeof(pthread_t) * n_workers);
    if (!workers)
    {
        printf("ERROR al reservar memoria para los trabajadores");
        fclose(pf);

        return -1;
    }

    workers_data = (Wd *)malloc(sizeof(Wd) * n_workers);
    if (!workers_data)
    {
        fclose(pf);
        free(workers);
        return -1;
    }

    long section = PRIME / n_workers;
    /* CREACION DEL PRIMER BLOQUE */
    b = create_new_block(NULL);
    if (!b)
    {
        printf("error en la reserva de memoria del bloque");
        fclose(pf);
        free(workers);
        free(workers_data);
        return -1;
    }

    /* Rondas pasadas como argumento */
    if (n_cycles > 0)
    {
        for (int a = 0; a < n_cycles && end == 0; a++)
        {
            for (int i = 0; i < n_workers; i++)
            {
                workers_data[i].target = b->target;
                workers_data[i].to_check = section * i;
                workers_data[i].cycles = n_cycles;
            }

            for (int i = 0; i < n_workers; i++)
            {
                pthread_create(&workers[i], NULL, work_mine, &workers_data[i]);
            }
            for (int i = 0; i < n_workers; i++)
            {
                pthread_join(workers[i], NULL);
            }

            if (sol_found)
            {
                if (sol_found_dependancies(&b, pf, workers, workers_data, &mq) == -1)
                    return -1;
            }
        }
    }
    else
    {
        /* Hasta que no recibes señal no acaba */
        while (!end)
        {

            for (int i = 0; i < n_workers; i++)
            {
                workers_data[i].target = b->target; // cambiarlo por el target buscado par cada caso
                workers_data[i].to_check = section * i;
                workers_data[i].cycles = n_cycles;
            }

            for (int i = 0; i < n_workers; i++)
            {
                pthread_create(&workers[i], NULL, work_mine, &workers_data[i]);
            }
            for (int i = 0; i < n_workers; i++)
            {
                pthread_join(workers[i], NULL);
            }

            if (sol_found)
            {
                if (sol_found_dependancies(&b, pf, workers, workers_data, &mq) == -1){
                    break;
                }
            }
        }
    }

    exit(EXIT_SUCCESS);
}