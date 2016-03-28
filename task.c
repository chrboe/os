#include "task.h"

static char stack_a[4096];
static char stack_b[4096];

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

struct stackframe* init_task(char* stack, void* func)
{
    struct stackframe new_state;
    new_state.eax = 0;
    new_state.ebx = 0;
    new_state.ecx = 0;
    new_state.edx = 0;
    new_state.eip = (uint32_t)func;
    new_state.cs = 0x08;
    new_state.eflags = 0x202;

    struct stackframe* current_state = (void*)(stack + 4096 - sizeof(new_state));
    *current_state = new_state;

    kprintf(COL_NOR, "setting new cpu to eip=%d\r\n", new_state.eip);
    return current_state;
}

static int current_task = -1;
static int num_tasks = 2;
static struct stackframe* tasks[2];
void init_multitasking()
{
    tasks[0] = init_task(stack_a, task_a);
    tasks[1] = init_task(stack_b, task_b);
}

struct stackframe* schedule(struct stackframe* frame)
{
    if(current_task >= 0) {
        tasks[current_task] = frame;
    }

    current_task++;
    current_task %= num_tasks;

    return frame = tasks[current_task];
}
