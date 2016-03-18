#include "gdt.h"

static unsigned long long gdt[GDT_ENTRIES];

/* build_gdt_segment builds a single segment of the global descriptor table
 * and returns it as a 64 bit integer. it just basically shifts everything
 * into the correct place.
 */
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

/*
 * setup_gdt creates 5 segments at the moment:
 * - null descriptor: the first entry is never accessed by the processor,
 *   so we just leave it blank
 * - kernel code: this is the segment where our kernel code will live. it is
 *   executable and in ring 0 (kernel mode).
 * - kernel data: same as kernel code, but non executable and writable.
 * - user code: same as kernel code, but in ring 3 (user mode).
 * - user data: same as kernel data, but in ring 3 (user mode).
 */
void setup_gdt()
{
    gdt[0] = 0;
    gdt[1] = build_gdt_segment(0, 0x100000-1, GDT_FLAGS_KERNEL_CODE); /* kernel code */
    gdt[2] = build_gdt_segment(0, 0x100000-1, GDT_FLAGS_KERNEL_DATA); /* kernel data*/
    gdt[3] = build_gdt_segment(0, 0x100000-1, GDT_FLAGS_USER_CODE); /* user code */
    gdt[4] = build_gdt_segment(0, 0x100000-1, GDT_FLAGS_USER_DATA); /* user data */
}

/*
 * after the gdt is created, it needs to be loaded via the lgdt
 * instruction. the processor expects a pointer to the location
 * where the gdt lies.
 */
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

/*
 * after the gdt is loaded by the processor, it still isn't used.
 * in order for it to get used, we need to reload all segment registers.
 */
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

