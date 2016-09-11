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
	if(ata_init() == ERR_OK) {
		kputs(COL_SUC, "OK\r\n");
	} else {
		kputs(COL_CRI, "ERR\r\n");
	}

	uint16_t dst[256];
	uint8_t res = ata_read(dst, ATA_READ_MASTER, 0, 1);
	if(res == ERR_OK) {
		kputs(COL_SUC, "READ OK\r\n");
		kprintf(COL_NOR, "DATA: %s\r\n", dst);
	}

	

	while(1);

    kprintf(COL_NOR, "Initializing Multitasking...");
    init_multitasking();
    kprintf(COL_SUC, "OK\r\n");

    asm volatile("int $48");
    while(1);
}
