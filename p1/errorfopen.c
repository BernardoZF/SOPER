#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv){
    FILE *fp= NULL;
    int aux;

    if(argc!= 2){
        return -1;
    }

    fp= fopen(argv[1], "r");
    aux = errno;
    if(!fp){
        printf("%d\n", aux);
        perror("Error ");
  }
  return 0;
}