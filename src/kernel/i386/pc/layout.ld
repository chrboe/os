/* define _start as the entry point for the kernel */
ENTRY(_loader)

/*
 * this section of the file tells the linker where to put the different
 * sections in the binary.
 * this is important because the multiboot sector needs to be first
 * in the binary, and this order is normally not guaranteed.
 */
SECTIONS
{
    /*
     * . defines the offset at which to start. because we want to load our
     * kernel to 0x100000, we set it to that.
     */
	. = 0xC0100000;

	kernel_start = .;

    /*
     * this ensures that the multiboot header is first in the binary.
     */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	kernel_end = .;
}
