#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <mqueue.h>

#define SHM_NAME "/shm_example"
#define SERVER "/server"
#define CLIENT "/client"

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
    So *so = NULL;
    pid_t server = 1, client=1;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = 5*sizeof(char)
    };

    if(argc != 3){
        return -1;
    }
    
    shm_unlink (SHM_NAME);

    mqd_t serverq = mq_open(SERVER,
        O_CREAT | O_WRONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if (serverq == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    mqd_t clientq = mq_open(CLIENT,
        O_CREAT | O_WRONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if (clientq == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    /* Creation of the shared memory. */
    if ((fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL,  S_IRUSR | S_IWUSR)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    printf("Archivo creado con exito\n");

    /* Resize of the memory segment. */
    if (ftruncate(fd_shm, sizeof(So)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    } printf("memoria truncada con exito\n");

    /* Mapping of the memory segment. */
    so = (So *)mmap(NULL, sizeof(So), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
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
    

    printf("Entrando creacion primer semaforo\n");
    if(sem_init(&so->sem_mutex, 1, 1) == -1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
        //CDE
    }
    printf("despues de crear primer semaforo\n");
    if(sem_init(&so->sem_fill, 1, 0) == -1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        sem_destroy(&so->sem_mutex);
        exit(EXIT_FAILURE);
        //CDE
    }
    if(sem_init(&so->sem_empty, 1, 5)==-1){
        munmap(so, sizeof(so));
        shm_unlink(SHM_NAME);
        sem_destroy(&so->sem_mutex);
        sem_destroy(&so->sem_fill);
        exit(EXIT_FAILURE);
        //CDE
    }

    server = fork();
    if(server<0){
        return -1;
        //CDE
    }
    if(server == 0){
        //EXEC SERVER
        printf("exec server lanzado\n");
        execl("stream-server", "stream-server", argv[1], SHM_NAME, SERVER, (char * )NULL);
    }else{
        client = fork();
        if(client<0){
            return -1;
        }
        else if(client == 0){
            //EXEC cliente
            printf("exec cliente lanzado\n");
            execl("stream-client", "stream-client", argv[2], SHM_NAME, CLIENT, (char * )NULL);

        }else{
            while(1){
               char entrada[5];
               fgets(entrada, sizeof(entrada),stdin);
                if(strcmp(entrada, "post")==0){
                                        printf("entrada == post \n");
                    if (mq_send(serverq, entrada, strlen(entrada) + 1, 1) == -1) {
                        perror("mq_send");
                        mq_close(server);
                        mq_close(client);
                        exit(EXIT_FAILURE);
                    }

                }else if (strcmp(entrada, "get\n") == 0){
                    printf("entrada == get \n");
                    if (mq_send(clientq, entrada, strlen(entrada), 1) == -1) {
                        perror("mq_send");
                        mq_close(server);
                        mq_close(client);
                        exit(EXIT_FAILURE);
                    }
                    printf("enviado el mensaje en la cola client\n");
                }
                else if(strcmp(entrada, "exit")==0){
                     if (mq_send(serverq, entrada, strlen(entrada) + 1, 1) == -1) {
                        perror("mq_send");
                        mq_close(server);
                        mq_close(client);
                        exit(EXIT_FAILURE);
                    }
                    if (mq_send(clientq, entrada, strlen(entrada) + 1, 1) == -1) {
                        perror("mq_send");
                        mq_close(server);
                        mq_close(client);
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
            }
            wait(NULL);
            wait(NULL);
        }
    }
    

    


    

    /* Unmapping and freeing of the shared memory */
    munmap(so, sizeof(so));
    shm_unlink(SHM_NAME);
    mq_unlink(SERVER);
    mq_unlink(CLIENT);
    mq_close(server);
    mq_close(client);
    sem_destroy(&so->sem_mutex);
    sem_destroy(&so->sem_fill);
    sem_destroy(&so->sem_empty);
    
    

    exit(EXIT_SUCCESS);
}