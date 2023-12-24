#include "shared.h"

void logAction(const char* filename, const char* action, int page, DWORD time);

int main() {
    DWORD startTime, endTime;
    srand((unsigned int)time(NULL));
    FILE *logFile = fopen("logs/writer_log.txt", "w");

    // Create/Open a memory-mapped file for shared memory
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,       // Use paging file - shared memory
        NULL,                       // Default security attributes
        PAGE_READWRITE,             // Read/write access
        0,                          // Maximum object size (high-order DWORD)
        sizeof(Page) * BUFFER_COUNT,// Buffer size
        "SharedBuffer");            // Name of mapping object

    if (hMapFile == NULL) {
        printf("Could not create file mapping object (%d).\n", GetLastError());
        return 1;
    }

    // Map a view of the file mapping into the address space of the calling process
    Page* sharedBuffer = (Page*)MapViewOfFile(
        hMapFile,            // Handle to map object
        FILE_MAP_ALL_ACCESS, // Read/write permission
        0,                   
        0,                   
        sizeof(Page) * BUFFER_COUNT);

    if (sharedBuffer == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // Create/Open synchronization objects
    mutex = CreateMutex(NULL, FALSE, "BufferMutex");
    if (mutex == NULL) {
        printf("Could not create mutex (%d).\n", GetLastError());
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canRead = CreateSemaphore(NULL, 0, BUFFER_COUNT, "CanReadSemaphore");
    if (canRead == NULL) {
        printf("Could not create semaphore (%d).\n", GetLastError());
        CloseHandle(mutex);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canWrite = CreateSemaphore(NULL, BUFFER_COUNT, BUFFER_COUNT, "CanWriteSemaphore");
    if (canWrite == NULL) {
        printf("Could not create semaphore (%d).\n", GetLastError());
        CloseHandle(canRead);
        CloseHandle(mutex);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    // Main loop for writing
    printf("Writing...");
    int page_number;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        startTime = timeGetTime();
        int page_number = rand() % BUFFER_COUNT;  // Assign a random page number here

        logAction("logs/writer_log.txt", "Waiting to Write", page_number, startTime);
        WaitForSingleObject(canWrite, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        startTime = timeGetTime(); // Update the start time after acquiring the mutex
        logAction("logs/writer_log.txt", "Start Writing", page_number, startTime);

        Sleep(rand() % 1000 + 500);

        endTime = timeGetTime();
        logAction("logs/writer_log.txt", "End Writing", page_number, endTime);

        ReleaseMutex(mutex);
        ReleaseSemaphore(canRead, 1, NULL);
    }

    // Cleanup
    fclose(logFile);
    CloseHandle(canWrite);
    CloseHandle(canRead);
    CloseHandle(mutex);
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