#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_PALABRAS 10
#define MAX_LINE 300

typedef struct _comando{
    char *line;
    char *word[MAX_PALABRAS];
} comando;

void* trocear(void * arg){
    comando *l = arg;
    char delim[2] = " ";
    char * token;
    int i = 0;
    token  = strtok(l->line, delim);
    l->word[i] = token;
    while ( l->word[i]){
       
       i++;
       l->word[i] = strtok(NULL, delim);
      
    }
    /* pthread_exit(NULL); preguntar */
    return NULL;
}

int main(int argc, char **argv){
    int fd[2];
    char readbuffer[300];
    pthread_t procesar;
    size_t len;
    char * line;
    int nbytes;
    int error;
    int pipe_status;
    int status= 0;
    pid_t pid;
    pid_t log;
    comando l;
    FILE *pf;

 
    

    pipe_status = pipe(fd);
	if (pipe_status == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

    log = fork();
    if(log < 0 ){
        perror("fork");
		exit(EXIT_FAILURE);
    }
    if (log == 0 ){

        pf = fopen ("log.txt","w");
        if(!pf){
            exit(EXIT_FAILURE);
        }
        /* Cierre del descriptor de salida en el hijo */
		close(fd[1]);
		do
        {   
            
            nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
		    if (nbytes == -1) {
			    perror("read");
			    exit(EXIT_FAILURE);
		    }
            if(nbytes>0){
                
                fprintf(pf, "%s", readbuffer);
                fflush(pf);
            }

        } while (nbytes!=0);
        
        fclose(pf);

		exit(EXIT_SUCCESS);
    }
    else{
        close(fd[0]);
        l.line = (char * )malloc(MAX_LINE * sizeof(char));
        while(fgets(l.line, MAX_LINE, stdin)){
            
            nbytes = dprintf(fd[1],"%s", l.line);
		    if (nbytes == -1) {
			    perror("write");
			    exit(EXIT_FAILURE);
		    } 

            l.line = strtok(l.line, "\n"); /* Al programa no le gusta el \n y a mi tampoco*/
            

            error = pthread_create(&procesar, NULL, trocear, &l);
            if (error != 0) {
		        fprintf(stderr, "ERROR en pthread_create: %s\n", strerror(error));
		        exit(EXIT_FAILURE);
	        }
        
        
            error = pthread_join(procesar, NULL);
            if (error != 0) {
		        fprintf(stderr, "ERROR en pthread_join: %s\n", strerror(error));
		        exit(EXIT_FAILURE);
	        }

            pid = fork();
            if(pid < 0){
                perror("fork");
		        exit(EXIT_FAILURE);
            }
            if(pid == 0 ){
                if((status = execvp(l.word[0], l.word))){
                    perror("execvp");
                    exit(EXIT_FAILURE);
              }
            }
            else{
                waitpid(pid, &status, 0);
                if(WIFEXITED(status)){
                    
                    dprintf(fd[1], "Exited with value: %d\n", WEXITSTATUS(status));
                    fflush(NULL);
                
                }
                if(WIFSIGNALED(status)){
                    
                    dprintf(fd[1], "Terminated by signal: %d\n", WTERMSIG(status));
                    fflush(NULL);
                }

            }
        
        


        }
        close(fd[1]);
        waitpid(log,NULL,0);
    }
    

}
