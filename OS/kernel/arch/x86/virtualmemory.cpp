//
// Created on 6/1/20.
//

#include <virtualmemory.h>
#include <registers.h>

void *kpagetable; // populated in boot.S

/*! @brief Virtual memory namespace
 */
namespace VM // virtual memory
{
    const size_t PD_SIZE = 1024; ///< page directory size
    const size_t PT_SIZE = 1024; ///< page table size
    const size_t PG_SIZE = 4096; ///< page size

    uint32_t page_directory[PD_SIZE][[gnu::aligned(PG_SIZE)]];
    uint32_t pagetable0    [PT_SIZE][[gnu::aligned(PG_SIZE)]];

    void InitializePageDirectory(uint32_t PageDirectory[PD_SIZE])
    {
        for (size_t i = 0; i < PD_SIZE; ++i)
        {
            PageDirectory[i] = DWord<PDA::R>();
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
            PageTable[i] = (i * PG_SIZE)
                         | (DWord<PTA::R>() | DWord<PTA::P>());
        }

        PageDirectory[Idx] = ( (uint32_t) PageTable
                             | (DWord<PDA::R>() | DWord<PDA::P>())
                             );
    }

    /*! @brief Set cr3 to page directory, and turn on paging
     * @param PageDirectory
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
}

namespace INIT
{
    /*! @brief set up page directory, page table, and turn on paging
     *  @todo only setting up page table 0 for now, i.e. 0 - 4 MiB
     */
    void PAGE()
    {
        VM::InitializePageDirectory(VM::page_directory);
        VM::MapPageTable(0, VM::page_directory, VM::pagetable0);
        VM::InstallPaging(VM::page_directory);
    }
} // namespace INIT
