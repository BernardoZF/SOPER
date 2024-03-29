#include <stdio.h>
#include <stdlib.h>
#include "miner.h"

#define PRIME 99997669
#define BIG_X 435679812
#define BIG_Y 100001819

long int simple_hash(long int number) {
    long int result = (number * BIG_X + BIG_Y) % PRIME;
    return result;
}

void print_blocks(Block *plast_block, int num_wallets) {
    Block *block = NULL;
    int i, j;

    for(i = 0, block = plast_block; block != NULL; block = block->prev, i++) {
        printf("Block number: %d; Target: %ld;    Solution: %ld\n", block->id, block->target, block->solution);
        for(j = 0; j < num_wallets; j++) {
            printf("%d: %d;         ", j, block->wallets[j]);
        }
        printf("\n\n\n");
    }
    printf("A total of %d blocks were printed\n", i);
}

void print_blocks_to_file(Block *plast_block, int num_wallets, FILE  *pf) {
    Block *block = NULL;
    int i, j;

    for(i = 0, block = plast_block; block != NULL; block = block->prev, i++) {
        fprintf(pf, "Block number: %d; Target: %ld;    Solution: %ld\n", block->id, block->target, block->solution);
        fflush(pf);
        for(j = 0; j < num_wallets; j++) {
            fprintf(pf, "%d: %d;         ", j, block->wallets[j]);
            fflush(pf);
        }
        fprintf(pf,"\n\n\n");
        fflush(pf);
    }
    /*fprintf(pf, "A total of %d blocks were printed\n", i);
    fflush(pf);*/
}

void blocks_free(Block *plast_block) {
    Block * block;
    Block * aux;
    block = plast_block;
    while(block->prev != NULL){
        aux = block->prev;
        free(block);
        block = aux;
    }
    free(block);     
}
/*int main(int argc, char *argv[]) {
    long int i, target;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <TARGET>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    target = atol(argv[1]);
    for (i = 0; i < PRIME; i++) {
        fprintf(stdout, "Searching... %6.2f%%\r", 100.0 * i / PRIME);
        if (target == simple_hash(i)) {
            fprintf(stdout, "\nSolution: %ld\n", i);
            exit(EXIT_SUCCESS);
        }
    }
    fprintf(stderr, "\nSearch failed\n");
    exit(EXIT_FAILURE);
}*/