#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_ENTRIES 5

void setup_gdt();
void load_gdt();
void reload_segment_registers();

#endif /* !GDT_H */
