#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    pid_t server = 1, client=1;

    if(argc != 3){
        return -1;
    }
    


    /* Creation of the shared memory. */
    if ((fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL,  S_IRUSR | S_IWUSR)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /* Resize of the memory segment. */
    if (ftruncate(fd_shm, sizeof(so)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* Mapping of the memory segment. */
    so = mmap(NULL, sizeof(so), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (so == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    printf("Pointer to shared memory segment: %p\n", (void*)so);

    /* Initialization of the memory. */
    so->get_pos = 0;
    so->post_pos = 0;

    server = fork();
    if(server<0){
        //CDE
    }
    if(server != 0){
        client = fork();
    }
    else{
        //EXEC SERVER
    }
    if(client == 0){
        //EXEC cliente
    }

    if(server != 0){
        waitpid(server);
        waitpid(client);
    }


    

    /* Unmapping and freeing of the shared memory */
    munmap(so, sizeof(so));
    shm_unlink(SHM_NAME);
    

    exit(EXIT_SUCCESS);
}