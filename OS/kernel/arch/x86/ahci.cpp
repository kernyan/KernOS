#include <kprintf.h>
#include <utilities.h>
#include <ahci.h>
#include <pci.h>

namespace AHCI
{
  void ReadHBA(uint32_t Addr)
  {
    volatile HBA_MEM hba_mem;
    volatile char* hba_ptr = (char*) &hba_mem;
    volatile char* fis_s   = (char*) Addr;

    for (size_t i = 0; i < sizeof(HBA_MEM); ++i)
    {
      //if (*hba_ptr) kprintf("%i %#010x\n", i, *hba_ptr);
      *hba_ptr++ = *fis_s++;
    }

    // checking IDE or AHCI status
    kprintf("reading hba %#010x\n", Addr);
    kprintf("cap %#010x\n", hba_mem.cap);
    kprintf("ghc %#010x\n", hba_mem.ghc);
    hba_mem.ghc = 1 << 31;

    // probing size of memory region
    //volatile uint32_t *HBA = (uint32_t*) Addr;
    //volatile uint32_t Content1 = *HBA;
    //*HBA = -1;
    //volatile uint32_t Content2 = *HBA;
    //kprintf("%#010x %#010x\n", Content1, Content2);

    kprintf("cap  %#010x\n", hba_mem.cap);
    kprintf("ghc  %#010x\n", hba_mem.ghc);
    kprintf("is   %#010x\n", hba_mem.is);
    kprintf("pi   %#010x\n", hba_mem.pi);
    kprintf("vs   %#010x\n", hba_mem.vs);
    kprintf("ctl  %#010x\n", hba_mem.ccc_ctl);
    kprintf("pts  %#010x\n", hba_mem.ccc_pts);
    kprintf("eloc %#010x\n", hba_mem.em_loc);
    kprintf("ectl %#010x\n", hba_mem.em_ctl);
    kprintf("cap2 %#010x\n", hba_mem.cap2);
    kprintf("bohc %#010x\n", hba_mem.bohc);
    kprintf("vend %#04x\n",  hba_mem.vendor[0]);

    //volatile HBA_PORT& hba_port = hba_mem.ports[0];

    //kprintf("clb  %#010x\n", hba_port.clb);
    //kprintf("fb   %#010x\n", hba_port.fb);
    //kprintf("is   %#010x\n", hba_port.is);
    //kprintf("ie   %#010x\n", hba_port.ie);
    //kprintf("cmd  %#010x\n", hba_port.cmd);
    //kprintf("tfd  %#010x\n", hba_port.tfd);
    //kprintf("sig  %#010x\n", hba_port.sig);
    //kprintf("ssts %#010x\n", hba_port.ssts);
    //kprintf("sctl %#010x\n", hba_port.sctl);
    //kprintf("serr %#010x\n", hba_port.serr);
    //kprintf("sact %#010x\n", hba_port.sact);
    //kprintf("ci   %#010x\n", hba_port.ci);
    //kprintf("sntf %#010x\n", hba_port.sntf);
    //kprintf("fbs  %#010x\n", hba_port.fbs);
    //kprintf("vend %#010x\n", hba_port.vendor[0]);

    kpanic("stop");
  }
} // namespace AHCI
