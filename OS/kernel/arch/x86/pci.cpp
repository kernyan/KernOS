#include <kprintf.h>
#include <utilities.h>

#define PCI_ADDRESS_PORT 0xCF8

namespace PCI
{
bool test_pci_io()
{
    kprintf("Test PCI IO");
    uint32_t tmp = 0x80000000;
    out32(PCI_ADDRESS_PORT, tmp);
    tmp = in32(PCI_ADDRESS_PORT);
    if (tmp == 0x80000000) {
        kprintf("PCI IO supported");
        return true;
    }

    kprintf("PCI IO not supported");
    return false;
}
} // namespace PCI
