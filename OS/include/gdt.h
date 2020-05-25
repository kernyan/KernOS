//
// Created on 5/20/20.
//

#ifndef KERNOS_GDT_H
#define KERNOS_GDT_H

namespace GDT
{
    const uint8_t GDT_ENTRIES = 5;

    enum Segment : uint16_t
    {
        Null = 0, // null descriptor
        K_CS,     // kernel code segment
        K_DS,     // kernel data segment
        U_CS,     // user code segment
        U_DS,     // user data segment
    };

    inline constexpr uint8_t SEG_OFFSET(const Segment Seg)
    {
        return (((Seg) << 3) & 0xFF); // each gdt entry is 8 bytes
    }
}

namespace INIT
{
    void gdt();
}

#endif //KERNOS_GDT_H
