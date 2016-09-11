#ifndef ATA_H
#define ATA_H

#include "util.h"
#include "errors.h"
#include "console.h"

#define ATA_MASTER 0xA0
#define ATA_SLAVE 0xB0

#define ATA_READ_MASTER 0xE0
#define ATA_READ_SLAVE 0xF0

int ata_init();
uint8_t ata_read(uint16_t* dst, uint16_t select, uint32_t lba, uint8_t count);

#endif /* !ATA_H */
