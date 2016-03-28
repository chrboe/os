#ifndef TASK_H
#define TASK_H

#include "console.h"
#include "idt.h"

struct stackframe* schedule(struct stackframe* frame);
void init_multitasking();
void task_a();
void task_b();

#endif /* !TASK_H */
