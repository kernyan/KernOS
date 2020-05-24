#ifndef KERNOS_UTILITIES_H
#define KERNOS_UTILITIES_H

#include <common.h>
#include <kprintf.h>

size_t Strlen(const char *Str);

namespace
{
    inline void out8(uint16_t Port, uint8_t Value)
    {
        asm volatile // extended asm syntax, volatile to prevent optimization
        (
        "outb %0, %1"
        :              // No output
        : "a"(Value),  // Input 0: "a" constraint is to place Value in eax before asm command
          "Nd"(Port)   //       1: "Nd" constraint is to place Port as one byte literal (without using register)
        );
    }

    [[noreturn]] inline void Hang()
    {
        asm volatile("cli; hlt");
        for (;;) {
        }
    }

    template<uint8_t N>
    inline uint32_t DWord()
    {
        return (1 << N);
    }

    #define GHETTO_GET_CR(x)  uint32_t Local_##x;     \
                              asm volatile            \
                              (                       \
                              "mov %%"#x", %%eax\n\t" \
                              "mov %%eax, %0\n\t"     \
                              : "=m"(Local_##x)       \
                              :                       \
                              : "%eax"                \
                              );

    inline uint32_t ReadCR0()
    {
        GHETTO_GET_CR(cr0)
        return Local_cr0;
    }

    inline uint32_t ReadCR1()
    {
        GHETTO_GET_CR(cr1)
        return Local_cr1;
    }

    inline uint32_t ReadCR2()
    {
        GHETTO_GET_CR(cr2)
        return Local_cr2;
    }

    inline uint32_t ReadCR3()
    {
        GHETTO_GET_CR(cr3)
        return Local_cr3;
    }

    inline uint32_t ReadCR4()
    {
        GHETTO_GET_CR(cr4)
        return Local_cr4;
    }

    inline void kassert (bool Condition, const char *Str = nullptr)
    {
       if (!Condition) {
           if (Str)
               kprintf (Str);
           Hang();
       }
    }
} // unnamed namespace

#endif //KERNOS_UTILITIES_H
