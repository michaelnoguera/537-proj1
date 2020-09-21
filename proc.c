#define _GNU_SOURCE

#include <assert.h>
#include <dirent.h>  //contains readdir
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "proc.h"

// returns the state character for the given process
char getState(int pid) {
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

// returns the real UID (uid of the user that created the process) for a given pid
int getUid(int pid) {
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
            perror("Error parsing statusfile or EOF reached without finding Uid.\n");
            exit(EXIT_FAILURE);
        }
        
		if (strstr(statusline, "Uid:") != NULL) { // if statusline is the line containing the status
            break; // "State:" line found, parse it
        } else {
            free(statusline); // continue on to next line
        }
	}

    // Parse the "State:" line
    int uid;
    if (sscanf(statusline, "Uid: %d %*d %*d %*d", &uid) != 1) {
        perror("Error parsing Uid: line from /proc/[pid]/status.\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(statusfile);
    free(statusline);
    return uid;
}

// returns the number of virtual memory pages used by the given process
int getVirtMemory(int pid) {
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

char* getCmdline(int pid) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/cmdline", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // attempt to access cmdline file
    FILE* cmdline_file = fopen(filepath, "r");
    if (cmdline_file == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed

    // Set buffer for use with getdelim, and declare pointers to 
    size_t bufsize = 0;
    char *cmdline = NULL;
    char *cmdline_temp; // string that holds each arg parsed by getdelim

    // Loop through each arg (separated by null bytes) in the cmdline file with getdelim,
    // storing the result in cmdline_temp.
    // TODO: add error handling? it will just stop if it finds an error, unfortunately this can't
    // readily be converted to an infinite-while with an if statement because even when it works
    // as intendended, getdelim returns an error at the end of the string.
    while (getdelim(&cmdline_temp, &bufsize, 0, cmdline_file) != -1) {
        if (cmdline == NULL) {
            // Print the new arg alone to cmdline.
            asprintf(&cmdline, "%s", cmdline_temp);
        } else {
            // Use asprintf to extend the current string.
            // Note: we prevent a memory leak by setting a pointer to the same address
            // that cmdline currently has and freeing it, so that when asprintf gives
            // a new address, the old one doesn't stick around.

            char *tmp_cmdline_oldaddr = cmdline;
            asprintf(&cmdline, "%s %s", cmdline, cmdline_temp);
            free(tmp_cmdline_oldaddr);
        }
    }

    free(cmdline_temp);
    fclose(cmdline_file);
    return cmdline;
}

// Gets a list of processes belonging to the current user.
// Adds the user processes' pids to the provided linked list
void getCurrentUserProcesses(linkedlist** pids) {
    DIR* proc;
    if ((proc = opendir("/proc/")) == NULL) {
        perror("Error accessing /proc directory.\n");
        exit(EXIT_FAILURE);
    }

    const int uid = (int)getuid();

    struct dirent* file;
    while ((file = readdir(proc)) != NULL) {
        // skip entries that aren't valid numbers and thus can't be pids
        errno = 0;
        int pid = (int)strtol(file->d_name, (char**)NULL, 10);
        if (errno != 0 || pid <= 0) continue;

        // uid is valid, see if it belongs to the right user
        if (uid == getUid(pid)) {
            ll_push(*pids, pid);
        }
    }

    return;
}
