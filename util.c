#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"

void flipOption(char *optarg, int *opt) {
    if (optarg != 0) 
    {
        if (strcmp(optarg,"-") == 0) {
            *opt = 0;
        }
    } else {
        *opt = 1;
    }
}

void push(pidslist_t** pl, int pid) {
    pidslist_t* newPidNode;
    newPidNode = (pidslist_t*) malloc(sizeof(pidslist_t));

    newPidNode->pid = pid;
    newPidNode->next = *pl;
    *pl = newPidNode;
}

void traverse(pidslist_t* pl) {
    pidslist_t* pidTemp = pl;
    while (pidTemp != NULL) {
        printf("%d\n", pidTemp->pid);
        pidTemp = pidTemp->next;
    }   
}