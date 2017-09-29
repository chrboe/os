#include "paging.h"

extern const void kernel_start;
extern const void kernel_end;

struct vmm_context *active_context;
static struct vmm_context tmp_kernel_context;

static void blank_directory(uint32_t *pagedir)
{
    for(int i = 0; i < 1024; i++) {
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
    uart_printf("allocating new context\r\n");
    struct vmm_context *context = vmm_kalloc_pages(1);

    uart_printf("allocating page directory\r\n");
    context->page_directory = vmm_kalloc_pages(1);
    uart_printf("page directory = %x\r\n", context->page_directory);
    blank_directory(context->page_directory);
    uart_printf("after blank\r\n");

    /* map the last page to the page directory itself */
    context->page_directory[1023] = (uint32_t)context->page_directory;
    uart_printf("after id map\r\n");

    /* map the kernel address space to the context */
    for(physaddr_t i = &kernel_start; i < &kernel_end; i += 4096) {
        uart_printf("%x\r\n", i);
        vmm_map_page(context, i, i - 0xC0000000);
    }

    return context;
}

static void vmm_unmap_page(struct vmm_context *context, virtaddr_t addr)
{
    /* TODO */
}

static uint32_t get_pt_entry(uint32_t *page_directory, uintptr_t minimum,
        uintptr_t maximum, virtaddr_t addr)
{
    uint32_t page_number = (uint32_t)addr / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    if (page_directory[pd_index] & PAGE_PRESENT) {
        physaddr_t page_table_phys = clear_flags(page_directory[pd_index]);

        /*
         * just remap the address real quick. may seem dirty,
         * but it's the easiest thing to do
         */
        uint32_t *page_table = vmm_find_free_area(active_context,
                minimum, maximum, 1);

        vmm_map_page(active_context, page_table, page_table_phys);

        uint32_t result = 0;
        if (page_table[pt_index] & PAGE_PRESENT) {
            result = page_table[pt_index];
        }

        vmm_unmap_page(active_context, page_table);

        return result;
    } else {
        return 0;
    }
}

physaddr_t vmm_uresolve(struct vmm_context *context, virtaddr_t addr)
{
    return vmm_resolve(context, 0x00000000, 0xBFFFFFFF, addr);
}

physaddr_t vmm_kresolve(virtaddr_t addr)
{
    return vmm_resolve(active_context, 0xC0000000, 0xFFFFFFFF, addr);
}

physaddr_t vmm_resolve(struct vmm_context *context, uintptr_t minimum,
        uintptr_t maximum, virtaddr_t addr)
{
    uint32_t pte = get_pt_entry(context->page_directory,
            minimum, maximum, addr);

    if (pte & PAGE_PRESENT) {
        return clear_flags(pte) | ((uint32_t)addr & 0xFFF);
    } else {
        return 0;
    }
}

virtaddr_t vmm_map_consecutive(struct vmm_context *context, virtaddr_t start,
        uint32_t num)
{
    uart_printf("vmm_map_consecutive: start: %x, num: %x\r\n", start, num);
    for (int i = 0; i < num; i++) {
        physaddr_t phys = (physaddr_t)pmm_alloc();
        //TODO add error checking (out of memory)
        vmm_map_page(context, start + i * 4096, phys);
    }

    uart_printf("%s: return %x\r\n", __func__, start);
    return start;
}

void* vmm_find_free_area(struct vmm_context *context, uintptr_t minimum,
        uintptr_t maximum, uint32_t num_pages)
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

    uart_printf("[find] minimum = %x (%d)\r\n", minimum,
            (minimum >> 12) / 1024);

    uart_printf("[find] maximum = %x (%d)\r\n", maximum,
            (maximum >> 12) /1024);

    for(int j = (minimum >> 12) / 1024;
            j < 1024 && j < (maximum >> 12) / 1024;
            j++) {
        uart_printf("[find] pd[%d] (%x)\r\n", j, pd[j]);
        if (pd[j] & PAGE_PRESENT) {
            uart_printf("[find] page table present\r\n");
            uint32_t *current_pt = (uint32_t*)(minimum + clear_flags(pd[j]));
            uart_printf("[find] current pt: %x\r\n", current_pt);
            for(int i = 0; i < 1024; i++) {
                //uart_printf("[find] pt[%d] (%x)\r\n", i, current_pt[i]);
                if (current_pt[i] & PAGE_PRESENT) {
                    //uart_printf("[find] pt[%d] is present, resetting\r\n", i);
                    consecutive = 0;
                    first_consecutive = 0;
                } else {
                    uart_printf("[find] pt[%d] is not present\r\n", i);
                    consecutive++;

                    if (first_consecutive == 0) {
                        uart_printf("[find] pt: first_consecutive: %x ->",
                                first_consecutive);

                        first_consecutive = (uint32_t*)((j * 1024 * 4096)
                                + (i * 4096));

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
                uart_printf("[find] pd: first_consecutive: %x ->",
                        first_consecutive);

                first_consecutive = (uint32_t*)(j << 12);
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
    uart_printf("allocating %x pages\r\n", num_pages);
    virtaddr_t virt = vmm_find_free_area(context, 0x0, 0xBFFFFFFF, num_pages);
    uart_printf("found free area at %x\r\n", virt);

    if(virt == 0) {
        return 0;
    }

    virt = vmm_map_consecutive(context, virt, num_pages);

    uart_printf("%s: return %x\r\n", __func__, virt);

    return virt;
}

void* vmm_alloc(struct vmm_context *context, uint32_t size)
{
    uint32_t num_pages = (size-1)/ 4096 + 1;
    return vmm_alloc_pages(context, num_pages);
}

void* vmm_kalloc_pages(uint32_t num_pages)
{
    uart_printf("allocating %x pages (kernel)\r\n", num_pages);
    uart_printf("active context: %x\r\n", active_context);
    virtaddr_t virt = vmm_find_free_area(active_context,
            0xC0000000, 0xFFFFFFFF, num_pages);
    uart_printf("found free area at %x\r\n", virt);

    if(virt == 0) {
        return 0;
    }

    vmm_map_consecutive(active_context, virt, num_pages);

    return virt;
}

void* vmm_kalloc(uint32_t size)
{
    uint32_t num_pages = (size-1)/4096 + 1;
    return vmm_kalloc_pages(num_pages);
}

uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt,
        uintptr_t phys)
{
    uart_printf("vmm_map_page: %x -> %x\r\n", phys, virt);
    uint32_t page_number = (uint32_t)virt / 4096;
    uint32_t pd_index = page_number / 1024;
    uint32_t pt_index = page_number % 1024;

    uart_printf("pagenum %x pd_index %x pt_index %x\r\n", page_number, pd_index,
            pt_index);

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

        directory_put(context, pd_index,
                (uint32_t)page_table, PAGE_PRESENT | PAGE_WRITE);

        invalidate_tlb(context->page_directory);
        uart_puts("after directory_put\r\n");
    }


    virtaddr_t page_table_virt;
    virtaddr_t tmp;

    uart_printf("active_context = %x, context = %x\r\n",
            active_context, context);

    if (context == active_context || context == &tmp_kernel_context) {
        page_table_virt = (uint32_t*)(0xC0000000 + clear_flags(page_table));
    } else {
        /* we have to temporarily map it into the kernel space */
        uart_printf("temporarily mapping...\r\n");
        tmp = vmm_find_free_area(active_context, 0xC0000000, 0xFFFFFFFF, 1);
        uart_printf("... to %x\r\n", tmp);
        vmm_map_page(active_context, tmp, clear_flags(page_table));
    }

    uart_printf("page_table_virt = %x\r\n", page_table_virt);

    table_put(page_table_virt, pt_index, phys, PAGE_PRESENT | PAGE_WRITE);

    if (context != active_context) {
        vmm_unmap_page(active_context, tmp);
    }
    uart_puts("after table_put\r\n");
    /* invalidate tlb cache */
    invalidate_tlb(page_table_virt);


    uart_puts("after invalidate\r\n");

    return ERR_OK;
}

void vmm_free(virtaddr_t addr)
{
    /* TODO */
}

uint32_t vmm_init(uint32_t *kernel_pagedir)
{
    /* bootstrap kernel context */
    tmp_kernel_context.page_directory = kernel_pagedir;
    uart_printf("kernel_pagedir = %x\r\n", kernel_pagedir);


    active_context = vmm_find_free_area(&tmp_kernel_context,
            0xC0000000, 0xFFFFFFFF, 1);

    if(active_context == 0) {
        panic("no more space at vmm init");
    }

    active_context = vmm_map_consecutive(&tmp_kernel_context,
            active_context, 1);

    uart_printf("active_context = %x\r\n", active_context);
    active_context->page_directory = kernel_pagedir;

    return ERR_OK;
}
