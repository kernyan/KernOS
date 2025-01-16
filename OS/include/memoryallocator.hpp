//
// Created  on 5/26/20.
//

#ifndef KERNOS_MEMORYALLOCATOR_H
#define KERNOS_MEMORYALLOCATOR_H

#include <common.hpp>

namespace INIT
{
    void pagetables();
}

/*! @brief Kernel memory namespace
 */
namespace KM // kernel memory
{
    class MemoryAllocator; // forward declare
    class Allocator4K;     // forward declare

    extern MemoryAllocator mem_alloc;    // unaligned memory
    extern Allocator4K     mem_alloc_4k; // for 4KB aligned memory

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

    class Allocator4K
    {
        uint32_t m_StartAdd = 0; ///< range of reserved address for kernel heap memory
        uint32_t m_EndAdd   = 0; ///< range of reserved address for kernel heap memory
        size_t   m_Offset   = 0;

    public:
        void Initialize(const uint32_t StartAdd, const uint32_t EndAdd);
        void* kmalloc_4k();
    };

} // namespace KM, kernel memory

namespace INIT
{
   void KMALLOC();
}

#endif //KERNOS_MEMORYALLOCATOR_H
