//
// Created on 5/20/20.
//

#include <common.hpp>
#include <gdt.hpp>
#include <utilities.hpp>
#include <registers.hpp>
#include <accessright.hpp>

namespace GDT
{
    /*! @brief Global descriptor table entry
     *  @details
     *  @verbatim
        Code segment descriptor entry
        |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
        | Base address (24-31)  |G |D |  |A |Limit 16-19|P | DPL |1 |1 |C |R |A | Base address (16-23)  |
        | Base address ( 0-15)                          |                         Segment limit (0-15)  |

        Data segment descriptor entry
        |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
        | Base address (24-31)  |G |DB|  |A |Limit 16-19|P | DPL |1 |0 |E |W |A | Base address (16-23)  |
        | Base address ( 0-15)                          |                         Segment limit (0-15)  |
        @endverbatim
     *  @see AMD64 Architecture Programmer's Manual Volume 2: System Programming (PDF) (Technical report). 2013. p. 80.
     */
    union [[gnu::packed]] GdtEntry
    {
        struct
        {
            uint16_t LimitLow;
            uint16_t BaseLow;
            uint8_t BaseMid;
            uint8_t Access;
            uint8_t LimitHighAndGranularity;
            uint8_t BaseHigh;
        };
        struct
        {
            uint32_t Low;
            uint32_t High;
        };
    };

    /*! @brief convenience structure used to set bits of GdtEntry
     */
    struct Selector
    {
        Segment m_Seg;         ///< Segment enum
        uint8_t m_Offset;      ///< Segment byte position in gdt_table
        uint32_t m_Base;       ///< Base address of segment
        uint32_t m_Limit;      ///< Limit address of segment
        uint8_t m_Access;      ///< Access right bytes @see AR
        uint8_t m_Granularity; ///< Granularity setting @see <a href="https://en.wikipedia.org/wiki/Global_Descriptor_Table">bit 16-23</a>
    };

    const uint32_t NULL_LIMIT    = 0x00000000;
    const uint32_t KERN_CS_LIMIT = 0x000FFFFF; // only low 20 bits used
    const uint32_t KERN_DS_LIMIT = 0x000FFFFF; // only low 20 bits used
    const uint32_t USER_CS_LIMIT = 0x000FFFFF; // only low 20 bits used
    const uint32_t USER_DS_LIMIT = 0x000FFFFF; // only low 20 bits used

    const uint32_t NULL_BASE    = 0x00000000;
    const uint32_t KERN_CS_BASE = 0x00000000;
    const uint32_t KERN_DS_BASE = 0x00000000;
    const uint32_t USER_CS_BASE = 0x00000000;
    const uint32_t USER_DS_BASE = 0x00000000;

    const uint8_t NULL_GRANULARITY    = 0x00; //  |G|D| |A|  only high 4 bits used
    const uint8_t KERN_CS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t KERN_DS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t USER_CS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t USER_DS_GRANULARITY = 0xC0; //  |1|1|0|0|

    static GdtEntry gdt_table[GDT_ENTRIES];

    /*! @brief Set entries in GdtTable from array of Selector
     * @param[out] GdtTable to be loaded into CPU later
     * @param[in] SelectorTable used to specify entries to GdtTable
     */
    void SetGlobalDescriptorEntry(GdtEntry GdtTable[], const Selector SelectorTable[])
    {
        for (size_t Idx = 0; Idx < GDT_ENTRIES; ++Idx) {

            const auto &Sel = SelectorTable[Idx];

            GdtTable[Sel.m_Seg].BaseLow  = (Sel.m_Base & 0xFFFF);
            GdtTable[Sel.m_Seg].BaseMid  = (Sel.m_Base >> 16) & 0xFF;
            GdtTable[Sel.m_Seg].BaseHigh = (Sel.m_Base >> 24) & 0xFF;

            GdtTable[Sel.m_Seg].LimitLow = (Sel.m_Limit & 0xFFFF);
            GdtTable[Sel.m_Seg].LimitHighAndGranularity = ((Sel.m_Limit >> 16) & 0x0F);

            GdtTable[Sel.m_Seg].LimitHighAndGranularity |= (Sel.m_Granularity & 0xF0);
            GdtTable[Sel.m_Seg].Access = Sel.m_Access;
        }
    }

    /*! @brief assembly instruction to load gdt table to CPU
     * @param gdtAddress
     * @param LimitUse
     */
    inline void Load_gdt(void *gdtAddress, uint16_t LimitUse)
    {
        struct [[gnu::packed]]
        {
            uint16_t Limit;
            void *Base;
        } GTR = { LimitUse, gdtAddress };

        asm volatile (
        "lgdt %0"  // load interrupt descriptor table
        :          // no output
        : "m"(GTR) // memory operand allowed, with any kind of address that the machine supports
        );
    }

    /*! @brief Creates global descriptor entries in gdt_table, and loads into CPU
     *  @details
     *  We are going to use x86 protected mode, thus the segment selectors won't be used for either virtual memory, or memory protection.
     *  However since segmentation cannot be turned off on an x86, we simply set the kernel/user space segments to 0 base to simulate flat memory model
     *  @see <a href="https://en.wikipedia.org/wiki/X86_memory_segmentation#Practices">Segmentation in x86 in practice</a>
     */
    void Install_gdt()
    {
        constexpr struct Selector SelTable[GDT_ENTRIES] =
        {
            { Null, SEG_OFFSET(Null), NULL_BASE,    NULL_LIMIT,    AR::NULL_ACCESS,    NULL_GRANULARITY    },
            { K_CS, SEG_OFFSET(K_CS), KERN_CS_BASE, KERN_CS_LIMIT, AR::KERN_CS_ACCESS, KERN_CS_GRANULARITY },
            { K_DS, SEG_OFFSET(K_DS), KERN_DS_BASE, KERN_DS_LIMIT, AR::KERN_DS_ACCESS, KERN_DS_GRANULARITY },
            { U_CS, SEG_OFFSET(U_CS), USER_CS_BASE, USER_CS_LIMIT, AR::USER_CS_ACCESS, USER_CS_GRANULARITY },
            { U_DS, SEG_OFFSET(U_DS), USER_DS_BASE, USER_DS_LIMIT, AR::USER_DS_ACCESS, USER_DS_GRANULARITY }
        };

        SetGlobalDescriptorEntry(gdt_table, SelTable);

        Load_gdt(gdt_table, GDT_ENTRIES * 8 - 1);

        kassert (ReadCR0() & DWord<CR0::PE>(), "PE bit of cr0 must be set for protected mode\n");

        // while grub sets CS, DS, ES, FS, GS, and SS, the exact values are undefined, thus manually set them
        // http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state

        // set DS, ES, FS, GS, and SS
        asm volatile (
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        : // no output
        : "a"(SelTable[K_DS].m_Offset)
        : "memory"
        );

        // set CS
        asm volatile (
        "ljmpl %0, $CS_LABEL\n\t"  // dummy long jump to force kernel code selector update
        "CS_LABEL:\n\t"
        : // no output
        : "n"(SelTable[K_CS].m_Offset) // SelTable is constexpr, thus allowed to be used as immediate integer operand
        );
    }
}

namespace INIT
{
    /*! @brief creates global descriptor table and loads it into CPU
     *  @see GDT::Install_gdt
     */
    void gdt()
    {
        GDT::Install_gdt();
    }
}

