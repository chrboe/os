#ifndef IDT_H
#define IDT_H

#include <stdint.h> /* for uintX_t */

#include "console.h" /* for kprintf */

#define IDT_ENTRIES 256

void setup_idt();
void load_idt();

#endif /* !IDT_H */
