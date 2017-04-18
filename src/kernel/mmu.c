#include "mmu.h"

extern void load_page_directory(uint32_t *);
extern void enable_paging();


static void blank_directory(uint32_t *pagedir)
{
    for(int i = 0; i < 1024; i++) {
        /*
         * this effectively invalidates the pages.
         * it sets the following flags:
         * - supervisor (only kernel can access)
         * - read & write
         * - not present
         */
        pagedir[i] = 0x00000002;
    }
}

struct vmm_context *vmm_create_context()
{
    struct vmm_context *context = pmm_alloc();

    blank_directory(context->page_directory);

    return context;
}

uint32_t vmm_map_page(struct vmm_context *context, uintptr_t virt, uintptr_t phys)
{
    uint32_t page_number = virt / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    uint32_t *page_table;

    /* check if page is already present */
    if (context->page_directory[pd_index] & PAGE_PRESENT) {
        page_table = (uint32_t*) (context->page_directory[pd_index] & ~0xFFF);
    } else {
        /* if not, allocate it */
        page_table = pmm_alloc();
        blank_directory(page_table);

        context->page_directory[pd_index] = (uint32_t) page_table | PAGE_PRESENT | PAGE_WRITE;
    }

    page_table[pt_index] = phys | PAGE_PRESENT | PAGE_WRITE;

    /* invalidate tlb cache */
    asm volatile("invlpg %0" : : "m" (*(char *)virt));

    return ERR_OK;
}


static struct vmm_context *kernel_context;

uint32_t init_paging()
{
    kernel_context = vmm_create_context();

    for(int i = 0; i < 1024 * 4096; i+= 0x1000) {
        vmm_map_page(kernel_context, i, i);
    }

    load_page_directory(kernel_context->page_directory);
    enable_paging();

    return ERR_OK;
}