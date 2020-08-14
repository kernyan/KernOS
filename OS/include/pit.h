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
     *  1) Initializes 8254 Programmable Interrupt Timer\n
     *  2) Installs timer interrupt handler
     *  @see TIMER::PIT_825x
     */
    void PIT();
}

#endif //KERNOS_PIT_H
