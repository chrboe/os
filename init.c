#include "init.h"

void init(struct multiboot_structure* mb_struc)
{
    kcls();

    kprintf(COL_NOR, "Building GDT... ");
    setup_gdt();
    kprintf(COL_SUC, "OK\r\n");

    kprintf(COL_NOR, "Loading GDT... ");
    load_gdt();
    kprintf(COL_SUC, "OK\r\n");

    kprintf(COL_NOR, "Reloading segment registers... ");
    reload_segment_registers();
    kprintf(COL_SUC, "OK\r\n");

    kprintf(COL_NOR, "Building IDT... ");
    setup_idt();
    kprintf(COL_SUC, "OK\r\n");

    kprintf(COL_NOR, "Loading IDT... ");
    load_idt();
    kprintf(COL_SUC, "OK\r\n");

    kprintf(COL_NOR, "Initializing Keyboard... ");
    if(init_keyboard() == ERR_OK) {
        kputs(COL_SUC, "OK\r\n");
    } else {
        kputs(COL_CRI, "ERR\r\n");
    }

    int baud = 38400;
    kprintf(COL_NOR, "Initializing Serial Interface (%d baud)...", baud);
    int uart_init_stat = uart_init(baud);

    if(uart_init_stat == ERR_OK) {
        kputs(COL_SUC, "OK\r\n");
    } else {
        kputs(COL_CRI, "ERR\r\n");
    }

    kprintf(COL_NOR, "Initializing Physical Memory Manger... ");
    pmm_init(mb_struc);
    kprintf(COL_SUC, "OK\r\n");

	kprintf(COL_NOR, "Initializing ATA interface... ");
	struct ata_device *devices[2];
	devices[0] = pmm_alloc();
	devices[1] = pmm_alloc();

	int ata_init_stat = ata_init(devices);
	if(ata_init_stat == ERR_OK) {
		kputs(COL_SUC, "OK\r\n");
	} else if(ata_init_stat == ERR_NOTFOUND) {
		kputs(COL_WAR, "OK\r\n");
	} else {
		kputs(COL_CRI, "ERR\r\n");
	}

	kprintf(COL_NOR, "Testing ATA read... ");

	uint16_t data[512];
	int stat = ata_read(data, devices[0], 1, 2);

	if(stat == ERR_OK) {
		kprintf(COL_SUC, "OK\r\n");
	} else {
		kprintf(COL_CRI, "ERROR\r\n");
	}


	/*
	kprintf(COL_NOR, "Testing ATA write... ");
	data[0]++;
	int wstat = ata_write(data, devices[0], 0, 1);
	if(wstat == ERR_OK) {
		kprintf(COL_SUC, "OK\r\n");
	} else {
		kprintf(COL_CRI, "ERROR\r\n");
	}
	*/

	/*for(int i = 0; i < 128; i++) {
		kprintf(COL_NOR, "%x ", data[i]);
	}*/
/*
	kputs(COL_NOR, "Discovering GPT... ");

	int gpt_stat = discover_gpt(devices[0]);
	if(gpt_stat == ERR_OK) {
		kputs(COL_SUC, "OK\r\n");
	} else {
		kputs(COL_CRI, "ERROR\r\n");
	}

	kprintf(COL_NOR, "Signature: %x %x %x %x\r\n", data[0], data[1], data[2], data[3]);
*/
	/*kputs(COL_NOR, "\r\n");
	kprintf(COL_NOR, "revision: %d\r\n", COMBINE16TO32(data[5], data[4]));
	kprintf(COL_NOR, "header size: %d\r\n", COMBINE16TO32(data[7], data[6]));
	kprintf(COL_NOR, "CRC32: 0x%x\r\n", COMBINE16TO32(data[9], data[8]));
	kprintf(COL_NOR, "reserved (0): %d\r\n", COMBINE16TO32(data[10], data[11]));
	kprintf(COL_NOR, "current LBA: %d\r\n", COMBINE16TO64(data[15], data[14], data[13], data[12]));
	kprintf(COL_NOR, "backup LBA: %d\r\n", COMBINE16TO64(data[19], data[18], data[17], data[16]));
	kprintf(COL_NOR, "first usable LBA: %d\r\n", COMBINE16TO64(data[23], data[22], data[21], data[20]));
	kprintf(COL_NOR, "last usable LBA: %d\r\n", COMBINE16TO64(data[27], data[26], data[25], data[24]));
	kprintf(COL_NOR, "disk GUID: ?\r\n");
	kprintf(COL_NOR, "start LBA of array: %d\r\n", (uint32_t)COMBINE16TO64(data[39], data[38], data[37], data[36]));
	kprintf(COL_NOR, "number of entries: %d\r\n", COMBINE16TO32(data[41], data[40]));
	kprintf(COL_NOR, "size of entry: %d\r\n", COMBINE16TO32(data[43], data[42]));
	kprintf(COL_NOR, "CRC32 of array: 0x%x\r\n", COMBINE16TO32(data[45], data[44]));

    uint32_t array_start = (uint32_t)COMBINE16TO64(data[39], data[38], data[37], data[36]);
    kprintf(COL_NOR, "start %d\r\n", array_start);
    uint16_t *array = data + array_start*128;
    kprintf(COL_NOR, "\r\nPARTITION:\r\n");
    kprintf(COL_NOR, "first lba: %d\r\n", COMBINE16TO64(array[19], array[18], array[17], array[16]));
    kprintf(COL_NOR, "last lba: %d\r\n", COMBINE16TO64(array[23], array[22], array[21], array[20]));*/


    kprintf(COL_NOR, "Initializing Multitasking...");
    init_multitasking();
    kprintf(COL_SUC, "OK\r\n");

    /*
     * BUG: moving these two lines above init_multitasking() makes the kernel triple-fault.
     * this is probably because the memory is left initialized and not cleared by pmm_alloc.
     * this probably confuses the task initializer somehow when it allocates space for the stack,
     * but i don't really know.
     * proposed fix: wait for a proper paging mechanism
     */
    pmm_free(devices[0]);
    pmm_free(devices[1]);

    while(1);
}
