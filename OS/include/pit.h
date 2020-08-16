//
// Created on 7/5/20.
//

#ifndef KERNOS_PIT_H
#define KERNOS_PIT_H

#include <common.h>

/*! @brief Timer namespace
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

} // namespace TIMER

namespace INIT
{
    /*! @brief Initializes programmable interval timer
     *  @details
     *  1) Initializes 8254 Programmable Interrupt Timer\n
     *  2) Installs timer interrupt handler
     *  @see TIMER::PIT_825x
     */
    void PIT();
}

#endif //KERNOS_PIT_H
