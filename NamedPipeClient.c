#include <windows.h>
#include <stdio.h>

#define PIPE_NAME "\\\\.\\pipe\\maradns_synth"
#define BUF_SIZE 512

int main() {
    HANDLE hPipe;
    DWORD bytesWritten, bytesRead;
    char buffer[BUF_SIZE];

    // Try to connect to the named pipe (waits until server is available)
    printf("Connecting to pipe server at %s...\n", PIPE_NAME);
    hPipe = CreateFileA(
        PIPE_NAME,                  // pipe name
        GENERIC_READ | GENERIC_WRITE, // read and write access
        0,                          // no sharing
        NULL,                       // default security attributes
        OPEN_EXISTING,              // opens existing pipe
        0,                          // default attributes
        NULL);                      // no template file

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Failed to connect to pipe. Error: %lu\n", GetLastError());
        return 1;
    }

    // Send command to the server
    const char *command = "getAllSyntheticMappings\n";
    if (!WriteFile(hPipe, command, (DWORD)strlen(command), &bytesWritten, NULL)) {
        printf("Failed to write to pipe. Error: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    // Read response from the server
    printf("Sent command. Waiting for response...\n");
    while (ReadFile(hPipe, buffer, BUF_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    printf("\nDone.\n");
    CloseHandle(hPipe);
    return 0;
}