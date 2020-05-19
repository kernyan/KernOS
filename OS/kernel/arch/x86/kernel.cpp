#include <common.h>
#include <vga.h>
#include <cpu.h>

#if !defined(__i386__)
#error "Failed - use i686 compiler instead"
#endif 
 
extern "C" void kernel_main (ptr_t PageTablePhyAddress)
{
  INIT::SSE ();

  VGA::Vga Display;
     
  Display.Puts ("KernOS - v0.1\n");
  Display.Puts ("Hello World!\n");
}
