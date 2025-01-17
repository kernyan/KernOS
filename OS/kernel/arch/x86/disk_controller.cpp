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

    bool identifyDevice(volatile HBA_PORT* port, uint32_t portIndex, uint32_t maxCmdSlots)
    {
        // 1. Find a free command slot
        int slot = findCmdSlot(port, maxCmdSlots);
        if (slot < 0) {
            // No free command slot
            return false;
        }

        // 2. Get a pointer to the Command List Header array
        //    Each port's PxCLB points to a 1024-byte region with up to 32 command headers.
        auto cmdHeader = reinterpret_cast<HBA_CMD_HEADER*>(
            static_cast<uintptr_t>(port->clb) // low 32 bits
            // + ((uint64_t)port->clbu << 32)  // if your hardware uses 64-bit addresses
        );

        // We'll index into cmdHeader[slot]
        HBA_CMD_HEADER* header = &cmdHeader[slot];
        kmemset(header, 0, sizeof(HBA_CMD_HEADER));

        // For IDENTIFY, we only need a single PRDT entry for a 512-byte response.
        header->cmdFISLength = sizeof(FIS_REG_H2D) / 4;  // FIS_REG_H2D is 20 bytes, plus alignment; typically 5 DWORDS
        header->write = 0;          // This is a read (device -> memory)
        header->prdtLength = 1;     // 1 PRDT entry
        header->clearBusy = 1;      // Clears BSY bit upon completion

        // 3. Allocate/reserve a command table. 
        //    In a real OS, you'd have done this once and stored it. 
        //    For demo, let’s assume you have a pre-allocated region for command tables.
        //    Suppose each command table is 256 bytes (or more). We index by slot to get the correct command table.
        uintptr_t cmdTableAddr = port->clb + (slot * 256); // <-- you must implement this
        // kmemset(reinterpret_cast<void*>(cmdTableAddr), 0, sizeof(HBA_CMD_TABLE));

        // // Fill in the command table base address into the command header
        header->ctba  = (uint32_t)(cmdTableAddr & 0xFFFFFFFF);
        header->ctbau = 0;

        // 4. Build the PRDT entry (point it to a 512-byte buffer)
        HBA_CMD_TABLE* cmdTable = reinterpret_cast<HBA_CMD_TABLE*>(cmdTableAddr);
        constexpr uint32_t BUF_SIZE = 512;
        static uint8_t identifyBuffer[BUF_SIZE] __attribute__((aligned(64)));  
        // In a real OS, you'd want a per-port, per-slot buffer or a dynamically allocated DMA buffer

        cmdTable->prdtEntry[0].dataBase      = (uint32_t)(reinterpret_cast<uintptr_t>(identifyBuffer) & 0xFFFFFFFF);
        cmdTable->prdtEntry[0].dataBaseUpper = (uint32_t)((reinterpret_cast<uintptr_t>(identifyBuffer) >> 32) & 0xFFFFFFFF);
        cmdTable->prdtEntry[0].byteCount     = BUF_SIZE - 1;  // byteCount is zero-based
        cmdTable->prdtEntry[0].interruptOnComplete = 1;

        // 5. Build the Command FIS (CFIS) in the command table
        //    The first 20 bytes (plus padding) is FIS_REG_H2D
        uint8_t* cfis = cmdTable->cfis;
        // FIS Type = Reg H2D
        cfis[0] = 0x27;        // Host to device
        cfis[1] = 1 << 7;      // Command
        cfis[2] = 0xEC;        // ATA command: IDENTIFY DEVICE
        cfis[3] = 0x00;        // Features (upper byte is 0 as well)
        
        // LBA, count fields are zero for IDENTIFY
        cfis[4] = 0;   // LBA low
        cfis[5] = 0;
        cfis[6] = 0;
        cfis[7] = 0;   // device register
        cfis[8] = 0;   
        cfis[9] = 0;
        cfis[10] = 0;
        cfis[11] = 0;
        cfis[12] = 0;  // sector count low
        cfis[13] = 0;  // sector count high
        // Remaining fields can be left zero

        // 6. Issue the command
        //    We tell the controller that slot 'slot' is now active.
        //    This is done by setting bit=1 << slot in PxCI (Command Issue).
        port->ci |= (1 << slot);

        // 7. Wait for completion
        while (true) {
            // Check if command is done: the bit for 'slot' will be cleared in PxCI
            if ((port->ci & (1 << slot)) == 0) {
                break;  // Command completed
            }
            // Optionally check for errors: PxIS, PxTFD, etc.
            if (port->is & (1 << 30)) {
                // Some error bit, handle or break
                // ...
                return false;
            }
            // In a real OS, you might use an interrupt or yield CPU here
        }

        // 8. Now 'identifyBuffer' has the 512-byte IDENTIFY data
        //    For instance, word 27..46 is the model string in ATA spec.
        //    You can parse it as needed:
        //    - identifyBuffer[0..1] = general config
        //    - identifyBuffer[54..93] = model string, etc.

        // As a quick check, we might look at the word 0 (general config):
        uint16_t* dataWords = reinterpret_cast<uint16_t*>(identifyBuffer);
        uint16_t config = dataWords[0];
        if (config == 0xFFFF || config == 0x0000) {
            // Something’s off, drive not responding properly
            return false;
        }

        // If we reach here, we’ve successfully identified the drive.
        // You could parse more fields, e.g. the model, firmware, capabilities, etc.
        return true;
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