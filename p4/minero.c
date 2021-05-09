#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "miner.h"
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#define PRIME 99997669
#define MAX_LENGTH 256

typedef struct worker_data
{
    long int target;
    long int to_check;
    int cycles;
} Wd;

long int solution = 0;
int sol_found = 0;
int stop = 0;
static int end = 0;

/**
 * @brief manejador de la senyal SIGINT
 * @param sig senyal a manejar
 */
void manejador_SIGINT(int sig);

/**
 * @brief manejador de la senyal SIGUSR2
 * @param sig senyal a manejar
 */
void manejador_SIGUSR2(int sig);

/**
 * @brief funcion que realiza cada hilo, esto selo que hacen los "trabajadores"
 * @param arg argumento del hilo
 * 
 * @return NULL
 */
void *work_mine(void *arg);

/**
 * @brief Funcion usada para que el proceso minero cree los bloques de su cadena local
 * @param prev Es el bloque previo al actual, si es el primero el parametro sera NULL
 * 
 * @return El bloque de memoria actual
 */
Block *create_new_block(Block *prev);

/**
 * @brief Funcion que gestiona todas las dependencias de haber encontrado una solucion
 * @param b puntero a la direccion de memria en la que esta el bloque local actual
 * @param sb puntero al bloque compartido
 * @param pf puntero al fichero al que imprimiremos
 * @param workers puntero al conjunto de hilos que tiene el proceso 
 * @param workers_data puntero a la estructura que se le pasa a cada uno de los hilos
 * @param mq puntero a la cola de mensajes que se comunica con el monitor
 * 
 * @return devuelve -1 en caso de error, si no hubo error devuelve 0
 */
int sol_found_dependancies(Block **b, Block *sb, FILE *pf, pthread_t *workers, Wd *workers_data, mqd_t *mq, NetData *nd);

/**
 * @brief esta funcion actualiza el bloque que esta en memoria compartida
 * @param sb puntero al bloque en memoria compartida
 * @param wallet numero de wallet de este proceso
 */
void shared_block_f5(Block *sb, int wallet);

