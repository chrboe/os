#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#include "init.h"

#define NUM_PAGES 32768

uint32_t page_free_bits[NUM_PAGES];

uintptr_t pmm_alloc();
void pmm_free(uintptr_t);
void pmm_init(struct multiboot_structure*);

#endif /* !PMM_H */
