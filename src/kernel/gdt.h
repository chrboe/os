#ifndef GDT_H
#define GDT_H

#include <stdint.h> /* for uintX_t */

#define GDT_ENTRIES 6

#define SEG_DESCTYPE(x)  ((x) << 0x04) /* descriptor type (0 for system, 1 for code/data) */
#define SEG_PRES(x)      ((x) << 0x07) /* present */
#define SEG_SAVL(x)      ((x) << 0x0C) /* available for system use */
#define SEG_LONG(x)      ((x) << 0x0D) /* long mode */
#define SEG_SIZE(x)      ((x) << 0x0E) /* size (0 for 16 bit, 1 for 32) */
#define SEG_GRAN(x)      ((x) << 0x0F) /* granularity (0 -> each step is 1B, 1 -> each step is 4kB) */
#define SEG_RING(x)     (((x) &  0x03) << 0x05) /* set ring (0 (=kernel mode) to 3 (=user mode)) */
 
#define SEG_DATA_RD        0x00 /* read only */
#define SEG_DATA_RDA       0x01 /* read only, accessed */
#define SEG_DATA_RDWR      0x02 /* read/write */
#define SEG_DATA_RDWRA     0x03 /* read/write, accessed */
#define SEG_DATA_RDEXPD    0x04 /* read only, expand down */
#define SEG_DATA_RDEXPDA   0x05 /* rad only, expand down, accessed */
#define SEG_DATA_RDWREXPD  0x06 /* read/write, expand down */
#define SEG_DATA_RDWREXPDA 0x07 /* read/write, expand down, accessed */
#define SEG_CODE_EX        0x08 /* execute only */
#define SEG_CODE_EXA       0x09 /* execute only, accessed */
#define SEG_CODE_EXRD      0x0A /* execute/read */
#define SEG_CODE_EXRDA     0x0B /* execute/read, accessed */
#define SEG_CODE_EXC       0x0C /* execute only, conforming */
#define SEG_CODE_EXCA      0x0D /* execute-only, conforming, accessed */
#define SEG_CODE_EXRDC     0x0E /* execute/read, conforming */
#define SEG_CODE_EXRDCA    0x0F /* execute/read, conforming, accessed */

#define SEG_TSS            0x09

/* flags for the kernel code segment:
 * - present
 * - ring 0
 * - executable, read only
 * - 32 bit
 */
#define GDT_FLAGS_KERNEL_CODE 	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
				SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
				SEG_RING(0)     | SEG_CODE_EXRD

/*
 * flags for the kernel data segment:
 * - present
 * - ring 0
 * - non-executable, read/write
 * - 32 bit
 */
#define GDT_FLAGS_KERNEL_DATA	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
				SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
				SEG_RING(0)     | SEG_DATA_RDWR

/*
 * flags for the user code segment:
 * - present
 * - ring 3
 * - executable, read only
 * - 32 bit
 */
#define GDT_FLAGS_USER_CODE	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
				SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
				SEG_RING(3)     | SEG_CODE_EXRD

/*
 * flags for the user data segment:
 * - present
 * - ring 3
 * - non-executable, read/write
 * - 32 bit
 */
#define GDT_FLAGS_USER_DATA	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
				SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
				SEG_RING(3)     | SEG_DATA_RDWR

#define GDT_FLAGS_TSS SEG_TSS | SEG_PRES(1) | SEG_RING(3)

uint32_t tss[32];

void setup_gdt();
void load_gdt();
void reload_segment_registers();

#endif /* !GDT_H */
