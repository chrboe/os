#include "uart.h"

void uart_puts(const char* text)
{
    for(int i = 0; text[i]; i++) {
        uart_write(text[i]);
    }
}

void uart_dump_frame(struct stackframe *f)
{
    uart_printf("eax=0x%x  ebx=0x%x  ecx=0x%x\r\n", f->eax, f->ebx, f->ecx);
    uart_printf("edx=0x%x  esi=0x%x  edi=0x%x\r\n", f->edx, f->esi, f->edi);
    uart_printf("ebp=0x%x  int=0x%x  err=0x%x\r\n", f->ebp, f->interrupt, f->error);
    uart_printf("eip=0x%x  cs =0x%x  efl=0x%x\r\n", f->eip, f->cs, f->eflags);
    uart_printf("esp=0x%x  ss =0x%x\r\n", f->esp, f->ss);
}

void uart_puti(uint32_t num, int base)
{
    char* chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char buf[65];
    char* p;

    if(base < 2 || base > 36) {
        return;
    }

    p = buf + 64;
    *p = 0;

    do {
        *--p = chars[num % base];
        num /= base;
    } while(num);

    uart_puts(p);
}

void uart_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    for(char* p = (char*)format; *p; p++) {
        if(*p == '%') {
            switch(*++p) {
                case 'd':
                    uart_puti(va_arg(args, unsigned long), 10);
                    break;
                case 'x':
                    uart_puti(va_arg(args, unsigned long), 16);
                    break;
                case 's':
                    uart_puts(va_arg(args, char*));
                    break;
                case 'c':
                    uart_write((char)va_arg(args, int));
                    break;
            }
        } else {
            uart_write(*p);
        }
    }

    va_end(args);
}

int uart_received() {
    return inb(COM1 + 5) & 1;
}

char uart_read() {
    while (uart_received() == 0);

    return inb(COM1);
}

int uart_is_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

void uart_write(char a) {
    while (uart_is_transmit_empty() == 0);

    outb(COM1,a);
}

int uart_init(uint32_t baud)
{
    uint16_t divisor = 115200/baud;

    outb(COM1 + 1, 0x00); /* disable interrupts for setup */
    outb(COM1 + 3, 0x80); /* enable dlab to set baud rate divisor */

    outb(COM1 + 0, divisor & 0xFF); /* divisor low byte */
    outb(COM1 + 1, divisor >> 8);   /* divisor high byte */

    outb(COM1 + 3, 0x03); /* 8 bits, no parity, one stop bit */
    outb(COM1 + 2, 0xC7); /* enable FIFO, clear them, with 14-byte threshold */
    outb(COM1 + 4, 0x0B); /* IRQs enabled, RTS/DSR set */

    return ERR_OK;
}