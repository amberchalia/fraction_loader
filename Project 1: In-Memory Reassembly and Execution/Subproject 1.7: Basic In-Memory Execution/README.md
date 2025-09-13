# Fraction Loader: In-Memory Loader Project

## Overview
This repository documents my journey to build a **stealthy in-memory loader** from scratch. The goal is to _deeply understand_ modern malware evasion techniques and improve my skills in *offensive security*, *Windows internals*, and *low-level programming*<sub>1</sub>.

This project is a series of sub-projects, each building on the previous one. I will update this repository as I complete each stage.

---

## Sub-Projects and Learning Objectives

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

- **Subproject 1.6: Robust Error Handling & Crash Resistance** *(This document)*  
  This subproject focuses on making the loader resilient: **no crashes**, graceful cleanup, secure overwrite of sensitive memory, and a top-level SEH safety net.

---

## Guide to Implementing Sub-project 1.6

The objective of Sub-project 1.6 is to ensure the loader never crashes and always cleans up sensitive data. Instead of `try/catch` (C++ exception overhead), use structured error checking with `goto clean;` and a Windows-specific Structured Exception Handling (SEH) frame (`__try/__except`).

### Step 1: Fortify Your Existing Cleanup
Ensure your `clean:` label handles partial state — only free resources that were actually allocated. Wipe sensitive payload memory before releasing it.

```c
clean:
    // 1. Clean up the main payload buffer if it was allocated
    if (pFinalPayload) {
        SecureZeroMemory(pFinalPayload, total_payload_size); // Wipe it first!
        VirtualFree(pFinalPayload, 0, MEM_RELEASE);
    }

    // 2. Clean up any other loop-specific resources that might still be open
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    // ... Free any other dynamically allocated memory ...
    // Note: Resources freed inside the loop (like per-fraction buffers) don't need to be here.
```

**Key Addition:** `SecureZeroMemory` is critical. It wipes the sensitive payload data from memory before freeing it, reducing forensic recoverability of freed memory.

---

### Step 2: Add a Top-Level SEH Frame
Wrap your entire `main()` logic (after variable declarations) in `__try/__except` so unexpected access violations or other exceptions are caught and routed to cleanup.

```c
int main() {
    // ... [Your variable declarations] ...

    __try {
        // --- ALL YOUR MAIN LOGIC GOES HERE ---
        // Your two loops, allocation, download, execution code all go inside here.
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // This block runs if ANY exception happens in the __try block
        debugprint(L"[!] A critical exception occurred: 0x%08x\n", GetExceptionCode());
        // Call your cleanup function to free resources gracefully
        // It's safe to call it even if some resources are NULL.
        goto clean;
    }

clean:
    // ... [Your consolidated cleanup code] ...
    return 0;
}
```

---

### Step 3: Create a Dedicated Cleanup Function
Centralize cleanup into a single function so it can be called from multiple exit paths (normal returns, `goto` error paths, or the `__except` handler).

```c
void CleanupResources(HINTERNET hSession, HINTERNET hConnect, HINTERNET hRequest, BYTE* pFinalPayload, SIZE_T payloadSize) {
    if (pFinalPayload) {
        SecureZeroMemory(pFinalPayload, payloadSize);
        VirtualFree(pFinalPayload, 0, MEM_RELEASE);
    }
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}
```

Call `CleanupResources(...)` from your `clean:` label and from the `__except` block as needed.

---

### Goal of Sub-project 1.6
The outcome is that your loader should **never crash**. Any failure—a failed download, a failed memory allocation, a network timeout, or even an access violation—should be caught, logged (quietly) to your debug channel, and result in the program exiting cleanly after securely wiping all sensitive memory.

This produces a quiet, professional, and resilient loader.

---

## Future Projects
- **Project 2: Encryption Layer**  
  Will add `XOR`, `RC4`, and `AES` to *obfuscate payloads*<sub>security</sub>.

- **Project 3: Custom HTTP Client**  
  Will replace `WinHTTP` with raw sockets for **stealth**.

- **Project 4: API Unhooking and Syscalls**  
  Will bypass user-mode <ins>EDR hooks</ins> using direct syscalls.

- **Project 5 and Beyond**  
  Will explore advanced techniques like *process hollowing*, *anti-analysis*, and `ETW` patching<sup>advanced</sup>.

---

## Technical Skills
- **Languages**: `C/C++`, `Win32 API/NTAPI`, `x86 Assembly`  
- **Concepts**: Windows Internals (`PE` Format, Memory Management, `IAT`), _Process Injection_  
- **Networking**: `HTTP/S` protocols, `WinHTTP`, *Raw Sockets*  
- **Security**: Cryptography (`XOR`, `AES`), Evasion (`Syscalls`, `API Hashing`)  
- **Tools**: `Visual Studio`, `WinDbg`, `Process Monitor`, `DebugView`

---

## Disclaimer
This project is for **educational and research purposes only**. It is intended for:
- Penetration testers learning *post-exploitation techniques*.
- Security researchers studying _evasion methods_.
- Blue teamers developing **detection strategies**<sub>defense</sub>.

Use this code only on systems you **own** or have <ins>explicit permission</ins> to test. The author is ~~not responsible~~ for any misuse.

---

## Progress
- **Status**: *Actively developing*  
- **Completed**: Subprojects 1.1 to 1.5  
- **Next Steps**: Implementing Subproject 1.6 (error handling, SEH, secure cleanup) and planning Subprojects 1.7–1.10 and Project 2<sup>upcoming</sup>.

Check the commit history or sub-project folders for the *latest updates*.

---

## Learn More
Read about my challenges and technical insights on my blog: [rootfu.in](https://rootfu.in).

![Hit Counter](https://hits.sh/github.com/amberchalia/fraction_loader.svg)