int main(int argc, char **argv)
{
    int wallet;
    int fd_shm_net;
    int fd_shm_block;
    int n_workers;
    int n_cycles;
    int first_block = 0;
    int first_net = 0;
    pthread_t *workers = NULL;
    Wd *workers_data = NULL;
    Block *b;
    Block *block_shared;
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

    /* Se arma la señal SIGINT. */
    act.sa_handler = manejador_SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    act.sa_handler = manejador_SIGUSR2;
    if (sigaction(SIGUSR2, &act, NULL) < 0)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (argc != 3)
    {
        printf("Parametros de entrada erroneos\n");
        return -1;
    }

    if ((mq = mq_open(Q_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    pf = fopen(filename, "w");
    if (!pf)
    {
        mq_close(mq);
        printf("Error abriendo archivo log del minero");
        return -1;
    }

    n_workers = atoi(argv[1]);
    if (n_workers < 1)
    {
        mq_close(mq);
        printf("Numero de hilos menor que uno por favor introduzca un numero valido \n");
        fclose(pf);
        return -1;
    }
    else if (n_workers > MAX_WORKERS)
    {
        mq_close(mq);
        printf("Numero de hilos mayor que el maximo favor introduzca un numero valido \n");
        fclose(pf);
        return -1;
    }

    n_cycles = atoi(argv[2]);

    workers = malloc(sizeof(pthread_t) * n_workers);
    if (!workers)
    {
        mq_close(mq);
        printf("ERROR al reservar memoria para los trabajadores");
        fclose(pf);

        return -1;
    }

    workers_data = (Wd *)malloc(sizeof(Wd) * n_workers);
    if (!workers_data)
    {
        mq_close(mq);

        fclose(pf);
        free(workers);
        return -1;
    }

    long section = PRIME / n_workers;
    /* CREACION DEL PRIMER BLOQUE */
    b = create_new_block(NULL);
    if (!b)
    {
        mq_close(mq);
        printf("error en la reserva de memoria del bloque");
        fclose(pf);
        free(workers);
        free(workers_data);
        return -1;
    }

    if (first_block)
    {
        block_shared->target = 1;
        block_shared->solution = -1;
        block_shared->id = 1;
        block_shared->is_valid = 0;
        block_shared->prev = NULL;
        block_shared->next = NULL;
    }

    /* Rondas pasadas como argumento */
    if (n_cycles > 0)
    {
        for (int a = 0; a < n_cycles && end == 0; a++)
        {
            for (int i = 0; i < n_workers; i++)
            {
                workers_data[i].target = block_shared->target;
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
                if (sol_found_dependancies(&b, block_shared, pf, workers, workers_data, &mq, nd) == -1)
                {
                    mq_close(mq);
                    fclose(pf);
                    free(workers);
                    free(workers_data);
                    shm_unlink(SHM_NAME_NET);
                    munmap(nd, sizeof(*nd));
                    munmap(block_shared, sizeof(*block_shared));
                    shm_unlink(SHM_NAME_BLOCK);
                    blocks_free(b);
                }
                else
                {
                    /* De momento el wallet va a ser 0 pero tendremos que obtener que wallet es este proceso */
                    shared_block_f5(block_shared, wallet);
                }
            }
            else if (stop)
            {
                sem_wait(&nd->start);
                // Gestionar para la votacion
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
                workers_data[i].target = block_shared->target; // cambiarlo por el target buscado par cada caso
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
                if (sol_found_dependancies(&b, block_shared, pf, workers, workers_data, &mq, nd) == -1)
                {
                    mq_close(mq);
                    fclose(pf);
                    free(workers);
                    free(workers_data);
                    shm_unlink(SHM_NAME_NET);
                    munmap(nd, sizeof(*nd));
                    munmap(block_shared, sizeof(*block_shared));
                    shm_unlink(SHM_NAME_BLOCK);
                    blocks_free(b);
                }
                else
                {
                    sem_wait(&nd->mutex);
                    /* De momento el wallet va a ser 0 pero tendremos que obtener que wallet es este proceso */
                    shared_block_f5(block_shared, wallet);
                    sem_post(&nd->mutex);
                }
            }
            else if (stop && !sol_found)
            {
                sem_wait(&nd->start);
                // Gestionar para la votacion
            }
        }
    }

    print_blocks_to_file(b, 1, pf);

    mq_close(mq);
    fclose(pf);
    free(workers);
    free(workers_data);
    shm_unlink(SHM_NAME_NET);
    munmap(block_shared, sizeof(*block_shared));
    shm_unlink(SHM_NAME_BLOCK);
    blocks_free(b);
    exit(EXIT_SUCCESS);
}

void manejador_SIGINT(int sig)
{
    end = 1;
}

void manejador_SIGUSR2(int sig)
{
    stop = 1;
}

void *work_mine(void *arg)
{
    Wd *wd;
    if (!arg)
    {
        return NULL;
    }
    wd = arg;

    if (!end)
    {
        while (sol_found == 0 && stop == 0)
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
    b->prev = prev;
    b->next = NULL;

    return b;
}

int sol_found_dependancies(Block **b, Block *sb, FILE *pf, pthread_t *workers, Wd *workers_data, mqd_t *mq, NetData *nd)
{
    Block *next;

    if (!b || !sb || !pf || !workers || !workers_data || !mq)
    {
        return -1;
    }
    for (int i = 0; i < MAX_MINERS; i++)
    {
        if (nd->miners_pid[i] && nd->miners_pid[i] != getpid())
        {
            kill(nd->miners_pid[i], SIGUSR2);
        }
    }
    //kill(0, SIGUSR2);
    /* actualizo la solucion */
    sb->solution = solution;
    (*b)->solution = solution;

    /* hasta que no se cree el sitema de votacion esto se queda
     como por defecto a que la solucion es valida */
    sb->is_valid = 1;
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
    if (mq_send(*mq, (char *)*b, sizeof(**b), 1) == -1 && !end)
    {
        printf("Error en el enviado del bloque \n");
        return -1;
    }

    /*b->next = next;*/
    /* Y apunto al siguiente bloque*/
    *b = next;

    sol_found = 0;
    for (int i = 0; i < nd->total_miners - 1; i++)
    {
        sem_post(&nd->start);
    }
    return 0;
}

void shared_block_f5(Block *sb, int wallet)
{
    if (!sb || wallet < 0 || wallet > MAX_MINERS)
    {
        return;
    }
    sb->id++;
    sb->target = sb->solution;
    sb->solution = 0;
    sb->wallets[wallet]++;
}