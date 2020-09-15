#define _GNU_SOURCE

#include "proc.h"

#include <assert.h>
#include <dirent.h>  //contains readdir
#include <stdio.h>
#include <stdlib.h>
// #include <dirent.h> //contains readdir

// Gets a list of processes belonging to the current user.
// Returns the processes by value as a stack-allocated 1d array.
void getProcesses() { // TODO should this take a pointer arg to mutate
    struct dirent* process;

    // based off IBM documentation example linked from assignment page https://www.ibm.com/support/knowledgecenter/SSLTBW_2.4.0/com.ibm.zos.v2r4.bpxbd00/rtread.htm
    DIR* proc;
    if ((proc = opendir("/proc/")) == NULL) {
        printf("Error accessing /proc directory.\n");
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
    if (asprintf(&filepath, "/proc/%d/stat", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // attempt to access statfile
    FILE* statfile = fopen(filepath, "r");
    if (statfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed

    // parse beginning of statfile
    int foundPid;
    char state;
    fscanf(statfile, "%d (%*s)) %c", &foundPid, &state); // ignore executable name using astrisk, see https://stackoverflow.com/q/1410132/11639533
    // cleanup and validity check
    fclose(statfile);
    assert(pid == foundPid);  /* pid from stat file must match provided process pid */

    return state;
}
// state is the third argument, with scanf specifier %c

const int getUserTime(int pid);

const int getSystemTime(int pid);