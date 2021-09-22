//
// Created on 5/24/20.
//

#include <kprintf.h>
#include <vga.h>

namespace VGA
{
  extern Vga Display;
}

namespace util
{
  void _putchar(char c)
  {
      VGA::Display.PutChar(c);
  }
}
