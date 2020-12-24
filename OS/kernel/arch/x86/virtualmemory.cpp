//
// Created on 6/1/20.
//

#include <virtualmemory.h>
#include <registers.h>
#include <interrupt.h>
#include <kprintf.h>

void *kpagetable;                     // populated in boot.S
multiboot_info_t *multiboot_info_ptr; // populated in boot.S

/*! @brief Virtual memory namespace
 */
namespace VM // virtual memory
{
   uint32_t kernel_page_directory[PD_SIZE][[gnu::aligned(PG_SIZE)]];
   uint32_t pagetable0           [PT_SIZE][[gnu::aligned(PG_SIZE)]];
   uint32_t pagetable1           [PT_SIZE][[gnu::aligned(PG_SIZE)]];

   VMManager S;

   void InitializePageDirectory(uint32_t PageDirectory[PD_SIZE])
   {
       for (size_t i = 0; i < PD_SIZE; ++i)
       {
           PageDirectory[i] = DWord<PDA::R>(); // set read bit
       }
   }

   /*! @brief Map page table
    *  @param Idx
    *  @param PageDirectory
    *  @param PageTable
    */
   void MapPageTable(const size_t Idx, 
                     uint32_t PageDirectory[PD_SIZE], 
                     uint32_t PageTable[PT_SIZE])
   {
       for (size_t i = 0; i < PT_SIZE; ++i)
       {
           PageTable[i] = (i * PG_SIZE)     // top 20 bits is page offset
                        | ( DWord<PTA::R>() // bottom 12 bits are access rights
                          | DWord<PTA::P>()
                          );
       }

       PageDirectory[Idx] = ( (uint32_t) PageTable // top 20 bits is addr of page frame
                            | ( DWord<PDA::R>()    // bottom 12 bits are access rights
                              | DWord<PDA::P>()
                              )
                            );
   }

   void SetPage(const PageAttributes PageAttr, 
                uint32_t PageDirectory[PD_SIZE], 
                uint32_t PageTable[PT_SIZE])
   {
      size_t PDE     = PageAttr.m_PhysPage.m_PDE;
      size_t PTE     = PageAttr.m_PhysPage.m_PTE;
      PageTable[PTE] = (PTE * PG_SIZE)
                       | ( DWord<PTA::R>()
                         | DWord<PTA::P>()
                         );

      if (PageDirectory[PDE] == (uint32_t) PageTable)
      {
         kprintf("PageDirectoryEntry already exists. Access rights not updated\n");
      }
      else
      {
         PageDirectory[PDE] = ( (uint32_t) PageTable // top 20 bits is addr of page frame
                              | ( DWord<PDA::R>()    // bottom 12 bits are access rights
                                | DWord<PDA::P>()
                                )
                              );
      }
   };

   /*! @brief Set cr3 to page directory, and turn on paging
    *  @param PageDirectory
    */
   void InstallPaging(const uint32_t PageDirectory[])
   {
       asm volatile
       (
       "mov %0, %%cr3\n"
       :
       : "a"(PageDirectory)
       );

       asm volatile
       (
       "mov %%cr0, %%eax\n"
       "or %0, %%eax\n"
       "mov %%eax, %%cr0"
       :
       : "n"(DWord<CR0::PG>())
       : "eax"
       );
   }

   extern "C" void FaultPageHandler()
   {
      uint32_t Fault_Address;
      asm volatile("mov %%cr2, %0" : "=r" (Fault_Address));

      kprintf("Page fault handler called\n");
      VM::S.MapPageTable(Fault_Address);
   }

   void ParseMultibootMemoryMap(const multiboot_info_t& MultibootInfo)
   {
      kprintf("\nMemory map from bootloader\n");

      uint32_t BaseAddrMax = 0;
      size_t   SizeMax     = 0;

      for (auto* MMap = (multiboot_memory_map_t*) MultibootInfo.mmap_addr;
          (unsigned long) MMap < MultibootInfo.mmap_addr + MultibootInfo.mmap_length;
          MMap = (multiboot_memory_map_t*) ((unsigned long) MMap + MMap->size + sizeof(MMap->size))
          )
      {
        const auto BaseAddr = (uint32_t) (MMap->addr);
        const auto Length   = (uint32_t) (MMap->len);

        if (SizeMax < Length)
        {
           BaseAddrMax = BaseAddr;
           SizeMax     = Length;
        }

        kprintf("BaseAddr: %h Length: %h\n", BaseAddr, Length);
      }

      if (SizeMax > PG_SIZE)
      {
        S.m_PhyPageAllocator.Initialize(BaseAddrMax, SizeMax);
      }
   }

   void FlushTLB(const uint32_t Fault_Address)
   {
      asm volatile( // flush tlb
      "invlpg %0"
      :
      : "m"(*(char*) Fault_Address)
      : "memory"
      );
   }

   void VMManager::Initialize(multiboot_info_t &BootMemoryMap)
   {
      ParseMultibootMemoryMap(BootMemoryMap);
      VM::InitializePageDirectory(VM::kernel_page_directory);
      VM::MapPageTable(0, VM::kernel_page_directory, VM::pagetable0);
      VM::MapPageTable(1, VM::kernel_page_directory, VM::pagetable1);
      VM::InstallPaging(VM::kernel_page_directory);
   }

   void VMManager::MapPageTable(uint32_t VAddr)
   {
      if (VAddr < 8 * MB)
      {
        kprintf("Lower 8MB is always mapped. Page fault here is logical fault\n");
        Hang();
      }

      kprintf("VMManger mapping virtual address %h\n", VAddr);

      PageAttributes PageAttr {VAddr};

      switch (GetAttr(PageAttr))
      {
      case PAGE_ATTR::MAPPED:
         break;

      case PAGE_ATTR::UNMAPPED:
         auto FreePhyPageAddr = m_PhyPageAllocator.GetFreePage();
         kprintf("FreePhyPageAddr: %h\n", FreePhyPageAddr);
         SetPage(PageAttr, kernel_page_directory, (uint32_t*) FreePhyPageAddr);
         FlushTLB(VAddr);
         break;
      }
   }

} // namespace VM

namespace INIT
{
   /*! @brief set up page directory, page table, and turn on paging
    *  @detail identity map first 8MB of virtual address to physical address
    */
   void PAGE()
   {
     VM::S.Initialize(*multiboot_info_ptr);
   }
} // namespace INIT
