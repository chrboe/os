/* define _start as the entry point for the kernel */
ENTRY(_start)

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
    . = 0x100000;

	kernel_start = .;

    /*
     * this ensures that the multiboot header is first in the binary.
     */
    .text : {
        *(multiboot)
        *(.text)
    }

    /*
     * all the other sections are after multiboot, the order doesn't 
     * really matter here.
     */
    .data ALIGN(4096) : {
        *(.data)
    }

    .rodata ALIGN(4096) : {
        *(.rodata)
    }

    .bss ALIGN(4096) : {
        *(.bss)
    }

	. = ALIGN(4096);
	kernel_end = .;
}
