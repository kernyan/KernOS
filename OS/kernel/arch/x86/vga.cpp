#include <vga.h>

namespace VGA
{
    Vga Display;

    void Vga::PutChar(unsigned char Char)
    {
        if (Char == '\n') { // new line

            m_Col = 0;
            (m_Row == VGA_HEIGHT) ? m_Row = 0 : ++m_Row; // wrap around display

        } else {

            PutChar(Entry(Char, m_Color), m_Row, m_Col);

            if (++m_Col == VGA_WIDTH) { // wrap around display

                m_Col = 0;

                if (++m_Row == VGA_HEIGHT) {
                    m_Row = 0;
                }
            }
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
        for (size_t i = 0; i < Strlen(Str); ++i)
            PutChar(Str[i]);
    }
} // namespace VGA

namespace INIT
{
    void VGA()
    {
        VGA::Display.Puts("KernOS - v0.1\n");
        VGA::Display.Puts("Hello World!\n");
    }
}