//
// Created on 9/23/21.
//

#ifndef KERNOS_AHCI_H
#define KERNOS_AHCI_H

#include <common.h>

namespace AHCI
{
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
  };

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
  };

  struct FIS_DATA
  {
    uint8_t fis_type;
    uint8_t pmport:4;
    uint8_t rsv0:4;
    uint8_t rsv1[2];
    uint32_t data[1];
  };

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
  };

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
  };
  
  volatile struct HBA_PORT
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
  };

  volatile struct HBA_MEM
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
    HBA_PORT ports[1];
  };

  volatile struct HBA_FIS
  {
    FIS_DMA_SETUP dsfis;
    uint8_t pad0[4];

    FIS_PIO_SETUP psfis;
    uint8_t pad1[12];

    FIS_REG_D2H rfis;
    uint8_t pad2[4];

    FIS_DEV_BITS sdbfis;

    uint8_t ufis[64];
    uint8_t rsv[0x100-0xA0];
  };

  struct HBA_CMD_HEADER
  {
    uint8_t cfl:5;
    uint8_t a:1;
    uint8_t w:1;
    uint8_t p:1;
    uint8_t r:1;
    uint8_t b:1;
    uint8_t c:1;
    uint8_t rsv0:1;
    uint8_t pmp:4;
    uint16_t prdtl;
    volatile uint32_t prdbc;
    uint32_t ctba;
    uint32_t ctbau;
    uint32_t rsv1[4];
  };

  struct HBA_PRDT_ENTRY
  {
    uint32_t dba;
    uint32_t dbau;
    uint32_t rsv0;
    uint32_t dbc:22;
    uint32_t rsv1:9;
    uint32_t i:1;
  };

  struct HBA_CMD_TBL
  {
    uint8_t cfis[64];
    uint8_t acmd[16];
    uint8_t rsv[48];
    HDA_PRDT_ENTRY prdt_entry[1];
  };
} // namespace AHCI

#endif //KERNOS_AHCI_H
