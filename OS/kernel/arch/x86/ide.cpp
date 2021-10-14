#include <kprintf.h>
#include <utilities.h>
#include <ide.h>
#include <pci.h>

namespace IDE
{
  void Initialize(const PCI::pci_dev &pci_config)
  {
    kprintf("YOYYYYYYYYYYY\n");
    kpanic("stop here");
  }
} // namespace IDE
