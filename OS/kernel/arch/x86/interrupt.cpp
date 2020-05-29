//
// Created on 5/19/20.
//

#include <common.h>
#include <interrupt.h>
#include <utilities.h>
#include <gdt.h>
#include <accessright.h>

/*! @brief 8259 Progammable interrupt controller
 *  @see <a href="http://www.brokenthorn.com/Resources/OSDevPic.html">Broken Thorn's remapping PIC</a>
 *
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
        out8(SLAVE_DATA_PORT,  ICW4_CPU); // set slave PIC to x86 mode

        // disable interrupts (bit set indicates masking of interrupt)
        out8(MASTER_DATA_PORT, 0xff);
        out8(SLAVE_DATA_PORT,  0xff);
        out8(MASTER_DATA_PORT, ~ICW3_MASTER_SLAVE_POS); // enable IRQ2 as cascade on IRQ1
    }
} // namespace PIC

namespace INTRP // interrupt
{
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

    static DescriptorEntry idt_table[IDT_ENTRIES];

    /*! @brief Creates interrupt descriptor entries in idt_table, and loads into CPU
     *  @details
     *  Installing exception or interrupt handler by mapping handler address to idt_table
     *  @param[out] IdtTable to be loaded into CPU later
     *  @param[in] Idx into IdtTable for corresponding exception/interrupt number
     *  @param[in] Handler address of exception/interrupt handler
     */
    void RegisterHandler(DescriptorEntry IdtTable[], size_t Idx, func_ptr Handler)
    {
        IdtTable[Idx].m_OffsetLow   = (ptr_t) (Handler) & 0xFFFF;
        IdtTable[Idx].m_CS_Selector = GDT::SEG_OFFSET(GDT::Segment::K_CS);
        IdtTable[Idx].m_Reserve     = 0x0;
        IdtTable[Idx].m_Access      = AR::INTERRUPT_ACCESS;
        IdtTable[Idx].m_OffsetHigh  = ((ptr_t) (Handler) >> 16) & 0xFFFF;
    }

    /*! @brief default unhandled exception handler
     */
    void UnhandledException()
    {
        kprintf("Unhandled exception encountered\n");
        Hang();
    }

    /*! @brief default unhandled interrupt handler
     */
    void UnhandledInterrupt()
    {
        kprintf("Unhandled interrupt encountered\n");
        Hang();
    }

    /*! @brief Installs default exception handler to all exceptions
     * @param IdtTable
     */
    void SetExceptionHandler(DescriptorEntry IdtTable[])
    {
        for (size_t Idx = IVT::RESERVED_START; Idx <= IVT::RESERVED_END; ++Idx)
            RegisterHandler(IdtTable, Idx, UnhandledException);
    }

    /*! @brief Installs default interrupt handler to all interrupts
     * @param IdtTable
     */
    void SetInterruptHandler(DescriptorEntry IdtTable[])
    {
        for (size_t Idx = IVT::USER_DEFINED_START; Idx <= IVT::USER_DEFINED_END; ++Idx)
            RegisterHandler(IdtTable, Idx, UnhandledInterrupt);
    }

    /*! @brief assembly instruction to load idt table to CPU
     * @param lidtAddress
     * @param LimitUse
     */
    inline void Load_lidt(void *lidtAddress, uint16_t LimitUse)
    {
        struct [[gnu::packed]]
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

    /*! @brief Assigns exception and interrupt handler to idt_table, and load to CPU
     */
    void Install_idt()
    {
        SetExceptionHandler (idt_table);
        SetInterruptHandler (idt_table);
        Load_lidt(idt_table, IDT_ENTRIES * 8 - 1);
    }
} // namespace INTRP

namespace INIT
{
    void idt()
    {
        PIC::Remap();         // remap PIC's interrupt number as default by Bios unsuitable in protected mode
        INTRP::Install_idt(); // install exception, and interrupt handlers
    }
}

