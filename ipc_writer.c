#include "shared.h"

void logAction(const char* filename, const char* action, int page, DWORD time);

int main() {
    DWORD startTime, endTime;
    srand((unsigned int)time(NULL));
    FILE *logFile = fopen("logs/writer_log.txt", "w");

    SIZE_T pageSize = getPageSize();
    const int BUFFER_COUNT = 19;
    SIZE_T bufferSize = pageSize * BUFFER_COUNT;

    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        bufferSize,
        "SharedBuffer");

    if (hMapFile == NULL) {
        printf("Could not create file mapping object (%d).\n", GetLastError());
        return 1;
    }

    Page* sharedBuffer = (Page*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        bufferSize);

    if (sharedBuffer == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    if (!VirtualLock(sharedBuffer, bufferSize)) {
        printf("Could not lock pages in memory (%d).\n", GetLastError());
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    mutex = CreateMutex(NULL, FALSE, "BufferMutex");
    if (mutex == NULL) {
        printf("Could not create mutex (%d).\n", GetLastError());
        VirtualUnlock(sharedBuffer, bufferSize);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canRead = CreateSemaphore(NULL, 0, BUFFER_COUNT, "CanReadSemaphore");
    if (canRead == NULL) {
        printf("Could not create semaphore (%d).\n", GetLastError());
        CloseHandle(mutex);
        VirtualUnlock(sharedBuffer, bufferSize);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canWrite = CreateSemaphore(NULL, BUFFER_COUNT, BUFFER_COUNT, "CanWriteSemaphore");
    if (canWrite == NULL) {
        printf("Could not create semaphore (%d).\n", GetLastError());
        CloseHandle(canRead);
        CloseHandle(mutex);
        VirtualUnlock(sharedBuffer, bufferSize);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    printf("Writing...");
    int page_number;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        startTime = timeGetTime();
        page_number = rand() % BUFFER_COUNT;

        logAction("logs/writer_log.txt", "Waiting to Write", page_number, startTime);
        WaitForSingleObject(canWrite, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        startTime = timeGetTime();
        logAction("logs/writer_log.txt", "Start Writing", page_number, startTime);

        Sleep(rand() % 1000 + 500);

        endTime = timeGetTime();
        logAction("logs/writer_log.txt", "End Writing", page_number, endTime);

        ReleaseMutex(mutex);
        ReleaseSemaphore(canRead, 1, NULL);
    }

    fclose(logFile);
    CloseHandle(canWrite);
    CloseHandle(canRead);
    CloseHandle(mutex);
    VirtualUnlock(sharedBuffer, bufferSize);
    UnmapViewOfFile(sharedBuffer);
    CloseHandle(hMapFile);
    return 0;
}

void logAction(const char* filename, const char* action, int page, DWORD time) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fprintf(file, "%lu, %s, %d\n", time, action, page);
        fclose(file);
    }
}
