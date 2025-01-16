#include <vga.hpp>
#include <ascii.hpp>

#ifdef TEST_BUILD
#include <iostream>
#endif

namespace VGA
{
   Vga Display;

   size_t Vga::NextRow()
   {
      if (++m_Row == VGA_HEIGHT)
         m_Row = 0;

      do { // clear line
         PutChar(Entry(ASCII::SPACE, m_Color), m_Row, m_Col++);
      } while (m_Col != VGA_WIDTH);

      m_Col = 0;

      return m_Row;
   }

   size_t Vga::NextCol()
   {
      if (++m_Col == VGA_WIDTH) // wrap around display
      { 
         m_Col = 0;

         NextRow();
      }

      return m_Col;
   }

   void Vga::PutChar(unsigned char Char)
   {
      auto Put = [&](const char c)
         {
            PutChar(Entry(c, m_Color), m_Row, m_Col);
            NextCol();
         };

      if (Char == '\n') // new line
      {  
         m_Col = 0;
         NextRow();
      } 
      else if (Char == '\t') // tab
      {  
         for (size_t i = 0; i < 3; ++i) 
         {
            Put(ASCII::SPACE);
         }
      } 
      else 
      {
         Put(Char);
      }
   }

   void Vga::Fill(const VgaChar Char)
   {
       for (m_Row = 0; m_Row < VGA_HEIGHT; ++m_Row) {
           for (m_Col = 0; m_Col < VGA_WIDTH; ++m_Col) {
               PutChar(Char, m_Row, m_Col);
           }
       }

       m_Row = 0;
       m_Col = 0;
   }

   void Vga::Initialize()
   {
       SetColor(EntryColor(COLOR::LIGHT_GREY, COLOR::BLACK));

       const auto Char = Entry(' ', m_Color);

       Fill(Char);
   }

   void Vga::Puts(const char *Str)
   {
#ifdef TEST_BUILD
      std::cout << Str;
#else
      for (size_t i = 0; i < Strlen(Str); ++i)
         PutChar(Str[i]);
#endif
   }
} // namespace VGA

namespace INIT
{
   void VGA()
   {
      VGA::Display.Puts("KernOS - v0.2\n");
      VGA::Display.Puts("Reticulating splines!\n");
   }
}
