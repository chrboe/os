        .text
        .global arch_do_context_switch
        .type arch_do_context_switch, @function

# void arch_do_context_switch(uint32_t esp, uint32_t pagedir, uint32_t *old_esp);

arch_do_context_switch:
	pusha                         # Save caller state
	movl    %esp         , %ebp    # Set base pointer
	movl    0x20(%ebp)      , %eax    # eax=param0
	movl    0x28(%ebp)      , %ecx    # ecx=param1
	movl    0x30(%ebp)     , %edx    # edx=param2
        a: jmp a
	movl    %esp         , (%edx)  # *param2 = esp
	movl    %eax         , %esp    # esp = param0
	movl    %ecx         , %cr3    # cr3 = param1
	popa                          # Restore caller state
	ret                           # Return
