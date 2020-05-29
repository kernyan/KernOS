//
// Created on 5/19/20.
//

#ifndef KERNOS_INTERRUPT_H
#define KERNOS_INTERRUPT_H

#include <common.h>
#include <utilities.h>
#include <registers.h>

namespace INTRP // interrupt
{
    enum IVT : uint8_t
    {
        RESERVED_START        = 0x0,
        DIV_0_FAULT           = RESERVED_START,
        DEBUG_TRAP            = 0x1,
        NMI_INTERRUPT         = 0x2,
        BREAKPOINT_TRAP       = 0x3,
        OVERFLOW_TRAP         = 0x4,
        OUT_OF_BOUNDS_FAULT   = 0x5,
        INVALID_OPCODE_FAULT  = 0x6,
        NO_MATH_COP_FAULT     = 0x7,
        DOUBLE_FAULT          = 0x8,
        COP_SEG_OVERRUN_FAULT = 0x9,
        INVALID_TSS_FAULT     = 0x0A,
        SEG_NOT_PRESENT_FAULT = 0x0B,
        STACK_SEG_FAULT       = 0x0C,
        GEN_PROTECTION_FAULT  = 0x0D,
        PAGE_FAULT            = 0x0E,
        RESERVED              = 0x0F,
        MATH_FAULT            = 0x10,
        ALIGNMENT_CHECK_FAULT = 0x11,
        MACHINE_CHECK_ABORT   = 0x12,
        SIMD_FP_XF_FAULT      = 0x13,
        RESERVED_END          = 0x1F,

        USER_DEFINED_START    = RESERVED_END + 1,
        USER_DEFINED_END      = 0xFF
    };

    class Mask
    {
    private:
        bool m_InterruptFlag;

    public:
        Mask() : m_InterruptFlag (FlagsRegister() & FLAGS::IF)
        {
#ifndef TEST_BUILD // TODO: have cleaner test_build setup
           cli();
#endif
        }

        ~Mask()
        {
#ifndef TEST_BUILD
            if (m_InterruptFlag) // restore interrupts if enabled previously
                sti();
#endif
        }

    };
} // namespace INTRP

namespace INIT
{
    void idt();
}


#endif //KERNOS_INTERRUPT_H
