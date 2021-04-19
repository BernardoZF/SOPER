#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
    pid_t server = 1, client=1;

    if(argc != 3){
        return -1;
    }
    
    shm_unlink (SHM_NAME);

    /* Creation of the shared memory. */
    if ((fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL,  S_IRUSR | S_IWUSR)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    printf("Archivo creado con exito\n");

    /* Resize of the memory segment. */
    if (ftruncate(fd_shm, sizeof(so)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    } printf("memoria truncada con exito\n");

    /* Mapping of the memory segment. */
    so = mmap(NULL, sizeof(so), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (so == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    printf("Memoria enlazada con exito\n");
    printf("Pointer to shared memory segment: %p\n", (void*)so);

    /* Initialization of the memory. */
    so->get_pos = 0;
    so->post_pos = 0;
    for(i = 0; i<5;i++){
        so->bf[i]='a';
    }

    /*printf("Entrando creacion primer semaforo\n");
    if(sem_init(so->sem_mutex, 1, 1) == -1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
        //CDE
    }
    printf("despues de crear primer semaforo\n");
    if(sem_init(so->sem_fill, 1, 0) == -1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        sem_destroy(so->sem_mutex);
        exit(EXIT_FAILURE);
        //CDE
    }
    if(sem_init(so->sem_empty, 1, 5)==-1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        sem_destroy(so->sem_mutex);
        sem_destroy(so->sem_fill);
        exit(EXIT_FAILURE);
        //CDE
    }*/

    server = fork();
    if(server<0){
        return;
        //CDE
    }
    if(server == 0){
        //EXEC SERVER
        printf("exec server lanzado\n");
        execl("stream-server", "stream-server", argv[1], (char *)so, (char * )NULL);
    }else{
        client = fork();
        if(client<0){
            return -1;
        }
        else if(client == 0){
            //EXEC cliente
            printf("exec cliente lanzado\n");
            execl("stream-client", "stream-client", argv[2], (char * )NULL);

        }else{
            wait(NULL);
            wait(NULL);
        }
    }
    

    


    

    /* Unmapping and freeing of the shared memory */
    munmap(so, sizeof(so));
    shm_unlink(SHM_NAME);
    /*sem_destroy(so->sem_mutex);
    sem_destroy(so->sem_fill);
    sem_destroy(so->sem_empty);*/
    

    exit(EXIT_SUCCESS);
}