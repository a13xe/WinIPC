#include "shared.h"

// Logging function declaration
void logAction(const char* filename, const char* action, int page);

int main() {
    srand((unsigned int)time(NULL)); // Seed the random number generator
    FILE *logFile = fopen("reader_log.txt", "w");

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
    printf("Reading...");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        Sleep(rand() % 1000 + 500); // Random delay between 0.5 to 1.5 seconds

        WaitForSingleObject(canRead, INFINITE); // Wait until it's possible to read
        WaitForSingleObject(mutex, INFINITE);   // Wait for exclusive access

        // Read from a random page in the buffer
        int page_number = rand() % BUFFER_COUNT;
        char buffer[BUFFER_SIZE];
        strncpy(buffer, sharedBuffer[page_number].data, BUFFER_SIZE);
        logAction("reader_log.txt", "Read", page_number);

        ReleaseMutex(mutex);                    // Release exclusive access
        ReleaseSemaphore(canWrite, 1, NULL);    // Signal that writing can be done
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
