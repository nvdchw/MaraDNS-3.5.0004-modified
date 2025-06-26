// Windows named pipe server example
#include <windows.h>
#include <stdio.h>

#define PIPE_NAME "\\\\.\\pipe\\maradns_synth"
#define BUF_SIZE 512

int main() {
    HANDLE hPipe;
    char buffer[BUF_SIZE];
    DWORD bytesRead;

    hPipe = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1, BUF_SIZE, BUF_SIZE, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Failed to create named pipe.\n");
        return 1;
    }

    printf("Waiting for client to connect to %s...\n", PIPE_NAME);
    if (!ConnectNamedPipe(hPipe, NULL)) {
        printf("Failed to connect named pipe.\n");
        CloseHandle(hPipe);
        return 1;
    }

    printf("Client connected. Waiting for data...\n");
    while (ReadFile(hPipe, buffer, BUF_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Received: %s", buffer);
    }

    CloseHandle(hPipe);
    return 0;
}