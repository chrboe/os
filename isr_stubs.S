.macro interrupt_stub n
.global interrupt_stub_\n
interrupt_stub_\n:
	pushl $0
	pushl $\n
	jmp isr_common
.endm

.macro interrupt_stub_error n
.global interrupt_stub_\n
interrupt_stub_\n:
	pushl $\n
	jmp isr_common
.endm

/* exceptions */
interrupt_stub 0
interrupt_stub 1
interrupt_stub 2
interrupt_stub 3
interrupt_stub 4
interrupt_stub 5
interrupt_stub 6
interrupt_stub 7
interrupt_stub_error 8
interrupt_stub 9
interrupt_stub_error 10
interrupt_stub_error 11
interrupt_stub_error 12
interrupt_stub_error 13
interrupt_stub_error 14
interrupt_stub 15
interrupt_stub 16
interrupt_stub_error 17
interrupt_stub 18
interrupt_stub 19

/* IRQs (we remapped these in idt.c) */
interrupt_stub 32
interrupt_stub 33
interrupt_stub 34
interrupt_stub 35
interrupt_stub 36
interrupt_stub 37
interrupt_stub 38
interrupt_stub 39
interrupt_stub 40
interrupt_stub 41
interrupt_stub 42
interrupt_stub 43
interrupt_stub 44
interrupt_stub 45
interrupt_stub 46
interrupt_stub 47

/* system call */
interrupt_stub 48


/*
 * this is the common routine for all
 * ISRs
 */
isr_common:
	/*
	 * first, save all general purpose 
	 * registers to stack
	 */
	push %ebp
	push %edi
	push %esi
	push %edx
	push %ecx
	push %ebx
	push %eax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es

	push %esp
	call isr_handler_common
	mov %eax, %esp

	/*
	 * in the end, retrieve the general
	 * purpose registers that we saved
	 */
	pop %eax
	pop %ebx
	pop %ecx
	pop %edx
	pop %esi
	pop %edi
	pop %ebp

	/*
	 * remove the interrupt number and
	 * error code from the stack
	 */
	add $8, %esp
	iret
