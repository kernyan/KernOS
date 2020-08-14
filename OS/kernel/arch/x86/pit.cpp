//
// Created on 7/5/20.
//

#include <pit.h>
#include <common.h>
#include <utilities.h>
#include <interrupt.h>

namespace INTRP
{
    extern DescriptorEntry idt_table[IDT_ENTRIES];

    void TimerInterruptHandler();
}

/*! @brief Programmable Interval Timer (PIT) 8253/8254 chip
 *  @see https://wiki.osdev.org/Programmable_Interval_Timer, www.osdever.net/bkerndev/Docs/pit.htm
 *  @details
 *  @verbatim
    Mode/Command byte
    | 07 | 06 | 05 | 04 | 03 | 02 | 01 |     00     |
    | Channel |  Access |   Operating  | BCD/Binary |
    @endverbatim
 */
namespace TIMER
{
    enum PORT : uint8_t
    {
        CHANNEL_O = 0x40, ///< I/O port for channel 0 data port
        CHANNEL_1 = 0x41, ///< I/O port for channel 1 data port
        CHANNEL_2 = 0x42, ///< I/O port for channel 2 data port
        COMMAND   = 0x43  ///< Mode/command register
    };

    enum CHANNEL : uint8_t
    {
        TIMER0 = 0x00, ///< (00)00'0000 Channel 0
        TIMER1 = 0x40, ///< (01)00'0000 Channel 1
        TIMER2 = 0x80  ///< (10)00'0000 Channel 2
    };

    enum ACCESSMODE : uint8_t
    {
        LATCH   = 0x00, ///< 00(00)'0000 Latch count value
        LO_ONLY = 0x10, ///< 00(01)'0000 Low byte only
        HI_ONLY = 0x20, ///< 00(10)'0000 Low byte only
        HI_LO   = 0x30  ///< 00(11)'0000 Low byte only
    };

    enum OPMODE : uint8_t
    {
        COUNTDOWN             = 0x0, ///< 0000'(000)0
        ONESHOT               = 0x2, ///< 0000'(001)0
        RATE_GENERATOR        = 0x4, ///< 0000'(010)0
        SQUAREWAVE_GENERATOR  = 0x6, ///< 0000'(011)0
        SOFTWARE_STROBE       = 0x8, ///< 0000'(100)0
        HARDWARE_STROBE       = 0xA, ///< 0000'(101)0
        RATE_GENERATOR2       = 0xC, ///< 0000'(110)0
        SQUAREWAVE_GENERATOR2 = 0xE  ///< 0000'(111)0
    };

    enum DECIMALMODE : uint8_t
    {
        BINARY = 0x0, ///< 0000'000(0) 16 bit binary
        BCD    = 0x1  ///< 0000'000(1)  4 digit binary coded decimal
    };

    const uint32_t BASE_FREQ        = 1'193'182; ///< 14.31818 MHz / 12 (due to hardware historical reason)
    const uint16_t TICKS_PER_SECOND = 1'000;

    void PIT_825x()
    {
        constexpr uint8_t PIT_SETTING = CHANNEL::TIMER0 | ACCESSMODE::HI_LO | OPMODE::SQUAREWAVE_GENERATOR | DECIMALMODE::BINARY;

        out8(PORT::COMMAND, PIT_SETTING);

        constexpr uint16_t timer_reload = (BASE_FREQ / TICKS_PER_SECOND);

        out8(PORT::CHANNEL_O, timer_reload & 0xFF);
        out8(PORT::CHANNEL_O, (timer_reload >> 8) & 0xFF);
    }
}

namespace INIT
{
    void PIT()
    {
        TIMER::PIT_825x();
        // TODO: install interrupt handler, unmask IRQ, and enable interrupt here instead of in INIT::idt and kernel_main
    }
}

