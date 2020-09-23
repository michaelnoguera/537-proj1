#define _GNU_SOURCE

#include <assert.h>
#include <dirent.h>  //contains readdir
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

// -m includes

#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

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

    // Set buffer size for use with getdelim, and declare pointers to the entire cmdline string
    // as well as each sub-arg.
    size_t bufsize = 0;
    char *cmdline = NULL;
    char *arg = NULL; // string that holds each arg parsed by getdelim

    // Loop through each arg (separated by null bytes) in the cmdline file with getdelim,
    // storing the result in arg.
    // TODO: add error handling? it will just stop if it finds an error, unfortunately this can't
    // readily be converted to an infinite-while with an if statement because even when it works
    // as intendended, getdelim returns an error at the end of the string.
    while (getdelim(&arg, &bufsize, 0, cmdline_file) != -1) {
        if (cmdline == NULL) {
            // Print the new arg alone to cmdline.
            if(asprintf(&cmdline, "%s", arg) == -1) {
                perror("error allocating memory to hold string");
                exit(EXIT_FAILURE);
            }
        } else {
            // Use asprintf to extend the current string.
            // Note: we prevent a memory leak by setting a pointer to the same address
            // that cmdline currently has and freeing it, so that when asprintf gives
            // a new address, the old one doesn't stick around.

            char *temp = cmdline;
            if (asprintf(&cmdline, "%s %s", cmdline, arg) == -1) {
                perror("error allocating memory to hold string");
                exit(EXIT_FAILURE);
            }
            free(temp);
        }
    }

    free(arg);
    fclose(cmdline_file);
    return cmdline;
}

static bool isValidMem(int pid, unsigned long offset, int len) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/maps", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // create mapsfile input stream
    FILE* mapsfile = fopen(filepath, "r");
    if (mapsfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed
    
   
    size_t bufsize = 0;
    char *line;
    while(1) {
        // read line into temp heap-alloc'd buffer
        // "If *lineptr is set to NULL and *n is set 0 before the call, 
        // then getline() will allocate a buffer for storing the line.  
        line = NULL;
        bufsize = 0;
        if (getline(&line, &bufsize, mapsfile) < 0) return false;

        unsigned long start_address;
        unsigned long end_address;
        
        if (sscanf(line, "%lx-%lx ", &start_address, &end_address) != 2) {
            perror("Error reading mem map file.\n");
            exit(EXIT_FAILURE);
        }

        if (start_address <= offset && offset + ((unsigned long)len) <= end_address) {
            free(line);
            return true;
        }
        
        free(line);

    }
    return false;
}

char* readMem(int pid, unsigned long offset, int len) {
    if(!isValidMem(pid, offset, len)) {
        printf("Invalid memory range specified. Please specify memory in the process's address space.");
        exit(EXIT_FAILURE);
    }

    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/mem", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // attempt to access mem file
    int mem_fd = open(filepath, O_RDONLY);
    if (mem_fd == 0) {
        printf("Error accessing %s.\n Are the permissions set correctly for PTRACE_ATTACH? (Try as root)", filepath);
        exit(EXIT_FAILURE);
    }
    free(filepath); //upon success, filepath is no longer needed

    // from StackOverflow post linked on assignment page
    ptrace(PTRACE_ATTACH, pid, NULL, NULL); // obtain lock on proc mem
    waitpid(pid, NULL, 0); // make sure proc isn't active
    lseek(mem_fd, offset, SEEK_SET);

    char byte = 0;
    char *outstring = NULL;
    
    for (int i=0; i < len; i++) {
        int readOutput = -1;

        if ((readOutput = read(mem_fd, &byte, 1)) < 0) {
            printf("\n");
            exit(EXIT_FAILURE);
        }

        if (outstring == NULL) {
            if (asprintf(&outstring, "%02x",  byte) == -1) {
                perror("Error allocating memory to hold output string.");
                exit(EXIT_FAILURE);  
            } 
        } else {
            char *temp = outstring;
            if (asprintf(&outstring, "%s %02x", outstring, byte) == -1) {
                perror("Error allocating memory to hold output string.");
                free(temp);
                exit(EXIT_FAILURE);  

            }
            free(temp);
        }
    }
    
    ptrace(PTRACE_DETACH, pid, NULL, NULL);

    return outstring;
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

    closedir(proc);
}

// Reads and returns the "pid" (first) field of the /stat file
// Fails on read error
// Advances the provided file buffer
static int readPid(FILE* statfile) {
    assert(statfile != NULL && !ferror(statfile) && !feof(statfile));

    int foundPid;
    if (fscanf(statfile, "%d ", &foundPid) != 1 || ferror(statfile)) {
        perror("Error parsing pid (field 1) from stat file.\n");
        exit(EXIT_FAILURE);
    }
    return foundPid;
}



