//
// Created on 8/15/20.
//

#ifndef KERNOS_PROCESS_H
#define KERNOS_PROCESS_H

#include <common.hpp>
#include <utilities.hpp>

namespace PROCESS
{
    const uint32_t STACK_SIZE = 65536;

    void UserReturn();

    enum STATE : uint8_t
    {
        FREE,
        CURR,
        READY,
        RECEIVING,
        SLEEP,
        SUSPEND,
        WAIT,
        SLEEP_OR_RECEIVING
    };

    // define process struct

    const uint8_t  NAME_MAX_LEN      =   255;
    const uint32_t PROCESS_MAX_COUNT = 1'000;

    struct ProcessEntry
    {
        STATE   m_State;
        uint8_t m_Priority;

        void    *m_StackPtr;
        uint32_t m_StackLen;
        char     m_Name[NAME_MAX_LEN];
        uint32_t m_ParentId;
    };

    extern ProcessEntry* proctab[];
    extern uint32_t      process_count;
    extern uint32_t      currpid;

    void Initialize ();

    void Reschedule();

    //void ContextSwitch(void *OldStack, void *NewStack);

    void CreateProcess(func_ptr Func);

    // declare process table

    // create ready list

    // reschedule algorithm

    // perform context switch

} // namespace PROCESS


namespace INIT
{
    void NULLPROCESS ();
} // namespace INIT

#endif //KERNOS_PROCESS_H
