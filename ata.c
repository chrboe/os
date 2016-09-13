#include "ata.h"

#define DEV_NONE 0
#define DEV_MASTER 1
#define DEV_SLAVE 2

#define IDEN_NONEXIST 0
#define IDEN_INVAL 1
#define IDEN_VALID 2

#define DIR_READ 0
#define DIR_WRITE 1

static void select_master()
{
	outb(ATA_SELECT, ATA_SELECT_MASTER);
}

static void select_slave()
{
	outb(ATA_SELECT, ATA_SELECT_SLAVE);
}

static int identify(uint16_t select)
{
	outb(ATA_SELECT, select);
	outb(ATA_SECCOUNT, 0x00); /* set sectorcount to 0 */
	outb(ATA_LBA_LO, 0x00); /* set the lba ports to 0 */
	outb(ATA_LBA_MI, 0x00);
	outb(ATA_LBA_HI, 0x00);

	outb(ATA_COMMAND, 0xEC); /* send the IDENTIFY command */

	uint8_t status = inb(ATA_STATUS);

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
		status = inb(ATA_STATUS);
	}

	if(inb(ATA_LBA_LO) != 0 || inb(ATA_LBA_MI) != 0) {
		return IDEN_INVAL;
	}

	while(1) {
		status = inb(ATA_STATUS);
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
		data[i]  = inw(ATA_DATA);
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

	uint8_t status = inb(ATA_STATUS);
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

	int id = identify(ATA_SELECT_MASTER);
	if(id == IDEN_VALID) {
		res |= DEV_MASTER;
	}

	id = identify(ATA_SELECT_SLAVE);
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

static int ata_io(uint16_t* data, uint16_t select, uint32_t lba, uint8_t count, int direction)
{
	/*
	 * this function reads from or writes to
	 * an ATA harddisk.
	 * currently it only supports polling.
	 *
	 * to get data from or write data to the
	 * harddisk,we first have to select our drive
	 * (master or slave) and tell the drive how 
	 * much data we wanna read or write and 
	 * from/to where we wanna read/write it.
	 * this is done by sending 0xE0 (master)
	 * or 0xF0 (slave) ORed with the highest 4
	 * bits of the LBA to port 0x1F6.
	 */
	outb(ATA_SELECT, select | ((lba >> 24) & 0x0F));

	/*
	 * write how many sectors we wanna read
	 * to 0x1F2.
	 */
	outb(ATA_SECCOUNT, count);

	/*
	 * write the lba to 0x1F[3-5]
	 */
	outb(ATA_LBA_LO, (uint8_t)lba);
	outb(ATA_LBA_MI, (uint8_t)(lba >> 8));
	outb(ATA_LBA_HI, (uint8_t)(lba >> 16));

	/*
	 * finally, send the actual command
	 */
	if(direction == DIR_READ) {
		outb(ATA_COMMAND, 0x20);
	} else {
		outb(ATA_COMMAND, 0x30);
	}

	/*
	 * now we have to poll for our results.
	 * the first 4 polls can be discarded because
	 * the error bits are all fucked up.
	 */
	uint8_t status = 0;
	for(int i = 0; i < 4; i++) {
		inb(ATA_STATUS);
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
			status = inb(ATA_STATUS);
		} while(((status & 0x80) && !(status & 0x08)) && !(status & 0x01) && !(status & 0x20));

		/*
		 * if an error occoured, we just print it to
		 * the console right now. (TODO make this
		 * better)
		 */
		if((status & 0x01) || (status & 0x20)) {
			kprintf(COL_ERR, "ERROR\r\n");
			uint8_t err = inb(0x1F1);
			if(err & 1<<1) {
				kprintf(COL_CRI, "No Media\r\n");
			}
			
			if(err & 1<<2) {
				kprintf(COL_CRI, "Command aborted\r\n");
			}
			
			if(err & 1<<3) {
				kprintf(COL_CRI, "CD eject\r\n");
			}

			if(err & 1<<4) {
				kprintf(COL_CRI, "Sector-ID not found\r\n");
			}

			if(err & 1<<5) {
				kprintf(COL_CRI, "Medium changed\r\n");
			}

			if(err & 1<<6) {
				kprintf(COL_CRI, "Error in data field\r\n");
			}

			if(err & 1<<7) {
				kprintf(COL_CRI, "CRC error\r\n");
			}
			return ERR_OTHER;
		}

		/*
		 * once we got our successful response,
		 * continue by reading/writing 256 words
		 * (= 512 bytes = 1 sector) from io port 0x1F0.
		 */
		for(uint16_t j = 0; j < 256; j++) {
			if(direction == DIR_READ) {
				data[(i*256)+j] = inw(ATA_DATA);
			} else {	
				outw(ATA_DATA, data[(i*256)+j]);
			}
		}

		i++;

		/*
		 * continue until we have read/written all
		 * our data.
		 */
	}

	return ERR_OK;
}

int ata_read(uint16_t* dst, uint16_t select, uint32_t lba, uint8_t count)
{
	return ata_io(dst, select, lba, count, DIR_READ);
}

int ata_write(uint16_t* src, uint16_t select, uint32_t lba, uint8_t count)
{
	return ata_io(src, select, lba, count, DIR_WRITE);
}

#undef IDEN_NONEXIST
#undef IDEN_INVAL
#undef IDEN_VALID

#undef DEV_NONE
#undef DEV_MASTER
#undef DEV_SLAVE
