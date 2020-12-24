#include <common.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>
#include <gdt.h>
#include <global.h>
#include <memoryallocator.h>
#include <virtualmemory.h>
#include <pit.h>
#include <process.h>

#ifndef __i686__
#error "Failed - use i686 compiler instead"
#endif

#ifndef ARCH_32
#error "Failed - use 32 bit architecture"
#endif

/*! @brief Kernel entry function
 */
//![Kernel entry function]

void BusyThread1()
{
    kprintf("First process started\n");

    for (;;)
    {
        // busy loop
    }
}

void AccessMemory()
{
    volatile uint32_t *Mem = (uint32_t*) 0x400000;
    *Mem = 10;

    volatile uint32_t *Mem2 = (uint32_t*) 0x500000;
    *Mem2 = 20;

    volatile uint32_t *Mem3 = (uint32_t*) 0x900000;
    *Mem3 = 30;

    volatile uint32_t *Mem4 = (uint32_t*) 0x9010FF;
    *Mem4 = 40;
}

extern "C" void kernel_main()
{
    INIT::ctors();   // initialize global constructors
    INIT::VGA();     // display kernel banner
    INIT::KMALLOC(); // initialize kernel malloc, i.e. kmalloc use supported after this point
    INIT::SSE();     // enable SSE instruction set
    INIT::gdt();     // prepare global descriptor table for x86 protected mode
    INIT::idt();     // install exceptions, interrupts, e.g. page fault handler for paging use later
    INIT::PAGE();    // initialize page directory, page table

    AccessMemory();

    INIT::PIT();     // initialize timer

    INIT::NULLPROCESS();
    PROCESS::CreateProcess (BusyThread1);

    sti();           // enable interrupt

    for(;;)
    {
       // busy loop
    }
}
//![Kernel entry function]
