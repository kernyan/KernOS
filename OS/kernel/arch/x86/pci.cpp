#include <kprintf.h>
#include <utilities.h>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT    0xCFC

namespace PCI
{
bool test_pci_io()
{
    kprintf("Test PCI IO\n");
    uint32_t tmp = 0x80000000;
    out32(PCI_ADDRESS_PORT, tmp);
    tmp = in32(PCI_ADDRESS_PORT);
    if (tmp == 0x80000000)
    {
        kprintf("PCI IO supported\n");
        return true;
    }

    kprintf("PCI IO not supported\n");
    return false;
}


uint32_t Read(uint8_t bus, uint8_t slot, uint8_t offset)
{
    uint32_t out;

    out = 0x80000000 | (bus << 16) | (slot << 11) | offset;
    out32(PCI_ADDRESS_PORT, out);

    return in32(PCI_DATA_PORT);
}

void Dump(uint8_t bus, uint8_t slot, uint32_t buf[2])
{
    uint32_t out;

    for (size_t i = 0; i < 2; ++i)  // actual length depends on headertype
    {
        out = Read(bus, slot, i);

        buf[i*4] = out;
    }
}

} // namespace PCI

namespace INIT
{
    void PCI()
    {
        if (PCI::test_pci_io())
        {
            uint32_t buf[2];

            for (size_t i = 0; i < 2; ++i)
                buf[i] = 0;

            for (uint16_t bus = 0; bus < 256; ++bus)
            {
                for (uint16_t slot = 0; slot < 32; ++slot)
                {
                    PCI::Dump(bus, slot, buf);

                    if (buf[0] != 0xFFFFFFFF)
                    {
                      kprintf("bus %i slot %i\n", (uint32_t) bus, (uint32_t) slot);

                      kprintf("Vendor: %h Device:%h\n", buf[0] & 0xFFFF, (buf[0] >> 16) & 0xFFFF );
                    }
                }
            }
        }
    }
}
