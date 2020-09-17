// CS 537 Programming Assignment 1 (Fall 2020)
// Michael Noguera and Julien de Castelnau

#include <dirent.h> //contains readdir

char getState(int pid);

int getUserTime(int pid);

int getSystemTime(int pid);

int getVirtMemory(int pid);