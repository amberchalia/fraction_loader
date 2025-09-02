# Subproject 1.3: Parsing Headers In-Memory

This project shows allocation and deallocation using Windows API, simulating a downloaded data fraction with a 32-byte header. It parses the header to extract an ordinal number and payload, using VirtualAlloc and VirtualFree for memory management. Debug output via OutputDebugStringA helps trace execution, designed for Windows with winhttp.lib.

## Compilation
x86_64-w64-mingw32-g++ sub3.cpp -o sub3.exe  -mwindows

## PoC Video
▶ Watch here: https://youtu.be/QsLKzwCjShI?si=u6W0B6BxEI2wepn8
