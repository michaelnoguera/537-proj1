// CS 537 Programming Assignment 1 (Fall 2020)
// Michael Noguera and Julien de Castelnau

#include <dirent.h> //contains readdir

const char* getState(int pid);
// state is the third argument, with scanf specifier %c

const int getUserTime(int pid);
const int getSystemTime(int pid);