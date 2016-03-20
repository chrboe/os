#include "idt.h"

static uint64_t idt[IDT_ENTRIES];

static uint64_t build_idt_entry(void (*f)(), uint32_t sel, uint32_t flags)
{
	uint64_t entry;
	uint32_t handler = (uint32_t)f;
	entry  = handler & 0xFFFFLL;
	entry |= (sel & 0xFFFFLL) << 16;
	entry |= (flags & 0xFFLL) << 40;
	entry |= ((handler >> 16) & 0xFFFFLL) << 48;
	return entry;
}

void test()
{
	static int i = 0;
	kprintf(COL_NOR, "test %d\r\n", i++);
}

void setup_idt()
{
	for(int i = 0; i < IDT_ENTRIES; i++)
		idt[i] = build_idt_entry(test, 8, 0x80 | 0x00 | 0xe);
}

void load_idt()
{
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp = {
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};

	asm volatile("lidt %0" : : "m" (idtp));
}
