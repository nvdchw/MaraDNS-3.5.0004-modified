// This program implements a simple named pipe server in C.
// It creates a named pipe and waits for clients to connect.
// When a client connects, it reads data from the pipe and prints it to the console.

#include <windows.h>
#include <stdio.h>

#define PIPE_NAME "\\\\.\\pipe\\maradns_synth"
#define BUF_SIZE 512

int main() {
    HANDLE hPipe;
    char buffer[BUF_SIZE];
    DWORD bytesRead;

    // Infinite loop to keep the server running
    printf("Starting named pipe server...\n");
    while (1) {
        // Create the named pipe
        hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, BUF_SIZE, BUF_SIZE, 0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            printf("Failed to create named pipe.\n");
            return 1;
        }

        // Wait for a client to connect
        printf("Waiting for client to connect to %s...\n", PIPE_NAME);
        if (!ConnectNamedPipe(hPipe, NULL)) {
            printf("Failed to connect named pipe.\n");
            CloseHandle(hPipe);
            continue; // Try again
        }

        // Client connected, read data from the pipe and print it
        printf("Client connected. Waiting for data...\n");
        while (ReadFile(hPipe, buffer, BUF_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("Received: %s", buffer);
            fflush(stdout);
        }
    
        CloseHandle(hPipe);
        printf("Client disconnected.\n");
    }
    return 0;
}