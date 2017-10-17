        .text
        .global arch_do_context_switch
        .type arch_do_context_switch, @function

# void arch_do_context_switch(uint32_t esp, uint32_t *old_esp);

arch_do_context_switch:
        pusha                         # Save caller state
        movl    %esp         , %ebp    # Set base pointer
        movl    0x24(%ebp)   , %eax    # eax=param0
        movl    0x28(%ebp)   , %ecx    # ecx=param1
        movl    %esp         , (%ecx)  # *param2 = esp
        movl    %eax         , %esp    # esp = param0
        popa                          # Restore caller state
        ret                           # Return
