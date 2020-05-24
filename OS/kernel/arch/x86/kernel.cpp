#include <common.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>
#include <gdt.h>
#include <global.h>

#if !defined(__i386__)
#error "Failed - use i686 compiler instead"
#endif

extern "C" void kernel_main()
{
    INIT::ctors();
    INIT::VGA();
    INIT::SSE();
    INIT::gdt();
    INIT::idt();
}
