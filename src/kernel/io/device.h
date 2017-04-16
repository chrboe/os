#ifndef DEVICE_H
#define DEVICE_H

#include "../util.h"

struct ata_device {
	uint8_t supports_lba28;
	uint8_t supports_lba48;
	uint32_t lba28_sectors;
	uint64_t lba48_sectors;
	uint16_t bus;
	uint16_t select;
	uint16_t select_read;
};

#endif /* !DEVICE_H */
