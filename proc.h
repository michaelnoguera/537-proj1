// CS 537 Programming Assignment 1 (Fall 2020)
// Michael Noguera and Julien de Castelnau

#include "linkedlist.h"

// Gets a list of processes belonging to the current user.
// Adds the user processes' pids to the provided linked list
void getCurrentUserProcesses(linkedlist** pids);

char getState(int pid);

int getVirtMemory(int pid);

char* getCmdline(int pid);

// Get the user time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getUserTime(int pid);

// Get the system time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getSystemTime(int pid);
