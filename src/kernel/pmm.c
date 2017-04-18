#include "pmm.h"

static uint32_t page_free_bits[NUM_PAGES];
extern const void kernel_start;
extern const void kernel_end;

void* pmm_alloc()
{
    for(int i = 0; i < NUM_PAGES; i++) {
        for(int j = 0; j < 32; j++) {
            if(page_free_bits[i] & 1<<j) {
                page_free_bits[i] &= ~(1 << j);
                uart_printf("malloc %x\r\n", ((i * 32 + j) * 4096));
                return (void*)((i * 32 + j) * 4096);
            }
        }
    }
    uart_puts("malloc 0\r\n");
    return 0;
}

void pmm_free(void* p)
{
    uintptr_t i = (uintptr_t) p / 4096;
    page_free_bits[i / 32] |= (1 << (i % 32));
}

void pmm_mark_used(void *p)
{
    uintptr_t i = (uintptr_t) p / 4096;
    uart_printf("mark used %x (i=%d, i/32=%d)\r\n", p, i, i/32);
    page_free_bits[i / 32] &= ~(1 << (i % 32));
}

void dump_free_bits()
{
    asm("cli");
    uart_puts("MEMORY DUMP\r\n");
    for(int i = 0; i < NUM_PAGES; i++) {
        uart_printf("%x ", page_free_bits[i]);
        if(i != 0 && i % 64 == 0) {
            uart_printf("aaa %d\r\n", i);
        }
    }
    asm("sti");
}

void pmm_init(struct multiboot_structure* mb_struc)
{
    struct multiboot_mmap* mmap = mb_struc->mmap_addr;
    struct multiboot_mmap* mmap_end = (void*)((uintptr_t) mb_struc->mmap_addr + mb_struc->mmap_length);

    uart_printf("mmap: %x\r\nmmap_length: %d\r\nmmap_end: %x\r\n", mmap, mb_struc->mmap_length, mmap_end);

    /* reserve everything by default */
    for(int i = 0; i < NUM_PAGES; i++) {
        page_free_bits[i] = 0x00;
    }

    /* free everything that should be freed according to the BIOS */
    while(mmap < mmap_end) {
        uart_printf("type %d\r\n", mmap->type);
        if(mmap->type == 1) {
            /* mem is free */
            uintptr_t base = (uintptr_t)mmap->baseaddr;
            uintptr_t end = (uintptr_t)(base + mmap->length);

            while(base < end) {
                pmm_free((void*)base);
                base += 0x1000;
            }
        }
        mmap++;
    }

    /* mark the kernel as used again */
    uintptr_t kern_base = (uintptr_t)&kernel_start;
    while(kern_base < (uintptr_t)&kernel_end) {
        pmm_mark_used((void *)kern_base);
        kern_base += 0x1000;
    }

    /* reserve the multiboot structure aswell as the module list */
    struct multiboot_module* modules = mb_struc->mods_addr;
    pmm_mark_used(mb_struc);
    pmm_mark_used(modules);

    /* now reserve the modules themselves */
    int i;
    for (i = 0; i < mb_struc->mods_count; i++) {
        kern_base = modules[i].mod_start;
        while (kern_base < modules[i].mod_end) {
            pmm_mark_used((void*) kern_base);
            kern_base += 0x1000;
        }
    }
    uart_printf("kernel base %x\r\nkernel end %x\r\n", kern_base, (uintptr_t)&kernel_end);
}
