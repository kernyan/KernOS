//
// Created on 9/20/21.
//

#ifndef KERNOS_PCI_H
#define KERNOS_PCI_H

#include <common.h>

namespace PCI
{
  bool test_pci_io();

  uint32_t Read(uint8_t bus, uint8_t slot, uint8_t offset);

} // namespace PCI

namespace INIT
{
  void PCI();
}

#endif //KERNOS_PCI_H
