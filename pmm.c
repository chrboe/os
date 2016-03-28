#include "pmm.h"

static uint32_t page_free_bits[NUM_PAGES];
extern const void kernel_start;
extern const void kernel_end;

void* pmm_alloc()
{
	for(int i = 0; i < NUM_PAGES; i++) {
		for(int j = 0; j < 32; j++) {
			if(page_free_bits[i] & 1<<j) {
				page_free_bits[i] &= ~(1 << j);
				return (void*)((i * 32 + j) * 4096);
			}
		}
	}
	return 0;
}

void pmm_free(void* p)
{
	uint32_t i = (uint32_t)p / 4096;
	page_free_bits[i/32] |= 1<<(i%32);
}

void pmm_init(struct multiboot_structure* mb_struc)
{
	struct multiboot_mmap* mmap = mb_struc->mmap_addr;
	struct multiboot_mmap* mmap_end = (void*)(mb_struc->mmap_length + mb_struc->mmap_length);
	
	while(mmap < mmap_end) {
		if(mmap->type == 1) {
			/* mem is free */
			uintptr_t base = (uintptr_t)mmap->baseaddr;
			uintptr_t end = (uintptr_t)(base + mmap->length);
			while(base < end) {
				pmm_free((void*)base);
				base += 0x100;
			}
		}
		mmap++;
	}

	uintptr_t kern_base = (uintptr_t)&kernel_start;
	while(kern_base < (uintptr_t)&kernel_end) {
		uintptr_t index = (uintptr_t)kern_base / 4096;
		page_free_bits[index/32] |= (1<<(index%32));
		kern_base += 0x100;
	}
}
