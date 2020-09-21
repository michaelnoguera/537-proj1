#define _GNU_SOURCE
#include "stat.h"


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


// Reads and ignores the "comm" (second) field of the /stat file
// Fails if unpaired parenthesis are present in the "comm" field for parsing
// safety
// Advances the provided file buffer
static void skipComm(FILE* statfile) {
    assert(statfile != NULL && !ferror(statfile) && !feof(statfile));

    int openParens = 0;
    for (int i = 0; i < 18; i++){
        char c = fgetc(statfile); // read next character
        
        // Stop reading on the last open parenthesis. If the executable filename would
        // unsafely inject a ')' into this field, error out with explanation.
        if (c == '(') openParens++;
        if (c == ')') openParens--;
        if (openParens < 0) {
            perror("The /proc/[pid]/stat file cannot be safely parsed when the command\
            field contains unmatched parenthesis.\n");
            exit(EXIT_FAILURE);
        }
        if (openParens == 0 && c == ')') break;
    }
    fgetc(statfile); // consume trailing space
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