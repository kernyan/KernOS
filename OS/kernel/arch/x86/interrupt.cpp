//
// Created on 5/19/20.
//

#include <common.h>
#include <interrupt.h>
#include <utilities.h>

namespace PIC
{
    // 8259 Programmable interrupt controller

    const uint8_t MASTER_COMMAND_PORT = 0x20;
    const uint8_t MASTER_DATA_PORT    = 0x21;
    const uint8_t SLAVE_COMMAND_PORT  = 0xA0;
    const uint8_t SLAVE_DATA_PORT     = 0xA1;

    // refer http://www.brokenthorn.com/Resources/OSDevPic.html
    // ICW1
    // to configure PIC
    /*
     *   | bit | on (off)                        |
     *   |   0 | expect ICW 4 (not expect)       |
     *   |   1 | cascade with slave (only 1 PIC) |
     *   |   2 | 8 byte interval (4 byte)        |
     *   |   3 | edge triggered (level)          |
     *   |   4 | initialize PIC (not initialize) |
     *   | 5-7 | zero for x86                    |
     */
    const uint8_t ICW1_ICW4NEEDED = 0x1;  // expect ICW 4
    const uint8_t ICW1_INIT       = 0x10; // initialize PIC
    const uint8_t ICW1_CONFIG     = ICW1_ICW4NEEDED | ICW1_INIT;

    // ICW2
    // to map base IRQ address to PIC
    /*
     *   | bit | description                                                                         |
     *   | 0-7 | specifies start of interrupt number, must be byte aligned on x86, thus 0-3 not used |
     */
    const uint8_t ICW2_MASTER_OFFSET = 0x20; // IVT 0-31 are reserved for intel exception
    const uint8_t ICW2_SLAVE_OFFSET  = 0x28; // master handles IRQ0-7 (IVT 0x20), slave handles IRQ8-15 (IVT 0x28)

    // ICW3
    // to specify relationship of master/slave to slave/master
    /*  Master
     * | bit | description                     |
     * | 0-7 | IRQ position connected to slave |
     *
     * Slave
     * | bit | description                                  |
     * | 0-2 | IRQ number assigned by master, starts from 1 |
     * | 3-7 | must be zero                                 |
     */
    const uint8_t ICW3_MASTER_SLAVE_POS = 0x4; // tells master that slave is in 3rd position (IRQ2)
    const uint8_t ICW3_SLAVE_ID         = 0x2; // tells slave that it is assigned as IRQ2 on master

    // ICW4
    // to specify PIC operation
    /*
     *   | bit | on (off)                                  |
     *   |   0 | MCS-80/86 mode (x86 mode)                 |
     *   |   1 | perform EOI on last interrupt acknowledge |
     *   | 2-4 | not used                                  |
     *   | 5-7 | must be 0                                 |
     */
    const uint8_t ICW4_CPU = 0x1; // 8086/8088 mode


    void Remap()
    {
        // In protected mode,
        // Bios sets master PIC interrupt number to 0x8:0xF
        // Bios sets slave  PIC interrupt number to 0x70:0x77
        // However master's 0x8:0xF clashes with Intel reserved exception from 0x0:0x1F
        // Thus we remap PIC's interrupt number to 0x20:0x27 (master) and 0x28:0x2F
        // While we only care about ICW2, resetting PIC requires specifying all ICW1-4

        // ICW1 - initialize PIC1, PIC2, and tell them to receive ICW 2,3,4 next
        out8(MASTER_COMMAND_PORT, ICW1_CONFIG); // init flag asks PIC1 to read ICW2,3,4
        out8(SLAVE_COMMAND_PORT,  ICW1_CONFIG); // init flag asks PIC2 to read ICW2,3,4

        // ICW2 - remap IRQ
        out8(MASTER_DATA_PORT, ICW2_MASTER_OFFSET); // specify new interrupt number for master
        out8(SLAVE_DATA_PORT,  ICW2_SLAVE_OFFSET);  // specify new interrupt number for slave

        // ICW3 - inform master/slave about each other
        out8(MASTER_DATA_PORT, ICW3_MASTER_SLAVE_POS); // tells master position of slave in cascade (use IRQ2)
        out8(SLAVE_DATA_PORT,  ICW3_SLAVE_ID);         // tells slave its IRQ number assigned on master

        // ICW4 - set PIC operation
        out8(MASTER_DATA_PORT, ICW4_CPU); // set master PIC to x86 mode
        out8(SLAVE_DATA_PORT, ICW4_CPU);  // set slave PIC to x86 mode

        // disable interrupts (bit set indicates masking of interrupt)
        out8(MASTER_DATA_PORT, 0xff);
        out8(SLAVE_DATA_PORT,  0xff);
        out8(MASTER_DATA_PORT, ~ICW3_MASTER_SLAVE_POS); // enable IRQ2 as cascade on IRQ1
    }

    union __attribute__((packed)) DescriptorEntry
    {
        struct
        {
            uint16_t OffsetLow;
            uint16_t Selector;
            uint8_t Zero;
            uint8_t TypeAttr;
            uint16_t OffsetHi;
        };
        struct
        {
            uint32_t Low;
            uint32_t High;
        };
    };

    const uint16_t IDT_ENTRIES = 256;

    static DescriptorEntry idt_table[IDT_ENTRIES];

    void SetInterruptHandler(DescriptorEntry IdtTable[], size_t Idx, void (*Handler)())
    {
        IdtTable[Idx].OffsetLow = (ptr_t) (Handler) & 0xFFFF;
        IdtTable[Idx].Selector = 0x8; // TODO: match up to gdt
        IdtTable[Idx].Zero = 0x0;
        IdtTable[Idx].TypeAttr = 0x8e;   // TypeAttribute = 0b1000_1110 (P = 1, DPL = 00, S = 0, Type 1110 = 32-bit interrupt gate)
        IdtTable[Idx].OffsetHi = ((ptr_t) (Handler) >> 16) & 0xFFFF;
    }

    void UnhandledIrq()
    {
        Hang();
    }

    inline void Load_lidt(void *lidtAddress, uint16_t LimitUse)
    {
        struct __attribute__((packed))
        {
            uint16_t Limit;
            void *Base;
        } IDTR = { LimitUse, lidtAddress };

        asm volatile
        (
        "lidt %0"   // load interrupt descriptor table
        :           // no output
        : "m"(IDTR) // memory operand allowed, with any kind of address that the machine supports
        );
    }

    void Install_idt()
    {
        for (size_t Idx = 0; Idx < IDT_ENTRIES; ++Idx) {
            SetInterruptHandler(idt_table, Idx, UnhandledIrq);
        }

        Load_lidt(idt_table, IDT_ENTRIES * 8 - 1);
    }
}

namespace INIT
{
    void idt()
    {
        PIC::Remap();  // remap PIC's interrupt number as default by Bios unsuitable in protected mode
        PIC::Install_idt();
    }
}

