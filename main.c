// CS 537 Programming Assignment 1 (Fall 2020)
// Michael Noguera and Julien de Castelnau

/* main: parses command line arguments and constructs output
 * exits with code 0 upon success
 */

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linkedlist.h"
#include "proc.h"

// Struct containing boolean values for each flag the program handles
typedef struct Flags_t {
    bool singleChar;
    bool userTime;
    bool systemTime;
    bool virtMemory;
    bool command;
    bool procMem;
} Flags;

// toggles a command line option to the opposite of its current value if
// specified with a trailing dash
static void flipOption(char *optarg, bool *opt) {
    if (optarg != 0) {
        if (strcmp(optarg, "-") == 0) {
            *opt = false;
        } else {
            printf("Cannot group options.\n");
            printf("Usage: 537ps [-p <pid>] [-s] [-U] [-S] [-v] [-c] [-m <addr> <len>]\n");
            exit(EXIT_FAILURE);
        }
    } else {
        *opt = true;
    }
}

int main(int argc, char *argv[]) {
    // Initialize linked list to hold the list of pids.
    linkedlist *pids = ll_initialize();
    unsigned long mem_addr = 0;
    int mem_len = 0;

    // Store the program's option flags, 1 = on & 0 = off.
    // Note: Default values are encoded in this step.
    Flags psFlags = {
        .singleChar = 0,
        .userTime = 1,
        .systemTime = 0,
        .virtMemory = 0,
        .command = 1,
        .procMem = 0,
    };

    int opt = 0;
    while ((opt = getopt(argc, argv, "-p:s::U::S::v::c::m:")) != -1) {
        // PARSE FLAGS
        // For each valid yes/no option (matching the optarg specification), we flip the value of the
        // flag in the psFlags struct depending on if an extra "-" character has been added to the end of the
        // option.
        switch ((char)opt) {
        case 'p':
            errno = 0;
            int pid = (int)strtol(optarg, (char **)NULL, 10);
            // Ensure that integer PID number parsed from optarg is valid, error if not
            if (errno != 0 || pid <= 0) {
                printf("Enter a valid process ID (pid) after -p.\n");
                exit(EXIT_FAILURE);
            }
            // Add pid to Pids linked list.
            ll_push(pids, pid);
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
            psFlags.procMem = 1;
            // For -m we check that the correct number of arguments have been supplied, and attempt to parse.
            // That is, there should be at least 2 valid arguments after "-m" was parsed (optind-1).
            if (optind < argc) {
                mem_addr = strtoul(argv[optind - 1], NULL, 16);
                mem_len = (int)strtol(argv[optind], NULL, 10);
            }
            // If any error has occured in parsing both optargs, issue an error.
            if (mem_addr == 0 || mem_len < 0) {
                printf("Specify a valid (positive) amount of memory at a valid (nonzero) memory address in hex and length in decimal with \'-m\'.\n");
                exit(EXIT_FAILURE);
            }
            optind += 1;
            break;
        case 1:
        default:
            printf("Usage: 537ps [-p <pid>] [-s] [-U] [-S] [-v] [-c] [-m <addr> <len>]\n");
            exit(EXIT_FAILURE);
        }
    }

    // if no pid(s) are specified, find all of the current user's pids
    if (pids->size == 0) {
        getCurrentUserProcesses(&pids);
    }

    if (pids->size != 1 && psFlags.procMem) {
        printf(
            "The -m option is incompatible with multiple processes.\
        Please specify only one pid when using the -p flag.\n");
    }

    // PRINT PROCESS INFO
    node *curr = pids->head;
    for (int i = 0; i < pids->size; i++) {
        printf("%6d: ", curr->value);
        if (psFlags.singleChar) printf("%c ", getState(curr->value));
        if (psFlags.userTime) printf("utime=%lu ", getUserTime(curr->value));
        if (psFlags.systemTime) printf("stime=%lu ", getSystemTime(curr->value));
        if (psFlags.virtMemory) printf("%d ", getVirtMemory(curr->value));
        if (psFlags.command) {
            // For cmdline, we take the str and then free it once it's been printed.
            char *cmdline_str = getCmdline(curr->value);
            printf("[%s] ", cmdline_str);
            free(cmdline_str);
        }
        if (psFlags.procMem) {
            char *memdump = readMem(curr->value, mem_addr, mem_len);
            printf("[%lx: %s]", mem_addr, memdump);
            free(memdump);
        }
        printf("\n");
        curr = curr->next;
    }

    // Free pids linked list recursively.
    ll_free(pids);

    exit(EXIT_SUCCESS);
}