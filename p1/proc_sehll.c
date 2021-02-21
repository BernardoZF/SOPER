#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_PALABRAS 10

typedef struct _comando{
    char * line;
    char *word[MAX_PALABRAS];
} comando;

void trocear(void * arg){
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
}

int main(int argc, char **argv){
    pthread_t procesar;
    size_t len;
    char * line;
    int error;
    comando *l=NULL;

    l =(comando *) malloc(sizeof(comando));
    if(!l){
        exit(EXIT_FAILURE);
    }

    while(getline(&l.line, &len, stdin) != EOF ){
        error = pthread_create(procesar, NULL, trocear, l);
        if (error != 0) {
		    fprintf(stderr, "pthread_create: %s\n", strerror(error));
		    pthread_exit(NULL);
	    }
        
        
        sleep(MAX_PALABRAS/5);
        
        
        
    }

}
