//
// Created on 5/20/20.
//

#ifndef KERNOS_GDT_H
#define KERNOS_GDT_H

/*! @brief global descriptor table
 */
namespace GDT
{
    const uint8_t GDT_ENTRIES = 5;

    /*! @brief Code segments
     */
    enum Segment : uint16_t
    {
        Null = 0, ///< null descriptor
        K_CS,     ///< kernel code segment
        K_DS,     ///< kernel data segment
        U_CS,     ///< user code segment
        U_DS,     ///< user data segment
    };

    /*! @brief Translates code segment enum to code segment selector
     * @param Seg - enum of kernel/user space code/data segment
     * @return Byte position of code segment in gdt_table
     */
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
