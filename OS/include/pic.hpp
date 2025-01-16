//
// Created on 8/15/20.
//

#ifndef KERNOS_PIC_H
#define KERNOS_PIC_H

#include <common.hpp>

/*! @brief 8259 Progammable interrupt controller
 *  @see <a href="http://www.brokenthorn.com/Resources/OSDevPic.html">Broken Thorn's remapping PIC</a>
 *  @see <a href="https://stanislavs.org/helppc/8259.html">8259 Programmable Interrupt Controller (PIC)</a>
 *  @details
 *  @verbatim
    ICW1 to configure PIC
    | bit | on (off)                        |
    |   0 | expect ICW 4 (not expect)       |
    |   1 | cascade with slave (only 1 PIC) |
    |   2 | 8 byte interval (4 byte)        |
    |   3 | edge triggered (level)          |
    |   4 | initialize PIC (not initialize) |
    | 5-7 | zero for x86                    |

    ICW2 to map base IRQ address to PIC
    | bit | description                                                                         |
    | 0-7 | specifies start of interrupt number, must be byte aligned on x86, thus 0-3 not used |

    ICW3 to specify relationship of master/slave to slave/master
        Master
        | bit | description                     |
        | 0-7 | IRQ position connected to slave |

        Slave
        | bit | description                                  |
        | 0-2 | IRQ number assigned by master, starts from 1 |
        | 3-7 | must be zero                                 |

    ICW4 to specify PIC operation
    | bit | on (off)                                  |
    |   0 | MCS-80/86 mode (x86 mode)                 |
    |   1 | perform EOI on last interrupt acknowledge |
    | 2-4 | not used                                  |
    | 5-7 | must be 0                                 |
    @endverbatim
 */
namespace PIC
{
    enum ICW1_0 : uint8_t
    {
        NO_ICW4  = 0b0,
        USE_ICW4 = 0b1
    };

    enum ICW1_1 : uint8_t
    {
        CASCADE = 0b00,
        SINGLE  = 0b10
    };

    enum ICW1_2 : uint8_t
    {
        INTRP_8B = 0b000,
        INTRP_4B = 0b100
    };

    enum ICW1_3 : uint8_t
    {
        EDGE_TRIGGERED  = 0b0000,
        LEVEL_TRIGGERED = 0b1000
    };

    enum ICW1_4 : uint8_t
    {
        INVALID  = 0b00000,
        USE_ICW1 = 0b10000
    };

    enum ICW4_0 : uint8_t
    {
        MODE_8085 = 0b0,
        MODE_8086 = 0b1
    };

    enum ICW4_1 : uint8_t
    {
        EOI      = 0b00,
        AUTO_EOI = 0b10
    };

    enum ICW4_4 : uint8_t
    {
        SEQUENTIAL           = 0b00000,
        SPECIAL_FULLY_NESTED = 0b10000
    };

    enum OCW2 : uint8_t ///< Operation Control Word 2 (use with PORTS::CsrPort)
    {
        NON_SPECIFIC_EOI    = 0b0010'0000, ///< 0x20
        NOP                 = 0b0100'0000,
        SPECIFIC_EOI        = 0b0110'0000,
        ROTATE_AEOI         = 0b1000'0000,
        ROTATE_NON_SPEC_EOI = 0b1010'0000,
        SET_PRIORITY_CMD    = 0b1100'0000,
        ROTATE_SPEC_EOI     = 0b1110'0000,
    };

    void Remap();

} // namespace PIC

#endif //KERNOS_PIC_H
