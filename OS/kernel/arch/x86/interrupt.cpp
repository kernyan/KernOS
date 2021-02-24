//
// Created on 5/19/20.
//

#include <interrupt.h>
#include <common.h>
#include <utilities.h>
#include <gdt.h>
#include <accessright.h>
#include <pic.h>
#include <virtualmemory.h>

#define INTRP_ENTRY(Type)                       \
    extern "C" void Interrupt##Type##Entry();   \
    extern "C" void Interrupt##Type##Handler(); \
    asm(                                        \
    ".globl Interrupt" #Type "Entry \n"         \
    "Interrupt" #Type "Entry:       \n"         \
    "call Interrupt" #Type "Handler \n"         \
    "    iret\n");

INTRP_ENTRY(Timer)

#define FAULT_ENTRY(Type)                           \
    extern "C" void Fault##Type##Entry();           \
    extern "C" void Fault##Type##Handler(RegState); \
    asm(                                            \
    ".globl Fault" #Type "Entry \n"                 \
    "Fault" #Type "Entry:       \n"                 \
    "    cld\n"                                     \
    "    call Fault" #Type "Handler \n"             \
    "    add $0x4, %esp\n"                          \
    "    iret\n");

FAULT_ENTRY(Page)

namespace INTRP // interrupt
{
    DescriptorEntry idt_table[IDT_ENTRIES];

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

        RegisterHandler(IdtTable, IVT::PAGE_FAULT, FaultPageEntry);
    }

    /*! @brief Installs default interrupt handler to all interrupts
     * @param IdtTable
     */
    void SetInterruptHandler(DescriptorEntry IdtTable[])
    {
        for (size_t Idx = IVT::USER_DEFINED_START; Idx <= IVT::USER_DEFINED_END; ++Idx)
            RegisterHandler(IdtTable, Idx, UnhandledInterrupt);

        RegisterHandler(IdtTable, IVT::TIMER, InterruptTimerEntry);
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

