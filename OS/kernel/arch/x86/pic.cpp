//
// Created on 8/15/20.
//

#include <pic.hpp>
#include <interrupt.hpp>

namespace PIC
{
    /*! @brief Remap default 8259 PIC interrupt number to new range
     *  @details
     *  In protected mode,\n
     *  Bios sets master PIC interrupt number to 0x8:0xF\n
     *  Bios sets slave  PIC interrupt number to 0x70:0x77\n
     *  However master's 0x8:0xF clashes with Intel reserved exception from 0x0:0x1F
     *  Thus we remap PIC's interrupt number to 0x20:0x27 (master) and 0x28:0x2F
     *  While we only care about ICW2, resetting PIC requires specifying all ICW1-4
     *  @see <a href="https://wiki.osdev.org/8259_PIC#Protected_Mode">Remapping PIC on protected mode</a>
     */
    void Remap()
    {
        // ICW1 - initialize PIC1, PIC2, and tell them to receive ICW 2,3,4 next
        constexpr uint8_t ICW1_SETTING = ICW1_0::USE_ICW4
                                       | ICW1_1::CASCADE
                                       | ICW1_2::INTRP_8B
                                       | ICW1_3::EDGE_TRIGGERED
                                       | ICW1_4::USE_ICW1;

        out8(PORTS::PIC1_COMMAND, ICW1_SETTING); // init flag asks PIC1 to read ICW2,3,4
        out8(PORTS::PIC2_COMMAND, ICW1_SETTING); // init flag asks PIC2 to read ICW2,3,4

        // ICW2 - remap IRQ
        out8(PORTS::PIC1_DATA, INTRP::IVT::PIC1_OFFSET);  // PIC1 handles IRQ0-7 (IVT 0x20)
        out8(PORTS::PIC2_DATA, INTRP::IVT::PIC2_OFFSET);  // PIC2 handles IRQ8-15 (IVT 0x28)

        // ICW3 - inform master/slave about each other
        out8(PORTS::PIC1_DATA, IrqPort(INTRP::IVT::PIC2_CASCADE).m_IrqNumber);             // tells PIC1 position of PIC2 in cascade (use IRQ2)
        out8(PORTS::PIC2_DATA, INTRP::IVT::PIC2_CASCADE - INTRP::IVT::USER_DEFINED_START); // tells PIC2 its IRQ number assigned on PIC1

        // ICW4 - set PIC operation
        constexpr uint8_t ICW4_SETTING = PIC::ICW4_0::MODE_8086 | PIC::ICW4_1::EOI | PIC::ICW4_4::SEQUENTIAL;

        out8(PORTS::PIC1_DATA, ICW4_SETTING);
        out8(PORTS::PIC2_DATA, ICW4_SETTING);

        // disable interrupts (bit set indicates masking of interrupt)
        out8(PORTS::PIC1_DATA, INTRP::IVT::USER_DEFINED_END);
        out8(PORTS::PIC2_DATA, INTRP::IVT::USER_DEFINED_END);

        UnmaskInterrupt(INTRP::IVT::PIC2_CASCADE); // enable IRQ2 as cascade on IRQ1
    }

} // namespace PIC

