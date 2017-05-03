#include "paging.h"

extern void load_page_directory(uint32_t *);
extern void enable_paging();

extern const void kernel_start;
extern const void kernel_end;

/*
 * whether to use physical addresses for addressing page tables
 */
static uint32_t page_tables_physical = 1;

static void blank_directory(uint32_t *pagedir)
{
    for(int i = 0; i < 1024; i++) {
        /*
         * this effectively invalidates the pages.
         */

        //uart_printf("blank %d of %x\r\n", i, pagedir);
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

uintptr_t vmm_map_consecutive(struct vmm_context *context, physaddr_t start, uint32_t num)
{
    for (int i = 0; i < num; i++) {
        physaddr_t phys = (physaddr_t)pmm_alloc();
        //TODO add error checking (out of memory)
        vmm_map_page(context, start + num * 4096, phys);
    }

    return start;
}

uintptr_t vmm_find_free_area(struct vmm_context *context, uint32_t num_pages)
{
    uart_printf("read pd at %x\r\n", context);
    uint32_t *pd = context->page_directory;

    int consecutive = 0;
    uintptr_t first_consecutive = 0;
    for(int i = 0; i < 1024; i++) {
        uart_printf("pd[%d]\r\n", i);
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
    uint32_t num_pages = (size-1)/ 4096 + 1;
    return vmm_alloc_pages(context, num_pages);
}

static struct vmm_context *kernel_context;


static inline uint32_t clear_flags(uint32_t entry)
{
    return entry & ~0xFFF;
}

static inline void table_put(uint32_t *table, uint32_t index, uint32_t entry, uint32_t flags)
{
    table[index] = clear_flags(entry) | flags;
}

static inline void directory_put(struct vmm_context *context, uint32_t index, uint32_t table, uint32_t flags)
{
    context->page_directory[index] = clear_flags(table) | flags;
}

static inline void invalidate_tlb(virtaddr_t addr)
{
    asm volatile("invlpg %0" : : "m" (*(char *)addr));
}

/* TODO */
virtaddr_t vmm_get_virtual(struct vmm_context *context, physaddr_t physical)
{
    return 0;
}

uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt, uintptr_t phys)
{
    uint32_t page_number = (uint32_t)virt / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    uart_printf("pagenum %x pd_index %x pt_index %x\r\n", page_number, pd_index, pt_index);
    uint32_t *page_table;

    /* check if page is already present */
    if (context->page_directory[pd_index] & PAGE_PRESENT) {
        uart_puts("present\r\n");
        page_table = (uint32_t*) clear_flags(context->page_directory[pd_index]);
    } else {
        uart_puts("not present\r\n");
        /* if not, allocate it */
        if (page_tables_physical) {
            page_table = pmm_alloc();
        } else {
            physaddr_t pt_phys = (physaddr_t)pmm_alloc();
            page_table = vmm_get_virtual(context, pt_phys);
        }
        uart_puts("after malloc\r\n");

        directory_put(context, pd_index, (uint32_t)page_table, PAGE_PRESENT | PAGE_WRITE);
    }

    blank_directory(page_table);
    uart_puts("after blank\r\n");

    table_put(page_table, pt_index, phys, PAGE_PRESENT | PAGE_WRITE);

    /* invalidate tlb cache */
    invalidate_tlb(page_table);

    return ERR_OK;
}


uint32_t init_paging()
{
    kernel_context = vmm_create_context();

    /* map kernel context */
    vmm_map_page(kernel_context, PAGE_TABLES_V, (uintptr_t )kernel_context);

    /* map kernel to kernel base */
    physaddr_t kern_base = (physaddr_t)&kernel_start;
    while(kern_base < (physaddr_t)&kernel_end) {
        vmm_map_page(kernel_context, (virtaddr_t)(KERNEL_BASE_V + kern_base), (physaddr_t)kern_base);
        kern_base += 0x1000;
    }

    /* identity map the first 4 mb to itself */
    for(physaddr_t i = 0; i < 1024 * 4096; i+= 0x1000) {
        vmm_map_page(kernel_context, (virtaddr_t)i, i);
    }

    load_page_directory(kernel_context->page_directory);
    enable_paging();

    /* don't use physical page table addresses from now on */
    page_tables_physical = 0;

    return ERR_OK;
}