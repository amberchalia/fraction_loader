Project: Building a Stealthy In-Memory Loader from Scratch
Overview
This repository documents my journey in building a sophisticated, in-memory loader from the ground up. The goal is to deeply understand modern malware evasion techniques by implementing them, thereby enhancing my skills in offensive security, Windows internals, and low-level programming.

This is not a single project, but a structured series of sub-projects, each building upon the last. I will be uploading and updating this repository as I complete each stage.

🧩 Sub-Projects & Learning Objectives
The project is broken down into the following main phases and sub-projects:

Main Project 1: In-Memory Reassembly & Execution
1.1: Basic Memory Allocation: Mastered VirtualAlloc and in-memory data manipulation.

1.2: Downloading to Memory: Implemented a custom HTTP client using WinHTTP to retrieve payloads directly into memory, bypassing disk.

1.3: Parsing Headers In-Memory: Developed a parser to extract metadata (ordinals) from downloaded fractions without file handles.

1.4: Allocating Executable Memory: Prepared the final destination buffer with PAGE_EXECUTE_READWRITE permissions.

1.5: Sequential Fraction Assembly: Created the logic to reassemble multiple payload chunks into a contiguous executable memory region based on their ordinals.

1.6-1.10: Upcoming: Error handling, execution, protection adjustments, and final testing.

Future Main Projects (In Progress)
Project 2: Encryption/Decryption Layer: Will integrate XOR, RC4, and AES encryption to obfuscate payloads.

Project 3: Custom HTTP Client: Will replace WinHTTP with a raw socket implementation for increased stealth.

Project 4: API Unhooking & Direct Syscalls: Will implement techniques to bypass user-mode EDR hooks.

Project 5+: Advanced techniques including process hollowing, anti-analysis, and ETW patching.

🛠️ Technical Skills Demonstrated
Languages: C/C++, Win32 API/NTAPI, x86 Assembly

Core Concepts: Windows Internals (PE Format, Memory Management, IAT), Process Injection

Networking: HTTP/S protocols, WinHTTP, Raw Sockets

Security: Cryptography (XOR, AES), Evasion Techniques (Syscalls, API Hashing)

Tools: Visual Studio, WinDbg, Process Monitor, DebugView

⚠️ Disclaimer & Purpose
This project is for educational and research purposes only. The techniques demonstrated are intended for:

Penetration Testers to improve their understanding of post-exploitation tradecraft.

Security Researchers to analyze modern evasion methods.

Blue Teamers to develop better detection strategies.

All code should only be used on systems you own and have explicit permission to test. The author is not responsible for any misuse of this information.

📁 Repository Structure
text
/Project-1-In-Memory-Loader
│   README.md          # This file
│   /Subproject-1.1    # Code and notes for each sub-project
│   /Subproject-1.2
│   /Subproject-1.3
│   ...
📈 Progress Status
Current Status: Actively developing. Sub-projects 1.1 through 1.5 are complete.
I am uploading code and documentation as I finish each sub-project. Check the commit history and individual sub-project folders for the most current status.

🔗 Connect & Learn More
I document my thought process, challenges, and technical deep dives on my personal blog: rootfu.in
