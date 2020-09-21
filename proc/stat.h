// Interface for accessing fields of /proc/[pid]/stat

#ifndef _PROC_STAT_
#define _PROC_STAT_

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Get the user time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getUserTime(int pid);

// Get the system time for a specified process
// Fails if process command contains unpaired parenthesis
unsigned long getSystemTime(int pid);

#endif