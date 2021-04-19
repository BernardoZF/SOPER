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
    sem_t sem_mutex;
    sem_t sem_empty; 
    sem_t sem_fill;
}So;



int main(int argc, char *argv[]) {
    int fd_shm;
    So *so;
    FILE *pf = NULL;
    struct timespec t;
    

    if(argc != 2){
        printf("mal argumentos en el servidor\n");
        return 0;
    }

    pf = fopen(argv[1], "w");
    if(!pf){
        return -1;
    }

    /* Open of the shared memory. */
    if ((fd_shm = shm_open(SHM_NAME, O_RDWR, 0)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /* Mapping of the memory segment. */
    so = (So *)mmap(NULL, sizeof(*so), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (so == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }



    /*fputc*/
  
    while(so->bf[so->get_pos] != '\0'){
        if (clock_gettime(CLOCK_REALTIME, &t) == -1){   
            printf("Error estableciendo el tiempo de espera\n");
            return 0;
        }

        t.tv_sec += 2;
        if(sem_timedwait(&so->sem_fill, &t)){
            printf("Error cola llena\n");
            return 0;
        }
        sem_wait(&so->sem_mutex);
            fputc(so->bf[so->get_pos], pf);
        sem_post(&so->sem_mutex);
        so->get_pos = (so->get_pos+1)%5;
        sem_post(&so->sem_empty);
    }

   
    

    /* Unmapping of the shared memory */
    munmap(so, sizeof(so));

    exit(EXIT_SUCCESS);
}