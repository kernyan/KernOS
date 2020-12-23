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
  void FlushTLB(const uint32_t Fault_Address);

  struct PhysPage
  {
     uint32_t m_PhysicalAddress;
     size_t   m_PDE;
     size_t   m_PTE;
     size_t   m_Offset;

     PhysPage(const uint32_t PhysAddr) :
        m_PhysicalAddress (PhysAddr),
        m_PDE             (PhysAddr >> 22 & 0x3FF),
        m_PTE             (PhysAddr >> 12 & 0x3FF),
        m_Offset          (PhysAddr       & 0xFFF)
     {
     }
  };

  enum class PAGE_ATTR
  {
     MAPPED  = 0,
     UNMAPPED
  };

  struct PhysicalPageAllocator
  {
     uint32_t m_BaseAddr; // start of memory region
     size_t   m_Size;     // size of free memory region
     size_t   m_Offset;   // allocated memory

     uint32_t GetFreePage()
     {
        if (m_Offset + PG_SIZE <= m_Size)
        {
           uint32_t FreePage = m_BaseAddr + m_Offset;

           m_Offset += PG_SIZE;

           return FreePage;
        }

        kprintf("Failed to get free page\n");
        Hang();
     }

     void ReturnPage(const uint32_t VAddr); // not yet implemented

     void Initialize(uint32_t BaseAddr, size_t Size)
     {
        Size = (Size / PG_SIZE) * PG_SIZE; // round down

        if (BaseAddr & 0xFFF)
        {
           kprintf("Warning: Physical memory is not 4KB aligned at %h\n", BaseAddr);
        }

        m_BaseAddr = BaseAddr;
        m_Size     = Size;
        m_Offset   = 0;
     }

     PAGE_ATTR GetAttr(const uint32_t VAddr)
     {
        //if (m_Bitmap.test(VAddr >> 12 & 0xFFFFF))
        //{
        //   return PAGE_ATTR::MAPPED; // TODO implement later
        //}
        //else
        {
           return PAGE_ATTR::UNMAPPED;
        }
     }
  };

  struct PageAttributes
  {
     PhysPage m_PhysPage;

     PageAttributes(const uint32_t PhysAddr) :
        m_PhysPage (PhysAddr)
     {
     }
  };

  class VMManager
  {
     public:
        void MapPageTable(uint32_t VAddr);
        void Initialize(multiboot_info_t &BootMemoryMap);
        PhysicalPageAllocator           m_PhyPageAllocator; // TODO make private
  };

  extern VMManager S;

} // namespace VM

#endif //KERNOS_VIRTUALMEMORY_H
