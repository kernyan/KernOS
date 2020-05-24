//
// Created by on 5/24/20.
//

#ifndef KERNOS_REGISTERS_H
#define KERNOS_REGISTERS_H

namespace CR0 // control register cr0
{
    enum : uint8_t
    {            // | name                | description
        PE = 0,  // | Protection enable   | Set to enable protected mode
        MP = 1,  // | Monitor coprocessor | Set to allow math coprocessor instruction execution
        EM = 2,  // | Emulation           | Set to use software emulation for x87 floating-point instruction
        TS = 3,  // | Task switched       | Set to delay saving of x87 FPU, SSE on task switch till actually used
        ET = 4,  // | Extension type      | processor dependent flag
        NE = 5,  // | Numeric error       | Set to use native mechanism for reporting x87 FPU error
        WP = 16, // | Write protect       | Set to inhibit kernel procedures from writing to user read-only pages
        AM = 18, // | Alignment mask      | Set to enable automatic alignment
        NW = 29, // | Not write-through   | Clear to enable write-back/write-through
        CD = 30, // | Cache disable       | Set to restrict caching
        PG = 31  // | Paging              | Set to enable paging
    };
}

namespace CR4 // control register cr4
{
    enum : uint8_t
    {                    // | name                        | description
        VME = 0,         // | Virtual 8086 extension      | Set to enable interrupt/exception extension in virtual 8086
        PVI = 1,         // | Protected virtual interrupt | Set to enable hardware support for virtual interrupt flag
        TSD = 2,         // | Time stamp disable          | Set to restrict RDTSC instructions to ring 0
        DE  = 3,         // | Debugging extensions        | Set to reserve debug registers DR4, and DR5
        PSE = 4,         // | Page size extensions        | Set to use 4MB page size
        PAE = 5,         // | Physical address extension  | Set to use 36 bit physical address
        MCE = 6,         // | Machine-check enable        | Set to enable machine-check exception
        PGE = 7,         // | Page global enable          | Set to enable global page, i.e. marking page as global to all user
        PCE = 8,         // | Performance-monitoring counter enable      | Set to enable RDPMC inst. at any protection level
        OSFXSR = 9,      // | OS support for FXSAVE/FXRSTOR              | Set to indicate OS support for save/restore of XMM, and MXCSR registers
        OSXMMEXCPT = 10  // | OS support for unmasked SIMD FP exceptions | Set to allow SIMD floating-point exception
    };
}

#endif //KERNOS_REGISTERS_H
