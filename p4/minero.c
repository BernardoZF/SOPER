#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "miner.h"

#define PRIME 99997669

long int solucion = 0;

typedef struct worker_data{
    long int target;
    long int to_check;
}Wd;

void * work_mine(void * arg){
    Wd * wd;
    wd = arg;
    if(wd->target == simple_hash(wd->to_check)){
        solucion = wd->to_check ;
    }else{
        wd->to_check++;
    }

    return NULL;
}



int main(int argc, char **argv){

    int n_workers;
    pthread_t *workers = NULL; 
    Wd * workers_data = NULL;
    int ciclo = 0;

    if(argc != 3){
        printf("Parametros de entrada erroneos\n");
        return -1;
    }
    
    n_workers = atoi(argv[1]);
    if(n_workers < 1){
        printf("Numero de hilos menor que uno por favor introduzca un numero valido \n");
        return -1;
    }else if(n_workers >MAX_WORKERS){
        printf("Numero de hilos mayor que el maximo favor introduzca un numero valido \n");
        return -1;
    }

    workers = malloc(sizeof(pthread_t) * n_workers);
    if(!workers){
        printf("ERROR al reservar memoria para los trabajadores");
        return -1;
    }

    workers_data =(Wd *) malloc(sizeof(Wd)* n_workers);
    if(!workers_data){
        free(workers);
        return -1;
    }

    
    long section = PRIME / n_workers;

    for(int i = 0 ; i < n_workers ; i++){
        workers_data[i].target = 1; // cambiarlo por el target buscado par cada caso
        workers_data[i].to_check = section * i;
    }
    while(solucion == 0){
        for(int i = 0; i < n_workers; i++){
            pthread_create(&workers[i],NULL, work_mine, &workers_data[i]);
            pthread_join(workers[i], NULL);
        }
        printf("ciclo %d \r", ciclo++);
    }
    printf("SOLUCION == %ld", solucion);

}