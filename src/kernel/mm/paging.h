#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "util.h"
#include "mm/pmm.h"
#include "mm/mmap.h"
#include "init.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

typedef uintptr_t physaddr_t;
typedef void * virtaddr_t;

struct vmm_context {
    uint32_t *page_directory;
};

struct vmm_context *vmm_create_context();
uintptr_t vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages);
uintptr_t vmm_alloc(struct vmm_context *context, uint32_t bytes);
uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt, physaddr_t phys);
uint32_t init_paging();

#endif // PAGING_H