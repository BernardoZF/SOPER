#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <mqueue.h>



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
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = 5*sizeof(char)
    };
    

    if(argc != 4){
        printf("mal argumentos en el servidor\n");
        return 0;
    }

    mqd_t client = mq_open(argv[3],
        O_CREAT | O_RDONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if (client == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    pf = fopen(argv[1], "w");
    if(!pf){
        return -1;
    }

   
    /* Open of the shared memory. */
    if ((fd_shm = shm_open(argv[2], O_RDWR, 0)) == -1) {
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
    printf("antes del bucle cliente\n");
    while(1){
        printf("esperando el mensaje en cliente\n");
        char entrada[5];
        if (mq_receive(client, entrada, sizeof(entrada), NULL) == -1) {
            fprintf(stderr, "Error receiving message\n");
            return EXIT_FAILURE;
        }
        if(strcmp(entrada, "exit")==0){
            break;
            
        }
        if(so->bf[so->get_pos] == '\0'){
            break;
        }
        else{
           if (clock_gettime(CLOCK_REALTIME, &t) == -1){   
                printf("Error estableciendo el tiempo de espera\n");
                return 0;
            }

            t.tv_sec += 2;
            if(sem_timedwait(&so->sem_fill, &t)){
                printf("Error cola llena\n");
            }else{
                sem_wait(&so->sem_mutex);
                printf("%c\n", so->bf[so->get_pos]);
                fputc(so->bf[so->get_pos], pf);
                sem_post(&so->sem_mutex);
                so->get_pos = (so->get_pos + 1) % 5;
                sem_post(&so->sem_empty);
            } 
        }
        

    }

    printf("fin del bucle de cliente\n");

   
    

    /* Unmapping of the shared memory */
    munmap(so, sizeof(so));
    mq_close(client);

    exit(EXIT_SUCCESS);
}