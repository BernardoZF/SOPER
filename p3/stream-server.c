#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/shm_example"

typedef struct _so{
    char bf[5];
    int post_pos;
    int get_pos;
}so;

int main(void) {
    int i, fd_shm;
    so *so = NULL;
    FILE *pf;

    if(argc != 2){
        return -1;
    }

    pf = fopen(argv[1], "r");
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



    /*fgetc*/

    /* Unmapping of the shared memory */
    munmap(so, sizeof(so));

    exit(EXIT_SUCCESS);
}