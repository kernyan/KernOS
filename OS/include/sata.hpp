//
// Created on 9/22/21.
//

#ifndef KERNOS_SATA_H
#define KERNOS_SATA_H

#include <common.hpp>
#include <pci.hpp>

enum STORAGE_SUB: uint8_t
{
  PCI_SCSI      = 0x00,
  PCI_IDE       = 0x01,
  PCI_FLOPPY    = 0x02,
  PCI_IPI       = 0x03,
  PCI_RAID      = 0x04,
  PCI_ATA       = 0x05,
  PCI_SATA      = 0x06,
  PCI_SAS       = 0x07,
  PCI_OTHER     = 0x80
};

namespace SATA
{

} // namespace SATA

namespace INIT
{
  void SATA(uint8_t bus, uint8_t slot, uint8_t f, const PCI::pci_dev &pci_config);
  void SATA2(uint8_t bus, uint8_t slot, const PCI::pci_dev &pci_config);
}

#endif //KERNOS_PCI_H
