#include <windows.h>
#include <stdio.h>
#include <process.h>

#define PIPE_NAME "\\\\.\\pipe\\maradns_synth"
#define BUF_SIZE 512

HANDLE hPipe;
volatile int running = 1;

// Thread to continuously read data from the pipe and print it
unsigned __stdcall read_thread(void *arg) {
    char buffer[BUF_SIZE];
    DWORD bytesRead, bytesAvail;
    while (running) {
        // Check if there is data available
        if (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
            // Read and print all available data
            while (bytesAvail > 0 && ReadFile(hPipe, buffer, BUF_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("\n[PIPE] %s", buffer);
                fflush(stdout);
                // Check again if more data is available
                if (!PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvail, NULL)) break;
            }
        } else {
            Sleep(50); // Avoid busy loop if no data
        }
    }
    return 0;
}

int main() {
    DWORD bytesWritten;
    printf("Connecting to pipe server at %s...\n", PIPE_NAME);
    hPipe = CreateFileA(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Failed to connect to pipe. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("Connected to pipe server at %s.\n", PIPE_NAME);

    // Start the read thread
    _beginthreadex(NULL, 0, read_thread, NULL, 0, NULL);

    // Main loop to send commands to the server
    while (1) {
        printf("Press ENTER to request all synthetic mappings, or 'q' then ENTER to quit: ");
        int ch = getchar();
        if (ch == 'q' || ch == 'Q')
            break;

        // Only send the command; do not read the response here
        const char *command = "getAllSyntheticMappings\n";
        if (!WriteFile(hPipe, command, (DWORD)strlen(command), &bytesWritten, NULL)) {
            printf("Failed to write to pipe. Error: %lu\n", GetLastError());
            break;
        }
    }

    // Signal the read thread to stop
    running = 0;
    CloseHandle(hPipe);
    printf("\nDone.\n");
    return 0;
}