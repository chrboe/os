#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

void kmemmove(void* dest, const void* src, uint32_t num);
void kmemset(char* start, char content, uint32_t bytes);

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);

#endif /* !UTIL_H */
