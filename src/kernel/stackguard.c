#include "stackguard.h"

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    kputs(COL_CRI, "Stack smashing detected");
    asm volatile("int $13");
}