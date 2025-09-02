#include <windows.h>
#include <stdio.h> // For sprintf

#pragma comment(lib, "winhttp.lib") // Still needed if you use your download function later

void DebugPrint(LPCSTR format, ...) {
    CHAR buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfA(buffer, format, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

int main() {
    // --- Sub-project 1.4 Parameters ---
    const SIZE_T FINAL_PAYLOAD_SIZE = 1024 * 1024; // 1. Hardcode a total size: 1MB for example
    const SIZE_T FRACTION_PAYLOAD_SIZE = 512;      // Simulated size of one fraction's payload

    BYTE* pFinalBuffer = NULL;                     // Pointer to our big executable buffer
    BYTE* pSingleFractionPayload = NULL;           // Pointer to a simulated downloaded fraction

    DebugPrint("[+] Starting Sub-Project 1.4: Allocating Final Executable Buffer\r\n");

    // 2. Allocate the CONTIGUOUS final buffer with EXECUTE permissions
    pFinalBuffer = (BYTE*)VirtualAlloc(
        NULL,                   // Let system choose address
        FINAL_PAYLOAD_SIZE,     // Size of the entire final payload
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE  // CRITICAL: This memory can be executed!
    );

    if (pFinalBuffer == NULL) {
        DebugPrint("[-] VirtualAlloc failed for final buffer! Error: %lu\r\n", GetLastError());
        return 1;
    }
    DebugPrint("[+] Final buffer allocated at address: 0x%p\r\n", pFinalBuffer);
    DebugPrint("[+] Final buffer size: %lu bytes\r\n", FINAL_PAYLOAD_SIZE);

    // 3. Simulate having a downloaded fraction's payload.
    //    In a real scenario, this would come from Project 1.2 and 1.3.
    pSingleFractionPayload = (BYTE*)VirtualAlloc(NULL, FRACTION_PAYLOAD_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pSingleFractionPayload == NULL) {
        DebugPrint("[-] VirtualAlloc failed for simulated fraction! Error: %lu\r\n", GetLastError());
        VirtualFree(pFinalBuffer, 0, MEM_RELEASE); // Clean up the final buffer too!
        return 1;
    }

    // Fill the simulated payload with some easily identifiable data
    for (SIZE_T i = 0; i < FRACTION_PAYLOAD_SIZE; i++) {
        pSingleFractionPayload[i] = (BYTE)(i % 256); // Fill with 0, 1, 2, 3,... FF, 0, 1...
    }
    DebugPrint("[+] Simulated fraction payload filled with test pattern.\r\n");

    // 4. Copy the single fraction's payload into the final buffer at offset 0.
    //    This simulates placing the first fraction (ordinal 0).
    CopyMemory(pFinalBuffer, // Destination: Start of the final buffer
               pSingleFractionPayload, // Source: Our simulated payload
               FRACTION_PAYLOAD_SIZE); // Size: Only copy the payload size

    DebugPrint("[+] Copied simulated fraction to final buffer at offset 0.\r\n");
    DebugPrint("[+] First 10 bytes of final buffer: ");
    for (int i = 0; i < 10; i++) {
        DebugPrint("%02X ", pFinalBuffer[i]); // Should print 00 01 02 03 04 05 06 07 08 09
    }
    DebugPrint("\r\n");

    // 5. Cleanup
    DebugPrint("[+] Starting cleanup...\r\n");
    if (pSingleFractionPayload) {
        VirtualFree(pSingleFractionPayload, 0, MEM_RELEASE);
        DebugPrint("[+] Freed simulated fraction buffer.\r\n");
    }
    if (pFinalBuffer) {
        VirtualFree(pFinalBuffer, 0, MEM_RELEASE);
        DebugPrint("[+] Freed final executable buffer.\r\n");
    }

    DebugPrint("[+] Sub-Project 1.4 complete!\r\n");
    return 0;
}