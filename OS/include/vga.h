#ifndef KERNOS_VGA_H
#define KERNOS_VGA_H

#include <utilities.h>

namespace VGA
{

// https://en.wikipedia.org/wiki/VGA-compatible_text_mode
// VGA text buffer convention
// ________________________________________________
// |         Attribute           |    Character    |
// |   7   |   6 5 4  | 3 2 1 0  | 7 6 5 4 3 2 1 0 |
// | Blink | Bg color | Fg color | Code point      |

    enum class COLOR : uint8_t
    {
        BLACK = 0,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        LIGHT_GREY,
        DARK_GREY,
        LIGHT_BLUE,
        LIGHT_GREEN,
        LIGHT_CYAN,
        LIGHT_RED,
        LIGHT_MAGENTA,
        LIGHT_BROWN,
        WHITE
    };

    typedef uint8_t VgaColor;
    typedef uint16_t VgaChar;

    class Vga
    {
        static const size_t VGA_WIDTH = 80;
        static const size_t VGA_HEIGHT = 25;

        size_t m_Row;
        size_t m_Col;
        uint16_t *const m_Buffer = (uint16_t *) 0xB8000; // VGA text buffer address, by convention
        VgaColor m_Color;

        VgaColor EntryColor(COLOR Fore, COLOR Back) const;

        VgaChar Entry(unsigned char uc, VgaColor Color) const;

        void Fill(const VgaChar Char);

        void PutChar(unsigned char Char);

        void Initialize();

    public:

        Vga()
        {
            Initialize();
        }

        void Puts(const char *Str);

        void PutChar(VgaChar Char, size_t Row, size_t Col);

        void SetColor(VgaColor Color);
    };

    inline VgaColor Vga::EntryColor(COLOR Fore, COLOR Back) const
    {
        return static_cast<VgaColor>(Fore)
               | static_cast<VgaColor>(Back) << 4;
    }

    inline VgaChar Vga::Entry(unsigned char uc, VgaColor Color) const
    {
        return static_cast<VgaChar> (uc)
               | static_cast<VgaChar> (Color) << 8;
    }

    inline void Vga::PutChar(VgaChar Char, size_t Row, size_t Col)
    {
        m_Buffer[Row * VGA_WIDTH + Col] = Char;
    }

    inline void Vga::SetColor(VgaColor Color)
    {
        m_Color = Color;
    }


} // namespace VGA

namespace INIT
{
    void VGA();
}

#endif //KERNOS_VGA_H
