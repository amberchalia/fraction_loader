# Subproject 1.7: Basic In-Memory Execution

A foundational implementation demonstrating the core technique of executing position-independent shellcode directly from memory. This project establishes the essential pattern of allocating executable memory, writing shellcode to it, and invoking it as a function, forming the critical building block for more advanced fileless execution techniques.

## Compilation
x86_64-w64-mingw32-g++ sub7.cpp -o sub7.exe  -mwindows
