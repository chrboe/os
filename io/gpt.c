#include "gpt.h"

static int check_signature(uint16_t data[])
{
	return (data[0] == 0x4645 && data[1] == 0x2049 && data[2] == 0x4150 && data[3] == 0x5452);
}

int discover_gpt(struct ata_device *dev)
{
	uint16_t data[256];
	ata_read(data, dev, 1, 1);
	if(check_signature(data)) {
		return ERR_OK;
	}
	return ERR_NOTFOUND;
}
