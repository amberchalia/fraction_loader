#include <windows.h>
#include <stdio.h> // for sprintf

void DebugPrint(LPCSTR format, ...) {
    CHAR buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfA(buffer, format, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

// Simulates the work of Project 1.3: Parsing the ordinal from a header
int SimulateParseHeader(BYTE* pFractionData, SIZE_T dwTotalSize, BYTE** ppPayload) {
    // Our simulated header is just a string like "FRACTION:1|"
    // The ordinal is the character at position 10.
    if (dwTotalSize < 11) return -1; // Check for minimal size

    char ordinal_char = (char)pFractionData[9]; // e.g., get '1' from "FRACTION:1|"
    int ordinal = ordinal_char - '0'; // Convert the char '1' to the integer 1

    *ppPayload = pFractionData + 10; // Payload starts after the 10-byte header
    return ordinal;
}

int main() {
    DebugPrint("[+] Starting Sub-Project 1.5: Handling Multiple Fractions\r\n");

    // 1. Define our test parameters
    const int NUM_FRACTIONS_TO_TEST = 3;
    const SIZE_T BLOCK_SIZE = 10; // Small size for easy testing
    const SIZE_T HEADER_SIZE = 10;
    const SIZE_T FRACTION_SIZE = HEADER_SIZE + BLOCK_SIZE;
    const SIZE_T FINAL_BUFFER_SIZE = NUM_FRACTIONS_TO_TEST * BLOCK_SIZE;

    // 2. Allocate the final executable buffer (from 1.4)
    BYTE* pFinalBuffer = (BYTE*)VirtualAlloc(NULL, FINAL_BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!pFinalBuffer) {
        DebugPrint("[-] Failed to allocate final buffer. Error: %lu\r\n", GetLastError());
        return 1;
    }
    DebugPrint("[+] Final buffer allocated at 0x%p. Size: %lu bytes\r\n", pFinalBuffer, FINAL_BUFFER_SIZE);

    // 3. Create and process simulated fractions
    for (int i = 0; i < NUM_FRACTIONS_TO_TEST; i++) {
        DebugPrint("[--- Processing Fraction %d ---]\r\n", i);

        // 3a. Simulate downloading a fraction (Project 1.2)
        //     We create a buffer that looks like: "FRACTION:X|PAYLOAD_DATA"
        BYTE* pFractionBuffer = (BYTE*)VirtualAlloc(NULL, FRACTION_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pFractionBuffer) {
            DebugPrint("[-] Failed to allocate fraction buffer %d.\r\n", i);
            goto CLEANUP; // Use goto for clean error handling (part of 1.6)
        }
        // Create the header: "FRACTION:0|", "FRACTION:1|", etc.
        sprintf_s((char*)pFractionBuffer, FRACTION_SIZE, "FRACTION:%d|", i);
        // Fill the payload part with a simple pattern based on the ordinal (e.g., all 'A', all 'B')
        for (SIZE_T j = HEADER_SIZE; j < FRACTION_SIZE; j++) {
            pFractionBuffer[j] = 'A' + i; // Payload for Fraction0: 'A's, Fraction1: 'B's, etc.
        }
        DebugPrint("[+] Simulated fraction %d created: Header='%s', Payload='%c's\r\n", i, pFractionBuffer, 'A' + i);

        // 3b. Simulate parsing the header and getting the payload pointer (Project 1.3)
        BYTE* pPayload = NULL;
        int ordinal = SimulateParseHeader(pFractionBuffer, FRACTION_SIZE, &pPayload);
        if (ordinal == -1) {
            DebugPrint("[-] Failed to parse header for fraction %d.\r\n", i);
            VirtualFree(pFractionBuffer, 0, MEM_RELEASE);
            goto CLEANUP;
        }
        DebugPrint("[+] Parsed ordinal: %d. Payload starts at 0x%p\r\n", ordinal, pPayload);

        // 3c. Calculate the destination offset in the final buffer
        SIZE_T offset = ordinal * BLOCK_SIZE;
        DebugPrint("[+] Copying payload to final buffer at offset: %lu\r\n", offset);

        // 3d. Copy the payload to the final buffer
        CopyMemory(pFinalBuffer + offset, // Destination: final buffer + calculated offset
                   pPayload,              // Source: the payload from the fraction
                   BLOCK_SIZE);           // Size: our fixed block size

        // 3e. Free the temporary fraction buffer (sequential cleanup)
        VirtualFree(pFractionBuffer, 0, MEM_RELEASE);
        DebugPrint("[+] Freed fraction buffer %d.\r\n", i);
    }

    // 4. VERIFICATION: Print the final assembled buffer
    DebugPrint("[+] Final assembled buffer contents:\r\n");
    for (SIZE_T i = 0; i < FINAL_BUFFER_SIZE; i++) {
        DebugPrint("%c", pFinalBuffer[i]); // Should print AAAAAAAAABBBBBBBBBBCCCCCCCCCC for 3 fractions
    }
    DebugPrint("\r\n");
    DebugPrint("[+] Sub-Project 1.5 completed successfully!\r\n");

CLEANUP:
    // 5. Final cleanup
    if (pFinalBuffer) {
        VirtualFree(pFinalBuffer, 0, MEM_RELEASE);
        DebugPrint("[+] Freed final buffer.\r\n");
    }
    return 0;
}