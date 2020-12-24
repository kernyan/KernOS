#ifndef KERNOS_VGA_H
#define KERNOS_VGA_H

#include <utilities.h>

/*! @brief Video Graphics Array namespace
 *  @details
 */
namespace VGA
{

    /*! @brief VGA text buffer convention
     *  @see <a href="https://en.wikipedia.org/wiki/VGA-compatible_text_mode">VGA compatible text mode</a>
     *  @verbatim
        ________________________________________________
        |         Attribute           |    Character    |
        |   7   |   6 5 4  | 3 2 1 0  | 7 6 5 4 3 2 1 0 |
        | Blink | Bg color | Fg color | Code point      |
        @endverbatim
     */
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

    /*! @brief Manages access to VGA driver
     *  @details Provides primitive console-like line printing, newline, and wrap-around
     */
    class Vga
    {
        static const size_t VGA_WIDTH = 80;
        static const size_t VGA_HEIGHT = 25;

        size_t m_Row;
        size_t m_Col;
        uint16_t *const m_Buffer = (uint16_t *) 0xB8000; ///< VGA memory mapped text buffer address
        VgaColor m_Color;

        VgaColor EntryColor(COLOR Fore, COLOR Back) const;

        VgaChar Entry(unsigned char uc, VgaColor Color) const;

        void Fill(const VgaChar Char);

        void Initialize();

        size_t NextRow();
        size_t NextCol();

    public:

        Vga()
        {
#ifndef TEST_BUILD  // TODO: temporary until we mock vga's direct memory map
            Initialize();
#endif
        }

        void PutChar(unsigned char Char);

        /*! @brief Print string to display
         * @param Str
         */
        void Puts(const char *Str);

        /*! @brief Put single character to display
         * @param Char
         * @param Row
         * @param Col
         */
        void PutChar(VgaChar Char, size_t Row, size_t Col);

        /*! @brief Set text color
         * @param VgaColor text color @see COLOR
         * @return
         */
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
    /*! @brief Clear VGA display, and print kernel banner
     */
    void VGA();
}

#endif //KERNOS_VGA_H
