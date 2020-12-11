//
// Created on 5/24/20.
//

#ifndef KERNOS_KPRINTF_H
#define KERNOS_KPRINTF_H

#include <common.h>

/*! @brief Prints string to display
 * @param Str - string to print
 */
void kprintf(const char* Str);
void kputchar(const char c);

template<typename T>
void PrintIntImpl(T);

template<typename T>
void PrintHexInt(T t)
{
  if (t < 10)
  {
    kputchar(t + 0x30);
  }
  else
  {
    kputchar((t - 10) + 0x41);
  }
}

template<typename T>
void PrintHex(T);

template<typename T>
void PrintInt(T t)
{
  if (t)
  {
    PrintIntImpl(t);
  }
  else
  {
    kputchar('0');
  }
}

template<typename T, typename... Args>
void kprintf(const char *Str, T t, Args... args)
{
  while (*Str)
  {
    if (*Str == '%')
    {
      switch (*(++Str))
      {
      case 'i':
        PrintInt(t);
        break;
      case 'h':
        PrintHex(t);
        break;
      }

      kprintf(++Str, args...);
      return;
    }

    kputchar(*Str++);
  }
}

#endif //KERNOS_KPRINTF_H
