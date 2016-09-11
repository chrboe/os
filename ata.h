#ifndef ATA_H
#define ATA_H

#include "util.h"
#include "errors.h"
#include "console.h"

#define ATA_SELECT_MASTER 0xA0
#define ATA_SELECT_SLAVE 0xB0

#define ATA_DATA 0x1F0
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA_LO 0x1F3
#define ATA_LBA_MI 0x1F4
#define ATA_LBA_HI 0x1F5
#define ATA_SELECT 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7

#define ATA_READ_MASTER 0xE0
#define ATA_READ_SLAVE 0xF0

int ata_init();
int ata_read(uint16_t* dst, uint16_t select, uint32_t lba, uint8_t count);
int ata_write(uint16_t* src, uint16_t select, uint32_t lba, uint8_t count);

#endif /* !ATA_H */
