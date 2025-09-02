# Sub-Project 1: Memory Allocation Demo

This project shows allocation and deallocation using Windows API, simulating a downloaded data fraction with a 32-byte header. It parses the header to extract an ordinal number and payload, using VirtualAlloc and VirtualFree for memory management. Debug output via OutputDebugStringA helps trace execution, designed for Windows with winhttp.lib.

## Compilation
x86_64-w64-mingw32-g++ sub5.cpp -o sub5.exe  -mwindows

## PoC Video
▶ Watch here: https://youtu.be/QsLKzwCjShI?si=u6W0B6BxEI2wepn8
