//
// Created on 6/1/20.
//

#include <virtualmemory.h>
#include <registers.h>
#include <interrupt.h>

void *kpagetable;                     // populated in boot.S
multiboot_info_t *multiboot_info_ptr; // populated in boot.S

/*! @brief Virtual memory namespace
 */
namespace VM // virtual memory
{
    uint32_t kernel_page_directory[PD_SIZE][[gnu::aligned(PG_SIZE)]];
    uint32_t pagetable0           [PT_SIZE][[gnu::aligned(PG_SIZE)]];
    uint32_t pagetable1           [PT_SIZE][[gnu::aligned(PG_SIZE)]];

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
    void MapPageTable(const size_t Idx, uint32_t PageDirectory[PD_SIZE], uint32_t PageTable[PT_SIZE])
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

        if (  Fault_Address >= 4 * MB
           && Fault_Address  < 8 * MB
           )
        {
          VM::MapPageTable(1, VM::kernel_page_directory, VM::pagetable1);

          asm volatile( // flush tlb
          "invlpg %0"
          :
          : "m"(*(char*) Fault_Address)
          : "memory"
          );
        }
        else
        {
            kprintf("Page fault handler only works for 4MB - 8MB range for now\n");
            Hang();
        }

        kprintf("Page fault handler called\n");
    }

    void ParseMultibootMemoryMap(const multiboot_info_t& MultibootInfo)
    {
      for (auto* MMap = (multiboot_memory_map_t*) MultibootInfo.mmap_addr;
          (unsigned long) MMap < MultibootInfo.mmap_addr + MultibootInfo.mmap_length;
          MMap = (multiboot_memory_map_t*) ((unsigned long) MMap + MMap->size + sizeof(MMap->size))
          )
      {
        auto BaseAddr = (uint32_t) (MMap->addr);
        auto Length   = (uint32_t) (MMap->len);
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
        VM::InitializePageDirectory(VM::kernel_page_directory);
        VM::MapPageTable(0, VM::kernel_page_directory, VM::pagetable0);
        //VM::MapPageTable(1, VM::kernel_page_directory, VM::pagetable1);
        VM::InstallPaging(VM::kernel_page_directory);
        VM::ParseMultibootMemoryMap(*multiboot_info_ptr);
    }
} // namespace INIT
