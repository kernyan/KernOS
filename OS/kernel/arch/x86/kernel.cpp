#include <common.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>
#include <gdt.h>
#include <global.h>
#include <memoryallocator.h>
#include <virtualmemory.h>
#include <pit.h>

#ifndef __i686__
#error "Failed - use i686 compiler instead"
#endif

#ifndef ARCH_32
#error "Failed - use 32 bit architecture"
#endif

/*! @brief Kernel entry function
 */
//![Kernel entry function]

int fib(int N)
{
    if (N == 0)
    {
        return 0;
    }
    else if (N == 1)
    {
        return 1;
    }

    return fib(N - 1) + fib (N - 2);
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
    INIT::PIT();     // initialize timer

    //asm volatile("sti" ::: "memory"); // test to trigger timer interrupt
    //int* a = (int*) 0xffffffff;
    //int b = (int) (*a); // test to trigger page fault handler
    //fib(150); // test to keep OS running long enough to trigger timer interrupt

    kprintf("reach");
}
//![Kernel entry function]
