#include <vga.h>

using namespace VGA;

void Vga::PutChar(unsigned char Char)
{
    if (Char == '\n') { // new line
        _Col = 0;
        (_Row == VGA_HEIGHT) ? _Row = 0 : ++_Row;
    } else {
        PutChar(Entry(Char, _Color), _Row, _Col);
        if (++_Col == VGA_WIDTH) { // wrap around display
            _Col = 0;
            if (++_Row == VGA_HEIGHT) {
                _Row = 0;
            }
        }
    }
}

void Vga::Fill(const VgaChar Char)
{
    for (_Row = 0; _Row < VGA_HEIGHT; ++_Row) {
        for (_Col = 0; _Col < VGA_WIDTH; ++_Col) {
            PutChar(Char, _Row, _Col);
        }
    }

    _Row = 0;
    _Col = 0;
}

void Vga::Initialize()
{
    SetColor(EntryColor(COLOR::LIGHT_GREY, COLOR::BLACK));

    const auto Char = Entry(' ', _Color);

    Fill(Char);
}

void Vga::Puts(const char *Str)
{
    for (size_t i = 0; i < Strlen(Str); ++i) {
        PutChar(Str[i]);
    }
}
