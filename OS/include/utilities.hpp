#ifndef KERNOS_UTILITIES_H
#define KERNOS_UTILITIES_H

#include <common.hpp>
#include <kprintf.hpp>

size_t Strlen(const char *Str);

namespace
{
    inline void cli() // disable maskable interrupt
    {
        asm volatile (
        "cli"
        : // no output
        : // no input
        : "memory"
        );
    }

    inline void sti() // enable maskable interrupt
    {
        asm volatile (
        "sti"
        : // no output
        : // no input
        : "memory" // memory barrier
        );

    }

    inline uint32_t FlagsRegister()
    {
        uint32_t Flags;
        asm volatile (
        "pushf\n"
        "pop %0\n"
        : "=rm"(Flags)  // output
        :               // no input
        :"memory"       // memory barrier
        );
        return Flags;
    }

    inline void out8(uint16_t Port, uint8_t Value)
    {
        asm volatile (
        "outb %0, %1"
        :              // No output
        : "a"(Value),  // Input 0: "a" constraint is to place Value in eax before asm command
          "Nd"(Port)   //       1: "Nd" constraint is to place Port as one byte literal (without using register)
        );
    }

    inline void out32(uint16_t Port, uint32_t Value)
    {
        asm volatile (
        "outl %0, %1"
        :              // No output
        : "a"(Value),  // Input 0: "a" constraint is to place Value in eax before asm command
          "Nd"(Port)   //       1: "Nd" constraint is to place Port as 4 byte literal (without using register)
        );
    }

    inline uint8_t in8(uint16_t Port)
    {
        uint8_t Value;

        asm volatile (
        "inb %1, %0"
        : "=a"(Value)  // Output 0: write byte to Value
        : "Nd"(Port)   // Input  1: "Nd" constraint is to place Port as one byte literal (without using register)
        );

        return Value;
    }

    inline uint32_t in32(uint16_t Port)
    {
        uint32_t Value;

        asm volatile (
        "inl %1, %0"
        : "=a"(Value)  // Output 0: write byte to Value
        : "Nd"(Port)   // Input  1: "Nd" constraint is to place Port as 4 byte literal (without using register)
        );

        return Value;
    }

    [[noreturn]] inline void Hang()
    {
        asm volatile (
        "cli\n"         // disable interrupt
        "hlt\n"         // halt processor when interrupt occurs, only non-maskable interrupt, NMI possible after cli
        );

        for (;;) {      // to prevent [[noreturn]] warning, and in case NMI occur
        }
    }

    template<uint8_t N>
    inline constexpr uint32_t DWord()
    {
        return (1 << N);
    }

    #define GHETTO_GET_CR(x)  uint32_t Local_##x;     \
                              asm volatile            \
                              (                       \
                              "mov %%"#x", %%eax\n"   \
                              "mov %%eax, %0\n"       \
                              : "=m"(Local_##x)       \
                              :                       \
                              : "%eax"                \
                              );

    inline uint32_t ReadCR0()
    {
        GHETTO_GET_CR(cr0)
        return Local_cr0;
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
               kprintf(Str);
           Hang();
       }
    }

    [[noreturn]] inline void kpanic (const char *Str = nullptr)
    {
       kprintf(Str);
       Hang();
    }

    inline void *kmemset(void* str, uint8_t c, size_t n)
    {
      asm volatile
      (
        "rep stosb\n"
			  : "=D"(str), "=c"(n)
			  : "0"(str), "1"(n), "a"(c)
				: "memory"
			);
			return str;
		}
} // unnamed namespace

#endif //KERNOS_UTILITIES_H
