#ifndef ATA_H
#define ATA_H

#include "../util.h"
#include "../errors.h"
#include "../console.h"
#include "device.h"

#define ATA_SELECT_MASTER 0xA0
#define ATA_SELECT_SLAVE 0xB0

#define ATA_BUS_MASTER 0x1F0
#define ATA_BUS_SLAVE 0x170

#define ATA_DATA(x) x
#define ATA_SECCOUNT(x) x+2
#define ATA_LBA_LO(x) x+3
#define ATA_LBA_MI(x) x+4
#define ATA_LBA_HI(x) x+5
#define ATA_SELECT(x) x+6
#define ATA_STATUS(x) x+7
#define ATA_COMMAND(x) x+7

#define ATA_CMD_IDENTIFY 0xEC

#define ATA_READ_MASTER 0xE0
#define ATA_READ_SLAVE 0xF0

int ata_init(struct ata_device *devices[2]);
int ata_read(uint16_t* dst, struct ata_device *dev,
        uint32_t lba, uint8_t count);

int ata_write(uint16_t* src, struct ata_device *dev,
        uint32_t lba, uint8_t count);

#endif /* !ATA_H */
