#include <common.h>
#include <vga.h>
#include <cpu.h>
#include <interrupt.h>
#include <gdt.h>

#if !defined(__i386__)
#error "Failed - use i686 compiler instead"
#endif

extern "C" void kernel_main()
{
    INIT::SSE();
    INIT::gdt();
    INIT::idt();

    VGA::Vga Display;

    Display.Puts("KernOS - v0.1\n");
    Display.Puts("Hello World!\n");
}
