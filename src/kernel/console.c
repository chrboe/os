#include "console.h"

/*
 * kputc prints a single ASCII character to video memory
 */
void kputc(char color, char chr)
{
    char* vmem = (char*)VIDMEM_ADDR;
    static int ypos = 0;
    static int xoffs = 0;

    if(chr == '\n') {
        if(ypos >= 24) {
            char* secondline = vmem + 160;
            kmemmove(vmem, secondline, 4096 - 160);
            kmemset(vmem + 160*24, 0, 160);
        } else {
            ypos++;
        }
    } else if(chr == '\r') {
        xoffs = 0;
    } else {
        int curroffs = (ypos*80 + xoffs)*2;
        vmem[curroffs] = chr;
        vmem[curroffs+1] = color;
        xoffs++;
    }
}


/*
 * kputs offers an early way of printing a constant, colored string
 * to video memory.
 */
void kputs(char color, const char* text)
{
    for(int i = 0; text[i]; i++) {
        kputc(color, text[i]);
    }
}

void kputi(char color, uint32_t num, int base)
{
    static char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char buf[65];
    char* p;

    //uart_printf("inside puti %d\r\n", num);

    if(base < 2 || base > 36) {
        return;
	}

	p = buf + 64;
	*p = 0;

	do {
        *--p = chars[num % base];
        num /= base;
    } while(num);

    kputs(color, p);
}

void kcls()
{
    char* vmem = (char*)VIDMEM_ADDR;
    kmemset(vmem, 0, 4096);
}

void kprintf(char color, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    for(char* p = (char*)format; *p; p++) {
        if(*p == '%') {
            switch(*++p) {
                case 'd':;
                    kputi(color, va_arg(args, uint32_t), 10);
                    break;
                case 'x':
                    kputi(color, va_arg(args, uint32_t), 16);
                    break;
                case 's':
                    kputs(color, va_arg(args, char*));
                    break;
                case 'c':
                    kputc(color, (char)va_arg(args, int));
                    break;
            }
        } else {
            kputc(color, *p);
        }
    }

    va_end(args);
}
