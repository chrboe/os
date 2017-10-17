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

extern struct vmm_context *kernel_context;

static inline uint32_t clear_flags(uint32_t entry)
{
    return entry & ~0xFFF;
}

static inline void table_put(uint32_t *table, uint32_t index,
        uint32_t entry, uint32_t flags)
{
    table[index] = clear_flags(entry) | flags;
}

static inline void directory_put(struct vmm_context *context,
        uint32_t index, uint32_t table, uint32_t flags)
{
    context->page_directory[index] = clear_flags(table) | flags;
}

static inline void invalidate_tlb(virtaddr_t addr)
{
    asm volatile("invlpg (%0)" : : "b"(addr) : "memory");
}


struct vmm_context *vmm_create_context();
void* vmm_alloc_pages(struct vmm_context *context, uint32_t num_pages);
void* vmm_alloc(struct vmm_context *context, uint32_t bytes);
void* vmm_kalloc_pages(uint32_t num_pages);
void* vmm_kalloc(uint32_t bytes);
void  vmm_free(virtaddr_t addr);
uint32_t vmm_map_page(struct vmm_context *context, virtaddr_t virt,
        physaddr_t phys);
uint32_t vmm_init(uint32_t *kernel_pagedir);
physaddr_t vmm_resolve(virtaddr_t addr);
void* vmm_find_free_area(struct vmm_context *context, uintptr_t minimum,
        uintptr_t maximum, uint32_t num_pages);

void vmm_switch_context(struct vmm_context *context);

#endif // PAGING_H
