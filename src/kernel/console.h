#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "util.h"
#include "mm/pmm.h"
#include "stackguard.h"

typedef __builtin_va_list       va_list;
#define va_start(ap, X)         __builtin_va_start(ap, X)
#define va_arg(ap, type)        __builtin_va_arg(ap, type)
#define va_end(ap)              __builtin_va_end(ap)

#define VIDMEM_ADDR 0xC03FF000

#define COL_NOR 0x7
#define COL_ERR 0xC
#define COL_WAR 0xE
#define COL_CRI 0x4
#define COL_SUC 0x2


void kputs(char color, const char* str);
void kputc(char color, char c);
void kputi(char color, uint32_t um, int base);
void kprintf(char color, const char* fmt, ...);
void kcls();
void set_cursor(uint8_t row, uint8_t col);

#endif /*CONSOLE_H_INCLUDED*/
