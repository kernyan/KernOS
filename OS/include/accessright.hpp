//
// Created on 5/24/20.
//

#ifndef KERNOS_ACCESSRIGHT_H
#define KERNOS_ACCESSRIGHT_H

/*! @brief Access right byte convention - part of descriptor entry
 *  @see AMD64 Architecture Programmer's Manual Volume 2: System Programming (PDF) (Technical report). 2013. p. 80.
 *  @see bit 40-47 of <a href="https://wiki.osdev.org/Descriptor">Osdev's descriptor</a>
 *
 *  @details
 *  Access right byte is used to specify read/write/execute access.
 *  Its bit field definition varies depending on whether used as code/data segment descriptor, or interrupt gate descriptor
 *  @verbatim
    Generic access right byte (bit 8-15 of descriptor)
    |15|14|13|12|11|10|09|08|
    |P |DPL  | S|    Type   |

    For Code segment descriptor
    | bit | name       | description
    |   8 | Accessed   | Set indicates descriptor is copied from GDT to CS register
    |   9 | Readable   | Set indicates executable, and readable
    |  10 | Conforming | Set indicates code segment as conforming
    |  11 | Code/data  | Set indicates code segment
    |  12 | S bit      | Set indicates code/data segment
    |13-14| DPL        | aka. Ring level, value range 0-3
    |  15 | Present    | Set indicates that segment referenced by descriptor is loaded in memory

    For Data segment descriptor
    | bit | name        | description
    |   8 | Accessed    | Set indicates descriptor is copied from GDT to CS register
    |   9 | Writable    | Set indicates readable, and writeable
    |  10 | Expand-down | Set indicates data address expands down, like stack
    |  11 | Code/data   | Clear indicates data segment
    |  12 | S bit       | Set indicates code/data segment
    |13-14| DPL         | aka. Ring level, value range 0-3
    |  15 | Present     | Set indicates that segment referenced by descriptor is loaded in memory

    For Interrupt gate segment descriptor
    | bit | name       | description
    | 8-11| Type       | 1110 indicates 32-bit interrupt gate type
    |  12 | S bit      | Clear indicates system descriptor
    |13-14| DPL        | aka. Ring level, value range 0-3
    |  15 | Present    | Set indicates that segment referenced by descriptor is loaded in memory
    @endverbatim
 */
namespace AR // access right
{
    const uint8_t NULL_ACCESS      = 0x00;

    /*! @brief Kernel code segment access descriptor
     *  @verbatim
        |P|DP|S|C|C|R|A|
        |1|00|1|1|0|1|0|
        |   9  |   A   |
        @endverbatim
     */
    const uint8_t KERN_CS_ACCESS   = 0x9A;

    /*! @brief User code segment access descriptor
     *  @verbatim
        |P|DP|S|C|C|R|A|
        |1|11|1|1|0|1|0|
        |   F  |   A   |
        @endverbatim
     */
    const uint8_t USER_CS_ACCESS   = 0xFA;

    /*! @brief Kernel data segment access descriptor
     *  @verbatim
        |P|DP|S|C|E|W|A|
        |1|00|1|0|0|1|0|
        |   9  |   2   |
        @endverbatim
     */
    const uint8_t KERN_DS_ACCESS   = 0x92;

    /*! @brief User data segment access descriptor
     *  @verbatim
        |P|DP|S|C|E|W|A|
        |1|11|1|0|0|1|0|
        |   F  |   2   |
        @endverbatim
     */
    const uint8_t USER_DS_ACCESS   = 0xF2;

    /*! @brief Interrupt access descriptor
     *  @verbatim
        |P|DP|S|Type|
        |1|00|0|1110|
        |   8  |  E |
        @endverbatim
     */
    const uint8_t INTERRUPT_ACCESS = 0x8E;
}

#endif //KERNOS_ACCESSRIGHT_H
