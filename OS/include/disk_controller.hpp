#pragma once

#include <ahci.hpp>

namespace DiskDevice
{
    using namespace AHCI;

    int findCmdSlot(volatile HBA_PORT* port, uint32_t maxCmdSlots = 32);
    bool identifyDevice(volatile HBA_PORT* port, uint32_t portIndex, uint32_t maxCmdSlots);

    void stopCmdEngine(volatile HBA_PORT* port);
    void startCmdEngine(volatile HBA_PORT* port);

    void initializePort(volatile HBA_PORT* port, uint32_t portIndex);

    bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);

} // namespace DiskDevice
