#include <windows.h>

int main(){
    BYTE msg[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x4D, 0x65, 0x6D, 0x6F, 0x72, 0x79, 0x21, 0x00 };
    size_t datasize = sizeof(msg);

    LPVOID needmemory = VirtualAlloc(NULL, datasize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    if(needmemory == NULL){
        OutputDebugStringA("failed to get space in mmeory");
        return 1;
    }

    memcpy(needmemory, msg, datasize);

    OutputDebugStringA((LPCSTR)needmemory);
    OutputDebugStringA("\n");

    BOOL freememory = VirtualFree(needmemory, 0, MEM_RELEASE);
    if(!freememory){
        OutputDebugStringA("failed to free the memory");
        return 1;
    }

    OutputDebugStringA("[+] Operation completed successfully.\n");
    return 1;
}
