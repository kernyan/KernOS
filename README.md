<p align="center">
    <img src="docs/res/logo.png" alt="KernOS logo" width="72" height="72">
</p>
<h3 align="center">KernOS</h3>

[//]: # (Image References)
[image1]: ./images/TaskSwitching.png "Task_Switching"

## Table of contents 
- [Project goal](#project-goal)
- [Documented topics](#documented-topics)
- [Status](#status)
- [Toolchain](#toolchain-and-qemu)
- [Build and run](#build-and-run)
- [Test](#test)
- [Debugging](#debugging)

## Project goal
To have a working well-documented Operating System code that lets the reader quickly
find code implementation of a specific topic.

## Documented topics 
The doxygen documentation below is a good place to start exploring 
- [Doxygen documentation](https://kernyan.github.io/KernOS/html/index.html)

For specific topics, use links below,
- [linking kernel](OS/compile/linker.ld)
- [VGA display](OS/kernel/arch/x86/vga.cpp)
- [Global constructor](OS/kernel/arch/x86/global.cpp) 
- [Kernel memory allocator](OS/kernel/arch/x86/memoryallocator.cpp)
- [Streaming SIMD extension initialization](OS/kernel/arch/x86/cpu.cpp)
- [Global descriptor table](OS/kernel/arch/x86/gdt.cpp)
- [Exception and interrupt handler](OS/kernel/arch/x86/interrupt.cpp)
- [Paging](OS/kernel/arch/x86/virtualmemory.cpp)
- [Task switching](OS/kernel/arch/x86/process.cpp)
- [Unit testing](Test/Tests)

## Status
Currently only supports
- C++
- x86 architecture

## Milestones
2020 11 01 - Task switching

![alt text][image1]

Next is to get TLB working, and ultimately to a GUI desktop!

## Toolchain and qemu
Build i686-elf toolchain as below
```bash
cd Toolchain
./BuildToolchain.sh
```
Add path to ~/.profile or ~/.bashrc by 
```
PATH="$HOME/opt/cross/bin:$PATH"
``` 

Get qemu from package manager. Or build from below if need to debug qemu, e.g. using rr
```bash
cd Qemu
./BuildQemu.sh
```

## Build and run
Build kern.bin
```bash
cd KernOS
mkdir build && cd build
cmake ../OS
make
```

The kernel is still far from being deployable to actual hardware, but feel free to try. 
*Disclaimer: do so at your own risk*

Otherwise, here's the script to running it in qemu
```bash
cd KernOS/build
qemu-system-i386 \
-m 128M \
-d cpu_reset,guest_errors \
-device VGA,vgamem_mb=64 \
-kernel kern.bin
```

## Test
Testing framework setup is unorthodox as 
1) I wanted access to standard libraries, and
2) gtest required different toolchain

This led to a separate CMakeLists.txt in the [Test](Test/Tests/CMakeLists.txt) directory.

To build and run test

```bash
cd KernOS/Test
mkdir build && cd build
cmake ../Tests
make && ctest
```

## Debugging
Debugging requires running qemu with `-S` command to prevent CPU from starting, 
and exposing a port for gdb to attach to.

Here's the script to launch qemu for debug
```bash
cd KernOS/build
qemu-system-i386 \
-gdb tcp::1234 -S \
-m 128M \
-d cpu_reset,guest_errors \
-device VGA,vgamem_mb=64 \
-kernel kern.bin
```

and to attach from gdb
```gdb
file kern.bin
target remote localhost:1234
break kernel_main
continue
```
