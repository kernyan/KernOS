//
// Created by osboxes on 8/15/20.
//

#include <process.hpp>
#include <interrupt.hpp>
#include <memoryallocator.hpp>

extern "C" void ContextSwitch(void* Old, void* New);

namespace PROCESS
{
   ProcessEntry* proctab[PROCESS_MAX_COUNT];
   uint32_t      process_count;
   uint32_t      currpid;

   void* SetupStack(uint32_t *StackAddr, func_ptr Func, func_ptr RetAddress)
   {
      uint32_t SavedSP = (uint32_t) StackAddr;

      *--StackAddr = (uint32_t) RetAddress;
      *--StackAddr = (uint32_t) Func;
      *--StackAddr = SavedSP;
      SavedSP      = (uint32_t) StackAddr;

      /* now we must emulate what ctxsw expects: flags, regs, and old SP */
      /* emulate 386 "pushal" instruction */
      *--StackAddr  = 0x00000200;
      *--StackAddr  = 0; /* %eax */
      *--StackAddr  = 0; /* %ecx */
      *--StackAddr  = 0; /* %edx */
      *--StackAddr  = 0; /* %ebx */
      *--StackAddr  = 0; /* %esp; "popal" doesn't actually pop it, so 0 is ok */
      *--StackAddr  = SavedSP; /* %ebp */
      *--StackAddr  = 0; /* %esi */
      *--StackAddr  = 0; /* %edi */

      return (void*) StackAddr;
   }

   void UserReturn()
   {
      kprintf("Error: Created process exit by return\n");
      Hang();
   }

   void CreateProcess(func_ptr Func)
   {
      INTRP::Mask DisableInterrupt;

      ProcessEntry *Process    = (ProcessEntry*) KM::kmalloc(sizeof(ProcessEntry));
      proctab[process_count++] = Process;
      uint32_t *Stack          = (uint32_t *) KM::kmalloc(STACK_SIZE);
      Process->m_State         = STATE::READY;
      Process->m_StackPtr      = SetupStack(Stack, Func, UserReturn);
      Process->m_StackLen      = STACK_SIZE;
   }

   void Initialize()
   {
      ProcessEntry *Process    = (ProcessEntry*) KM::kmalloc(sizeof(ProcessEntry));
      proctab[process_count++] = Process;
      void *Stack              = KM::kmalloc(STACK_SIZE); // TODO: maybe simply get esp?
      Process->m_State         = STATE::CURR;
      Process->m_StackPtr      = Stack;
      Process->m_StackLen      = STACK_SIZE;
   }

   void Reschedule ()
   {
      ProcessEntry *Old = nullptr;
      ProcessEntry *New = nullptr;

      uint32_t NewPid;
      uint32_t OldPid;

      INTRP::Mask DisableInterrupt;

      if (currpid == 0)
      {
          OldPid = currpid++;
          NewPid = currpid;
      }
      else
      {
         OldPid = currpid--;
         NewPid = currpid;
      }

      Old = proctab[OldPid];

      New = proctab[NewPid];

      ContextSwitch(&Old->m_StackPtr, &New->m_StackPtr);

#ifdef DEBUG
      kprintf("\tSwitched to process %i\n", currpid);
#endif
      return;
   }
} // namespace PROCESS

namespace INIT
{
   void NULLPROCESS ()
   {
      PROCESS::Initialize ();
   }
} // namespace INIT

extern "C" void ContextSwitch(void* Old, void* New);
asm
(
   ".globl ContextSwitch \n"
   "ContextSwitch:       \n"
   "pushl %ebp           \n"
   "movl %esp, %ebp      \n"
   "pushfl               \n"
   "pushal               \n"
   "movl 8(%ebp), %eax   \n"
   "movl %esp, (%eax)    \n"
   "movl 12(%ebp), %eax  \n"
   "movl (%eax), %esp    \n"
   "popal                \n"
   "popfl                \n"
   "movl %ebp, %esp      \n"
   "popl %ebp            \n"
   "ret                  \n"
);
