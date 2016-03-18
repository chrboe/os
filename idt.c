#include "idt.h"

static uint64_t build_idt_entry(uint32_t offset, void (*f)(), uint32_t sel, uint32_t flags)
{
	uint64_t entry;
	uint32_t handler = (uint32_t)f;
	entry  = handler & 0xFFFFLL;
	entry |= (selector & 0xFFFFLL) << 16;
	entry |= (flags & 0xFFLL) << 40;
	entry |= ((handler >> 16) & 0xFFFFLL) << 48;
	return entry;
}

void build_idt()
{
}
