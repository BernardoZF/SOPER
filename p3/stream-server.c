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
    So *so = NULL;
    FILE *pf;
    char c;
    char entrada[5];
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

    mqd_t server = mq_open(argv[3],
        O_CREAT | O_RDONLY, /* This process is only going to send messages */
        S_IRUSR | S_IWUSR, /* The user can read and write */
        &attributes);

    if (server == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        return EXIT_FAILURE;
    }

    pf = fopen(argv[1], "r");
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



    while((c = fgetc(pf)) != EOF){
        
        if (mq_receive(server, entrada, sizeof(entrada), NULL) == -1) {
            fprintf(stderr, "Error receiving message\n");
            return EXIT_FAILURE;
        }
        if(strcmp(entrada, "post")==0){
            if (clock_gettime(CLOCK_REALTIME, &t) == -1){   
                printf("Error estableciendo el tiempo de espera\n");
                return -1;
            }

            t.tv_sec += 2;
            if(sem_timedwait(&so->sem_empty, &t)){
                printf("Error en llenado de buffer\n");
                return 0;
            }
            sem_wait(&so->sem_mutex);
            so->bf[so->post_pos] = (char )c;
            sem_post(&so->sem_mutex);
            so->post_pos = (so->post_pos +1) % 5;
            sem_post(&so->sem_fill);
        }else{
            break;
        }
       
    }
    so->bf[so->post_pos] = '\0';


    /* Unmapping of the shared memory */
    munmap(so, sizeof(*so));
    mq_close(server);

    exit(EXIT_SUCCESS);
}