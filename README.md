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
find code implementation of a specific topic. Ultimately to have KernOS be programmable on [RISCV FPGA softcore](https://github.com/kernyan/riscv_de10lite), and perform a [wget](https://github.com/kernyan/pywget). Thereby completing geohot's [transistor to internet challenge](https://github.com/geohot/fromthetransistor)

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
- [PCI - SATA (in progress)](OS/kernel/arch/x86/sata.cpp)
- [Unit testing](Test/Tests)

## Status
Currently only supports
- C++
- x86 architecture

## Milestones
2020 11 01 - Task switching

![alt text][image1]

Currently working on supporting DMA/PIO disk access on PCI SATA. Next to port [Fat32 filesystem prototyped in python](https://github.com/kernyan/pyfs) to the OS.

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
# build kern.bin
mkdir build
pushd build
cmake ../OS
make -j$(nproc)

# build disk image
../Scripts/build-image-qemu.sh   # builds _disk_image and mount at mnt/
sudo ../Scripts/build-root-filesystem.sh  # upload content in KernOS/Disk to mnt/
popd
```

The kernel is still far from being deployable to actual hardware, but feel free to try. 
*Disclaimer: do so at your own risk*

Otherwise, here's the script to running it in qemu. Note that we are mounting _disk_image as AHCI SATA

```bash
cd build
qemu-system-i386 \
  -gdb tcp::1234 -S \
  -m 128M \
  -d cpu_reset,guest_errors \
  -device VGA,vgamem_mb=64 \
  -drive id=disk,file=_disk_image,if=none,format=raw \
  -device ahci,id=ahci\
  -device ide-hd,drive=disk,bus=ahci.0 \
  -kernel ../build/kern.bin
```

Or use 
```bash
cd build
../Scripts/Mount
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
