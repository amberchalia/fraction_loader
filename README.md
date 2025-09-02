# Fraction Loader: In-Memory Loader Project

## Overview
This repository documents my journey to build a **stealthy in-memory loader** from scratch. The goal is to _deeply understand_ modern malware evasion techniques and improve my skills in *offensive security*, *Windows internals*, and *low-level programming*<sub>1</sub>.

This project is a series of sub-projects, each building on the previous one. I will update this repository as I complete each stage.

## Sub-Projects and Learning Objectives
The project is organized into main phases, starting with:

### Project 1: In-Memory Reassembly and Execution
- **Subproject 1.1: Basic Memory Allocation**  
  Learned to use `VirtualAlloc` for in-memory data manipulation<sup>core</sup>.
- **Subproject 1.2: Downloading to Memory**  
  Built a custom HTTP client with `WinHTTP` to fetch payloads into memory, _avoiding disk writes_.
- **Subproject 1.3: Parsing Headers In-Memory**  
  Created a parser to extract metadata (ordinals) from payload fractions without file handles.
- **Subproject 1.4: Allocating Executable Memory**  
  Set up a memory buffer with <ins>`PAGE_EXECUTE_READWRITE` permissions</ins>.
- **Subproject 1.5: Sequential Fraction Assembly**  
  Developed logic to reassemble payload chunks into a **contiguous executable memory region** using ordinals.
- **Subprojects 1.6–1.10** (_In Progress_)  
  Will focus on error handling, execution, memory protection, and testing ~~(not yet started)~~.

### Future Projects
- **Project 2: Encryption Layer**  
  Will add `XOR`, `RC4`, and `AES` to *obfuscate payloads*<sub>security</sub>.
- **Project 3: Custom HTTP Client**  
  Will replace `WinHTTP` with raw sockets for **stealth**.
- **Project 4: API Unhooking and Syscalls**  
  Will bypass user-mode <ins>EDR hooks</ins> using direct syscalls.
- **Project 5 and Beyond**  
  Will explore advanced techniques like *process hollowing*, *anti-analysis*, and `ETW` patching<sup>advanced</sup>.

## Technical Skills
- **Languages**: `C/C++`, `Win32 API/NTAPI`, `x86 Assembly`
- **Concepts**: Windows Internals (`PE` Format, Memory Management, `IAT`), _Process Injection_
- **Networking**: `HTTP/S` protocols, `WinHTTP`, *Raw Sockets*
- **Security**: Cryptography (`XOR`, `AES`), Evasion (`Syscalls`, `API Hashing`)
- **Tools**: `Visual Studio`, `WinDbg`, `Process Monitor`, `DebugView`

## Disclaimer
This project is for ***educational and research purposes only***. It is intended for:
- Penetration testers learning *post-exploitation techniques*.
- Security researchers studying _evasion methods_.
- Blue teamers developing **detection strategies**<sub>defense</sub>.

Use this code only on systems you **own** or have <ins>explicit permission</ins> to test. The author is ~~not responsible~~ for any misuse.

## Progress
- **Status**: *Actively developing*
- **Completed**: Subprojects 1.1 to 1.5
- **Next Steps**: Working on Subprojects 1.6 to 1.10 and planning Project 2<sup>upcoming</sup>

Check the commit history or sub-project folders for the *latest updates*.

## Learn More
Read about my challenges and technical insights on my blog: [rootfu.in](https://rootfu.in).

![Hit Counter](https://hits.sh/github.com/amberchalia/fraction_loader.svg)
