//
// Created on 9/20/21.
//

#ifndef KERNOS_PCI_H
#define KERNOS_PCI_H

#include <common.hpp>

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
    uint32_t cardbus_cis_pointer; // 0x28
    uint16_t subsystem_vendor_id; // 0x2C
    uint16_t subsystem_id;        // 0x2E
    uint32_t expansion_rom_base_address; // 0x30
    uint8_t  capabilities_pointer; // 0x34
    uint8_t  reserved1[3];         // 0x35-0x37
    uint32_t reserved2;            // 0x38
    uint8_t  interrupt_line;       // 0x3C
    uint8_t  interrupt_pin;        // 0x3D
    uint8_t  min_grant;            // 0x3E
    uint8_t  max_latency;          // 0x3F
  } __attribute__((packed));

  enum HEADER: uint8_t
  {
    general = 0x00,
    pci2pci = 0x01,
    cardbus = 0x02,
  };

  bool test_pci_io();

  uint32_t Read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t value = 0);

  void load_pci_dev(uint8_t bus, uint8_t slot, uint8_t function, pci_dev& pci_config);

} // namespace PCI

namespace INIT
{
  void PCI();
}

#endif //KERNOS_PCI_H
