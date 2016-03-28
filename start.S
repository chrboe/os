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
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

/*
 * this part actually builds the multiboot header.
 */
.align 4
.int MB_MAGIC
.int MB_FLAGS
.int MB_CHECKSUM

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
.global _start

/*
 * THIS IS OUR ACTUAL ENTRY POINT.
 * when GRUB hands control over to us, this gets executed.
 */
_start:
	/*
	 * first, we initialize our kernel stack (see below for definition) by
	 * setting the stack pointer to its address.
	 */
	mov $kernel_stack, %esp

	/*
	 * then, we call our init function, which is defined in init.c.
	 */
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

/*
 * here, we define our (relatively small) kernel stack (8k).
 * note that the label is after the free space. this is because the stack grows
 * downwards.
 */
.section bss
.space 8192
kernel_stack:
