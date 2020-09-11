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

#include "util.h"
#include "linkedlist.h"

struct Flags {
    int singleChar;
    int userTime;
    int systemTime;
    int virtMemory;
    int command;
    int procMem;
};   

int main(int argc, char *argv[]) {
    linkedlist* pids = ll_initialize();
    ll_push(pids, -1);
    int all_pids = 1;
    int pid;
    int opt;
    int mem_addr = 0;
    int mem_len = 0;

    struct Flags psFlags = {
        .singleChar = 0,
        .userTime = 1,
        .systemTime = 0,
        .virtMemory = 0,
        .command = 1,
        .procMem = 0,
    };

    while ((opt = getopt(argc, argv, "p:s::S::v::c::m:")) != -1) {
        switch ((char) opt) {
            case 'p':
                all_pids = 0;
                pid = atoi(optarg);
                if (pid == 0) {
                    printf("Enter a valid (nonzero) process ID (pid) after -p.\n");
                    exit(EXIT_FAILURE);
                } else {
                    ll_push(pids,pid);
                }
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
                if (optind < argc) {
                    // Add error handling case? The assignment doesn't specify if it's okay for the program to *also* accept hex addresses without '0x'.
                    mem_addr = (int)strtol(argv[optind-1], NULL, 16);
                    mem_len = atoi(argv[optind]);
                }
                if (mem_addr == 0 || mem_len == 0) {
                    printf("Enter a valid (nonzero) memory address in hex and length in decimal with \'-m\'.\n");
                    exit(EXIT_FAILURE);
                }
                printf("addr: %d, len: %d\n", mem_addr, mem_len);
                break;
            default:
                printf("Usage: 537ps [-p <pid>] [-s] [-U] [-S] [-v] [-c] [-m <addr> <len>]\n");
                exit(EXIT_FAILURE);
        }
    }
    printf("s: %d\nU: %d\nS: %d\nv: %d\nc: %d\n", psFlags.singleChar,psFlags.userTime,psFlags.systemTime,psFlags.virtMemory,psFlags.command);

    // Call print funcs
    printf("List of PIDs:\n");
    ll_print(pids);

    ll_free(pids);
}