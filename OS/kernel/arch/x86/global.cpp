//
// Created on 5/24/20.
//

#include <global.h>
#include <ktypes.h>

// symbols defined in linker.ld
extern func_ptr start_ctors;
extern func_ptr end_ctors;

namespace INIT
{
    void ctors()
    {
        // as kernel is linked with -nostdlib,
        // __do_global_ctors_aux are not called (see https://github.com/gcc-mirror/gcc/blob/master/libgcc/crtstuff.c)
        // thus we manually call global constructors

        for (func_ptr *ctor = &start_ctors; ctor < &end_ctors; ++ctor) {
            (*ctor)();
        }
    }
} // namespace INIT

