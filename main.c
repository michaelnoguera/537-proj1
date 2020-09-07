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

int main(int argc, char *argv[]) {
    int pid;
    int opt;

    struct Flags {
        int singleChar;
        int userTime;
        int systemTime;
        int virtMemory;
        int command;
    };

    struct Flags psFlags = {
        .singleChar = 0,
        .userTime = 1,
        .systemTime = 0,
        .virtMemory = 0,
        .command = 1,
    };

    while ((opt = getopt(argc, argv, "p:s:S:v:c:")) != -1) {
        switch ((char) opt) {
            case 'p':
                printf("%s", "p was passed");
                break;
            case 's':
                if (strcmp(optarg, "-")) {
                    psFlags.singleChar = 0;
                }
                break;
            case 'U':
                if (strcmp(optarg, "-")) {
                    psFlags.userTime = 0;
                }
                break;
            case 'S':
                if (strcmp(optarg, "-")) {
                    psFlags.systemTime = 0;
                }
                break;
            case 'v':
                if (strcmp(optarg, "-")) {
                    psFlags.virtMemory = 0;
                }
                break;
            case 'c':
                if (strcmp(optarg, "-")) {
                    psFlags.command = 0;
                }
                break;
            case ':': // an argument that accepts an option was called without an option provided
                switch (opt) {
                    case 's': psFlags.singleChar = 1; break;
                    case 'U': psFlags.userTime = 1; break;
                    case 'S': psFlags.systemTime = 1; break;
                    case 'v': psFlags.virtMemory = 1; break;
                    case 'c': psFlags.command = 1; break;
                    case 'p': printf("error: list of process IDs must follow -p\n"); break;
                }
            default:
                printf("Usage: %s [-t nsecs] [-n] name\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("s: %d\nU: %d\nS: %d\nv: %d\nc: %d\n", psFlags.singleChar,psFlags.userTime,psFlags.systemTime,psFlags.virtMemory,psFlags.command);
}