//
// Created on 5/19/20.
//

#ifndef KERNOS_INTERRUPT_H
#define KERNOS_INTERRUPT_H

#include <common.h>
#include <utilities.h>
#include <registers.h>

/*! @brief interrupt namespace
 */
namespace INTRP // interrupt
{
    enum IVT : uint8_t
    {
        RESERVED_START        = 0x0,                 ///< 0x0
        DIV_0_FAULT           = RESERVED_START,      ///< 0x0
        DEBUG_TRAP            = 0x1,                 ///< 0x1
        NMI_INTERRUPT         = 0x2,                 ///< 0x2
        BREAKPOINT_TRAP       = 0x3,                 ///< 0x3
        OVERFLOW_TRAP         = 0x4,                 ///< 0x4
        OUT_OF_BOUNDS_FAULT   = 0x5,                 ///< 0x5
        INVALID_OPCODE_FAULT  = 0x6,                 ///< 0x6
        NO_MATH_COP_FAULT     = 0x7,                 ///< 0x7
        DOUBLE_FAULT          = 0x8,                 ///< 0x8
        COP_SEG_OVERRUN_FAULT = 0x9,                 ///< 0x9
        INVALID_TSS_FAULT     = 0x0A,                ///< 0x0A
        SEG_NOT_PRESENT_FAULT = 0x0B,                ///< 0x0B
        STACK_SEG_FAULT       = 0x0C,                ///< 0x0C
        GEN_PROTECTION_FAULT  = 0x0D,                ///< 0x0D
        PAGE_FAULT            = 0x0E,                ///< 0x0E
        RESERVED              = 0x0F,                ///< 0x0F
        MATH_FAULT            = 0x10,                ///< 0x10
        ALIGNMENT_CHECK_FAULT = 0x11,                ///< 0x11
        MACHINE_CHECK_ABORT   = 0x12,                ///< 0x12
        SIMD_FP_XF_FAULT      = 0x13,                ///< 0x13
        RESERVED_END          = 0x1F,                ///< 0x1F

        USER_DEFINED_START    = RESERVED_END + 1,    ///< 0x20
        USER_DEFINED_END      = 0xFF                 ///< 0xFF
    };

    /*! @brief Disables interrupt on construction, restores previous interrupt mask on scope exit
     *  @details CPU FLAGS::IF being set indicates interrupts enabled
     */
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
    /*! @brief Creates interrupt descriptor table and loads to CPU
     *  @details
     *  1) Remaps Programmable interrupt controller's interrupt number\n
     *  2) Assigns exceptions and interrupt handlers
     *  @see PIC::Remap
     *  @see INTRP::Install_idt
     */
    void idt();
}


#endif //KERNOS_INTERRUPT_H
