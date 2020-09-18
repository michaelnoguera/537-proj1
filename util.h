#include <stdint.h> // for intptr_t type
#include <stdbool.h>

// Struct containing boolean values for each flag the program handles
typedef struct Flags_t {
    bool singleChar;
    bool userTime;
    bool systemTime;
    bool virtMemory;
    bool command;
    bool procMem;
} Flags;

typedef struct mem_t { // TODO: use this for -m extra credit flag
    intptr_t mem_addr;
    size_t mem_len;
} mem_t;

void flipOption(char *optarg, bool *opt);