# KernOS
Minimal feature educational operating system

# Timeline
1. 02/02/2020
  - boot to [kernel_main](OS/kernel/arch/x86/kernel.cpp)
  - display through [VGA display](OS/include/vga.h)
2. 23/05/2020
  - load [global descriptor table](OS/kernel/arch/x86/gdt.cpp) for x86 protected mode
  - load [interrupt descriptor table](OS/kernel/arch/x86/interrupt.cpp)

# Miscellaneous
1. 05/03/2020
  - migrate from Makefile to [CMake](OS/CMakeLists.txt)
  - set up GTest
