#include <kprintf.h>
#include <utilities.h>
#include <ahci.h>

namespace AHCI
{
  void ReadHBA(uint32_t Addr)
  {
    volatile HBA_MEM hba_mem;
    char* hba_ptr = (char*) &hba_mem;

    char* fis_s   = (char*) Addr;

    for (size_t i = 0; i < sizeof(HBA_MEM); ++i)
      *hba_ptr++ = *fis_s++;

    kprintf("reading hba %#010x\n", Addr);
    kprintf("cap %#010x\n", hba_mem.cap);
    kprintf("ghc %#010x\n", hba_mem.ghc);
    kprintf("is  %#010x\n", hba_mem.is);
    kprintf("pi  %#010x\n", hba_mem.pi);
    kprintf("vs  %#010x\n", hba_mem.vs);
    kprintf("ctl %#010x\n", hba_mem.ccc_ctl);
    kprintf("pts %#010x\n", hba_mem.ccc_pts);
    kprintf("loc %#010x\n", hba_mem.em_loc);
    kprintf("ect %#010x\n", hba_mem.em_ctl);
    kprintf("ca2 %#010x\n", hba_mem.cap2);
    kprintf("boh %#010x\n", hba_mem.bohc);
    kprintf("ven %#04x\n",  hba_mem.vendor[0]);

    volatile HBA_PORT& hba_port = hba_mem.ports[0];

    kprintf("clb  %#010x\n", hba_port.clb);
    kprintf("fb   %#010x\n", hba_port.fb);
    kprintf("is   %#010x\n", hba_port.is);
    kprintf("ie   %#010x\n", hba_port.ie);
    kprintf("cmd  %#010x\n", hba_port.cmd);
    kprintf("tfd  %#010x\n", hba_port.tfd);
    kprintf("sig  %#010x\n", hba_port.sig);
    kprintf("ssts %#010x\n", hba_port.ssts);
    kprintf("sctl %#010x\n", hba_port.sctl);
    kprintf("serr %#010x\n", hba_port.serr);
    kprintf("sact %#010x\n", hba_port.sact);
    kprintf("ci   %#010x\n", hba_port.ci);
    kprintf("sntf %#010x\n", hba_port.sntf);
    kprintf("fbs  %#010x\n", hba_port.fbs);
    kprintf("vend %#010x\n", hba_port.vendor[0]);


    kpanic("stop");
  }
} // namespace AHCI


//  struct HBA_PORT // need to be volatile
//  {
//    uint32_t clb;
//    uint32_t clbu;
//    uint32_t fb;
//    uint32_t fbu;
//    uint32_t is;
//    uint32_t ie;
//    uint32_t cmd;
//    uint32_t rsv0;
//    uint32_t tfd;
//    uint32_t sig;
//    uint32_t ssts;
//    uint32_t sctl;
//    uint32_t serr;
//    uint32_t sact;
//    uint32_t ci;
//    uint32_t sntf;
//    uint32_t fbs;
//    uint32_t rsv1[11];
//    uint32_t vendor[4];
//  };

//  struct HBA_MEM // need to be volatile
//  {
//    uint32_t cap;
//    uint32_t ghc;
//    uint32_t is;
//    uint32_t pi;
//    uint32_t vs;
//    uint32_t ccc_ctl;
//    uint32_t ccc_pts;
//    uint32_t em_loc;
//    uint32_t em_ctl;
//    uint32_t cap2;
//    uint32_t bohc;
//    uint8_t  rsv[0xA0-0x2C];
//    uint8_t  vendor[0x100-0xA0];
//    HBA_PORT ports[1];
//  };
