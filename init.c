#define COL_NOR 0x7
#define COL_ERR 0xC
#define COL_WAR 0xE
#define COL_CRI 0x4
#define COL_SUC 0x2

#include "console.h"

unsigned long long gdt[7];

unsigned long long build_gdt_segment(unsigned int base, unsigned int limit, 
                                     unsigned int flags)
{
    unsigned long long val = 0;
    val |= base & 0xFFFFFFLL << 16;
    val |= ((base >> 24) & 0xFFLL) << 56;
    val |= limit & 0xffffLL;
    val |= ((limit >> 16) & 0xFLL) << 48;
    val |= (flags & 0xFFLL) << 40;
    val |= ((flags >> 8) & 0xFFLL) << 52;
    return val;
}

void setup_gdt()
{
    gdt[0] = 0;
    gdt[1] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<4)|(1<<3)); /* kernel code */
    gdt[2] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<4)|(1<<1)); /* kernel data*/
    gdt[3] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)); /* user code */
    gdt[4] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<1)); /* user data */
//    gdt[5] = build_gdt_segment(0, 0x100000-1, (1<<15)|(1<<14)|(1<<7)|(1<<6)|(1<<5)|(1<<1)); /* tss for multitasking */
}

void init()
{
    kcls();
    kprintf(COL_NOR, "Hello World!\r\n");
    kprintf(COL_NOR, "This %d is %x a %s test\r\n", 5, 0x64, "printf");
    kprintf(COL_ERR, "This is an error\r\n");
    kprintf(COL_WAR, "This is a warning\r\nWith a second line\r\n");
    kprintf(COL_CRI, "This is a critical error\r\n");
    kprintf(COL_SUC, "Success!\r\n");
    
}
