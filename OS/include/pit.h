//
// Created on 7/5/20.
//

#ifndef KERNOS_PIT_H
#define KERNOS_PIT_H

/*! @brief Timer namespace
 */
namespace TIMER
{
} // namespace TIMER

namespace INIT
{
    /*! @brief Initializes programmable interval timer
     *  @details
     *  1) Remaps Programmable interrupt controller's interrupt number\n
     *  2) Assigns exceptions and interrupt handlers
     *  @see PIC::Remap
     *  @see INTRP::Install_idt
     */
    void PIT();
}

#endif //KERNOS_PIT_H
