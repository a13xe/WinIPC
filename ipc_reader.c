#include "shared.h"


void logAction(const char* filename, const char* action, int page, DWORD time);


int main() {
    DWORD startTime, endTime;
    char* file = "logs/reader_log.txt";
    srand((unsigned int)time(NULL)); // Seed the random number generator
    FILE *logFile = fopen(file, "w");

    // Open a handle to the existing memory-mapped file
    HANDLE hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS, // Read/write access
        FALSE,              // Do not inherit the name
        "SharedBuffer");    // Name of mapping object

    if (hMapFile == NULL) {
        printf("Could not open file mapping object (%d).\n", GetLastError());
        return 1;
    }

    // Map a view of the file mapping into the address space of the calling process
    Page* sharedBuffer = (Page*)MapViewOfFile(
        hMapFile,           // Handle to map object
        FILE_MAP_ALL_ACCESS,// Read/write permission
        0,                  
        0,                  
        sizeof(Page) * BUFFER_COUNT);

    if (sharedBuffer == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // Open synchronization objects
    mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "BufferMutex");
    if (mutex == NULL) {
        printf("Could not open mutex (%d).\n", GetLastError());
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "CanReadSemaphore");
    if (canRead == NULL) {
        printf("Could not open semaphore (%d).\n", GetLastError());
        CloseHandle(mutex);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    canWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "CanWriteSemaphore");
    if (canWrite == NULL) {
        printf("Could not open semaphore (%d).\n", GetLastError());
        CloseHandle(canRead);
        CloseHandle(mutex);
        UnmapViewOfFile(sharedBuffer);
        CloseHandle(hMapFile);
        return 1;
    }

    // Main loop for reading
    int page_number;
    printf("Reading...");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        page_number = rand() % BUFFER_COUNT;

        startTime = timeGetTime();
        logAction("logs/reader_log.txt", "Waiting to Read", page_number, startTime);

        WaitForSingleObject(canRead, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        startTime = timeGetTime(); // Update the start time after acquiring the lock
        logAction("logs/reader_log.txt", "Start Reading", page_number, startTime);

        Sleep(rand() % 1000 + 500);

        endTime = timeGetTime();
        logAction("logs/reader_log.txt", "End Reading", page_number, endTime);

        ReleaseMutex(mutex);
        ReleaseSemaphore(canWrite, 1, NULL);
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
