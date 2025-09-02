# Environment & Tooling Setup

## Overview
This document outlines the **tools**, *software*, and <ins>environment configuration</ins> used to develop, build, and test the projects in the *Fraction Loader* repository. Consistency in tooling is **crucial** for reproducing results and debugging _low-level system interactions_<sub>1</sub>.

## Host Operating System
- **Primary OS**: `Kali Linux` (Rolling Release)<sup>host</sup>
- **Hypervisor**: `KVM/QEMU` with `virt-manager` for GUI management
- **Reasoning**: Using a *Linux host* with `KVM` provides <ins>near-native performance</ins> for Windows VMs and is the standard for ~~casual~~ serious security work.

## Development & Build Tools
- **Compiler**: `MinGW-w64` (`x86_64-w64-mingw32-g++`)  
  Used to _cross-compile_ Windows executables from the Linux host.  
  *Installation*: `sudo apt install mingw-w64`
- **Code Editor**: `Visual Studio Code`  
  *Extensions*: `C/C++`, `Compiler Explorer`, `Hex Editor`, `WSL` (if applicable)
- **Version Control**: `Git`

## Target / Analysis Environment
- **OS**: `Microsoft Windows 10/11` (Virtual Machine)<sup>target</sup>  
  *Purpose*: The target for testing all loaders and payloads, _isolated_ from the host network for safety.
- **Configuration**:  
  - `Windows Defender` disabled (to test against EDRs manually later).  
  - <ins>Administrative access</ins> enabled.

## Analysis & Debugging Tools (Windows VM)
The following tools are installed on the Windows VM for *dynamic analysis* and **debugging**:

- **Sysinternals Suite**: The cornerstone of Windows analysis.
- **Process Monitor (Procmon)**: Monitors _file system_, _registry_, _process_, and *network activity* in real-time.
- **Process Explorer (Procexp)**: Advanced task manager for inspecting processes, handles, and DLLs.
- **VMMap**: Analyzes process memory, <ins>crucial</ins> for verifying `VirtualAlloc` operations and memory permissions.
- **DebugView (Dbgview.exe)**: Captures `OutputDebugString` output from loaders. Run as *Administrator*.
- **Event Viewer**: For viewing logs from `Sysmon` and Windows itself.
- **Sysmon**: System activity monitor configured with a strong config (e.g., `SwiftOnSecurity`'s) to log process creations, network connections, and more.
- **Optional**: `IDA Pro`/`Ghidra` for ~~dynamic~~ static analysis of compiled binaries.

## Networking
- **VM Network**: Configured in *Host-Only* or *NAT* mode in `KVM`.  
  *Purpose*: Allows the Windows VM to communicate with the host (`Kali`) for downloading payloads, while being <ins>isolated</ins> from the external internet. The host acts as the web server.

## Testing Workflow
- **Code**: Write and edit code on the *Kali Linux* host.
- **Compile**: Cross-compile on `Kali` using `x86_64-w64-mingw32-g++`.
- **Transfer**: Move the compiled `.exe` to the Windows VM (using a _shared folder_ or `python3 -m http.server` on Kali and downloading in the VM).
- **Analyze**: On the Windows VM, open `DebugView` and `Procmon` to set up monitoring.
- **Execute**: Run the loader executable and observe its behavior in the monitoring tools.
- **Debug**: Analyze logs and process behavior to identify and fix ~~errors~~ issues.
