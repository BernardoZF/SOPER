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
    char line[MAX_LINE];
    char *word[MAX_PALABRAS];
} comando;

void* trocear(void * arg){
    comando *l = arg;
    char delim[2] = " ";
    char * token;
    
    for(int i = 0; i<MAX_PALABRAS;i++){
        token  = strtok(l->line, delim);
        if(token != NULL){
            strcpy (l->word[i],  token);
        }else{
            l->word[i] = NULL;
        }
    }
    /* pthread_exit(NULL); preguntar */
    return NULL;
}

int main(int argc, char **argv){
    pthread_t procesar;
    size_t len;
    char * line;
    int error;
    int status= 0;
    pid_t pid;
    pid_t hijo_a_esperar;
    comando *l=NULL;

    l =(comando *) malloc(sizeof(comando));
    if(!l){
        exit(EXIT_FAILURE);
    }

    while(getline(&l->line, &len, stdin) != EOF ){
        error = pthread_create(&procesar, NULL, trocear, l);
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
            if(execvp(l->word[0], l->word)){
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            hijo_a_esperar = getpid();
        }
        else{
            sleep(1);
            waitpid(hijo_a_esperar, &status, 0);
            if(WIFEXITED(status)!=0){
                fprintf(stderr, "Terminated by signal: %d", WIFSIGNALED(status));
            }else {
                fprintf(stderr, "Exited with value: %d", WEXITSTATUS(status));
            }

        }
        
        


    }

}
