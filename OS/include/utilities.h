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

    #define GHETTO_GET_CONTROL_REGISTERS  uint32_t Local_cr0, Local_cr2 , Local_cr3;                \
                                          asm volatile                                              \
                                          (                                                         \
                                          "mov %%cr0, %%eax\n\t"                                    \
                                          "mov %%eax, %0\n\t"                                       \
                                          "mov %%cr2, %%eax\n\t"                                    \
                                          "mov %%eax, %1\n\t"                                       \
                                          "mov %%cr3, %%eax\n\t"                                    \
                                          "mov %%eax, %2\n\t"                                       \
                                          : "=m"(Local_cr0), "=m"(Local_cr2), "=m"(Local_cr3)       \
                                          :                                                         \
                                          : "%eax"                                                  \
                                          );


    inline void kassert (bool Condition)
    {
       if (!Condition)
       {
          Hang();
       }
    }
} // unnamed namespace

#endif // __utilities_h__
