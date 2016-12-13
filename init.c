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
	int ata_init_stat = ata_init();
	if(ata_init_stat == ERR_OK) {
		kputs(COL_SUC, "OK\r\n");
	} else if(ata_init_stat == ERR_NOTFOUND) {
		kputs(COL_WAR, "OK\r\n");
	} else {
		kputs(COL_CRI, "ERR\r\n");
	}

	/*struct device dev = { "test", DEVICE_ATA, ATA_SELECT_MASTER };

	struct fsfs filesys;
	fsfs_load(&dev, 0, &filesys);*/

	kprintf(COL_NOR, "Testing ATA read... ");

	uint16_t data[256];
	int stat = ata_read(data, ATA_READ_MASTER, 0, 1);

	if(stat == ERR_OK) {
		kprintf(COL_SUC, "OK\r\n");
	} else {
		kprintf(COL_CRI, "ERROR\r\n");
	}

	for(int i = 0; i < 256; i++) {
		kprintf(COL_NOR, "%x ", data[i]);
	}

	while(1);
    kprintf(COL_NOR, "Initializing Multitasking...");
    init_multitasking();
    kprintf(COL_SUC, "OK\r\n");

    asm volatile("int $48");
    while(1);
}
