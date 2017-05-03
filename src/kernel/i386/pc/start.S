.section multiboot
/*
 * these are the flags for the multiboot header.
 * multiboot is needed because GRUB demands it.
 * it is a standard for well-defining the state of a kernel
 * after the bootloader is done loading it.
 *
 * documentation: 
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.pdf
 */
#define MB_MAGIC 0x1badb002
#define MB_FLAGS 0x3
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

/*
 * this part actually builds the multiboot header.
 */
.align 4
.int MB_MAGIC
.int MB_FLAGS
.int MB_CHECKSUM

#define KERNEL_VIRTUAL_BASE  0xC0000000
#define KERNEL_PAGE_NUMBER   (KERNEL_VIRTUAL_BASE >> 22)

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.section .bss, "aw", @nobits
	.align 4096
boot_pagedir:
	.skip 4096
boot_pagetable:
	.skip 4096

.section .text

/*
 * this extern statement is needed because init is defined in init.c, not here.
 * (see below for usage of this function)
 */
.extern init

/*
 * this line defines the _start label as global.
 * this is needed in order for the linker to find it, as non-global
 * labels are only visible in the current file.
 * (this behaviour is similar to static/non-static functions in C)
 */
.global _loader
.type _loader, @function

/*
 * this routine loads the kernel to the proper virtual address and
 * then jumps to the entry point
 */
_loader:
	movl $(boot_pagetable - 0xC0000000), %edi
	movl $0, %esi
	movl $1023, %ecx
1:
	/* Only map the kernel. */
	cmpl $(kernel_start - 0xC0000000), %esi
	jl 2f
	cmpl $(kernel_end - 0xC0000000), %esi
	jge 3f

	/*
	 * Map physical address as "present, writable". Note that this maps
	 * .text and .rodata as writable. Mind security and map them as non-writable.
	 */
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	/* Size of page is 4096 bytes. */
	addl $4096, %esi
	/* Size of entries in boot_pagetab1 is 4 bytes. */
	addl $4, %edi
	/* Loop to the next entry if we haven't finished. */
	loop 1b

3:
	/* Map VGA video memory to 0xC03FF000 as "present, writable". */
	movl $(0x000B8000 | 0x003), boot_pagetable - 0xC0000000 + 1023 * 4

	/*
	 * The page table is used at both page directory entry 0 (virtually from 0x0
	 * to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	 * 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	 * higher half). The kernel is identity mapped because enabling paging does
	 * not change the next instruction, which continues to be physical. The CPU
	 * would instead page fault if there was no identity mapping.
	 */

	/* Map the page table to both virtual addresses 0x00000000 and 0xC0000000. */
	movl $(boot_pagetable - 0xC0000000 + 0x003), boot_pagedir - 0xC0000000 + 0
	movl $(boot_pagetable - 0xC0000000 + 0x003), boot_pagedir - 0xC0000000 + 768 * 4

	/* Set cr3 to the address of the boot_page_directory. */
	movl $(boot_pagedir - 0xC0000000), %ecx
	movl %ecx, %cr3

	/* Enable paging and the write-protect bit. */
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	/* Jump to higher half with an absolute jump. */
	lea _start, %ecx
	jmp *%ecx

/*
 * THIS IS OUR ACTUAL ENTRY POINT.
 * when GRUB hands control over to us, this gets executed.
 */
_start:
    /* unmap the bootstrapping identity page */
	movl $0, boot_pagedir + 0
	movl %cr3, %ecx
	movl %ecx, %cr3

	/*
	 * first, we initialize our kernel stack (see below for definition) by
	 * setting the stack pointer to its address.
	 */
	mov $stack_top, %esp

	/*
	 * then, we call our init function, which is defined in init.c.
	 */
	add  $KERNEL_VIRTUAL_BASE, %ebx
	push %ebx
	call init

	/*
	 * when (if) our init function returns, that means the OS has been halted.
	 * therefore, we just turn off interrupts and send the processor into an
	 * endless loop.
	 * NOTE: the hlt statement causes the processor to only do work when it has
	 * to (i.e. it suspends the CPU). this eliminates the problem of having
	 * 100% CPU usage and extreme power consumption when the OS is done running.
	 */
	cli
_stop:
	hlt
	jmp _stop