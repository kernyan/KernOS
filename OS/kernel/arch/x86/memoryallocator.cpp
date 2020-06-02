//
// Created on 5/26/20.
//

#include <memoryallocator.h>
#include <interrupt.h>
#include <ktypes.h>

void *kheap;      // populated in boot.S

namespace KM // kernel memory
{
    MemoryAllocator mem_alloc;

    void MemoryAllocator::Initialize (const uint32_t StartAdd, const uint32_t EndAdd)
    {
        m_StartAdd = StartAdd;
        m_EndAdd   = EndAdd;

        m_Base.m_Next         = (Header*) m_StartAdd;
        m_Base.m_Size         = 0;
        m_Base.m_Next->m_Next = (Header*) m_EndAdd;
        m_Base.m_Next->m_Size = m_EndAdd - m_StartAdd;
    }

    void* kmalloc(size_t Size)
    {
        if (Size == 0)
            kpanic("0 sized malloc requested");

        Size += sizeof(Header);

        Header* Prev = &mem_alloc.m_Base;
        Header* Next = mem_alloc.m_Base.m_Next;

        INTRP::Mask DisableInterrupt;

        while ((uint32_t) Next != mem_alloc.m_EndAdd)
        {
            if (Next->m_Size == Size)
            {
                Prev->m_Next  = Next->m_Next;
                Prev->m_Size -= Size;

                Next->m_Next  = nullptr;
                Next->m_Size  = Size;

                return (void*) (Next + 1);
            }
            else if (Next->m_Size > Size)
            {
                Header* Leftover = (Header*) ((uint32_t) Next + Size);
                Leftover->m_Next = Next->m_Next;
                Leftover->m_Size = Next->m_Size - Size;
                Prev->m_Next     = Leftover;

                Next->m_Next      = nullptr;
                Next->m_Size      = Size;

                return (void*) (Next + 1);
            }

            Prev = Next;
            Next = Next->m_Next;
        }

        kpanic("Failed to allocate memory");
    }

    void free(void* Ptr)
    {
        Header* Prev  = &mem_alloc.m_Base;
        Header* Next  = mem_alloc.m_Base.m_Next;
        Header* Block = (Header*) Ptr - 1;
        size_t Size   = ((Header*) Block)->m_Size;

        INTRP::Mask DisableInterrupt;

        while (  ((uint32_t) Next != mem_alloc.m_EndAdd)
              && (Next < Ptr)
              )
        {
            Prev = Next;
            Next = Next->m_Next;
        }

        if (Prev == &mem_alloc.m_Base)
        {
            Prev->m_Next  = Block;
            Block->m_Next = Next;
        }
        else
        {
            if ((uint32_t) Prev + Prev->m_Size == (uint32_t) Block) // coalesce previous free memory if adjacent
            {
                Prev->m_Size += Size;
                Block         = Prev;
            }

            if ((uint32_t) Block + Block->m_Size == (uint32_t) Next) // coalesce next free memory if adjacent
            {
                Block->m_Size += Next->m_Size;
                Block->m_Next  = Next->m_Next;
            }
        }
    }
} // namespace kernel memory

namespace INIT
{
    /*! @brief Provides memory allocator with range of reserved memory address to manage
     *  @todo don't hardcode heap size
     */
    void KMALLOC()
    {
        KM::mem_alloc.Initialize((uint32_t) kheap, (uint32_t)(kheap) + 4096*3);
    }
}
