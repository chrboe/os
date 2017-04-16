#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "util.h"
#include "errors.h"
#include "idt.h"

#define COM1 0x3F8

typedef __builtin_va_list       va_list;
#define va_start(ap, X)         __builtin_va_start(ap, X)
#define va_arg(ap, type)        __builtin_va_arg(ap, type)
#define va_end(ap)              __builtin_va_end(ap)

int uart_init(uint32_t baud);
void uart_puts(const char* str);
void uart_putc(char c);
void uart_puti(uint32_t um, int base);
void uart_printf(const char* fmt, ...);
void uart_write(char c);
char uart_read();
void uart_dump_frame(struct stackframe *f);

#endif //UART_H
