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

    kprintf(COL_NOR, "Initializing Physical Memory Manger... ");
    pmm_init(mb_struc);
    kprintf(COL_SUC, "OK\r\n");

	kprintf(COL_NOR, "Initializing ATA interface... ");

	struct ata_device *devices[2];
	devices[0] = pmm_alloc(sizeof(struct ata_device));
	devices[1] = pmm_alloc(sizeof(struct ata_device));

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

	kputs(COL_NOR, "Discovering GPT... ");

	int gpt_stat = discover_gpt(devices[0]);
	if(gpt_stat == ERR_OK) {
		kputs(COL_SUC, "OK\r\n");
	} else {
		kputs(COL_CRI, "ERROR\r\n");
	}

	kprintf(COL_NOR, "Signature: %x %x %x %x\r\n", data[0], data[1], data[2], data[3]);

	kputs(COL_NOR, "\r\n");
	kprintf(COL_NOR, "inodes: %d\r\n", *(uint32_t*)data);
	kprintf(COL_NOR, "blocks: %d\r\n", *(uint32_t*)(data+4));
	pmm_free(devices[0]);
	pmm_free(devices[1]);

	while(1);
    kprintf(COL_NOR, "Initializing Multitasking...");
    init_multitasking();
    kprintf(COL_SUC, "OK\r\n");

    asm volatile("int $48");
    while(1);
}
