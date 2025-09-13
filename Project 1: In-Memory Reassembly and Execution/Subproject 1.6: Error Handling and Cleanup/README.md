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

Check the commit history or sub-project folders for the *latest updates*.

---

## Learn More
Read about my challenges and technical insights on my blog: [rootfu.in](https://rootfu.in).

![Hit Counter](https://hits.sh/github.com/amberchalia/fraction_loader.svg)
