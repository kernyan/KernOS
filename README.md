# KernOS
Minimal feature educational operating system

# Timeline
1. 02/02/2020
  - boot to [kernel_main](OS/kernel/arch/x86/kernel.cpp)
  - setup [VGA display](OS/include/vga.h)
2. 19/05/2020
  - initialize [8259 PIC](OS/kernel/arch/x86/interrupt.cpp)
  - load [interrupt descriptor table](OS/kernel/arch/x86/interrupt.cpp)
3. 23/05/2020
  - load [global descriptor table](OS/kernel/arch/x86/gdt.cpp) for x86 protected mode
  - call [global constructors](OS/kernel/arch/x86/global.cpp)
4. 29/05/2020
  - kernel malloc [implementation](OS/kernel/arch/x86/memoryallocator.cpp)
  - kernel malloc [test](Test/Tests/kmalloc_test.cpp)

# Miscellaneous
1. 05/03/2020
  - migrate from Makefile to [CMake](OS/CMakeLists.txt)
  - set up GTest
