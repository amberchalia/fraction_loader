#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// Debug print helper (avoids C runtime functions like printf)
// Uses wsprintfA instead of vsnprintf for smaller footprint
void DebugPrint(LPCSTR format, ...) {
    CHAR buffer[1024];
    va_list args;
    va_start(args, format);
    wvsprintfA(buffer, format, args); // Format string into buffer
    va_end(args);
    OutputDebugStringA(buffer);       // Send message to debugger
}

int main() {
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    BOOL bResults = FALSE;
    DWORD dwSize = 0, dwDownloaded = 0;
    BYTE* pDownloadBuffer = NULL;
    SIZE_T dwTotalSize = 0;        // Track total bytes downloaded
    SIZE_T dwBufferCapacity = 0;   // Current buffer capacity
    const SIZE_T dwInitialCapacity = 4096; // Initial buffer size

    // Target configuration
    LPCWSTR pwszServerName = L"192.168.29.80"; // Server IP/hostname
    INTERNET_PORT nPort = 80;                  // Change to 443 for HTTPS
    LPCWSTR pwszObjectName = L"/file.txt";     // File to fetch

    DebugPrint("[+] Initializing WinHTTP session.\r\n");

    // 1. Open a WinHTTP session with a common User-Agent string
    hSession = WinHttpOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession) {
        DebugPrint("[-] WinHttpOpen failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }

    // 2. Connect to the server
    hConnect = WinHttpConnect(hSession, pwszServerName, nPort, 0);
    if (!hConnect) {
        DebugPrint("[-] WinHttpConnect failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }

    // 3. Create a GET request
    hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",                 // HTTP method
        pwszObjectName,         // File path on server
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0 // Change to WINHTTP_FLAG_SECURE for HTTPS
    );
    if (!hRequest) {
        DebugPrint("[-] WinHttpOpenRequest failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }

    // 4. Send the request
    bResults = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0);
    if (!bResults) {
        DebugPrint("[-] WinHttpSendRequest failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }

    // 5. Wait for and receive the response
    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults) {
        DebugPrint("[-] WinHttpReceiveResponse failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }

    // 6. Allocate an initial buffer for downloaded data
    pDownloadBuffer = (BYTE*)VirtualAlloc(NULL, dwInitialCapacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDownloadBuffer) {
        DebugPrint("[-] Initial VirtualAlloc failed: %lu\r\n", GetLastError());
        goto CLEANUP;
    }
    dwBufferCapacity = dwInitialCapacity;
    DebugPrint("[+] Initial buffer allocated. Capacity: %lu bytes\r\n", dwBufferCapacity);

    // 7. Download loop
    DebugPrint("[+] Starting download loop...\r\n");
    do {
        // Query how many bytes are available
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            DebugPrint("[-] WinHttpQueryDataAvailable failed: %lu\r\n", GetLastError());
            break;
        }
        if (dwSize == 0) break; // No more data available

        // Expand buffer if needed
        if (dwTotalSize + dwSize > dwBufferCapacity) {
            SIZE_T dwNewCapacity = dwBufferCapacity;
            while (dwTotalSize + dwSize > dwNewCapacity) {
                dwNewCapacity *= 2; // Double buffer size until it fits
            }
            DebugPrint("[+] Growing buffer from %lu to %lu bytes.\r\n", dwBufferCapacity, dwNewCapacity);

            BYTE* pNewBuffer = (BYTE*)VirtualAlloc(NULL, dwNewCapacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (!pNewBuffer) {
                DebugPrint("[-] VirtualAlloc for growth failed: %lu\r\n", GetLastError());
                break;
            }
            CopyMemory(pNewBuffer, pDownloadBuffer, dwTotalSize); // Copy old data
            VirtualFree(pDownloadBuffer, 0, MEM_RELEASE);         // Free old buffer
            pDownloadBuffer = pNewBuffer;
            dwBufferCapacity = dwNewCapacity;
        }

        // Read chunk of data into buffer
        if (!WinHttpReadData(hRequest, (LPVOID)(pDownloadBuffer + dwTotalSize), dwSize, &dwDownloaded)) {
            DebugPrint("[-] WinHttpReadData failed: %lu\r\n", GetLastError());
            break;
        }

        // Update counters
        dwTotalSize += dwDownloaded;
        DebugPrint("[+] Read %lu of %lu available bytes. Total: %lu bytes\r\n", dwDownloaded, dwSize, dwTotalSize);

    } while (dwSize > 0);

    DebugPrint("[+] Download complete. Total size: %lu bytes\r\n", dwTotalSize);

    // 8. Debugging: print first 64 bytes and as string
    if (dwTotalSize > 0) {
        BYTE* pDisplayBuffer = (BYTE*)VirtualAlloc(NULL, dwTotalSize + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (pDisplayBuffer) {
            CopyMemory(pDisplayBuffer, pDownloadBuffer, dwTotalSize);
            pDisplayBuffer[dwTotalSize] = '\0'; // Null-terminate string

            DebugPrint("[+] First 64 bytes:\r\n");
            DWORD displaySize = (dwTotalSize < 64) ? (DWORD)dwTotalSize : 64;
            for (DWORD i = 0; i < displaySize; i++) {
                DebugPrint("%02X ", pDownloadBuffer[i]); // Print hex values
            }
            DebugPrint("\r\n[+] Content as string: %s\r\n", (char*)pDisplayBuffer);

            VirtualFree(pDisplayBuffer, 0, MEM_RELEASE);
        }
    }

CLEANUP:
    // 9. Clean up handles and memory
    DebugPrint("[+] Starting cleanup...\r\n");
    if (pDownloadBuffer) VirtualFree(pDownloadBuffer, 0, MEM_RELEASE);
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    DebugPrint("[+] Operation completed.\r\n");
    return 0;
}
