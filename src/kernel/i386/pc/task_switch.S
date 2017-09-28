.section text
.global arch_do_context_switch
.type arch_do_context_switch, @function
arch_do_context_switch:
	pusha                         # Save caller state
	mov    %esp         , %ebp    # Set base pointer
	mov    0x24(%ebp)   , %eax    # eax=param0
	mov    0x28(%ebp)   , %ecx    # ecx=param1
	mov    0x2C(%ebp)   , %edx    # edx=param2
	mov    %esp         , (%edx)  # *param2 = esp
	mov    %eax         , %esp    # esp = param0
	mov    %ecx         , %cr3    # cr3 = param1
	popa                          # Restore caller state
	ret                           # Return
