#include <common.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>
#include <gdt.h>
#include <global.h>
#include <memoryallocator.h>

#ifndef __i686__
#error "Failed - use i686 compiler instead"
#endif

#ifndef ARCH_32
#error "Failed - use 32 bit architecture"
#endif

extern "C" void kernel_main()
{
    INIT::ctors();   // initialize global constructors
    INIT::VGA();     // display kernel banner
    INIT::KMALLOC(); // initialize kernel malloc, i.e. kmalloc use supported after this point
    INIT::SSE();     // enable SSE instruction set
    INIT::gdt();     // prepare global descriptor table for x86 protected mode
    INIT::idt();     // install exceptions, interrupts, e.g. page fault handler for paging use later
}
