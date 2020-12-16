//
// Created on 6/1/20.
//

#ifndef KERNOS_VIRTUALMEMORY_H
#define KERNOS_VIRTUALMEMORY_H

#include <common.h>
#include <utilities.h>
#include <multiboot.h>

namespace INIT
{
  void PAGE();
}

namespace VM
{
  /*! @brief Page directory access (lower 12 bits of page directory entry)
   */
  namespace PDA
  {
      enum : uint16_t
      {
          P = 0, ///< Present
          R = 1, ///< Read/Write
          U = 2, ///< User/Supervisor
          W = 3, ///< Write through
          D = 4, ///< Cache disabled
          A = 5, ///< Accessed
          S = 7  ///< Page size (0 for 4KiB page)
      };
  }

  /*! @brief Page table access (lower 12 bits of page table entry)
   */
  namespace PTA
  {
      enum : uint16_t
      {
          P = 0, ///< Present
          R = 1, ///< Read/Write
          U = 2, ///< User/Supervisor
          W = 3, ///< Write through
          C = 4, ///< Cache disabled
          A = 5, ///< Accessed
          D = 6, ///< Dirty
          G = 8  ///< Page size (0 for 4KiB page)
      };
  }

  const size_t PD_SIZE = 1024; ///< page directory size
  const size_t PT_SIZE = 1024; ///< page table size
  const size_t PG_SIZE = 4096; ///< page size

  extern uint32_t kernel_page_directory[PD_SIZE];
  extern uint32_t pagetable0           [PT_SIZE];
  extern uint32_t pagetable1           [PT_SIZE];

  void MapPageTable(const size_t Idx,uint32_t PageDirectory[PD_SIZE], uint32_t PageTable[PT_SIZE]);

  void InstallPaging(const uint32_t PageDirectory[]);
  void ParseMultibootMemoryMap(const multiboot_info_t &MultibootInfoPtr);

  class VMManager
  {
    public:
      void MapPageTable(uint32_t VAddr);
      void Initialize(multiboot_info_t &BootMemoryMap);
  };

  extern VMManager S;

} // namespace VM

#endif //KERNOS_VIRTUALMEMORY_H
