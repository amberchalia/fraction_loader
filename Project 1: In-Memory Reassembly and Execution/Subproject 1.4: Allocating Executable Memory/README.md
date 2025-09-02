# Subproject 1.4: Allocating Executable Memory

This C program demonstrates allocating a contiguous executable memory buffer using Windows API's VirtualAlloc with PAGE_EXECUTE_READWRITE permissions. It simulates a fraction payload, copies it into the buffer, and verifies the operation with debug output. The code ensures proper cleanup with VirtualFree and is Windows-specific, requiring winhttp.lib.

## Compilation
x86_64-w64-mingw32-g++ sub4.cpp -o sub4.exe  -mwindows

## PoC Video
▶ Watch here: https://youtu.be/JDDVAkfgJjU?si=rU8svmYzfVkTsoJ_
