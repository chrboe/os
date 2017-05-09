#ifndef TASK_H
#define TASK_H

#include "console.h"
#include "idt.h"
#include "mm/pmm.h"

struct task {
	struct stackframe *frame;
	struct task *next;
	struct vmm_context *context;
};

struct stackframe* schedule(struct stackframe* frame);
void init_multitasking();
void task_a();
void task_b();

#endif /* !TASK_H */
