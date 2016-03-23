#include "init.h"

#include "console.h" /* for kprintf, kcls */
#include "gdt.h" /* for setup_gdt, load_gdt, reload_segment_registers */
#include "idt.h" /* for setup_idt */
#include "keyboard.h" /* for init_keyboard */

void init()
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
	if(init_keyboard()) {
		kprintf(COL_SUC, "OK\r\n");
	} else {
		kprintf(COL_CRI, "ERR\r\n");
	}

	asm volatile("int $48");
	while(1);
}
