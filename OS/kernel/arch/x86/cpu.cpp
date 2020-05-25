//
// Created on 5/19/20.
//

#include <common.h>
#include <utilities.h>
#include <kprintf.h>
#include <registers.h>

namespace INIT
{
    void SSE()
    {
        // See IA-32 Intel Architecture Software Developer's Manual Volume 3: System Programming Guide
        // Section 12.1.4 Initialization of the SSE/SSE2/SSE3 Extensions
        /*  cr0
         *  | bit | val | description                           |
         *  |   1 |  MP | required for IA-32 support of SSE     |
         *  |   2 |  EM | clear to disable emulation of x87 FPU |
         *
         *  cr4
         *  | bit |        val | description                                                                          |
         *  |   9 |     OSFXSR | set when OS provides facilities for saving/restoring SSE states using FXSAVE/FXRSTOR |
         *  |  10 | OSXMMEXCPT | set when OS provides SIMD floating point exception, #XF handler                      |
         */

        asm volatile (
        "mov %%cr0, %%eax\n\t"
        "orl %0, %%eax\n\t"
        "andl %1, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        "mov %%cr4, %%eax\n\t"
        "orl %2, %%eax\n\t"
        "orl %3, %%eax\n\t"
        "mov %%eax, %%cr4\n\t"
        : // no output
        : "n"(DWord<CR0::MP>()),
          "n"(~DWord<CR0::EM>()),
          "n"(DWord<CR4::OSFXSR>()),
          "n"(DWord<CR4::OSXMMEXCPT>())
        );

        kassert(ReadCR0() & DWord<CR0::MP>(), "MP bit of cr0 must be set for SSE support\n");
        kassert(~(ReadCR0() & DWord<CR0::EM>()), "EM bit of cr0 must be disabled for SSE support\n");
        kassert(ReadCR4() & (DWord<CR4::OSFXSR>() | DWord<CR4::OSXMMEXCPT>()));
    }

} // namespace INIT
