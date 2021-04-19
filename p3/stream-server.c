#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>


#define SHM_NAME "/shm_example"

typedef struct _so{
    char bf[5];
    int post_pos;
    int get_pos;
    sem_t *sem_mutex;
    sem_t *sem_empty; 
    sem_t *sem_fill;
}so;




int main(int argc, char *argv[]) {
    int i, fd_shm;
    so *so = NULL;
    FILE *pf;
    char c;
    struct timespec *t;
    t->tv_sec = 2;
    t->tv_nsec = 0;   
    

    printf("entrando progrma servidor\n");
    
    if(argc != 3){
        printf("mal argumentos en el servidor\n");
        return -1;
    }

    pf = fopen(argv[1], "r");
    if(!pf){
        return -1;
    }
    so = (so *) argv[3];
    /* Open of the shared memory. */
    /*if ((fd_shm = shm_open(SHM_NAME, O_RDONLY, 0)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }*/

    /* Mapping of the memory segment. */
    /*so = mmap(NULL, sizeof(so), PROT_READ, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (so == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }*/
    printf("Pointer to shared memory segment: %p\n", (void*)so);



    printf("Antes del bucle servidor\n");
    /*fgetc*/
    while((c = fgetc(pf)) != EOF){
        /*if(sem_timedwait(so->sem_empty, t)){
            printf("Error en llenado de buffer");
            return 0;
        }
        sem_wait(so->sem_mutex);*/
        printf("%c\n", (char )c);
        so->bf[so->post_pos] = (char )c;
        printf("hola\n");
        so->post_pos = (so->post_pos +1) % 5;
        /*sem_post(so->sem_mutex);
        sem_post(so->sem_fill);*/
    }
    sleep(2);
    so->bf[so->post_pos] = '\0';
    
    /* Unmapping of the shared memory */
    /*munmap(so, sizeof(so));*/

    exit(EXIT_SUCCESS);
}