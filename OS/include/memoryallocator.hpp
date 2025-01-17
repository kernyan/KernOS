//
// Created  on 5/26/20.
//

#ifndef KERNOS_MEMORYALLOCATOR_H
#define KERNOS_MEMORYALLOCATOR_H

#include <common.hpp>
#include <utilities.hpp>

namespace INIT
{
    void pagetables();
}

/*! @brief Kernel memory namespace
 */
namespace KM // kernel memory
{
    class MemoryAllocator; // forward declare
    template <size_t AlignmentSize>
    class Allocator;     // forward declare

    extern MemoryAllocator mem_alloc;    // unaligned memory
    extern Allocator<4096> mem_alloc_4k; // for 4KB aligned memory
    extern Allocator<1024> mem_alloc_1k; // for 1KB aligned memory
    extern Allocator<256>  mem_alloc_256; // for 256B aligned memory

    /*! @brief kernel malloc
     * @param Size
     * @return Ptr to allocated memory
     */
    void* kmalloc(size_t Size);

    /*! @brief kernel free
     * @param Ptr to memory to be returned
     */
    void kfree(void* Ptr);

    /*!
     * @brief Header is hidden information that is embedded before memory block
     * @details Contains pointer to next free memory, and size of current block (including header)
     * @verbatim
                       -------------------------------
                ______|______________________        v
       Header  |_m_Next_|_m_Size_|__________|     next free block
                                 ^
                                 | address returned to user
       @endverbatim
     */
    struct [[gnu::aligned(sizeof(uint32_t))]] Header
    {

        Header *m_Next;    ///< next block
        uint32_t m_Size;   ///< size of this block
    };

    /*! @brief Manages kernel heap memory
     * @details
     * 1) Keeps track of linked list of free memory,\n
     * 2) allocates requested memory,\n
     * 3) and coalesces freed memory if adjacent with other free block
     * @todo figure out a way to enforce singleton without heap alloc, or function static variable
     */
    class MemoryAllocator
    {
    public:

        uint32_t m_StartAdd = 0; ///< range of reserved address for kernel heap memory
        uint32_t m_EndAdd   = 0; ///< range of reserved address for kernel heap memory

        Header  m_Base {nullptr, 0};  ///< list start

        /*! @brief Points to range of reserved physical memory for heap allocator use
         * @param StartAdd start of reserved physical memory
         * @param EndAdd end of reserved physical memory
         */
        void Initialize(const uint32_t StartAdd, const uint32_t EndAdd);
    };

    template <size_t AlignmentSize>
    class Allocator
    {
        uint32_t m_StartAdd = 0; ///< range of reserved address for kernel heap memory
        uint32_t m_EndAdd   = 0; ///< range of reserved address for kernel heap memory
        size_t   m_Offset   = 0;
    
    public:

        void Initialize(const uint32_t StartAdd, const uint32_t EndAdd)
        {
            m_StartAdd = (StartAdd % AlignmentSize)
                        ? (StartAdd / AlignmentSize + 1) * AlignmentSize
                  : StartAdd;

            m_EndAdd   = EndAdd;
            m_Offset = 0; 

            if (m_EndAdd < m_StartAdd)
            {
                kpanic("Invalid range for Allocator4K\n");
            }
        }

        void* kmalloc()
        {
            const uint32_t NextMem = m_StartAdd + m_Offset;

            if (NextMem <= m_EndAdd)
            {
                m_Offset += AlignmentSize;

                kassert(!(NextMem & (AlignmentSize - 1)), "Allocated memory is not aligned\n");

                uint32_t* ClearMem = (uint32_t*) NextMem;

                for (size_t i = 0; i < AlignmentSize / sizeof(uint32_t); ++i) // TODO: replace with memset once implemented
                {
                    *(ClearMem + i) = 0;
                }

                return (void*) NextMem;
            }

            kpanic("Memory allocator ran out of space\n");
        };
    };

} // namespace KM, kernel memory

namespace INIT
{
   void KMALLOC();
}

#endif //KERNOS_MEMORYALLOCATOR_H
