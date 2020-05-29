//
// Created on 5/19/20.
//

#ifndef KERNOS_CPU_H
#define KERNOS_CPU_H

/*! @brief contains all kernel initialization routines
 */
namespace INIT
{
    /*! @brief initialize Streaming Single Instruction Multiple Data (SIMD) Extensions, (SSE)
     *  @see <a href="https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions">Streaming SIMD Extensions</a>
     */
    void SSE();
}

#endif //KERNOS_CPU_H
