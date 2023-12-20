#include "shared.h"

// Logging function declaration
void logAction(const char* filename, const char* action, int page);

int main() {
    srand((unsigned int)time(NULL)); // Seed the random number generator
    FILE *logFile = fopen("writer_log.txt", "w");

    // Create/Open a memory-mapped file for shared memory
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // Use paging file - shared memory
        NULL,                    // Default security attributes
        PAGE_READWRITE,          // Read/write access
        0,                       // Maximum object size (high-order DWORD)
        sizeof(Page) * BUFFER_COUNT, // Buffer size
        "SharedBuffer");         // Name of mapping object

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
    for (int i = 0; i < PROCESS_COUNT; i++) {
        Sleep(rand() % 1000 + 500); // Random delay between 0.5 to 1.5 seconds

        WaitForSingleObject(canWrite, INFINITE); // Wait until it's possible to write
        WaitForSingleObject(mutex, INFINITE);    // Wait for exclusive access

        // Write to a random page in the buffer
        int page_number = rand() % BUFFER_COUNT;
        sprintf(sharedBuffer[page_number].data, "Writing data to page %d", page_number);
        logAction("writer_log.txt", "Write", page_number);

        ReleaseMutex(mutex);                     // Release exclusive access
        ReleaseSemaphore(canRead, 1, NULL);      // Signal that reading can be done
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

void logAction(const char* filename, const char* action, int page) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        DWORD time = timeGetTime();
        fprintf(file, "%lu, %s, %d\n", time, action, page);
        fclose(file);
    }
}