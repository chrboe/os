#include "idt.h"

static uint64_t idt[IDT_ENTRIES];

static uint64_t build_idt_entry(void (*f)(), uint32_t sel, uint32_t flags)
{
    uint64_t entry;
    uint32_t handler = (uint32_t)f;
    entry  = handler & 0xFFFFLL;
    entry |= (sel & 0xFFFFLL) << 16;
    entry |= (flags & 0xFFLL) << 40;
    entry |= ((handler >> 16) & 0xFFFFLL) << 48;
    return entry;
}

void setup_idt()
{
    pic_remap_irqs(32);
    pic_apply_irq_mask(0);
    idt[0] = build_idt_entry(interrupt_stub_0, 8, 0x80 | 0x00 | 0xe);
    idt[1] = build_idt_entry(interrupt_stub_1, 8, 0x80 | 0x00 | 0xe);
    idt[2] = build_idt_entry(interrupt_stub_2, 8, 0x80 | 0x00 | 0xe);
    idt[3] = build_idt_entry(interrupt_stub_3, 8, 0x80 | 0x00 | 0xe);
    idt[4] = build_idt_entry(interrupt_stub_4, 8, 0x80 | 0x00 | 0xe);
    idt[5] = build_idt_entry(interrupt_stub_5, 8, 0x80 | 0x00 | 0xe);
    idt[6] = build_idt_entry(interrupt_stub_6, 8, 0x80 | 0x00 | 0xe);
    idt[7] = build_idt_entry(interrupt_stub_7, 8, 0x80 | 0x00 | 0xe);
    idt[8] = build_idt_entry(interrupt_stub_8, 8, 0x80 | 0x00 | 0xe);
    idt[9] = build_idt_entry(interrupt_stub_9, 8, 0x80 | 0x00 | 0xe);
    idt[10] = build_idt_entry(interrupt_stub_10, 8, 0x80 | 0x00 | 0xe);
    idt[11] = build_idt_entry(interrupt_stub_11, 8, 0x80 | 0x00 | 0xe);
    idt[12] = build_idt_entry(interrupt_stub_12, 8, 0x80 | 0x00 | 0xe);
    idt[13] = build_idt_entry(interrupt_stub_13, 8, 0x80 | 0x00 | 0xe);
    idt[14] = build_idt_entry(interrupt_stub_14, 8, 0x80 | 0x00 | 0xe);
    idt[15] = build_idt_entry(interrupt_stub_15, 8, 0x80 | 0x00 | 0xe);
    idt[16] = build_idt_entry(interrupt_stub_16, 8, 0x80 | 0x00 | 0xe);
    idt[17] = build_idt_entry(interrupt_stub_17, 8, 0x80 | 0x00 | 0xe);
    idt[18] = build_idt_entry(interrupt_stub_18, 8, 0x80 | 0x00 | 0xe);

    idt[32] = build_idt_entry(interrupt_stub_32, 8, 0x80 | 0x00 | 0xe);
    idt[33] = build_idt_entry(interrupt_stub_33, 8, 0x80 | 0x00 | 0xe);
    idt[34] = build_idt_entry(interrupt_stub_34, 8, 0x80 | 0x00 | 0xe);
    idt[35] = build_idt_entry(interrupt_stub_35, 8, 0x80 | 0x00 | 0xe);
    idt[36] = build_idt_entry(interrupt_stub_36, 8, 0x80 | 0x00 | 0xe);
    idt[37] = build_idt_entry(interrupt_stub_37, 8, 0x80 | 0x00 | 0xe);
    idt[38] = build_idt_entry(interrupt_stub_38, 8, 0x80 | 0x00 | 0xe);
    idt[39] = build_idt_entry(interrupt_stub_39, 8, 0x80 | 0x00 | 0xe);
    idt[30] = build_idt_entry(interrupt_stub_40, 8, 0x80 | 0x00 | 0xe);
    idt[41] = build_idt_entry(interrupt_stub_41, 8, 0x80 | 0x00 | 0xe);
    idt[42] = build_idt_entry(interrupt_stub_42, 8, 0x80 | 0x00 | 0xe);
    idt[43] = build_idt_entry(interrupt_stub_43, 8, 0x80 | 0x00 | 0xe);
    idt[44] = build_idt_entry(interrupt_stub_44, 8, 0x80 | 0x00 | 0xe);
    idt[45] = build_idt_entry(interrupt_stub_45, 8, 0x80 | 0x00 | 0xe);
    idt[46] = build_idt_entry(interrupt_stub_46, 8, 0x80 | 0x00 | 0xe);
    idt[47] = build_idt_entry(interrupt_stub_47, 8, 0x80 | 0x00 | 0xe);

    idt[48] = build_idt_entry(interrupt_stub_48, 8, 0x80 | 0x60 | 0xe);
}

