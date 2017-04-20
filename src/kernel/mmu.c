#include "mmu.h"

extern void load_page_directory(uint32_t *);
extern void enable_paging();


static void blank_directory(uint32_t *pagedir)
{
    for(int i = 0; i < 1024; i++) {
        /*
         * this effectively invalidates the pages.
         */
        pagedir[i] = 0;
    }
}

struct vmm_context *vmm_create_context()
{
    uart_printf("before malloc\r\n");
    struct vmm_context *context = pmm_alloc();
    uart_printf("after malloc\r\n");

    blank_directory(context->page_directory);

    /* map the last page to the page directory itself */
    context->page_directory[1024] = (uint32_t)context->page_directory;

    return context;
}

uintptr_t vmm_map_consecutive(struct vmm_context *context, uintptr_t start, uint32_t num)
{
    for(int i = 0; i < num; i++) {
        uintptr_t phys = (uintptr_t)pmm_alloc();
        //TODO add error checking (out of memory)
        vmm_map_page(context, start + num * 4096, phys);
    }

    return start;
}

uintptr_t vmm_find_free_area(struct vmm_context *context, uint32_t num_pages)
{
    uint32_t *pd = context->page_directory;

    int consecutive = 0;
    uintptr_t first_consecutive = 0;
    for(int i = 0; i < 1024; i++) {
        if (pd[i] & PAGE_PRESENT) {
            consecutive++;

            if (first_consecutive == 0) {
                first_consecutive = pd[i];
            }

            if (consecutive >= num_pages) {
                return first_consecutive;
            }
        } else {
            consecutive = 0;
            first_consecutive = 0;
        }
    }
    return 0;
}

uintptr_t vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages)
{
    uart_printf("allocating %x pages\r\n", num_pages);
    uintptr_t virt = vmm_find_free_area(context, num_pages);
    uart_printf("found free area at %x\r\n", virt);

    if(virt == 0) {
        return 0;
    }

    vmm_map_consecutive(context, virt, num_pages);

    return virt;
}

uintptr_t vmm_alloc(struct vmm_context *context, uint32_t size)
{
    uint32_t num_pages = size / 4096 + 1;
    return vmm_alloc_pages(context, num_pages);
}

uint32_t vmm_map_page(struct vmm_context *context, uintptr_t virt, uintptr_t phys)
{
    uint32_t page_number = virt / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    uart_printf("pagenum %x pd_index %x pt_index %x\r\n", page_number, pd_index, pt_index);
    uint32_t *page_table;

    /* check if page is already present */
    if (context->page_directory[pd_index] & PAGE_PRESENT) {
        uart_puts("present\r\n");
        page_table = (uint32_t*) (context->page_directory[pd_index] & ~0xFFF);
    } else {
        uart_puts("not present\r\n");
        /* if not, allocate it */
        page_table = pmm_alloc();
        uart_puts("after malloc\r\n");
        blank_directory(page_table);
        uart_puts("after blank\r\n");

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

    /* map kernel page directory */
    kernel_context->page_directory[KERNEL_PAGE_TABLES_VADDR >> PGDIR_SHIFT] = (uint32_t) kernel_context->page_directory | PAGE_WRITE | PAGE_PRESENT;

    for(int i = 0; i < 1024 * 4096; i+= 0x1000) {
        vmm_map_page(kernel_context, i, i);
    }

    load_page_directory(kernel_context->page_directory);
    enable_paging();

    return ERR_OK;
}