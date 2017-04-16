#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#include "init.h"

#define NUM_PAGES 32768

void* pmm_alloc();
void pmm_free(void*);
void pmm_init(struct multiboot_structure*);

#endif /* !PMM_H */
