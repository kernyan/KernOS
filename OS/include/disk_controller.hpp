#pragma once

#include <ahci.hpp>

namespace DiskDevice
{
    using namespace AHCI;

    int findCmdSlot(volatile HBA_PORT* port, uint32_t maxCmdSlots);
    bool identifyDevice(volatile HBA_PORT* port, uint32_t portIndex, uint32_t maxCmdSlots);

} // namespace DiskDevice
