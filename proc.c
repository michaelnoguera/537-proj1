#define _GNU_SOURCE

#include "proc.h"

#include <assert.h>
#include <dirent.h>  //contains readdir
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gets a list of processes belonging to the current user.
// Returns the processes by value as a stack-allocated 1d array.
void getProcesses() { // TODO should this take a pointer arg to mutate
    struct dirent* process;

    // based off IBM documentation example linked from assignment page https://www.ibm.com/support/knowledgecenter/SSLTBW_2.4.0/com.ibm.zos.v2r4.bpxbd00/rtread.htm
    DIR* proc;
    if ((proc = opendir("/proc/")) == NULL) {
        perror("Error accessing /proc directory.\n");
        exit(EXIT_FAILURE);
    } else {
        while ((process = readdir(proc)) != NULL) {
            //max pid size stored in /proc/sys/kernel/pid_max  

            // not finished
        }
    }
}

// returns the state character for the given process
const char getState(int pid) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/status", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // create statusfile input stream
    FILE* statusfile = fopen(filepath, "r");
    if (statusfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed

    // parse beginning of statusfile
    size_t bufsize = 0;
    char *statusline;
    while(1) {
        // read line into temp heap-alloc'd buffer
        // "If *lineptr is set to NULL and *n is set 0 before the call, 
        // then getline() will allocate a buffer for storing the line.  
        // This buffer should be freed by the user program even if getline() failed." See getline(3)
        statusline = NULL;
        bufsize = 0;
        if (getline(&statusline, &bufsize, statusfile) < 0) {
            perror("Error parsing statusfile or EOF reached without finding status.\n");
            exit(EXIT_FAILURE);
        }
        
		if (strstr(statusline, "State:") != NULL) { // if statusline is the line containing the status
            break; // "State:" line found, parse it
        } else {
            free(statusline); // continue on to next line
        }
	}

    // Parse the "State:" line
    char state;
    if (sscanf(statusline, "State: %c %*s", &state) != 1) {
        perror("Error parsing State: line from /proc/[pid]/status.\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(statusfile);
    free(statusline);
    return state;
}

// returns the number of virtual memory pages used by the given process
const int getVirtMemory(int pid) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/statm", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // attempt to access statm file
    // TODO: Change to status file? First number in statm file != VmSize in /proc/[pid]/status
    FILE* statmfile = fopen(filepath, "r");
    if (statmfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed

    // parse beginning of statusfile
    int pages;
    fscanf(statmfile, "%d", &pages);

    // cleanup and validity check
    fclose(statmfile);

    return pages;
}

const int getUserTime(int pid);

const int getSystemTime(int pid);