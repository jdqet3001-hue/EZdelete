# EZdelete

**A High-Performance Secure File Wiping Utility for Windows systems**

EZdelete is a command-line utility written in C designed to permanently and irreversibly delete files. It surpasses standard file removal operations by overwriting the file's data on the storage medium with garbage patterns before performing the final system call to unlink the file. This process ensures that deleted data cannot be recovered via forensic software or hardware analysis.

---

## Overview

The default behavior of standard deletion commands (such as `rm` or the Windows Recycle Bin) merely removes the file system entry pointing to the data. The actual data remains resident on the physical medium until it is overwritten by new system writes. EZdelete addresses this security gap by programmatically overwriting the file's allocated space, rendering the original content irretrievable.

---

## Core Features

- **Multi-Pass Overwriting**: Supports configurable overwrite passes (default: 3). Each pass writes a distinct pattern (e.g., `0x00`, `0xFF`, or pseudo-random data) to ensure data destruction.
- **Optimized I/O Buffer**: Utilizes a dynamic buffer (default 16 MB) to handle large files efficiently, minimizing system calls and maximizing throughput.
- **Zero Dependencies**: Built strictly with the standard C library (`libc`), ensuring portability across various Unix-like operating systems (Linux, macOS, WSL).
- **Sync Enforcement**: Invokes `fsync()` to force the operating system to flush the overwritten buffers to physical storage, guaranteeing that the garbage data is written to the disk, not just the OS cache.
- **User Prompts**: Implements interactive confirmation prompts by default to prevent accidental data loss, with a `-f` flag available for automated or scripted environments.

---
