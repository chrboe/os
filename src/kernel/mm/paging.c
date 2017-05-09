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

static void blank_table(uint32_t *table)
{
    for(int i = 0; i < 1024; i++) {
        table[i] = 0;
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
    uart_printf("vmm_map_consecutive start: %x, num: %x", start, num);
    for (int i = 0; i < num; i++) {
        physaddr_t phys = (physaddr_t)pmm_alloc();
        //TODO add error checking (out of memory)
        vmm_map_page(context, start + i * 4096, phys);
    }

    return start;
}

void* vmm_find_free_area(struct vmm_context *context, uintptr_t minimum, uintptr_t maximum, uint32_t num_pages)
{
    uart_printf("[find] read pd at %x\r\n", context);
    uint32_t *pd = context->page_directory;

    uart_printf("[find] pd=%x\r\n", pd);

    int consecutive = 0;
    virtaddr_t first_consecutive = 0;

    /* don't map NULL page */
    if (minimum == 0) {
        minimum = 0x1000;
    }

    uart_printf("[find] minimum = %x (%d)\r\n", minimum, (minimum >> 12) / 1024);
    uart_printf("[find] maximum = %x (%d)\r\n", maximum, (maximum >> 12) /1024);
    for(int j = (minimum >> 12) / 1024; j < 1024 && j < (maximum >> 12) / 1024; j++) {
        uart_printf("[find] pd[%d] (%x)\r\n", j, pd[j]);
        if (pd[j] & PAGE_PRESENT) {
            uart_printf("[find] page present\r\n");
            uint32_t *current_pt = 0xC0000000 + clear_flags(pd[j]);
            uart_printf("[find] current pt: %x\r\n", current_pt);
            for(int i = 0; i < 1024; i++) {
                uart_printf("[find] pt[%d] (%x)\r\n", i, current_pt[i]);
                if (current_pt[i] & PAGE_PRESENT) {
                    uart_printf("[find] pt[%d] is present, resetting\r\n", i);
                    consecutive = 0;
                    first_consecutive = 0;
                } else {
                    uart_printf("[find] pt[%d] is not present\r\n", i);
                    consecutive++;

                    if (first_consecutive == 0) {
                        uart_printf("[find] pt: first_consecutive: %x ->", first_consecutive);
                        first_consecutive = ((j * 1024 * 4096) + (i * 4096));
                        uart_printf(" %x\r\n", first_consecutive);
                        uart_printf("i: %d, j: %d", i, j);
                    }


                    if (consecutive >= num_pages) {
                        uart_printf("[find] return %x\r\n", first_consecutive);
                        return first_consecutive;
                    }
                }
            }
        } else {
            if (first_consecutive == 0) {
                uart_printf("[find] pd: first_consecutive: %x ->", first_consecutive);
                first_consecutive = (j << 12);
                uart_printf(" %x\r\n", first_consecutive);
            }
            consecutive += 1024;
        }

        if (consecutive >= num_pages) {
            uart_printf("[find] return %x\r\n", first_consecutive);
            return first_consecutive;
        }
    }

    return 0;
}

void* vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages)
{

    asm("cli");
    uart_printf("\r\n\r\nPD DUMP!\r\n");
    for(int i = 0; i < 1024; i++) {
        uart_printf("%x (%d)\t(%x)\t=\t%x\r\n", context->page_directory+i, i, i * 0x400000, context->page_directory[i]);
    }


    uart_printf("\r\n\r\nPT DUMP!\r\n");
    uint32_t *pt = 0xC0000000 + (clear_flags(context->page_directory[768]));
    for(int i = 0; i < 1024; i++) {
        uart_printf("%x (%d)\t(%x)\t=\t%x\r\n", pt+i, i, 0xC0000000 + i * 1024, pt[i]);
    }

    asm("sti");

    uart_printf("allocating %x pages\r\n", num_pages);
    uintptr_t virt = vmm_find_free_area(context, 0xC0000000, 0xFFFFFFFF, num_pages);
    uart_printf("found free area at %x\r\n", virt);

    if(virt == 0) {
        return 0;
    }

    vmm_map_consecutive(context, virt, num_pages);

    return virt;
}

void* vmm_alloc(struct vmm_context *context, uint32_t size)
{
    uint32_t num_pages = (size-1)/ 4096 + 1;
    return vmm_alloc_pages(context, num_pages);
}

static struct vmm_context *kernel_context;

uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt, uintptr_t phys)
{
    uart_printf("vmm_map_page: %x -> %x\r\n", phys, virt);
    uint32_t page_number = (uint32_t)virt / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    uart_printf("pagenum %x pd_index %x pt_index %x\r\n", page_number, pd_index, pt_index);
    physaddr_t page_table;

    /* check if page table is already present */
    if (context->page_directory[pd_index] & PAGE_PRESENT) {
        uart_printf("present: %x\r\n", context->page_directory[pd_index]);
        page_table = clear_flags(context->page_directory[pd_index]);
    } else {
        uart_puts("not present\r\n");
        /* if not, allocate it */
        page_table = pmm_alloc();
        uart_puts("after malloc\r\n");

        directory_put(context, pd_index, (uint32_t)page_table, PAGE_PRESENT | PAGE_WRITE);
        invalidate_tlb(context->page_directory);
        uart_puts("after directory_put\r\n");
    }

    virtaddr_t page_table_virt = 0xC0000000 + clear_flags(page_table);

    blank_directory(page_table_virt);
    uart_printf("page_table_virt = %x\r\n", page_table_virt);

    table_put(page_table_virt, pt_index, phys, PAGE_PRESENT | PAGE_WRITE);

    uart_puts("after table_put\r\n");
    /* invalidate tlb cache */
    invalidate_tlb(page_table_virt);


    uart_puts("after invalidate\r\n");

    return ERR_OK;
}

#if 0
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
#endif