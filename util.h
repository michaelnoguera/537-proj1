typedef struct pidslist {
    int pid;
    struct pidslist* next;
} pidslist_t;

void flipOption(char *optarg, int *opt);
void push(pidslist_t** pl, int pid);
void traverse(pidslist_t* pl);