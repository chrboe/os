#include "pit.h"

uint8_t pit_init(uint32_t freq)
{
    uint32_t counter = PIT_BASE_FREQ / freq;
    outb(PORT_PIT_INIT, PIT_MODE_2 | PIT_COUNTER_LSB_THEN_MSB);
    outb(PORT_PIT_CH0, counter & 0xFF);
    outb(PORT_PIT_CH0, counter >> 8);

    return ERR_OK;

}