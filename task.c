#include "task.h"

static struct task* first_task = 0;
static struct task* current_task = 0;

void task_a()
{
    while(1) {
        kprintf(COL_NOR, "a\r\n");
    }
}

void task_b()
{
    while(1) {
        kprintf(COL_NOR, "b\r\n");
    }
}

struct task* init_task(void* func)
{
    struct task* task = pmm_alloc();
    char* stack = pmm_alloc();
    char* ustack  = pmm_alloc();

    struct stackframe new_state;
    new_state.eax = 0;
    new_state.ebx = 0;
    new_state.ecx = 0;
    new_state.edx = 0;
    new_state.esp = (uint32_t)(ustack + 4096);
    new_state.eip = (uint32_t)func;
    new_state.cs = 0x18 | 0x03;
    new_state.ss = 0x20 | 0x03;
    new_state.eflags = 0x202;

    struct stackframe* current_state = (void*)(stack + 4096 - sizeof(new_state));
    *current_state = new_state;

    task->frame = current_state;
    task->next = first_task;
    first_task = task;

    return task;
}

void init_multitasking()
{
    init_task(task_a);
    init_task(task_b);
}

struct stackframe* schedule(struct stackframe* frame)
{
    if(first_task == 0) {
        return frame;
    }

    if(current_task == 0) {
        current_task = first_task;
    }

    if(current_task->next == 0) {
        current_task = first_task;
    } else {
        current_task = current_task->next;
    }

    return current_task->frame;
}
