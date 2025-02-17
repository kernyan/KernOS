//
// Created on 9/23/21.
//

#ifndef KERNOS_AHCI_H
#define KERNOS_AHCI_H

#include <common.hpp>

namespace AHCI
{
  enum HBA_CMD_STATUS: uint32_t
  {
    HBA_PxCMD_ST = (1 << 0),
    HBA_PxCMD_FRE = (1 << 4),
    HBA_PxCMD_FR = (1 << 14),
    HBA_PxCMD_CR = (1 << 15),
  };
  
  enum FIS_TYPE: uint8_t
  {
    REG_H2D   = 0x27,
    REG_D2H   = 0x34,
    DMA_ACT   = 0x39,
    DMA_SETUP = 0x41,
    DATA      = 0x46,
    BIST      = 0x58,
    PIO_SETUP = 0x5F,
    DEV_BITS  = 0xA1,
  };

  struct FIS_REG_H2D
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:3;
    uint8_t c:1;
    uint8_t command;
    uint8_t feature1;
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureh;
    uint8_t countl;
    uint8_t counth;
    uint8_t icc;
    uint8_t control;
    uint8_t rsv1[4];
  } __attribute__((packed));

  struct FIS_REG_D2H
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:2;
    uint8_t i:1;
    uint8_t rsv1:1;
    uint8_t status;
    uint8_t error;
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t rsv2;
    uint8_t countl;
    uint8_t counth;
    uint8_t rsv3[2];
    uint8_t rsv4[4];
  } __attribute__((packed));

  struct FIS_DATA
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:4;
    uint8_t rsv1[2];
    uint32_t data[1];
  } __attribute__((packed));

  struct FIS_PIO_SETUP
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:1;
    uint8_t d:1;
    uint8_t i:1;
    uint8_t rsv1:1;
    uint8_t status;
    uint8_t error;
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t rsv2;
    uint8_t countl;
    uint8_t counth;
    uint8_t rsv3;
    uint8_t e_status;
    uint16_t tc;
    uint8_t  rsv4[2];
  } __attribute__((packed));

  struct FIS_DMA_SETUP
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:1;
    uint8_t d:1;
    uint8_t i:1;
    uint8_t a:1;
    uint8_t rsved[2];

    uint64_t DMAbufferID;
    uint32_t rsvd;
    uint32_t DMAbufOffset;
    uint32_t TransferCount;
    uint32_t resvd;
  } __attribute__((packed));
  
  struct HBA_PORT // need to be volatile
  {
    uint32_t clb;
    uint32_t clbu;
    uint32_t fb;
    uint32_t fbu;
    uint32_t is;
    uint32_t ie;
    uint32_t cmd;
    uint32_t rsv0;
    uint32_t tfd;
    uint32_t sig;
    uint32_t ssts;
    uint32_t sctl;
    uint32_t serr;
    uint32_t sact;
    uint32_t ci;
    uint32_t sntf;
    uint32_t fbs;
    uint32_t rsv1[11];
    uint32_t vendor[4];
  } __attribute__((packed));

  struct HBA_MEM // need to be volatile
  {
    uint32_t cap;
    uint32_t ghc;
    uint32_t is;
    uint32_t pi;
    uint32_t vs;
    uint32_t ccc_ctl;
    uint32_t ccc_pts;
    uint32_t em_loc;
    uint32_t em_ctl;
    uint32_t cap2;
    uint32_t bohc;
    uint8_t  rsv[0xA0-0x2C];
    uint8_t  vendor[0x100-0xA0];
    HBA_PORT ports[32];
  } __attribute__((packed));

  struct HBA_FIS // need to be volatile
  {
    FIS_DMA_SETUP dsfis;
    uint8_t pad0[4];

    FIS_PIO_SETUP psfis;
    uint8_t pad1[12];

    FIS_REG_D2H rfis;
    uint8_t pad2[4];

    uint8_t sdbfis[8]; // not used?

    uint8_t ufis[64];
    uint8_t rsv[0x100-0xA0];
  } __attribute__((packed));

  // In AHCI, each port has 32 command headers (one per "command slot")
  struct HBA_CMD_HEADER {
      uint8_t  cmdFISLength   : 5;  // Command FIS length in DWORDS
      uint8_t  atapi          : 1;  // 1: ATAPI
      uint8_t  write          : 1;  // 1: write, 0: read
      uint8_t  prefetchable   : 1;  // Prefetchable

      uint8_t  reset          : 1;  // 1: RESET
      uint8_t  bist           : 1;  // 1: BIST
      uint8_t  clearBusy      : 1;  // 1: Clear busy upon R_OK
      uint8_t  reserved0      : 1;  
      uint8_t  portMult       : 4;  // Port multiplier

      uint16_t prdtLength;          // Physical region descriptor table length
      volatile uint32_t prdbc;      // Physical region descriptor byte count transferred

      uint32_t ctba;                // Command table descriptor base address
      uint32_t ctbau;               // Command table descriptor base address upper 32 bits

      uint32_t reserved1[4];        // Reserved
  } __attribute__((packed));

  // Physical Region Descriptor Table (PRDT) entry
  struct HBA_PRDT_ENTRY {
      uint32_t dataBase;        // Physical base address
      uint32_t dataBaseUpper;   // Upper 32 bits of base address
      uint32_t reserved0;       
      uint32_t byteCount : 22;  // Byte count (0-based: value+1)
      uint32_t reserved1 : 9;
      uint32_t interruptOnComplete : 1;
  } __attribute__((packed));

  // Command Table
  struct HBA_CMD_TABLE {
      uint8_t  cfis[64];       // Command FIS
      uint8_t  acmd[16];       // ATAPI command, if any
      uint8_t  reserved[48];   
      HBA_PRDT_ENTRY prdtEntry[1];  // Could be more than one entry if needed
  } __attribute__((packed));



  volatile HBA_MEM* ReadHBA(uint32_t Addr);

  void print_hba_mem(volatile HBA_MEM* hba_mem);
  void print_hba_port(volatile HBA_PORT* hba_port);
} // namespace AHCI

namespace FIS
{
  typedef struct tagFIS_REG_H2D
  {
    // DWORD 0
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:3;
    uint8_t c:1;
    uint8_t command;
    uint8_t feature1;

    // DWORD 1
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    // DWORD 2
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureh;

    // DWORD 3
    uint8_t countl;
    uint8_t counth;
    uint8_t icc;
    uint8_t control;

    // DWORD 4
    uint8_t rsv1[4];

  } FIS_REG_H2D;

  typedef struct tagFIS_REG_D2H
  {
    // DWORD 0
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:2;
    uint8_t i:1;
    uint8_t rsv1:1;
    uint8_t status;
    uint8_t error;

    // DWORD 1
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    // DWORD 2
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t rsv2;

    // DWORD 3
    uint8_t countl;
    uint8_t counth;
    uint8_t rsv3;
    uint8_t e_status;

    // DWORD 4
    uint16_t tc;
    uint8_t rsv4[2];
  } FIS_REG_D2H;

  typedef struct tagFIS_DATA
  {
    // DWORD 0
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:4;
    uint8_t rsv1[2];

    // DWORD 1
    uint32_t data[1];
  } FIS_DATA;

} // namespace FIS

#endif //KERNOS_AHCI_H
