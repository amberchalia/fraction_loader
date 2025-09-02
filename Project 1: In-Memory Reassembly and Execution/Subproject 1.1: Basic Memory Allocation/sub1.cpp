#include <windows.h>

int main(){
    // Message to be written into allocated memory (in ASCII hex values)
    BYTE msg[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 
                   0x2C, 0x20, 
                   0x4D, 0x65, 0x6D, 0x6F, 0x72, 0x79, 
                   0x21, 
                   0x00 }; // "Hello, Memory!" + null terminator

    // Store the size of the message
    size_t datasize = sizeof(msg);

    // Allocate memory in the process’s virtual address space
    // MEM_COMMIT | MEM_RESERVE -> reserves and commits memory
    // PAGE_READWRITE -> allows read and write access
    LPVOID needmemory = VirtualAlloc(NULL, datasize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    if(needmemory == NULL){
        // If allocation fails, log error to debugger
        OutputDebugStringA("failed to get space in memory");
        return 1;
    }

    // Copy the message into the allocated memory
    memcpy(needmemory, msg, datasize);

    // Print the message from allocated memory to debugger output
    OutputDebugStringA((LPCSTR)needmemory);
    OutputDebugStringA("\n");

    // Free the allocated memory when done
    BOOL freememory = VirtualFree(needmemory, 0, MEM_RELEASE);
    if(!freememory){
        // If freeing fails, log error
        OutputDebugStringA("failed to free the memory");
        return 1;
    }

    // Final success message to debugger
    OutputDebugStringA("[+] Operation completed successfully.\n");
    return 1;
}
