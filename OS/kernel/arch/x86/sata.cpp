#include <kprintf.h>
#include <utilities.h>
#include <sata.h>
#include <ahci.h>

namespace SATA
{
} // namespace SATA

namespace INIT
{
  void SATA(uint8_t bus, uint8_t slot, const PCI::pci_dev &pci_config)
  {
		(void) bus; (void) slot; // temporary to suppress unused warning

		if ( pci_config.classcode == 0x1 
			&& pci_config.subclass == 0x6)
		{
			if (pci_config.pif)
      {
        kprintf("SATA (AHCI 1.O) detected\n");
        kprintf("BAR4: %#06x BAR5: %#010x\n", pci_config.bar4, pci_config.bar5);

        AHCI::ReadHBA(pci_config.bar5);
      }
      else
      {
        kprintf("SATA (Vendor Specific Interface) detected, but not supported\n");
      }
		}
  }
}
