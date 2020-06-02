//
// Created on 6/1/20.
//

#ifndef KERNOS_VIRTUALMEMORY_H
#define KERNOS_VIRTUALMEMORY_H

#include <common.h>
#include <utilities.h>

namespace INIT
{
    void PAGE();
}

namespace VM
{
    /*! @brief Page directory access (lower 12 bits of page directory entry)
     */
    namespace PDA
    {
        enum : uint16_t
        {
            P = 0, ///< Present
            R = 1, ///< Read/Write
            U = 2, ///< User/Supervisor
            W = 3, ///< Write through
            D = 4, ///< Cache disabled
            A = 5, ///< Accessed
            S = 7  ///< Page size (0 for 4KiB page)
        };
    }


    /*! @brief Page table access (lower 12 bits of page table entry)
     */
    namespace PTA
    {
        enum : uint16_t
        {
            P = 0, ///< Present
            R = 1, ///< Read/Write
            U = 2, ///< User/Supervisor
            W = 3, ///< Write through
            C = 4, ///< Cache disabled
            A = 5, ///< Accessed
            D = 6, ///< Dirty
            G = 8  ///< Page size (0 for 4KiB page)
        };
    }
}

#endif //KERNOS_VIRTUALMEMORY_H
