#include <common.hpp>
#include <vga.hpp>
#include <cpu.hpp>
#include <interrupt.hpp>
#include <gdt.hpp>
#include <global.hpp>
#include <memoryallocator.hpp>
#include <virtualmemory.hpp>
#include <pit.hpp>
#include <process.hpp>
#include <test.hpp>
#include <pci.hpp>

#ifndef __i686__
#error "Failed - use i686 compiler instead"
#endif

#ifndef ARCH_32
#error "Failed - use 32 bit architecture"
#endif

/*! @brief Kernel entry function
 */
//![Kernel entry function]

extern "C" void kernel_main()
{
    INIT::ctors();   // initialize global constructors
    INIT::VGA();     // display kernel banner
    INIT::KMALLOC(); // initialize kernel malloc, i.e. kmalloc use supported after this point
    INIT::SSE();     // enable SSE instruction set
    INIT::gdt();     // prepare global descriptor table for x86 protected mode
    INIT::idt();     // install exceptions, interrupts, e.g. page fault handler for paging use later
    INIT::PCI();     // initialize PCI devices
    INIT::PIT();     // initialize timer
    INIT::PAGE();    // initialize page directory, page table (should be after PCI enumeration)
    INIT::NULLPROCESS();

    sti();           // enable interrupt

    RunTests();

    for(;;)
    {
       // busy loop
    }
}
//![Kernel entry function]
