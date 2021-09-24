//
// Created on 9/20/21.
//

#ifndef KERNOS_PCI_H
#define KERNOS_PCI_H

#include <common.h>

namespace PCI
{
  enum CLASSCODE: uint8_t
  {
    LEGACY       = 0x00,
    MASS_STORAGE = 0x01,
    NETWORK_CONT = 0x02,
    DISPLAY_CONT = 0x03,
    MMEDIA_CONT  = 0x04,
    MEMORY_CONT  = 0x05,
    BRIDGE_DEV   = 0x06,
    SIMPL_COM_D  = 0x07,
    BASE_PERIPH  = 0x08,
    INPUT_DEV    = 0x09,
    DOCKING_STN  = 0x0a,
    PROCESSORS   = 0x0b,
    SERIAL_BUS   = 0x0c,
    WIRELESS     = 0x0d
  };

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
