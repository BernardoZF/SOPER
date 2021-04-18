#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/shm_example"

typedef struct _so{
    char bf[5];
    int post_pos;
    int get_pos;
    sem_t sem_mutex;
    sem_t sem_empty;
    sem_t sem_fill;
}so;

int main(int argc, char *argv[]) {
    int i, fd_shm;
    so *so = NULL;
    FILE *pf;

    if(argc != 2){
        return -1;
    }
    pf = fopen(argv[1], "w");
    if(!pf){
        return -1;
    }

    /* Open of the shared memory. */
    if ((fd_shm = shm_open(SHM_NAME, O_RDONLY, 0)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /* Mapping of the memory segment. */
    so = mmap(NULL, sizeof(so), PROT_READ, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (so == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }


    /*fputc*/
    while(so->bf[so->get_pos] != '\0'){
        if(sem_timedwait(so->sem_fill, 2)){
            printf("Error cola llena");
            return 0;
        }
        sem_wait(so->sem_mutex);
            fputc(so->bf[so->get_pos], pf);
            so->get_pos = (so->get_pos+1)%5;
        sem_post(so->sem_mutex);
        sem_post(so->sem_empty);
    }

    fputc('\0', pf);

    

    /* Unmapping of the shared memory */
    munmap(so, sizeof(so));

    exit(EXIT_SUCCESS);
}