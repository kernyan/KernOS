//
// Created on 5/19/20.
//

#ifndef KERNOS_INTERRUPT_H
#define KERNOS_INTERRUPT_H

#include <common.h>
#include <utilities.h>
#include <registers.h>


namespace PIC
{

    /*! @brief PIC1 command port */
    const uint8_t MASTER_COMMAND_PORT = 0x20;

    /*! @brief PIC1 data port */
    const uint8_t MASTER_DATA_PORT    = 0x21;

    /*! @brief PIC2 command port */
    const uint8_t SLAVE_COMMAND_PORT  = 0xA0;

    /*! @brief PIC2 data port */
    const uint8_t SLAVE_DATA_PORT     = 0xA1;

    const uint8_t ICW1_ICW4NEEDED = 0x1;  ///< expect ICW 4
    const uint8_t ICW1_INIT       = 0x10; ///< initialize PIC
    const uint8_t ICW1_CONFIG     = ICW1_ICW4NEEDED | ICW1_INIT;

    const uint8_t ICW2_MASTER_OFFSET = 0x20; ///< IVT 0-31 are reserved for intel exception
    const uint8_t ICW2_SLAVE_OFFSET  = 0x28; ///< master handles IRQ0-7 (IVT 0x20), slave handles IRQ8-15 (IVT 0x28)

    const uint8_t ICW3_MASTER_SLAVE_POS = 0x4; ///< tells master that slave is in 3rd position (IRQ2)
    const uint8_t ICW3_SLAVE_ID         = 0x2; ///< tells slave that it is assigned as IRQ2 on master

    const uint8_t ICW4_CPU = 0x1; ///< 8086/8088 mode

    void UnmaskInterrupt (uint16_t Port, uint8_t Irq);
}

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

        USER_DEFINED_START    = PIC::ICW2_MASTER_OFFSET, ///< 0x20

        TIMER_INTERRUPT       = USER_DEFINED_START,

        USER_DEFINED_END      = 0xFF                 ///< 0xFF
    };

    static_assert(IVT::USER_DEFINED_START == (IVT::RESERVED_END + 1));

    const uint16_t IDT_ENTRIES = 256;

    /*! @brief Exception and interrupt gate descriptor entry
     *  @see See AMD64 Architecture Programmer's Manual Volume 2: System Programming (PDF) (Technical report). 2013. p. 86.
     *  @details
     *  @verbatim
        Interrupt & Trap gate descriptor entry
        |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
        | Higher 16 bits of target code address         |P |DPL  |0|    Type    | Reserved              |
        | Target code-segment selector                  | Lower 16 bits of target code address          |
        @endverbatim
     */
    union [[gnu::packed]] DescriptorEntry
    {
        struct
        {
            uint16_t m_OffsetLow;
            uint16_t m_CS_Selector;
            uint8_t m_Reserve;
            uint8_t m_Access;
            uint16_t m_OffsetHigh;
        };
        struct
        {
            uint32_t Low;
            uint32_t High;
        };
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

    void RegisterHandler(DescriptorEntry IdtTable[], size_t Idx, func_ptr Handler);

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
