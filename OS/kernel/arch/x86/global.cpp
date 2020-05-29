//
// Created on 5/24/20.
//

#include <global.h>
#include <ktypes.h>

/*! @brief starting address of pointers to global constructors
 *  @details symbol defined in linker.ld.
 */
extern func_ptr start_ctors;

/*! @brief ending address of pointers to global constructors
 *  @details symbol defined in linker.ld.
 */
extern func_ptr end_ctors;

namespace INIT
{
    /*! @brief calls constructors on all global objects
     *
     * @details
     * as kernel is linked with -nostdlib,
     * __do_global_ctors_aux are not called
     * thus we have to call global constructors ourselves
     *
     * @see <a href="https://github.com/gcc-mirror/gcc/blob/master/libgcc/crtstuff.c">crtstuff.c</a>
     */
    void ctors()
    {

        for (func_ptr *ctor = &start_ctors; ctor < &end_ctors; ++ctor) {
            (*ctor)();
        }
    }
} // namespace INIT

