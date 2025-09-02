# Subproject 1.5: Sequential Fraction Assembly

This C program simulates handling multiple data fractions, allocating a final executable buffer with VirtualAlloc and PAGE_EXECUTE_READWRITE permissions. It creates and parses simulated fractions with headers, copies payloads to the correct offsets, and verifies the result. Windows-specific, it uses winhttp.lib and debug output for tracing, with proper cleanup via VirtualFree.

## Compilation
x86_64-w64-mingw32-g++ sub5.cpp -o sub5.exe  -mwindows

## PoC Video
▶ Watch here: https://youtu.be/Cu6skxh_D3M?si=DcCfq1lgo6LpVK0t
