// CS 537 Programming Assignment 1 (Fall 2020)
// Michael Noguera and Julien de Castelnau


/* main: parses command line arguments and constructs output
 * returns 0 upon success
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

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

int main(int argc, char *argv[]) {
    int pid;
    int opt;

    struct Flags {
        int singleChar;
        int userTime;
        int systemTime;
        int virtMemory;
        int command;
        int procMem;
    };

    struct Flags psFlags = {
        .singleChar = 0,
        .userTime = 1,
        .systemTime = 0,
        .virtMemory = 0,
        .command = 1,
        .procMem = 0,
    };
    while ((opt = getopt(argc, argv, "p:s::S::v::c::")) != -1) {
        switch ((char) opt) {
            case 'p':
                printf("%s", "p was passed");
                break;
            case 's':
                flipOption(optarg, &psFlags.singleChar);
                break;
            case 'U':
                flipOption(optarg, &psFlags.userTime);
                break;
            case 'S':
                flipOption(optarg, &psFlags.systemTime);
                break;
            case 'v':
                flipOption(optarg, &psFlags.virtMemory);
                break;
            case 'c':
                flipOption(optarg, &psFlags.command);
                break;
            case 'm':
                // 'm' option has arguments itself, parsing for these options will need to be added.
                psFlags.procMem = 1;
                break;
            // TODO: Remove. 
            // Left here if necessary for some reason, for now it doesn't look like it's needed.
            /*case ':': // an argument that accepts an option was called without an option provided
                switch (opt) {
                    case 's': psFlags.singleChar = 1; break;
                    case 'U': psFlags.userTime = 1; break;
                    case 'S': psFlags.systemTime = 1; break;
                    case 'v': psFlags.virtMemory = 1; break;
                    case 'c': psFlags.command = 1; break;
                    case 'p': printf("error: list of process IDs must follow -p\n"); break;
                }*/
            default:
                printf("Usage: 537ps [-p <pid>] [-s] [-U] [-S] [-v] [-c] [-m <addr> <len>]\n");
                exit(EXIT_FAILURE);
        }
    }
    printf("s: %d\nU: %d\nS: %d\nv: %d\nc: %d\n", psFlags.singleChar,psFlags.userTime,psFlags.systemTime,psFlags.virtMemory,psFlags.command);
}