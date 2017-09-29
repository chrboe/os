#include "task.h"

static struct task* first_task = 0;
static struct task* current_task = 0;

void task_a()
{
    while(1) {

        asm volatile("movl $1, %eax");
        asm volatile("int $48");
        uart_puts("task a\r\n");
    }
}

void task_b()
{
    asm volatile("movl $2, %eax");
    while(1) {
        asm volatile("int $48");
        uart_puts("task b\r\n");
    }
}

void task_c()
{
    asm volatile("movl $3, %eax");
    while(1) {
        asm volatile("int $48");
        //uart_puts("task c\r\n");
    }
}

void task_d()
{
    asm volatile("movl $4, %eax");
    while(1) {
        asm volatile("int $48");
        //uart_puts("task d\r\n");
    }
}

struct task* init_task(void* func)
{
    uint8_t* stack = vmm_kalloc_pages(1);
    uint8_t* user_stack = vmm_kalloc_pages(1);

    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    struct stackframe new_frame = {
            .eax = 0,
            .ebx = 0,
            .ecx = 0,
            .edx = 0,
            .esi = 0,
            .edi = 0,
            .ebp = 0,
            .esp = (uint32_t) user_stack + 4096,
            .eip = (uint32_t) func,

            /* Ring-3-Segmentregister */
            .cs  = 0x18 | 0x03,
            .ss  = 0x20 | 0x03,

            /* IRQs einschalten (IF = 1) */
            .eflags = 0x200,
    };

    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    struct stackframe* frame = (struct stackframe*)
        (stack + 4096 - sizeof(new_frame));

    *frame = new_frame;

    /*
     * Neue Taskstruktur anlegen und in die Liste einhaengen
     */
    struct task *task = vmm_kalloc_pages(1);
    task->frame = frame;
    task->next = first_task;
    uart_printf("allocating context\r\n");
    task->context = vmm_create_context();
    first_task = task;

    uart_printf("task: 0x%x\r\nfirst: 0x%x\r\ncurrent: 0x%x\r\n",
            task, first_task, current_task);
    return task;
}

void init_multitasking()
{
    asm("cli");
    init_task(task_a);
    init_task(task_b);
    init_task(task_c);
    init_task(task_d);
    uart_printf("TASK INIT DONE");
    asm("sti");
}

void arch_do_context_switch(uint32_t esp, uint32_t pagedir, uint32_t *old_esp);

static void switch_task(struct task *const new_task)
{
    asm("cli");
    uart_printf("context switch\r\n");
    uart_printf("new context = %x\r\n", new_task->context);
    uart_printf("new pagedir = %x\r\n", new_task->context->page_directory);
    physaddr_t pagedir_phys = vmm_kresolve(new_task->context->page_directory);
    uart_printf("new pagedir_phys = %x\r\n", pagedir_phys);
    uart_printf("saving frames\r\n");

    struct stackframe *curr_frame = current_task->frame;
    uart_printf("saved current frame\r\n");
    struct stackframe *new_frame = new_task->frame;
    uart_printf("saved new frame\r\n");

    uart_printf("setting kernel context to %x...", new_task->context);
    active_context = new_task->context;
    uart_printf("done!\r\n");
    current_task = new_task;

    arch_do_context_switch(new_frame->esp, pagedir_phys, &curr_frame->esp);
    asm("sti");
}

static struct task *choose_next_task()
{
    struct task *next_task;
    if (current_task == 0) {
        uart_printf("this is the first scheduler call\r\n");
        /* we are just starting out */
        next_task = first_task;
    } else {
        uart_printf("using next task (%x) %s\r\n", current_task->next,
                current_task->next == first_task ? "(== first)" : "");

        next_task = current_task->next;
        if(current_task == 0) {
            uart_printf("this was the last entry\r\n");
            /* this was the last entry */
            next_task = first_task;
        }
    }

    uart_printf("chose %x\r\n", next_task);
    return next_task;
}

struct stackframe* schedule(struct stackframe* frame)
{
    uart_puts("schedule\r\n");
    uart_printf("first_task: 0x%x\r\ncurrent_task: 0x%x\r\n", first_task,
            current_task);
    struct task *orig_task = current_task;

    if (first_task == 0) {
        /* we have no tasks */
        uart_printf("no tasks\r\n");
        return frame;
    }

    if (current_task != 0) {
        /* save the stackframe */
        current_task->frame = frame;
    }

    /* choose next task */
    struct task *next_task = choose_next_task();

    /* switch context if necessary */
    if (next_task != current_task && next_task != 0) {
        switch_task(next_task);
    }

    //frame = current_task->frame;
    return current_task->frame;
}
