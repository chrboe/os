#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include "util.h"
#include "pmm.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

#define KERNEL_PAGE_TABLES_VADDR 0x3fc00000
#define PGDIR_SHIFT 22

struct vmm_context {
    uint32_t *page_directory;
};

struct vmm_context *vmm_create_context();
uintptr_t vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages);
uintptr_t vmm_alloc(struct vmm_context *context, uint32_t bytes);
uint32_t vmm_map_page(struct vmm_context *context, uintptr_t virt, uintptr_t phys);
uint32_t init_paging();

#endif // MMU_H