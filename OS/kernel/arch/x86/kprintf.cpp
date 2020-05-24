//
// Created on 5/24/20.
//

#include <kprintf.h>
#include <vga.h>

namespace VGA
{
    extern Vga Display;
}

void kprintf(const char* Str)
{
    VGA::Display.Puts(Str);
}

