#ifndef INIT_H
#define INIT_H

#include <stdint.h>

struct multiboot_structure
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t bootdevice;
    uint32_t cmdline;
    uint32_t mods_count;
    void* mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    void* mmap_addr;
} __attribute__((packed));

struct multiboot_mmap {
    uint32_t size;
    uint64_t baseaddr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

#include "console.h" /* for kprintf, kcls */
#include "gdt.h" /* for setup_gdt, load_gdt, reload_segment_registers */
#include "idt.h" /* for setup_idt */
#include "keyboard.h" /* for init_keyboard */
#include "task.h" /* for init_multitasking */
#include "pmm.h" /* for pmm_init */
#include "ata.h" /* for init_ata */
#include "fs/fsfs.h" /* for fsfs_load */

#endif /* !INIT_H */
