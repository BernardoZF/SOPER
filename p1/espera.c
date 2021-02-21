#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv){
    time_t espera=0;
    printf("inicio\n");
    /*espera = clock();
    while((clock()- espera)<(10 * CLOCKS_PER_SEC));*/
    sleep(10);
   printf("fin\n");
  return 0;
}