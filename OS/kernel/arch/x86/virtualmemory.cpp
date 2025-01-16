//
// Created on 6/1/20.
//

#include <virtualmemory.hpp>
#include <registers.hpp>
#include <interrupt.hpp>
#include <kprintf.hpp>
#include <memoryallocator.hpp>
#include <vga.hpp>

void *kpagetable;                     // populated in boot.S
multiboot_info_t *multiboot_info_ptr; // populated in boot.S

/*! @brief Virtual memory namespace
 */
namespace VM // virtual memory
{
   uint32_t kernel_page_directory[PD_SIZE][[gnu::aligned(PG_SIZE)]];
   uint32_t pagetable0           [PT_SIZE][[gnu::aligned(PG_SIZE)]];
   uint32_t pagetable1           [PT_SIZE][[gnu::aligned(PG_SIZE)]];
   uint32_t pagetable1018        [PT_SIZE][[gnu::aligned(PG_SIZE)]];

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
                     uint32_t PageTable[PT_SIZE],
                     bool IsDevice)
   {
       for (size_t i = 0; i < PT_SIZE; ++i)
       {
           PageTable[i] = (i * PG_SIZE)     // top 20 bits is page offset
                        | ( DWord<PTA::R>() // bottom 12 bits are access rights
                          | DWord<PTA::P>()
                          );

           if (IsDevice)
               PageTable[i] |= (DWord<PTA::W>() | DWord<PTA::C>()); // writethrough, no cache
       }

       PageDirectory[Idx] = ( (uint32_t) PageTable // top 20 bits is addr of page frame
                            | ( DWord<PDA::R>()    // bottom 12 bits are access rights
                              | DWord<PDA::P>()
                              )
                            );

       if (IsDevice)
           PageDirectory[Idx] |= (DWord<PDA::W>() | DWord<PDA::C>()); // writethrough, no cache
   }

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

   extern "C" void FaultPageHandler(RegState &Reg)
   {
#ifdef DEBUG
      kprintf("\tException code %#06x\n", Reg.m_exception_code);
      kprintf("\tEip code %#010x\n", Reg.m_eip);
#endif
      uint32_t Fault_Address;
      asm volatile("mov %%cr2, %0" : "=r" (Fault_Address));
      VM::S.MapPageTable(Fault_Address);
   }

   void ParseMultibootMemoryMap(const multiboot_info_t& MultibootInfo)
   {
      kprintf("\nMemory map from bootloader\n");

      uint32_t BaseAddrMax = 0;
      size_t   SizeMax     = 0;
      size_t   ChunkNumber = 0;

      for (auto* MMap = (multiboot_memory_map_t*) MultibootInfo.mmap_addr;
          (unsigned long) MMap < MultibootInfo.mmap_addr + MultibootInfo.mmap_length;
          MMap = (multiboot_memory_map_t*) ((unsigned long) MMap + MMap->size + sizeof(MMap->size))
          )
      {
        const auto BaseAddr = (uint32_t) (MMap->addr);
        const auto Length = (uint32_t) (MMap->len);

        if (SizeMax < Length)
        {
           BaseAddrMax = BaseAddr;
           SizeMax     = Length;
        }

        kprintf("Memory chunk %i: %#010x - %#010x\n",ChunkNumber++, BaseAddr, BaseAddr + Length);
      }

      if (SizeMax > PG_SIZE)
      {
         uint32_t KernelReservedEnd = 0x800000;
         S.m_PhyPageAllocator.Initialize(KernelReservedEnd, 
                                         SizeMax - (KernelReservedEnd - BaseAddrMax));
      }
   }

   void FlushTLB(const uint32_t Fault_Address)
   {
      asm volatile( // flu.hpp tlb
      "invlpg %0"
      :
      : "m"(*(char*) Fault_Address)
      : "memory"
      );
   }

   void VMManager::ProtectPage(uint32_t PageTable[PT_SIZE], const size_t PgCount)
   {
      for (uint32_t i = 0; i < PgCount; ++i)
      {
         PageTable[i] = PageTable[i] 
                      & !( DWord<PTA::P>()
                         | DWord<PTA::R>()
                         | DWord<PTA::U>()
                         );
      }
   }

   void VMManager::Initialize(multiboot_info_t &BootMemoryMap)
   {
      ParseMultibootMemoryMap(BootMemoryMap);
      VM::InitializePageDirectory(VM::kernel_page_directory);
      VM::MapPageTable(0, VM::kernel_page_directory, VM::pagetable0, false);
      VM::MapPageTable(1, VM::kernel_page_directory, VM::pagetable1, false);
      VM::MapPageTable(1018, VM::kernel_page_directory, VM::pagetable1018, true); // identity map for PCI SATA
      ProtectPage(VM::pagetable0, 1 /*Number of pages to protect*/);
      VM::InstallPaging(VM::kernel_page_directory);
   }

   void VMManager::MapPageTable(uint32_t VAddr)
   {
#ifdef DEBUG
      kprintf("\tVMManager mapping virtual address %#010x\n", VAddr);
#endif

      if (VAddr == 0)
      {
        kpanic("Null pointer dereferenced\n");
      }
      else if (VAddr < 8 * MB)
      {
        kpanic("Lower 8MB is always mapped. Page fault here is logical fault\n");
      }

      PageAttributes VirAttr {VAddr};

      switch (GetAttr(VirAttr))
      {
      case PAGE_ATTR::MAPPED:
         break;

      case PAGE_ATTR::UNMAPPED:
         {
            void* FreePage = KM::mem_alloc_4k.kmalloc_4k(); // TODO track and manage memory
            SetPage(VirAttr, kernel_page_directory, (uint32_t*) FreePage);
            FlushTLB(VAddr);
            break;
         }
      }
   }

   void VMManager::SetPage(const PageAttributes VirAttr, 
                           uint32_t PageDirectory[PD_SIZE], 
                           uint32_t PageTable[PT_SIZE])
   {
      AddressInfo PhyAddr {(uint32_t) m_PhyPageAllocator.GetFreePage()};
      uint32_t PhyTop20 = PhyAddr.m_Top20;

      size_t PDE     = VirAttr.m_Addr.m_PDE;
      size_t PTE     = VirAttr.m_Addr.m_PTE;
      PageTable[PTE] = PhyTop20
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
