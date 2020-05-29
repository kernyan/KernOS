//
// Created on 5/18/20.
//

#ifndef KERNOS_TYPES_H
#define KERNOS_TYPES_H

#include <common.h>

#if ARCH_32
typedef uint32_t ptr_t;
#else
typedef uint64_t ptr_t;
#endif

using func_ptr = void(*)();

const uint32_t SYSERR = -1;

#endif //KERNOS_TYPES_H
