//
// Created on 5/26/20.
//

#include <memoryallocator.hpp>
#include <interrupt.hpp>
#include <ktypes.hpp>
#include <virtualmemory.hpp>

void *kheap;             // populated in boot.S
extern void *kpagetable; // populated in boot.S

namespace KM // kernel memory
{
    MemoryAllocator mem_alloc;
    Allocator<256>  mem_alloc_256;
    Allocator<1024> mem_alloc_1k;
    Allocator<4096> mem_alloc_4k;

    void MemoryAllocator::Initialize (const uint32_t StartAdd, 
                                      const uint32_t EndAdd)
    {
       m_StartAdd = StartAdd;
       m_EndAdd   = EndAdd;

       if (m_EndAdd < m_StartAdd)
       {
          kpanic("Invalid range for memory allocator\n");
       }

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

    void kfree(void* Ptr)
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

    void PrintMemoryLayout()
    {
       const uint32_t Text_Start = 0x00100000;
       const uint32_t PTbl_Start = (uint32_t) kpagetable;
       const uint32_t Heap_Start = (uint32_t) kheap;

       kprintf("Kernel text loaded at: %#010x\n", Text_Start);
       kprintf("Multi boot  loaded at: %#010x - %#010x\n",Text_Start,Text_Start+VM::PG_SIZE);
       kprintf("Page tables loaded at: %#010x - %#010x\n",PTbl_Start,PTbl_Start+VM::PG_SIZE * 3);
       kprintf("Kernel inst loaded at: %#010x - %#010x\n",PTbl_Start+VM::PG_SIZE*3 , Heap_Start);
       kprintf("Kernel heap loaded at: %#010x - %#010x\n",Heap_Start, Heap_Start + MB);
    }
} // namespace kernel memory

namespace INIT
{
    /*! @brief Provides memory allocator with range of reserved memory address to manage
     *  @todo don't hardcode heap size
     */
    void KMALLOC()
    {
       const uint32_t Heap_Start = (uint32_t) kheap;
       KM::PrintMemoryLayout();
       KM::mem_alloc   .Initialize(Heap_Start,            Heap_Start + (MB / 2));
       KM::mem_alloc_4k.Initialize(Heap_Start + (MB / 2), Heap_Start + MB);

       // TODO: refactor allocator
       KM::mem_alloc_1k.Initialize(Heap_Start + MB, Heap_Start + MB + (MB / 2));
       KM::mem_alloc_256.Initialize(Heap_Start + MB + (MB / 2), Heap_Start + MB + MB);
    }
}
