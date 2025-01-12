#include <kprintf.h>
#include <utilities.h>
#include <ahci.h>
#include <pci.h>

namespace AHCI
{
  void print_hba_port(volatile HBA_PORT* hba_port)
  {
    kprintf("clb  %#010x\n", hba_port->clb);
    kprintf("fb   %#010x\n", hba_port->fb);
    kprintf("is   %#010x\n", hba_port->is);
    kprintf("ie   %#010x\n", hba_port->ie);
    kprintf("cmd  %#010x\n", hba_port->cmd);
    kprintf("tfd  %#010x\n", hba_port->tfd);
    kprintf("sig  %#010x\n", hba_port->sig);
    kprintf("ssts %#010x\n", hba_port->ssts);
    kprintf("sctl %#010x\n", hba_port->sctl); 
    kprintf("serr %#010x\n", hba_port->serr);
    kprintf("sact %#010x\n", hba_port->sact);
    kprintf("ci   %#010x\n", hba_port->ci);
    kprintf("sntf %#010x\n", hba_port->sntf);
    kprintf("fbs  %#010x\n", hba_port->fbs);
    kprintf("vend %#010x\n", hba_port->vendor[0]);
    kprintf("----------------------------------\n");
  } 

  void print_hba_mem(volatile HBA_MEM* hba_mem)
  {
    kprintf("cap  %#010x\n", hba_mem->cap);
    kprintf("ghc %#010x\n", hba_mem->ghc);
    kprintf("is   %#010x\n", hba_mem->is);
    kprintf("pi   %#010x\n", hba_mem->pi);
    kprintf("vs   %#010x\n", hba_mem->vs);  
    kprintf("ctl  %#010x\n", hba_mem->ccc_ctl);
    kprintf("pts  %#010x\n", hba_mem->ccc_pts);
    kprintf("eloc %#010x\n", hba_mem->em_loc);
    kprintf("ectl %#010x\n", hba_mem->em_ctl);
    kprintf("cap2 %#010x\n", hba_mem->cap2);
    kprintf("bohc %#010x\n", hba_mem->bohc);
    kprintf("vend %#04x\n",  hba_mem->vendor[0]);
    kprintf("----------------------------------\n");

    for (int i = 0; i < 32; i++)
    {
      if (hba_mem->pi & (1 << i))
      {
        print_hba_port(&hba_mem->ports[i]);
      }
    }
  } 

  volatile HBA_MEM* ReadHBA(uint32_t Addr)
  {
    volatile HBA_MEM* hba_mem_ptr = (volatile HBA_MEM*) Addr;

    /*
    // checking IDE or AHCI status
    volatile HBA_MEM& hba_mem = *hba_mem_ptr;

    kprintf("reading hba %#010x\n", Addr);
    kprintf("cap %#010x\n", hba_mem.cap);
    kprintf("ghc %#010x\n", hba_mem.ghc);
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

    // port 0
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
    */

    return hba_mem_ptr;
  }
} // namespace AHCI
