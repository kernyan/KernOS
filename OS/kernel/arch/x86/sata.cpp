#include <kprintf.h>
#include <utilities.h>
#include <sata.h>
#include <ahci.h>
#include <ide.h>

namespace SATA
{
} // namespace SATA

namespace INIT
{
  void SATA(uint8_t bus, uint8_t slot, uint8_t function, const PCI::pci_dev &pci_config)
  {
		(void) bus; (void) slot; // temporary to suppress unused warning

    switch(pci_config.subclass)
    {
    case PCI_SATA:
      if (pci_config.pif)
      {
        const size_t bar5pos = offsetof(PCI::pci_dev, bar5);
        uint32_t Len = ~PCI::Read(bus, slot, function, bar5pos, -1) + 1;
        uint32_t Res = PCI::Read(bus, slot, function, bar5pos, pci_config.bar5);
        kprintf("\tLen is %i, Res is %#010x\n", Len, Res);

        kprintf("\tSATA (AHCI 1.O) detected\n");
        kprintf("\tBAR4: %#06x BAR5: %#010x\n", pci_config.bar4, pci_config.bar5);

        AHCI::ReadHBA(pci_config.bar5);
      }
      else
      {
        kprintf("\tSATA (Vendor Specific Interface) detected, but not supported\n");
      }
      break;
    case PCI_IDE:
      IDE::Initialize(pci_config);
      kprintf("\tbus: %i slot: %i unimplemented - IDE\n", bus, slot);
      break;
    default:
      kpanic("\tUnimplemented PCI mass storage device");
    }

  }

  void SATA2(uint8_t bus, uint8_t slot, const PCI::pci_dev &pci_config)
  {
    (void) bus; (void) slot;
    switch (pci_config.subclass)
    {
    case 0x0:
      kprintf("\tbus: %i slot: %i unimplemented - Host bridge\n", bus, slot);
      break;
    case 0x1:
      kprintf("\tbus: %i slot: %i unimplemented - ISA bridge\n", bus, slot);
      break;
    default:
      kpanic("\tUnhandled PCI bridge\n"); 
    }
  }
}
