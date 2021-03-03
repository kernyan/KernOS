#include <test.h>
#include <common.h>
#include <Vector.h>
#include <kprintf.h>
#include <process.h>
#include <virtualmemory.h>

size_t TestCount = 0;

void MemoryTest()
{
   kprintf("Test %i: Page fault and virtual memory mapping\n", ++TestCount);

   volatile uint32_t *Mem = (uint32_t*) 0x500000;  // vm below 8MB is mapped at initialization
   *Mem;                                           // mem read will cause PF if hasn't been mapped

   volatile uint32_t *Mem2 = (uint32_t*) 0x900000; // will cause Page Fault
   *Mem2 = 1;                                      // mem read will cause PF if hasn't been mapped

   volatile uint32_t *Mem3 = (uint32_t*) 0x900F00; // will not cause PF as within 4KB of Mem2
   *Mem3;                                          // mem read will cause PF if hasn't been mapped

   volatile uint32_t *Mem4 = (uint32_t*) 0x9010FF; // will cause Page Fault as > 4KB of Mem2
   *Mem4;                                          // mem read will cause PF if hasn't been mapped

   //volatile uint32_t *Mem5 = nullptr;              // will cause segfault
   //*Mem5;                                          // reading protected memory

   //kprintf("WARNING: Nullptr dereferencing not causing exception\n");
}


void VectorTest()
{
   kprintf("Test %i: Vector and kmalloc/free\n", ++TestCount);

   KL::Vector<int> V(5);

   V[2] = 5;
   V[4] = 15;

   V.Pushback(30);
   V.Pushback(3);

   kassert(V[2] == 5, "Vector[] failed\n");
   kassert(V[6] == 3, "Vector::Pushback failed\n");
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
