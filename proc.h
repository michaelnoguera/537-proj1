/**
 * CS 537 Programming Assignment 1 (Fall 2020)
 * @authors Michael Noguera and Julien de Castelnau
 * @date 9/23/2020
 * 
 * @file proc.h
 * @brief Provides an interface over the /proc filesystem and exposes functions for getting 
 * process information.
 */

#include "linkedlist.h"

/**
 * Ennumerates the current user's processes, and adds them to a provided list.
 * 
 * @param pids LinkedList to add found pids to.
 */
void getCurrentUserProcesses(linkedlist** pids);

/**
 * Returns the current state of the given process. 
 * Reads from /proc/[pid]/status.
 * 
 * @param pid The process in question.
 * @return The status character for the process. See proc(5) for a list of options and
 * their meanings.
 */
char getState(int pid);

/**
 * Returns the real uid (the uid of the user that created the process) given a pid. 
 * Reads from /proc/[pid]/status.
 * 
 * @param pid The process in question.
 * @return The real (not effective) user id.
 */
int getUid(int pid);

/**
 * Returns the current virtual memory usage of the specified process. 
 * Reads from /proc/[pid]/statm.
 * 
 * @param pid The process in question.
 * @return the number of virtual memory pages used by the current process.
 */
int getVirtMemory(int pid);

/**
 * Gets the command line used to invoke the program with given pid. 
 * Depends on /proc/[pid]/cmdline.
 * 
 * @param pid The process in question.
 * @return A heap-allocated string containing the command line invocation. May contain 
 * spaces, as flags are included. This memory must be freed by the callee.
 */
char* getCmdline(int pid);

/**
 * Reads the specified memory range from a process, and returns a formatted string 
 * with its contents.
 * 
 * @param pid The process in question.
 * @param offset Memory address in the VAS to start reading from.
 * @param len Amount of memory to be read, in bytes.
 * 
 * @return A heap-allocated formatted string with the requested memory contents 
 * in hexadecimal. This memory must be freed by the callee.
 */
char* readMem(int pid, unsigned long offset, int len);

/**
 * Gets the user time for a specified process. 
 * Reads from /proc/[pid]/stat.
 * 
 * @param pid The process in question.
 * @return User time of the process.
 */
unsigned long getUserTime(int pid);

/**
 * Gets the system time for a specified process. 
 * Reads from /proc/[pid]/stat.
 * 
 * @param pid The process in question.
 * @return System time of the process.
 */
unsigned long getSystemTime(int pid);
