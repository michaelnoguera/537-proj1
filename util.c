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
        }
    } else {
        *opt = 1;
    }
}

