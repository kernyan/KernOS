//
// Created  on 5/26/20.
//

#ifndef KERNOS_MEMORYALLOCATOR_H
#define KERNOS_MEMORYALLOCATOR_H

#include <common.h>

namespace KM // kernel memory
{
    class MemoryAllocator; // forward declare

    extern MemoryAllocator mem_alloc; // singleton

    // public interface

    void* kmalloc(size_t Size);
    void free(void* Ptr);

    // implementations

    struct __attribute__((aligned(sizeof(uint32_t)))) Header
    {
        /*                 -------------------------------
         *          ______|______________________        v
         * Header  |_m_Next_|_m_Size_|__________|     next free block
         *                           ^
         *                           | address returned to user
         */

        Header *m_Next;    // next block
        uint32_t m_Size;   // size of this block
    };

    constexpr uint32_t RoundHeader (uint32_t Size)
    {
        return (((Size - 1) / sizeof(Header)) + 1) * sizeof(Header);  // round up to Header size
    }

    class MemoryAllocator // TODO: figure out a way to enforce singleton without heap alloc, or function static variable
    {
    public:

        uint32_t m_StartAdd = 0;        // range of reserved address for kernel heap memory
        uint32_t m_EndAdd   = 0;        // range of reserved address for kernel heap memory

        Header  m_Base {nullptr, 0};  // list start

        void Initialize(const uint32_t StartAdd, const uint32_t EndAdd);
    };


} // namespace KM, kernel memory

namespace INIT
{
   void KMALLOC();
}

#endif //KERNOS_MEMORYALLOCATOR_H
