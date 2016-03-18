#include "gdt.h"

static unsigned long long gdt[GDT_ENTRIES];

static unsigned long long build_gdt_segment(unsigned int base, unsigned int limit, 
                                     unsigned int flags)
{
	unsigned long long val;
	val  = limit        & 0x000F0000;
	val |= (flags << 8) & 0x00F0FF00;
	val |= (base >> 16) & 0x000000FF;
	val |= base         & 0xFF000000;

	val <<=32;

	val |= base << 16;
	val |= limit & 0x0000FFFF;
	return val;
}

void setup_gdt()
{
    gdt[0] = 0;
    gdt[1] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<4)|(1<<3)); /* kernel code */
    gdt[2] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<4)|(1<<1)); /* kernel data*/
    gdt[3] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)); /* user code */
    gdt[4] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<1)); /* user data */
//    gdt[5] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<1)); /* tss for multitasking */
}

void load_gdt()
{
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp = {
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};
	asm volatile("lgdt %0" : : "m" (gdtp));
}

void reload_segment_registers()
{
	asm volatile("mov $0x10, %ax\n\t"
		"mov %ax, %ds\n\t"
		"mov %ax, %es\n\t"
		"mov %ax, %fs\n\t"
		"mov %ax, %gs\n\t"
		"mov %ax, %ss\n\t"
		"ljmp $0x8, $.1\n\t"
		".1:");
}

