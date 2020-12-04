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

namespace VM
{
  extern uint32_t pagetable0;
  extern uint32_t pagetable1;
}

void AccessMemory()
{
  volatile uint32_t* FourBytes_Below4MB  = (uint32_t *) 0x3ffffc;
  *FourBytes_Below4MB = 10;

  volatile uint32_t* ThreeBytes_Below4MB  = (uint32_t *) 0x3ffffd;
  *ThreeBytes_Below4MB = 10;
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
