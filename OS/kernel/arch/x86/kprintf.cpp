//
// Created on 5/24/20.
//

#include <kprintf.hpp>
#include <vga.hpp>

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
