// not works, why?

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main() {
    SetConsoleCtrlHandler(NULL, TRUE); // Prevent own process from terminating

    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    CreateProcess(
        NULL,
        "sleep.exe 4",
        NULL, NULL, TRUE,
        CREATE_NEW_PROCESS_GROUP,
        NULL, NULL, &si, &pi);

    // Give the child time to start up
    Sleep(1000);

    printf("pi.dwProcessId = %ld\n", pi.dwProcessId);
    printf("GenerateConsoleCtrlEvent = %d\n", GenerateConsoleCtrlEvent(CTRL_C_EVENT, pi.dwProcessId)); // Send Ctrl+C

    // Wait and clean up
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}