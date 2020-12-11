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

void kputchar(const char c)
{
  VGA::Display.PutChar(c);
}

template<>
void PrintIntImpl(uint32_t t)
{
  if (t)
  {
    PrintIntImpl(t / 10);
    kputchar    (t % 10 + 0x30);
  }
}

template<>
void PrintIntImpl(uint64_t t)
{
  if (t)
  {
    PrintIntImpl(t / 10);
    kputchar    (t % 10 + 0x30);
  }
}

template<typename T>
void PrintHex(T t)
{
  uint8_t* Pos = (uint8_t*) &t;

  kputchar('0');
  kputchar('x');

  for (int8_t i = sizeof(T) - 1; i >= 0; --i)
  {
    uint8_t MSH = Pos[i] / 16; 
    uint8_t LSH = Pos[i] % 16; 

    PrintHexInt(MSH);
    PrintHexInt(LSH);
  }
}

template
void PrintHex(uint32_t t);

template
void PrintHex(uint64_t t);
