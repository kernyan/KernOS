//
// Created on 10/07/21.
//

#ifndef KERNOS_IDE_H
#define KERNOS_IDE_H

#include <common.h>

namespace PCI
{
  struct pci_dev;
}

namespace IDE
{
  void Initialize(const PCI::pci_dev &pci_config);
} // namespace IDE

#endif //KERNOS_IDE_H
