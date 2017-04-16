#include "gpt.h"

static int check_signature(uint16_t data[])
{
	return (data[0] == 0x4645 && data[1] == 0x2049 && data[2] == 0x4150 && data[3] == 0x5452);
}

static uint32_t get_revision(uint16_t data[])
{
	return COMBINE16TO32(data[4], data[5]);
}

static uint32_t get_header_size(uint16_t data[])
{
	return COMBINE16TO32(data[7], data[6]);
}

static uint32_t get_crc32(uint16_t data[])
{
	return COMBINE16TO32(data[8], data[9]);
}

int discover_gpt(struct ata_device *dev)
{
	uint16_t data[256];
	ata_read(data, dev, 1, 1);
	kprintf(COL_NOR, "info about device:\r\n");
	kprintf(COL_NOR, "revision: %d\r\n", get_revision(data));
	kprintf(COL_NOR, "header size: %d\r\n", get_header_size(data));
	kprintf(COL_NOR, "CRC32: 0x%x\r\n", get_crc32(data));
	
	if(check_signature(data)) {
		return ERR_OK;
	}
	return ERR_NOTFOUND;
}
