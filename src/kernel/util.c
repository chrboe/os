#include "util.h"

void kmemmove(void* destination, const void* source, uint32_t num)
{
    char* s = (char*)source;
    char* d = (char*)destination;
    while(num--) {
        *d++ = *s++;
    }
}

void kmemset(char* start, char content, uint32_t bytes)
{
    int i = 0;
    do {
        *start++ = content;
        i++;
    } while(i < bytes);
}

void outb(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

uint8_t inb(uint16_t port)
{
    char result;
    asm volatile("inb %1, %0" : "=a" (result) : "d" (port));
    return result;
}

uint16_t inw(uint16_t port)
{
	uint16_t result;
	asm volatile("inw %1, %0" : "=a" (result) : "d" (port));
	return result;
}

void outw(uint16_t port, uint16_t data)
{
    asm volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

void reboot()
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    asm volatile ("hlt");
}