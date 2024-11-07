#ifndef SHARED_H
#define SHARED_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PROCESS_COUNT 32

typedef struct { char* data; } Page;

HANDLE mutex;
HANDLE canRead;
HANDLE canWrite;

SIZE_T getPageSize() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwPageSize;
}

#endif
