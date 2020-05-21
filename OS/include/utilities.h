#ifndef __utilities_h__
#define __utilities_h__

#include <common.h>

size_t Strlen(const char *Str);

namespace
{
    inline void out8(uint16_t Port, uint8_t Value)
    {
        asm volatile // extended asm syntax, volatile to prevent optimization
        (
        "outb %0, %1"
        :              // No input
        : "a"(Value),  // Output 0: "a" constraint is to place Value in eax before asm command
        "Nd"(Port)     //        1: "Nd" constraint is to place Port as one byte literal (without using register)
        );
    }

    [[noreturn]] inline void Hang()
    {
        asm volatile("cli; hlt");
        for (;;) {
        }
    }

} // unnamed namespace

#endif // __utilities_h__
