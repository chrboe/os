#ifndef IDT_H
#define IDT_H

#include <stdint.h> /* for uintX_t */

#include "keyboard.h"
#include "console.h" /* for kprintf */

#define IDT_ENTRIES 256

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_MASTER_IMR 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_SLAVE_IMR 0xA1

#define EOI 0x20

struct stackframe
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;

	uint32_t interrupt;
	uint32_t error;

	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint32_t ss;
};

void interrupt_stub_0();
void interrupt_stub_1();
void interrupt_stub_2();
void interrupt_stub_3();
void interrupt_stub_4();
void interrupt_stub_5();
void interrupt_stub_6();
void interrupt_stub_7();
void interrupt_stub_8();
void interrupt_stub_9();
void interrupt_stub_10();
void interrupt_stub_11();
void interrupt_stub_12();
void interrupt_stub_13();
void interrupt_stub_14();
void interrupt_stub_15();
void interrupt_stub_16();
void interrupt_stub_17();
void interrupt_stub_18();

void interrupt_stub_32();
void interrupt_stub_33();
void interrupt_stub_34();
void interrupt_stub_35();
void interrupt_stub_36();
void interrupt_stub_37();
void interrupt_stub_38();
void interrupt_stub_39();
void interrupt_stub_40();
void interrupt_stub_41();
void interrupt_stub_42();
void interrupt_stub_43();
void interrupt_stub_44();
void interrupt_stub_45();
void interrupt_stub_46();
void interrupt_stub_47();

void interrupt_stub_48();

void pic_remap_irqs(int num);
void pic_apply_irq_mask(uint16_t mask);
void setup_idt();
void load_idt();

#endif /* !IDT_H */
