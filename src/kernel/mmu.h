#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include "util.h"
#include "pmm.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

struct vmm_context {
    uint32_t *page_directory;
};

/* 0 for 4KiB pages, 1 for 4MiB */
#define PAGING_PAGE_SIZE 0

uint32_t init_paging();

#endif // MMU_H