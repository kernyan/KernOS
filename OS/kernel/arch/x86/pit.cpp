//
// Created on 7/5/20.
//

#include <pit.h>
#include <utilities.h>
#include <interrupt.h>
#include <process.h>

namespace INTRP
{
    extern DescriptorEntry idt_table[IDT_ENTRIES];
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
    uint64_t timer_ticks;
    uint64_t seconds_since_boot;

    void PIT_825x()
    {
        constexpr uint8_t PIT_SETTING = CHANNEL::TIMER0 | ACCESSMODE::HI_LO | OPMODE::SQUAREWAVE_GENERATOR | DECIMALMODE::BINARY;

        out8(PORT::COMMAND, PIT_SETTING);

        constexpr uint16_t timer_reload = (BASE_FREQ / TICKS_PER_SECOND);

        out8(PORT::CHANNEL_O, timer_reload & 0xFF);
        out8(PORT::CHANNEL_O, (timer_reload >> 8) & 0xFF);
    }

    extern "C" void InterruptTimerHandler()
    {
        {
            IRQAcknowledge AckEOI(INTRP::IVT::TIMER);
        }

        if (++timer_ticks >= TICKS_PER_SECOND)
        {
            ++seconds_since_boot;
            timer_ticks = 0;
        }

        if (seconds_since_boot == 3)
        {
            seconds_since_boot = 0;
            kprintf("3 seconds passed\n");
            PROCESS::Reschedule();
        }
    }
} // namespace TIMER

namespace INIT
{
    void PIT()
    {
        TIMER::PIT_825x();

        UnmaskInterrupt(INTRP::IVT::TIMER);
    }
}