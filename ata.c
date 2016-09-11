#include "ata.h"

#define DEV_NONE 0
#define DEV_MASTER 1
#define DEV_SLAVE 2

#define IDEN_NONEXIST 0
#define IDEN_INVAL 1
#define IDEN_VALID 2

static void select_master()
{
	outb(0x1F6, 0xA0);
}

static void select_slave()
{
	outb(0x1F6, 0xB0);
}

static int identify(uint16_t select)
{
	outb(0x1F6, select);
	outb(0x1F2, 0x00); /* set sectorcount to 0 */
	outb(0x1F3, 0x00); /* set the lba ports to 0 */
	outb(0x1F4, 0x00);
	outb(0x1F5, 0x00);

	outb(0x1F7, 0xEC); /* send the IDENTIFY command */

	uint8_t status = inb(0x1F7);

	/*
	 * now that we know the "response" of the
	 * device, we need to act as follows:
	 *
	 * - if the value is 0, the drive doesn't
	 *   exist
	 * - for any other value, we need to poll the
	 *   status port until bit 7 (BSY) clears.
	 * - while polling, if the LBAm and LBAh
	 *   ports are non-zero when BSY clears, 
	 *   abort polling (that would be a non-ATA 
	 *   drive)
	 * - continue polling until bit 3 (DRQ)
	 *   sets or bit 0 (ERR) sets
	 */

	if(status == 0) {
		return IDEN_NONEXIST;
	}

	while(status & 0x80) {
		status = inb(0x1F7);
	}

	if(inb(0x1F4) != 0 || inb(0x1F5) != 0) {
		return IDEN_INVAL;
	}

	while(1) {
		status = inb(0x1F7);
		if((status & 0x08) | (status & 0x01)) {
			break;
		}
	}

	if(status & 0x01) {
		return IDEN_INVAL;
	}

	/*
	 * at this point, we can be pretty sure that
	 * the selected drive is working. now we can
	 * read 256 16-bit values from the data port
	 * in order to retrieve a bunch of useful
	 * information.
	 */

	uint16_t data[256];
	for(uint16_t i = 0; i < 256; i++) {
		data[i]  = inw(0x1F0) << 8;
	}

	return IDEN_VALID;
}

int discover_devices()
{
	/*
	 * so in this method we run a couple of tests
	 * in order to determine whether or not there
	 * are devices on the ATA bus (and which
	 * devices there are).
	 */

	int res = DEV_NONE;

	/*
	 * first up is the so-called "floating bus" 
	 * test. this relies on the fact that the
	 * physical cables get pulled up to logic
	 * level "high" by the pull-up resistors if
	 * they aren't connected to anything.
	 *
	 * here, we simply read the status register
	 * and check if all the bits are high.
	 */

	uint8_t status = inb(0x1F7);
	if(status == 0xFF) {
		return res;
	}

	/*
	 * if the above condition was true, there is
	 * definitely no device on the bus. note that
	 * getting to this point doesn't tell us that
	 * a device IS present, though.
	 *
	 * the next test uses the IDENTIFY DEVICE
	 * command. here we find out which devices
	 * are present, if any.
	 */

	int id = identify(0xA0);
	if(id == IDEN_VALID) {
		res |= DEV_MASTER;
	}

	id = identify(0xB0);
	if(id == IDEN_VALID) {
		res |= DEV_SLAVE;
	}
	
	return res;
}

int ata_init()
{
	int disc = discover_devices();
	if(disc) {
		return ERR_OK;
	}

	return ERR_NOTFOUND;
}

uint8_t ata_read(uint16_t* dst, uint16_t select, uint32_t lba, uint8_t count)
{
	/*
	 * this function reads from an ATA harddisk.
	 * currently it only supports polling
	 *
	 * to get data from the harddisk, we first
	 * have to select our drive (master or slave)
	 * and tell the drive how much data we wanna
	 * read and from where we wanna read it.
	 * this is done by sending 0xE0 (master)
	 * or 0xF0 (slave) ORed with the highest 4
	 * bits of the LBA to port 0x1F6.
	 */
	outb(0x1F6, select | ((lba >> 24) & 0x0F));

	/*
	 * this just writes a null byte to 0x1F1.
	 * this isn't technically needed i guess(?)
	 */
	outb(0x1F1, 0x00);

	/*
	 * write how many sectors we wanna read
	 * to 0x1F2.
	 */
	outb(0x1F2, count);

	/*
	 * write the lba to 0x1F[3-5]
	 */
	outb(0x1F3, (uint8_t)lba);
	outb(0x1F4, (uint8_t)(lba >> 8));
	outb(0x1F5, (uint8_t)(lba >> 16));

	/*
	 * finally, send the actual "read" command
	 */
	outb(0x1F7, 0x20);

	/*
	 * now we have to poll for our results.
	 * the first 4 polls can be discarded because
	 * the error bits are all fucked up.
	 */
	uint8_t status = 0;
	for(int i = 0; i < 4; i++) {
		inb(0x1F7);
	}

	/*
	 * now for our real polling:
	 */
	uint8_t i = 0;
	while(1) {
		if(i >= count) {
			return ERR_OK;
		}

		/*
		 * we poll until bit 7 clears and bit 3 sets,
		 * or until bit 0 or bit 5 sets. the latter
		 * two would indicate an error.
		 */
		do {
			status = inb(0x1F7);
		} while(((status & 0x80) && !(status & 0x08)) && !(status & 0x01) && !(status & 0x20));

		if((status & 0x01) || (status & 0x20)) {
			kprintf(COL_ERR, "ERROR\r\n");
			return ERR_OTHER;
		}

		/*
		 * once we got our successful response,
		 * continue by reading 256 words
		 * (= 512 bytes = 1 sector) from io port 0x1F0.
		 */
		for(uint16_t j = 0; j < 256; j++) {
			dst[(i*256)+j] = inw(0x1F0); 
		}

		i++;

		/*
		 * continue until we have all our data.
		 */
	}

	return ERR_OK;
}

#undef IDEN_NONEXIST
#undef IDEN_INVAL
#undef IDEN_VALID

#undef DEV_NONE
#undef DEV_MASTER
#undef DEV_SLAVE
