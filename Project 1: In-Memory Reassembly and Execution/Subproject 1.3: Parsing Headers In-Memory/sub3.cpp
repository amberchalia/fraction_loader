#include <windows.h>
#include <stdio.h> // For sprintf, optional for debug

#pragma comment(lib, "winhttp.lib")

void DebugPrint(LPCSTR format, ...) {
    CHAR buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfA(buffer, format, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

// Function to parse the fraction header and find the payload
// Returns the ordinal number on success, or -1 on failure.
// Sets *ppPayload to point to the start of the payload data.
int ParseFractionHeader(BYTE* pDownloadedData, SIZE_T dwTotalSize, BYTE** ppPayload, SIZE_T* pdwPayloadSize) {
    CHAR header[33] = { 0 }; // 32 bytes + 1 for null terminator
    CHAR* pOrdinalStart = NULL;
    CHAR* pOrdinalEnd = NULL;
    int ordinal = -1;

    if (dwTotalSize < 32) {
        DebugPrint("[-] Error: Downloaded data is too small to contain a header.\r\n");
        return -1;
    }

    // 1. Copy the first 32 bytes into a temporary character array so we can treat it as a string.
    CopyMemory(header, pDownloadedData, 32);
    header[32] = '\0'; // Ensure it's null-terminated

    DebugPrint("[+] Header string: '%s'\r\n", header);

    // 2. Find the part of the header that contains the number.
    // We look for the sequence ": " as a common separator (e.g., "Fraction: 5")
    pOrdinalStart = strstr(header, ": ");
    if (pOrdinalStart == NULL) {
        DebugPrint("[-] Error: Could not find separator ': ' in the header.\r\n");
        return -1;
    }
    pOrdinalStart += 2; // Move the pointer past the ": " to the start of the number.

    // 3. Extract the number using atoi. atoi will stop at the first non-digit character.
    ordinal = atoi(pOrdinalStart);
    if (ordinal == 0) {
        // atoi returns 0 on failure. This might also be a legitimate 0, so we need an extra check.
        // Check if the character at pOrdinalStart is actually a digit.
        if (pOrdinalStart[0] < '0' || pOrdinalStart[0] > '9') {
            DebugPrint("[-] Error: Could not find a valid ordinal number after the separator.\r\n");
            return -1;
        }
        // If it is a digit, then ordinal 0 is valid.
    }
    DebugPrint("[+] Parsed ordinal: %d\r\n", ordinal);

    // 4. Calculate the payload pointer and size.
    *ppPayload = pDownloadedData + 32; // Payload starts right after the 32-byte header.
    *pdwPayloadSize = dwTotalSize - 32; // Payload size is total size minus the header.

    DebugPrint("[+] Payload starts at address: 0x%p\r\n", *ppPayload);
    DebugPrint("[+] Payload size: %zu bytes\r\n", *pdwPayloadSize);

    return ordinal;
}

int main() {
    // --- This simulates your downloaded data from Project 1.2 ---
    // Instead of downloading, we create a fake fraction in memory.
    const char* fakeHeader = "Fraction: 3|This is just metadata";
    const char* fakePayload = "This is the actual secret payload data!";

    // Create a buffer that mimics a downloaded fraction: Header + Payload
    SIZE_T headerSize = 32; // Our header is 32 bytes
    SIZE_T payloadSize = strlen(fakePayload) + 1; // +1 for null terminator for printing
    SIZE_T totalSize = headerSize + payloadSize;

    BYTE* pFakeDownload = (BYTE*)VirtualAlloc(NULL, totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pFakeDownload) {
        DebugPrint("[-] Failed to allocate fake download buffer.\r\n");
        return 1;
    }

    // Copy the header (first 32 bytes). Pad with spaces if the string is shorter.
    ZeroMemory(pFakeDownload, headerSize); // Fill with zeros first
    strncpy_s((char*)pFakeDownload, headerSize, fakeHeader, _TRUNCATE);

    // Copy the payload after the header
    CopyMemory(pFakeDownload + headerSize, fakePayload, payloadSize);

    DebugPrint("[+] Simulated download complete. Total size: %zu bytes\r\n", totalSize);
    // --- End Simulation ---

    // Now, let's parse it!
    BYTE* pPayload = NULL;
    SIZE_T dwPayloadSize = 0;
    int ordinal = ParseFractionHeader(pFakeDownload, totalSize, &pPayload, &dwPayloadSize);

    if (ordinal != -1) {
        DebugPrint("[SUCCESS] Fraction #%d parsed successfully.\r\n", ordinal);
        // Print the payload as a string to prove we found it
        DebugPrint("[+] Payload content: %s\r\n", (char*)pPayload);
    } else {
        DebugPrint("[FAILURE] Could not parse the fraction header.\r\n");
    }

    // Cleanup
    VirtualFree(pFakeDownload, 0, MEM_RELEASE);
    DebugPrint("[+] Cleanup complete.\r\n");
    return 0;
}