#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>

// =============================================================
// HOW TO USE:
//   breaker.exe <input_file.exe> <output_folder>
//
// Example:
//   breaker.exe C:\malware\payload.exe C:\output
//
// This will split `payload.exe` into multiple files like:
//   Fraction0.vx, Fraction1.vx, Fraction2.vx, ...
//
// Each file contains:
//   - A 32-byte header (with FRACTION number)
//   - A chunk of the input file (default: 28 bytes per chunk here)
//
// =============================================================
//
// TO CHANGE BLOCK SIZE:
//   1. Go to WinMain() → look at BYTE Buffer[28].
//   2. Change 28 to your desired chunk size (e.g., 1024).
//   3. Also update the comment above CreateFraction()
//      so you remember the correct block size.
//
// Example:
//   BYTE Buffer[1024] = { 0 };   // Splits file into 1024-byte chunks
//
// =============================================================


// Check if the file path already exists
BOOL IsPathValidW(PWCHAR FilePath) {
    HANDLE hFile = CreateFileW(FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    CloseHandle(hFile);
    return TRUE;
}

// Write one block (DataBlock + 32-byte header) to a file
BOOL CreateFraction(PBYTE DataBlock, DWORD dwWriteSize, PWCHAR OutputDirectory) {
    HANDLE hHandle = INVALID_HANDLE_VALUE;
    WCHAR OutputPath[MAX_PATH] = { 0 };
    CHAR FileHeader[33] = { 0 }; // 32 bytes + null terminator
    DWORD dwOut = 0;
    BOOL bFlag = FALSE;

    // Find the next available filename (Fraction0, Fraction1, ...)
    DWORD dwFractionCount = 0;
    while (TRUE) {
        _snwprintf_s(OutputPath, MAX_PATH, _TRUNCATE, L"%ws\\Fraction%lu.vx", OutputDirectory, dwFractionCount);

        if (IsPathValidW(OutputPath)) {
            dwFractionCount++;
            continue;
        }

        // Prepare padded header: e.g., "FRACTION: <0>            "
        _snprintf_s(FileHeader, sizeof(FileHeader), _TRUNCATE, "FRACTION: <%lu>", dwFractionCount);
        size_t len = strlen(FileHeader);
        for (size_t i = len; i < 32; i++) {
            FileHeader[i] = ' ';
        }
        FileHeader[32] = '\0';
        break;
    }

    // Create the file and write header + data block
    hHandle = CreateFileW(OutputPath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hHandle == INVALID_HANDLE_VALUE)
        goto EXIT_ROUTINE;

    if (!WriteFile(hHandle, FileHeader, 32, &dwOut, NULL))
        goto EXIT_ROUTINE;

    dwOut = 0;
    if (!WriteFile(hHandle, DataBlock, dwWriteSize, &dwOut, NULL))
        goto EXIT_ROUTINE;

    bFlag = TRUE;

EXIT_ROUTINE:
    if (hHandle)
        CloseHandle(hHandle);

    return bFlag;
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    HANDLE hHandle = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    BOOL bFlag = FALSE;
    BOOL EndOfFile = FALSE;
    INT Arguments = 0;

    LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &Arguments);
    if (Arguments < 3) {
        MessageBoxW(NULL, L"Usage: breaker.exe <input_file.exe> <output_folder>", L"Error", MB_ICONERROR);
        return 1;
    }

    // Open the input file
    hHandle = CreateFileW(szArgList[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hHandle == INVALID_HANDLE_VALUE)
        goto EXIT_ROUTINE;

    // =============================================================
    // CHANGE BUFFER SIZE HERE:
    // Default: 28 bytes per chunk
    // Example: BYTE Buffer[1024] = { 0 }; // 1024-byte chunks
    // =============================================================
    do {
        BYTE Buffer[28] = { 0 };  // <<--- MODIFY THIS VALUE
        DWORD dwRead = 0;

        if (!ReadFile(hHandle, Buffer, sizeof(Buffer), &dwRead, NULL))
            goto EXIT_ROUTINE;

        if (dwRead < sizeof(Buffer))
            EndOfFile = TRUE;

        if (!CreateFraction(Buffer, dwRead, szArgList[2]))
            goto EXIT_ROUTINE;

    } while (!EndOfFile);

    bFlag = TRUE;

EXIT_ROUTINE:
    if (!bFlag)
        dwError = GetLastError();

    if (szArgList)
        LocalFree(szArgList);

    if (hHandle != INVALID_HANDLE_VALUE)
        CloseHandle(hHandle);

    return dwError;
}