void load_idt()
{
    struct {
        uint16_t limit;
        void* pointer;
    } __attribute__((packed)) idtp = {
        .limit = IDT_ENTRIES * 8 - 1,
        .pointer = idt,
    };

    asm volatile("lidt %0" : : "m" (idtp));
    asm volatile("sti");
}

void pic_remap_irqs(int intnum)
{
    /*
     * send ICW1:
     * 0x01 = we will send ICW4 later
     * 0x10 = actual initialization command
     */
    outb(PIC_MASTER_COMMAND, 0x10 | 0x01);
    outb(PIC_SLAVE_COMMAND, 0x10 | 0x01);

    /*
     * now we actually remap the IRQs (this is ICW2)
     */
    outb(PIC_MASTER_DATA, intnum);
    outb(PIC_SLAVE_DATA, intnum+8);

    /*
     * ICW3: since there are two PICs, they need
     * to communicate. this ICW tells them what
     * IRQ to use for communication.
     * in this case IRQ2 is used.
     */
    outb(PIC_MASTER_DATA, 0x04);
    outb(PIC_SLAVE_DATA, 2);

    /*
     * ICW4: this tells the PIC that we are in
     * 8086 mode (which basically means we're a PC).
     */
    outb(PIC_MASTER_DATA, 0x01);
    outb(PIC_SLAVE_DATA, 0x01);
}

/*
 * pic_apply_irq_mask enables the ability to mask
 * and demask IRQs. if a bit in "mask" is set,
 * the IRQ is masked (disabled). if the bit is
 * unset, the interrupt is active.
 */
void pic_apply_irq_mask(uint16_t mask)
{
    outb(PIC_MASTER_IMR, (uint8_t)mask);
    outb(PIC_SLAVE_IMR, (uint8_t)(mask>>8));
}

/*
 * pic_send_eoi sends an EOI (end of interrupt) to
 * one or both PICs. this is needed so the PICs
 * disable the IRQs after the ISR is done.
 */
void pic_send_eoi(uint8_t irq)
{
    outb(PIC_MASTER_COMMAND, EOI);

    /*
     * only send the EOI to the slave controller
     * if it is affected by it (i.e. if the IRQ
     * is above 7)
     */
    if(irq > 7) {
        outb(PIC_SLAVE_COMMAND, EOI);
    }
}


static uint8_t nmi_disable_bit = 0;

void enable_nmi()
{
    nmi_disable_bit = 0;
    outb(0x70, nmi_disable_bit<<7);
}

void disable_nmi()
{
    nmi_disable_bit = 1;
    outb(0x70, nmi_disable_bit<<7);
}

uint8_t is_nmi_disabled()
{
    return nmi_disable_bit;
}

static void abort()
{

    kprintf(COL_CRI, "Aborting Kernel.");
    while(1) {
        asm volatile("cli; hlt");
    }
}

static void handle_page_fault(struct stackframe *frame)
{
    kprintf(COL_CRI, "Page Fault!\r\n");
    kprintf(COL_ERR, "Error: 0x%x\r\n", frame->error);
    switch(frame->error & 0x7) {
        case 0: kprintf(COL_NOR, "Supervisory process tried to read a non-present page entry\r\n"); break;
        case 1: kprintf(COL_NOR, "Supervisory process tried to read a page and caused a protection fault\r\n"); break;
        case 2: kprintf(COL_NOR, "Supervisory process tried to write to a non-present page entry\r\n"); break;
        case 3: kprintf(COL_NOR, "Supervisory process tried to write a page and caused a protection fault\r\n"); break;
        case 4: kprintf(COL_NOR, "User process tried to read a non-present page entry\r\n"); break;
        case 5: kprintf(COL_NOR, "User process tried to read a page and caused a protection fault\r\n"); break;
        case 6: kprintf(COL_NOR, "User process tried to write to a non-present page entry\r\n"); break;
        case 7: kprintf(COL_NOR, "User process tried to write a page and caused a protection fault\r\n"); break;
    }

    abort();
}

