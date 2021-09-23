#include <kprintf.h>
#include <utilities.h>
#include <pci.h>
#include <sata.h>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT    0xCFC

namespace PCI
{
bool Test_pci_io()
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

      buf[i] = out;
  }
}

void load_pci_dev(uint8_t bus, uint8_t slot, pci_dev& pci_config)
{
  uint32_t *ptr = (uint32_t*) &pci_config;

  for (size_t i = 0; i < 10; ++i)
  {
    *ptr++ = Read(bus, slot, i*4);
  }
}

} // namespace PCI

namespace INIT
{
  void PCI()
  {
    if (PCI::Test_pci_io())
    {
      uint32_t Reg1 = 0;

      for (uint16_t bus = 0; bus < 256; ++bus)
      {
        for (uint16_t slot = 0; slot < 32; ++slot)
        {
          Reg1 = PCI::Read(bus, slot, 0);

          if (Reg1 != 0xFFFFFFFF)
          {
            kprintf("bus %i slot %i\n", (uint32_t) bus, (uint32_t) slot);

            uint16_t Vendor = Reg1 & 0xFFFF;
            uint16_t Device = (Reg1 >> 16) & 0xFFFF;

            kprintf("Vendor: %04x Device:%04x\n", Vendor, Device);

            if (Vendor == 0x8086 && Device == 0x2922)
            {
              uint8_t Header = PCI::Read(bus, slot, 0xC) & 0xFF;
              kassert(Header == PCI::HEADER::general, "Expected SATA PCI general header type");
              PCI::pci_dev pci_config;
              load_pci_dev(bus, slot, pci_config);
              SATA(bus, slot, pci_config);
            }
          }
        }
      }
    }
  }
}
