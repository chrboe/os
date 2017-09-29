#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define BSWAP16(x) (uint16_t)((x>>8) | (x<<8))
#define BSWAP32(x) (((x>>24)&0xff) | ((x<<8)&0xff0000) \
        | ((x>>8)&0xff00) | ((x<<24)&0xff000000))

#define COMBINE16TO32(x,y) (((uint32_t)x<<16) | y)
#define COMBINE16TO64(a,b,c,d) ((uint64_t)a<<48 | (uint64_t)b<<32 \
        | (uint64_t)c<<16 | (uint64_t)d)

void kmemmove(void* dest, const void* src, uint32_t num);
void kmemset(char* start, char content, uint32_t bytes);

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);
void reboot();

#endif /* !UTIL_H */