static void handle_exception(struct stackframe* frame)
{
    if (frame->interrupt == 14) {
        return handle_page_fault(frame);
    }

    kprintf(COL_ERR, "The following exception just occurred:\r\n");
    switch(frame->interrupt) {
        case 0:
            kprintf(COL_CRI, "Division by Zero");
            break;
        case 13:
            kprintf(COL_CRI, "General Protection Fault");
            break;
        default:
            kprintf(COL_CRI, "(Unknown)");
            break;
    }
    kprintf(COL_CRI, " (%d)\r\n\r\n", frame->interrupt);

	dump_frame(frame);

    abort();
}

static struct stackframe* handle_irq(struct stackframe* frame)
{
    struct stackframe* new_frame = frame;
    switch(frame->interrupt) {
        case 32:
            new_frame = schedule(frame);
            tss[1] = (uint32_t)(new_frame + 1);
            break;

        case 33:;
            /* keyboard */
            uint8_t scancode = inb(0x60);
            process_keyboard(scancode);

            if(is_key_pressed(KEY_LCTRL) && is_key_pressed(KEY_LALT) && is_key_pressed(KEY_DELETE1) && is_key_pressed(KEY_DELETE2)) {
                kprintf(COL_CRI, "RECEIVED THREE-FINGER-SALUTE, REBOOTING");
                reboot();
            }

            if(scancode & (1<<7)) {
            } else {
                uint8_t ascii = scancode_to_ascii(scancode & ~(1<<7));
                if(ascii == '\n') {
                    kputc(COL_NOR, '\r');
                }
                kprintf(COL_NOR, "%c", ascii);
            }
            break;
		case 46:
            break;
		default:
            kprintf(COL_WAR, "Unknown IRQ: 0x%x\r\n", frame->interrupt);
            break;
    }
    pic_send_eoi(frame->interrupt);
    return new_frame;
}

void dump_frame(struct stackframe* frame)
{
	kprintf(COL_NOR, "eax=0x%x  ebx=0x%x  ecx=0x%x\r\n", frame->eax, frame->ebx, frame->ecx);
	kprintf(COL_NOR, "edx=0x%x  esi=0x%x  edi=0x%x\r\n", frame->edx, frame->esi, frame->edi);
	kprintf(COL_NOR, "ebp=0x%x  int=0x%x  err=0x%x\r\n", frame->ebp, frame->interrupt, frame->error);
	kprintf(COL_NOR, "eip=0x%x  cs =0x%x  efl=0x%x\r\n", frame->eip, frame->cs, frame->eflags);
	kprintf(COL_NOR, "esp=0x%x  ss =0x%x\r\n", frame->esp, frame->ss);
}

void panic(const char *msg)
{
    kprintf(COL_CRI, "KERNEL PANIC\r\n%s\r\n", msg);
    abort();
}

static void handle_syscall(struct stackframe* frame)
{
    uint32_t scallnum = frame->eax;
    uart_printf("syscall %d\r\n", scallnum);
    kprintf(COL_SUC, "System Call #%d. Hello World!\r\n", scallnum);
}

struct stackframe* isr_handler_common(struct stackframe* frame)
{
    struct stackframe* new_frame = frame;
    //kprintf(COL_NOR, "interrupt %d\r\n", frame->interrupt);
    uart_printf("interrupt %d\r\n", frame->interrupt);
    if(frame->interrupt <= 0x1f) {
        handle_exception(frame); 
    } else if(frame->interrupt >= 0x20 && frame->interrupt <= 0x2f) {
        new_frame = handle_irq(frame);
    } else if(frame->interrupt == 0x30) {
        handle_syscall(frame);
    } else {
        kprintf(COL_CRI, "Unknown Interrupt (%d) occoured. Kernel aborted.", frame->interrupt);
        while(1) {
            asm volatile("cli; hlt");
        }

    }

    return new_frame;
}
