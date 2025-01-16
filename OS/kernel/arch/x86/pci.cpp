#include <kprintf.hpp>
#include <utilities.hpp>
#include <pci.hpp>
#include <sata.hpp>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT    0xCFC

namespace PCI
{
bool Test_pci_io()
{
  uint32_t tmp = 0x80000000;
  out32(PCI_ADDRESS_PORT, tmp);
  tmp = in32(PCI_ADDRESS_PORT);
  if (tmp == 0x80000000)
  {
    return true;
  }

  kprintf("PCI IO not supported\n");
  return false;
}

uint32_t Read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t val)
{
  uint32_t out;

  kassert(!(offset & 0x3), "offset must be multiple of 4\n");

  out = 0x80000000 | (bus << 16) | (slot << 11) | (function << 8) | offset;
  out32(PCI_ADDRESS_PORT, out);

  if (val)
    out32(PCI_DATA_PORT, val);

  return in32(PCI_DATA_PORT);
}

void load_pci_dev(uint8_t bus, uint8_t slot, uint8_t function, pci_dev& pci_config)
{
  uint32_t *ptr = (uint32_t*) &pci_config;

  for (size_t i = 0; i < sizeof(pci_dev); i+=4)
  {
    *ptr++ = Read(bus, slot, function, i);
  }
}

void TempBridgeLog(uint8_t bus, uint8_t slot, pci_dev& pci_config)
{
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

} // namespace PCI

namespace INIT
{
  void PCI()
  {
    if (PCI::Test_pci_io())
    {
      uint32_t Reg1 = 0;

      kprintf("\nEnumerating PCI\n");

      for (uint16_t bus = 0; bus < 256; ++bus)
      {
        for (uint16_t slot = 0; slot < 32; ++slot)
        {
          for (uint16_t f = 0; f < 2; ++f)
          {
            Reg1 = PCI::Read(bus, slot, f, 0);

            if (Reg1 != (uint32_t) -1)
            {
              uint16_t Vendor = Reg1 & 0xFFFF;
              uint16_t Device = (Reg1 >> 16) & 0xFFFF;
              uint8_t  Header = PCI::Read(bus, slot, f, 0xC) & 0xFF;

              kprintf("Vendor: %04x Device:%04x\n", Vendor, Device);

              PCI::pci_dev pci_config;
              load_pci_dev(bus, slot, f, pci_config);

              switch (Header)
              {
              case PCI::HEADER::general:
                {
                  switch (pci_config.classcode)
                  {
                  case PCI::CLASSCODE::MASS_STORAGE:
                    SATA(bus, slot, f, pci_config); break;
                  case PCI::CLASSCODE::BRIDGE_DEV:
                    TempBridgeLog(bus, slot, pci_config); break;
                  case PCI::CLASSCODE::NETWORK_CONT:
                    kprintf("\tbus:%i slot:%i unimplemented - Network\n", bus, slot);
                    break;
                  case PCI::CLASSCODE::DISPLAY_CONT:
                    kprintf("\tbus:%i slot:%i unimplemented - Display\n", bus, slot);
                    break;
                  default:
                    kprintf("\tgeneral classcode %i\n", pci_config.classcode);
                    kpanic("\tUnhandled general PCI header");
                  }
                }
                break;
              default:
                kprintf("\tPCI %02x unhandled\n", pci_config.classcode);
              }
            }
          }
        }
      }
    }
  }
}
