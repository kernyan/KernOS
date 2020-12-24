#include <test.h>
#include <common.h>
#include <Vector.h>
#include <kprintf.h>
#include <process.h>

size_t TestCount = 0;

void MemoryTest()
{
   kprintf("Test %i: Page fault and virtual memory mapping\n", ++TestCount);

   volatile uint32_t *Mem = (uint32_t*) 0x500000;  // virtual memory below 8MB is already mapped at initialization
   *Mem;                                           // reading memory will cause Page Fault if hasn't been mapped

   volatile uint32_t *Mem2 = (uint32_t*) 0x900000; // will cause Page Fault
   *Mem2;                                          // reading memory will cause Page Fault if hasn't been mapped

   volatile uint32_t *Mem3 = (uint32_t*) 0x900F00; // will not cause Page Fault as within 4KB of Mem2
   *Mem3;                                          // reading memory will cause Page Fault if hasn't been mapped

   volatile uint32_t *Mem4 = (uint32_t*) 0x9010FF; // will cause Page Fault as > 4KB of Mem2
   *Mem4;                                          // reading memory will cause Page Fault if hasn't been mapped

   volatile uint32_t *Mem5 = nullptr;              // will cause segfault
   *Mem5;                                          // reading protected memory

   kprintf("WARNING: Nullptr dereferencing not causing exception\n");
}

void VectorTest()
{
   kprintf("Test %i: Vector and kmalloc/free\n", ++TestCount);

   KL::Vector<int> V(5);

   V[2] = 5;
   V[4] = 15;

   V.Pushback(30);
   V.Pushback(31);

   kassert(V[2] == 5, "Vector[] failed\n");
   kassert(V[6] == 31, "Vector::Pushback failed\n");
   kassert(V.Back() == V[6], "Vector::Back failed\n");
}

void ContextSwitchTest()
{
   kprintf("Test %i: Context switching\n", ++TestCount);

   auto BusyThread1 = []()
   {
      kprintf("\tFirst process started\n");

      for (;;)
      {
         // busy loop
      }
   };

   PROCESS::CreateProcess(BusyThread1);
}

void RunTests()
{
   MemoryTest();
   VectorTest();
   ContextSwitchTest();
}
