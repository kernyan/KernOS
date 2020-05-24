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

#endif //KERNOS_TYPES_H