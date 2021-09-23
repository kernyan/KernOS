//
// Created on 9/22/21.
//

#ifndef KERNOS_SATA_H
#define KERNOS_SATA_H

#include <common.h>
#include <pci.h>

namespace SATA
{

} // namespace SATA

namespace INIT
{
  void SATA(uint8_t bus, uint8_t slot, const PCI::pci_dev &pci_config);
}

#endif //KERNOS_PCI_H