// Recursively consumes the "comm" (second) field of the /stat file
// in a safe manner by taking advantage of the automatic truncation
// at a specified length and searching backwards for the end parenthesis
static void skipCommHelper(FILE* buf, int depth, int* len) {
    // borrow character from buffer
    char c;
    if ((c = fgetc(buf)) == EOF || ferror(buf)) {
        perror("Invalid /proc/[pid]/stat file provided, EOF reached, or error reading stream.\n");
        exit(EXIT_FAILURE);
    }

    // recur downward until max length reached
    if (*len > depth) skipCommHelper(buf, depth+1, len);
    
    // find end, and return the rest of the characters to the buffer
    if (depth == *len && c != ')')
    {
        (*len)--;
        ungetc(c, buf);
    }
}


// Reads and ignores the "comm" (second) field of the /stat file
static void skipComm(FILE* statfile) {
    assert(statfile != NULL && !ferror(statfile) && !feof(statfile));

    int maxlen = 18;
    skipCommHelper(statfile, 1, &maxlen);

    assert(fgetc(statfile) == ' '); // consume trailing space
    return;
}

// Safely advances the provided file buffer past one "%c "
static void skipc(FILE* file) {
    assert(file != NULL && !ferror(file) && !feof(file));
    
    char temp;
    if (fscanf(file, " %c ", &temp) !=  1 || ferror(file)) { // include unconsumed space
        perror("Error parsing %c field.\n");
        exit(EXIT_FAILURE);
    }
}

// Safely advances the provided file buffer past one "%d "
static void skipd(FILE* file) {
    assert(file != NULL && !ferror(file) && !feof(file));

    int temp;
    if (fscanf(file, "%d ", &temp) !=  1 || ferror(file)) {
        perror("Error parsing from stat file (within fields 4-8).\n");
        exit(EXIT_FAILURE);
    }
}

// Safely advances the provided file buffer past one "%u "
static void skipu(FILE* file) {
    assert(file != NULL && !ferror(file) && !feof(file));

    unsigned int temp;
    if (fscanf(file, "%u ", &temp) !=  1 || ferror(file)) {
        perror("Error parsing from stat file (within fields 4-8).\n");
        exit(EXIT_FAILURE);
    }
}

// Safely advances the provided file buffer past one "%lu "
static void skiplu(FILE* file) {
    assert(file != NULL && !ferror(file) && !feof(file));

    unsigned long temp;
    if (fscanf(file, "%lu ", &temp) !=  1 || ferror(file)) {
        perror("Error parsing from stat file (within fields 4-8).\n");
        exit(EXIT_FAILURE);
    }
}


// Get the user time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getUserTime(int pid) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/stat", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // create statfile input stream
    FILE* statfile = fopen(filepath, "r");
    if (statfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    clearerr(statfile);
    free(filepath); //upon success, filepath is no longer needed

    // Validate: (1) pid %d The process ID.
    assert(pid==readPid(statfile));

    // Skip unneeded fields:   
    skipComm(statfile); // (2) comm %s with special formatting

    skipc(statfile); // (3) state %c 

    skipd(statfile); // (4) ppid %d
    skipd(statfile); // (5) pgrp %d
    skipd(statfile); // (6) session %d
    skipd(statfile); // (7) tty_nr %d
    skipd(statfile); // (8) tpgid %d

    skipu(statfile); // (9) flags  %u

    skiplu(statfile); // (10) minflt %lu
    skiplu(statfile); // (11) cminflt %lu
    skiplu(statfile); // (12) majflt %lu
    skiplu(statfile); // (13) cmajflt %lu

    // (14) utime  %lu
    unsigned long userTime = 0;
    if (fscanf(statfile, "%lu ", &userTime) !=  1 || ferror(statfile)) {
        perror("Error parsing utime (field 14) from stat file.\n");
        exit(EXIT_FAILURE);
    }

    fclose(statfile);    
    return userTime;
}

// Get the system time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getSystemTime(int pid) {
    // construct filepath from pid
    char* filepath;
    if (asprintf(&filepath, "/proc/%d/stat", pid) == -1) {
        printf("Error allocating memory to hold filepath for process number %d\n", pid);
        exit(EXIT_FAILURE);
    }

    // create statfile input stream
    FILE* statfile = fopen(filepath, "r");
    if (statfile == NULL) {
        printf("Error accessing %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    clearerr(statfile);
    free(filepath); //upon success, filepath is no longer needed

    // Validate: (1) pid %d The process ID.
    assert(pid==readPid(statfile));

    // Skip unneeded fields:   
    skipComm(statfile); // (2) comm %s with special formatting

    skipc(statfile); // (3) state %c 

    skipd(statfile); // (4) ppid %d
    skipd(statfile); // (5) pgrp %d
    skipd(statfile); // (6) session %d
    skipd(statfile); // (7) tty_nr %d
    skipd(statfile); // (8) tpgid %d

    skipu(statfile); // (9) flags  %u

    skiplu(statfile); // (10) minflt %lu
    skiplu(statfile); // (11) cminflt %lu
    skiplu(statfile); // (12) majflt %lu
    skiplu(statfile); // (13) cmajflt %lu
    skiplu(statfile); // (14) utime  %lu
    
    // (15) stime  %lu
    unsigned long systemTime = 0;
    if (fscanf(statfile, "%lu ", &systemTime) !=  1 || ferror(statfile)) {
        perror("Error parsing utime (field 14) from stat file.\n");
        exit(EXIT_FAILURE);
    }

    fclose(statfile);    
    return systemTime;
}