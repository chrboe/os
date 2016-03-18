#define COL_NOR 0x7
#define COL_ERR 0xC
#define COL_WAR 0xE
#define COL_CRI 0x4
#define COL_SUC 0x2

#include "init.h"

#include "console.h" /* for kprintf, kcls */
#include "gdt.h" /* for setup_gdt, load_gdt, reload_segment_registers */

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
}
