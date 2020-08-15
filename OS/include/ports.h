//
// Created by 8/14/20.
//

#ifndef KERNOS_PORTS_H
#define KERNOS_PORTS_H

namespace PORTS
{
    enum PORT : uint16_t
    {
        // Programmable Interrupt Controller ports
        PIC1_COMMAND  = 0x20,
        PIC1_DATA     = 0x21,
        PIC2_COMMAND  = 0xA0,
        PIC2_DATA     = 0xA1,

        // Programmable Interval Timer ports
        PIT_CHANNEL_O = 0x40,
        PIT_CHANNEL_1 = 0x41,
        PIT_CHANNEL_2 = 0x42,
        PIT_COMMAND   = 0x43
    };
}


#endif //KERNOS_PORTS_H
