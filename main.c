/** 
 * CS 537 Programming Assignment 1 (Fall 2020)
 * @authors Michael Noguera and Julien de Castelnau
 * @date 9/23/2020
 *
 * @file main.c
 * @brief parses command line arguments, constructs and prints output
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

/// Contains boolean values for each flag the program handles
typedef struct Flags_t {
    bool singleChar; ///< `-s` status character
    bool userTime; ///< `-U` user time
    bool systemTime; ///< `-S` system time
    bool virtMemory; ///< `-v` current virtual memory usage
    bool command; ///< `-c` command-line that started program
    bool procMem; ///< extra credit: `-m` contents of process's memory
} Flags;

/**
 * Helper function used for parsing command line flags. 
 * 
 * Toggles an option to the opposite of its current value if the option is
 * specified with a trailing dash.
 * 
 * @param optarg provided by getopt, the flag in question and its arguments
 * @param opt boolean flag representing the option's status. Will be toggled 
 * if needed.
 */
static void flipOption(const char *optarg, bool *opt) {
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

/**
 * Main. Parses command line args and handles output.
 * 
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @return EXIT_SUCCESS upon success, EXIT_FAILURE upon error.
 */
int main(int argc, char *argv[]) {
    // Initialize linked list to hold the list of pids.
    linkedlist *pids = ll_initialize();
    unsigned long mem_addr = 0;
    int mem_len = 0;

    // Store the program's option flags, 1 = on & 0 = off.
    // Default values are encoded in this step.
    Flags psFlags = {
        .singleChar = false,
        .userTime = true,
        .systemTime = false,
        .virtMemory = false,
        .command = true,
        .procMem = false,
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
            
            ll_push(pids, pid); // add pid to list (extra credit: support multiple `-p` flags)
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
            psFlags.procMem = true;
            // For -m we check that the correct number of arguments have been supplied,
            // and attempt to parse. That is, there should be at least 2 valid arguments
            // after "-m" was parsed (optind-1).
            if (optind < argc) {
                mem_addr = strtoul(argv[optind - 1], NULL, 16);
                mem_len = (int)strtol(argv[optind], NULL, 10);
            }
            // If any error has occured in parsing both optargs, issue an error.
            if (mem_addr == 0 || mem_len < 0) {
                printf("Specify a valid (positive) amount of memory at a valid (nonzero) ");
                printf("memory address in hex and length in decimal with \'-m\'.\n");
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

    // `-m` is only supported for one process at a time
    if (pids->size != 1 && psFlags.procMem) {
        printf("The `-m` option is incompatible with multiple processes.\n");
        printf("Please specify only one pid by using the `-p` flag.\n");
        exit(EXIT_FAILURE);
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

    ll_free(pids); // destruct linked list and free all memory

    exit(EXIT_SUCCESS);
}