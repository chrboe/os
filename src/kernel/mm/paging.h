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
typedef void* virtaddr_t;

struct vmm_context {
    uint32_t *page_directory;
};

struct page_directory {

} __attribute__ ((packed));

struct page_table {

} __attribute__ ((packed));



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
    uart_printf("directory put %x to %x (index %d)", table, context->page_directory + index, index);
    context->page_directory[index] = clear_flags(table) | flags;
}

static inline void invalidate_tlb(virtaddr_t addr)
{
    asm volatile("invlpg (%0)" : : "b"(addr) : "memory");
}


struct vmm_context *vmm_create_context();
void* vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages);
void* vmm_alloc(struct vmm_context *context, uint32_t bytes);
uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt, physaddr_t phys);
uint32_t init_paging();

#endif // PAGING_H