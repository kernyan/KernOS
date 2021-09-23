//
// Created on 9/20/21.
//

#ifndef KERNOS_PCI_H
#define KERNOS_PCI_H

#include <common.h>

namespace PCI
{
  struct pci_dev
  {
    uint16_t vendor;
    uint16_t device;
    uint16_t command;
    uint16_t status;
    uint8_t  revision;
    uint8_t  pif;
    uint8_t  subclass;
    uint8_t  classcode;
    uint8_t  cache;
    uint8_t  latency;
    uint8_t  header;
    uint8_t  bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
  };

  enum HEADER: uint8_t
  {
    general = 0x00,
    pci2pci = 0x01,
    cardbus = 0x02,
  };

  bool test_pci_io();

  uint32_t Read(uint8_t bus, uint8_t slot, uint8_t offset);

  void load_pci_dev(uint8_t bus, uint8_t slot, pci_dev& pci_config);

} // namespace PCI

namespace INIT
{
  void PCI();
}

#endif //KERNOS_PCI_H
