#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "miner.h"

#define PRIME 99997669

long int solution = 0;
int sol_found = 0;

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
    if (wd->cycles < 1)
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
    else
    {
        for (int i = 0; i < wd->cycles; i++)
        {
            if (wd->target == simple_hash(wd->to_check))
            {
                solution = wd->to_check;
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
        b->wallets[0] = 0;
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

int main(int argc, char **argv)
{

    int n_workers;
    int n_cycles;
    pthread_t *workers = NULL;
    Wd *workers_data = NULL;
    Block *b;
    Block *next;

    if (argc != 3)
    {
        printf("Parametros de entrada erroneos\n");
        return -1;
    }

    n_workers = atoi(argv[1]);
    if (n_workers < 1)
    {
        printf("Numero de hilos menor que uno por favor introduzca un numero valido \n");
        return -1;
    }
    else if (n_workers > MAX_WORKERS)
    {
        printf("Numero de hilos mayor que el maximo favor introduzca un numero valido \n");
        return -1;
    }

    n_cycles = atoi(argv[2]);

    workers = malloc(sizeof(pthread_t) * n_workers);
    if (!workers)
    {
        printf("ERROR al reservar memoria para los trabajadores");
        return -1;
    }

    workers_data = (Wd *)malloc(sizeof(Wd) * n_workers);
    if (!workers_data)
    {
        free(workers);
        return -1;
    }

    long section = PRIME / n_workers;
    /* CREACION DEL PRIMER BLOQUE */
    b = create_new_block(NULL);
    if (!b)
    {
        printf("error en la reserva de memoria del bloque");
        free(workers);
        free(workers_data);
        return -1;
    }

    /* Repeticion un numero cualquiera de veces para comprobar si se crean bien todos los bloques */
    for (int a = 0; a < 10; a++)
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
            /* actualizo la solucion */
            b->solution = solution;
            b->is_valid = 1;
            /* Creo el siguiente bloque */
            next = create_new_block(b);
            if (!next)
            {
                printf("error en la reserva de memoria del bloque");
                free(workers);
                free(workers_data);
                return -1;
            }
            /* Establezco el siguiente bloque */
            b->next = next;
            /* Y apunto al siguiente bloque*/
            b = b->next;

            sol_found = 0;
        }
    }

    print_blocks(b, 1);

    exit(EXIT_SUCCESS);
}