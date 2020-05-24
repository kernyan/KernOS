//
// Created on 5/20/20.
//

#include <common.h>
#include <gdt.h>
#include <utilities.h>

namespace GDT
{
    /*
     *   |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
     *   | Base address (24-31)  |G |DB|  |A |Limit 16-19|P |DPL  |S|    Type    | Base address (14-23)  |
     *   | Base address ( 0-15)                          |                         Segment limit (0-15)  |
     */

    union __attribute__((packed)) GdtEntry
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

    enum Segment
    {
        Null = 0, // null descriptor
        K_CS,     // kernel code segment
        K_DS,     // kernel data segment
        U_CS,     // user code segment
        U_DS,     // user data segment
    };

    #define SEG_OFFSET(x)  (((x) << 3) & 0xFF) // each gdt entry is 8 bytes

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

    const uint8_t NULL_ACCESS    = 0x00; // |P|DP|S|Type|
    const uint8_t KERN_CS_ACCESS = 0x9A; // |1|00|1|1010|
    const uint8_t KERN_DS_ACCESS = 0x92; // |1|00|1|0010|
    const uint8_t USER_CS_ACCESS = 0xFA; // |1|11|1|1010|
    const uint8_t USER_DS_ACCESS = 0xF2; // |1|11|1|0010|

    const uint8_t NULL_GRANULARITY    = 0x00; //  |G|D| |A|  only high 4 bits used
    const uint8_t KERN_CS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t KERN_DS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t USER_CS_GRANULARITY = 0xC0; //  |1|1|0|0|
    const uint8_t USER_DS_GRANULARITY = 0xC0; //  |1|1|0|0|

    struct Selector
    {
        Segment Seg;
        uint8_t gdtOffset;
        uint32_t Base;
        uint32_t Limit;
        uint8_t Access;
        uint8_t Granularity;
    };

    static GdtEntry gdt_table[GDT_ENTRIES];

    void SetGlobalDescriptorEntry(GdtEntry GdtTable[], const Selector SelectorTable[])
    {
        for (size_t Idx = 0; Idx < GDT_ENTRIES; ++Idx) {

            const auto &Sel = SelectorTable[Idx];

            GdtTable[Sel.Seg].BaseLow = (Sel.Base & 0xFFFF);
            GdtTable[Sel.Seg].BaseMid = (Sel.Base >> 16) & 0xFF;
            GdtTable[Sel.Seg].BaseHigh = (Sel.Base >> 24) & 0xFF;

            GdtTable[Sel.Seg].LimitLow = (Sel.Limit & 0xFFFF);
            GdtTable[Sel.Seg].LimitHighAndGranularity = ((Sel.Limit >> 16) & 0x0F);

            GdtTable[Sel.Seg].LimitHighAndGranularity |= (Sel.Granularity & 0xF0);
            GdtTable[Sel.Seg].Access = Sel.Access;
        }
    }

    inline void Load_gdt(void *gdtAddress, uint16_t LimitUse)
    {
        struct __attribute__((packed))
        {
            uint16_t Limit;
            void *Base;
        } GTR = { LimitUse, gdtAddress };

        asm volatile
        (
        "lgdt %0"  // load interrupt descriptor table
        :          // no output
        : "m"(GTR) // memory operand allowed, with any kind of address that the machine supports
        );
    }

    void Install_gdt()
    {
        constexpr struct Selector SelTable[GDT_ENTRIES] =
        {
            {Null,SEG_OFFSET(Null), NULL_BASE,    NULL_LIMIT,    NULL_ACCESS,    NULL_GRANULARITY    },
            {K_CS,SEG_OFFSET(K_CS), KERN_CS_BASE, KERN_CS_LIMIT, KERN_CS_ACCESS, KERN_CS_GRANULARITY },
            {K_DS,SEG_OFFSET(K_DS), KERN_DS_BASE, KERN_DS_LIMIT, KERN_DS_ACCESS, KERN_DS_GRANULARITY },
            {U_CS,SEG_OFFSET(U_CS), USER_CS_BASE, USER_CS_LIMIT, USER_CS_ACCESS, USER_CS_GRANULARITY },
            {U_DS,SEG_OFFSET(U_DS), USER_DS_BASE, USER_DS_LIMIT, USER_DS_ACCESS, USER_DS_GRANULARITY }
        };

        SetGlobalDescriptorEntry(gdt_table, SelTable);

        Load_gdt(gdt_table, GDT_ENTRIES * 8 - 1);

        #if DEBUG
        GHETTO_GET_CONTROL_REGISTERS
        kassert (Local_cr0 & 0x1); // bit 0 of cr0 must be 1 to be in protected mode
        #endif

        // while grub sets CS, DS, ES, FS, GS, and SS, the exact values are undefined, thus manually set them
        // http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state

        // set DS, ES, FS, GS, and SS
        asm volatile
        (
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        : // no output
        : "a"(SelTable[K_DS].gdtOffset)
        : "memory"
        );

        // set CS
        asm volatile
        (
        "ljmpl %0, $CS_LABEL\n\t"  // dummy long jump to force kernel code selector update
        "CS_LABEL:\n\t"
        : // no output
        : "n"(SelTable[K_CS].gdtOffset) // SelTable is constexpr, thus allowed to be used as immediate integer operand
        );
    }
}

namespace INIT
{
    void gdt()
    {
        GDT::Install_gdt();
    }
}

