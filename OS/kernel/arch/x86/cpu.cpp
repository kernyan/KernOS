//
// Created on 5/19/20.
//

#include <common.h>
#include <utilities.h>
#include <kprintf.h>
#include <registers.h>

namespace INIT
{
    /*! @see IA-32 Intel Architecture Software Developer's Manual Volume 3: System Programming Guide -
     * Section 12.1.4 Initialization of the SSE/SSE2/SSE3 Extensions
     * @see <a href="https://en.wikipedia.org/wiki/Control_register#CR0">cr0</a>
     * @see <a href="https://en.wikipedia.org/wiki/Control_register#CR4">cr4</a>
     *
     * @details
       @verbatim
       CPU control register 0
       | bit | val | description                           |
       |   1 |  MP | required for IA-32 support of SSE     |
       |   2 |  EM | clear to disable emulation of x87 FPU |

       CPU control register 4
       | bit |        val | description                                                                          |
       |   9 |     OSFXSR | set when OS provides facilities for saving/restoring SSE states using FXSAVE/FXRSTOR |
       |  10 | OSXMMEXCPT | set when OS provides SIMD floating point exception, #XF handler                      |
       @endverbatim
     */
    void SSE()
    {
        asm volatile (
        "mov %%cr0, %%eax\n"
        "orl %0, %%eax\n"
        "andl %1, %%eax\n"
        "mov %%eax, %%cr0\n"
        "mov %%cr4, %%eax\n"
        "orl %2, %%eax\n"
        "orl %3, %%eax\n"
        "mov %%eax, %%cr4\n"
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
