#include <utilities.hpp>
#include <memoryallocator.hpp>
#include <ahci.hpp>
#include <disk_controller.hpp>

namespace DiskDevice
{
    int findCmdSlot(volatile HBA_PORT* port, uint32_t maxCmdSlots)
    {
        uint32_t slotsInUse = port->sact | port->ci; // SATA Active | Command Issue
        for (int i = 0; i < (int) maxCmdSlots; ++i) {
            if ((slotsInUse & (1 << i)) == 0) {
                return i;
            }
        }

        return -1;
    }

    bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
    {
        // construct the command
        port->is = (uint32_t) -1;  // PxIS are RWC (i.e. read/write 1 to clear)
        int slot = findCmdSlot(port);
        if (slot < 0) {
            return false;
        }

        auto* cmdHeader = reinterpret_cast<HBA_CMD_HEADER*>(port->clb);
        cmdHeader += slot;  // point to slot's cmdHeader
        cmdHeader->cmdFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
        cmdHeader->write = 0;  // read from device
        cmdHeader->prdtLength = static_cast<uint16_t>((count - 1) >> 4) + 1;  // 1 prdt holds 16 count



        // send the command


        // wait for the command to complete
        return false;
    }

    // Start command engine
    void startCmdEngine(volatile HBA_PORT *port)
    {
        // Wait until CR (bit15) is cleared
        while (port->cmd & HBA_CMD_STATUS::HBA_PxCMD_CR)
            ;

        // Set FRE (bit4) and ST (bit0)
        port->cmd |= HBA_CMD_STATUS::HBA_PxCMD_FRE;
        port->cmd |= HBA_CMD_STATUS::HBA_PxCMD_ST; 
    }

    // Stop command engine
    void stopCmdEngine(volatile HBA_PORT *port)
    {
        // Clear ST (bit0)
        port->cmd &= ~HBA_CMD_STATUS::HBA_PxCMD_ST;

        // Clear FRE (bit4)
        port->cmd &= ~HBA_CMD_STATUS::HBA_PxCMD_FRE;

        // Wait until FR (bit14), CR (bit15) are cleared
        while(1)
        {
            if (port->cmd & HBA_CMD_STATUS::HBA_PxCMD_FR)
                continue;
            if (port->cmd & HBA_CMD_STATUS::HBA_PxCMD_CR)
                continue;
            break;
        }
    }

    void initializePort(volatile HBA_PORT* port, uint32_t portIndex)
    {
        stopCmdEngine(port);	// Stop command engine

        // Command list offset: 1K*portno
        // Command list entry size = 32
        // Command list entry maxim count = 32
        // Command list maxim size = 32*32 = 1K per port

        void* out = KM::mem_alloc_4k.kmalloc();
        const uint32_t AHCI_BASE = (uint32_t)out;


        port->clb = AHCI_BASE + (portIndex<<10);
        port->clbu = 0;
        kmemset((void*)(port->clb), 0, 1024);

        // FIS offset: 32K+256*portno
        // FIS entry size = 256 bytes per port
        port->fb = AHCI_BASE + (32<<10) + (portIndex<<8);
        port->fbu = 0;
        kmemset((void*)(port->fb), 0, 256);

        // Command table offset: 40K + 8K*portno
        // Command table size = 256*32 = 8K per port
        HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
        for (int i=0; i<32; i++)
        {
            cmdheader[i].prdtLength = 8;	// 8 prdt entries per command table
                        // 256 bytes per command table, 64+16+48+16*8
            // Command table offset: 40K + 8K*portno + cmdheader_index*256
            cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portIndex<<13) + (i<<8);
            cmdheader[i].ctbau = 0;
            kmemset((void*)cmdheader[i].ctba, 0, 256);
        }

        startCmdEngine(port);	// Start command engine
    }

} // namespace DiskDevice