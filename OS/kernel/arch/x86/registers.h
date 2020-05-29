//
// Created by on 5/24/20.
//

#ifndef KERNOS_REGISTERS_H
#define KERNOS_REGISTERS_H

/*! @brief Control register 0
 */
namespace CR0
{
    enum : uint8_t
    {            //  | name                    | description
        PE = 0,  ///< <pre>Protection enable   | Set to enable protected mode                                          </pre>
        MP = 1,  ///< <pre>Monitor coprocessor | Set to allow math coprocessor instruction execution                   </pre>
        EM = 2,  ///< <pre>Emulation           | Set to use software emulation for x87 floating-point instruction      </pre>
        TS = 3,  ///< <pre>Task switched       | Set to delay saving of x87 FPU, SSE on task switch till actually used </pre>
        ET = 4,  ///< <pre>Extension type      | processor dependent flag                                              </pre>
        NE = 5,  ///< <pre>Numeric error       | Set to use native mechanism for reporting x87 FPU error               </pre>
        WP = 16, ///< <pre>Write protect       | Set to inhibit kernel procedures from writing to user read-only pages </pre>
        AM = 18, ///< <pre>Alignment mask      | Set to enable automatic alignment                                     </pre>
        NW = 29, ///< <pre>Not write-through   | Clear to enable write-back/write-through                              </pre>
        CD = 30, ///< <pre>Cache disable       | Set to restrict caching                                               </pre>
        PG = 31  ///< <pre>Paging              | Set to enable paging                                                  </pre>
    };
}

/*! @brief Control register 4
 */
namespace CR4
{
    enum : uint8_t
    {                    //   | name                                          | description
        VME = 0,         ///< <pre>Virtual 8086 extension                     | Set to enable interrupt/exception extension in virtual 8086 </pre>
        PVI = 1,         ///< <pre>Protected virtual interrupt                | Set to enable hardware support for virtual interrupt flag </pre>
        TSD = 2,         ///< <pre>Time stamp disable                         | Set to restrict RDTSC instructions to ring 0 </pre>
        DE  = 3,         ///< <pre>Debugging extensions                       | Set to reserve debug registers DR4, and DR5 </pre>
        PSE = 4,         ///< <pre>Page size extensions                       | Set to use 4MB page size </pre>
        PAE = 5,         ///< <pre>Physical address extension                 | Set to use 36 bit physical address </pre>
        MCE = 6,         ///< <pre>Machine-check enable                       | Set to enable machine-check exception </pre>
        PGE = 7,         ///< <pre>Page global enable                         | Set to enable global page, i.e. marking page as global to all user </pre>
        PCE = 8,         ///< <pre>Performance-monitoring counter enable      | Set to enable RDPMC inst. at any protection level </pre>
        OSFXSR = 9,      ///< <pre>OS support for FXSAVE/FXRSTOR              | Set to indicate OS support for save/restore of XMM, and MXCSR registers </pre>
        OSXMMEXCPT = 10  ///< <pre>OS support for unmasked SIMD FP exceptions | Set to allow SIMD floating-point exception </pre>
    };
}

namespace FLAGS
{
   enum : uint32_t
   {
       CF  =      0x0001, ///< carry flag
       PF  =      0x0004, ///< parity flag
       AF  =      0x0010, ///< adjust flag
       ZF  =      0x0040, ///< zero flag
       SF  =      0x0080, ///< sign flag
       TF  =      0x0100, ///< trap flag
       IF  =      0x0200, ///< interrupt enable flag
       DF  =      0x0400, ///< direction flag
       OF  =      0x0800, ///< overflow flag
       RF  = 0x0001'0000, ///< resume flag
       VM  = 0x0002'0000, ///< virtual 8086 mode
       AC  = 0x0004'0000, ///< alignment check
       VIF = 0x0008'0000, ///< virtual interrupt flag
       VIP = 0x0010'0000, ///< virtual interrupt pending
       ID  = 0x0020'0000, ///< cpuid supported
   };
}

#endif //KERNOS_REGISTERS_H
