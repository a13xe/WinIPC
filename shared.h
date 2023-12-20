#ifndef SHARED_H
#define SHARED_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 4096 // Size of each buffer page
#define BUFFER_COUNT 19  // Number of buffer pages
#define PROCESS_COUNT 32 // Number of iterations for each process

// Structure representing a page in the buffer
typedef struct { char data[BUFFER_SIZE]; } Page;

// Synchronization objects
HANDLE mutex;
HANDLE canRead;
HANDLE canWrite;

#endif
