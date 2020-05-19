//
// Created on 5/19/20.
//

#include <common.h>

namespace INIT
{
  void SSE()
  {
    /* Support SSE instruction sets
     * clear cr0.EM bit (bit 2)
     *   set cr0.MP bit (bit 1)
     *   set cr4.OSFXSR (bit 9)
     *   set cr4.OSXMMEXCPT (bit 10)
     */

    asm volatile (
        "mov %cr0, %eax\n"         // load cr0
        "andl $0xfffffffb, %eax\n" // 1111 1111 1111 1011
        "orl $0x2, %eax\n"         // 0000 0000 0000 0010
        "mov %eax, %cr0\n"         // store cr0
        "mov %cr4, %eax\n"         // load cr4
        "orl $0x600, %eax\n"       // 0000 0110 0000 0000
        "mov %eax, %cr4\n"         // store cr4
      );
  }

} // namespace INIT
