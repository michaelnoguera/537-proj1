#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"

// toggles a command line option to the opposite of it's current value if
// specified with a trailing dash
void flipOption(char *optarg, int *opt) {
    if (optarg != 0) 
    {
        if (strcmp(optarg,"-") == 0) {
            *opt = 0;
        } else {
            printf("Cannot group options.\n");
            printf("Usage: 537ps [-p <pid>] [-s] [-U] [-S] [-v] [-c] [-m <addr> <len>]\n");
            exit(EXIT_FAILURE);
        }
    } else {
        *opt = 1;
    }
}

