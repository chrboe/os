#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include "util.h"
#include "errors.h"

#define PIT_BASE_FREQ 1193182

#define PORT_PIT_CH0 0x40
#define PORT_PIT_CH1 0x41
#define PORT_PIT_CH2 0x42
#define PORT_PIT_INIT 0x43

#define PIT_COUNTER_FORMAT_BINARY 0x00
#define PIT_COUNTER_FORMAT_BCD 0x01

#define PIT_MODE_0 0x00
#define PIT_MODE_1 0x02
#define PIT_MODE_2 0x04
#define PIT_MODE_3 0x06
#define PIT_MODE_4 0x08
#define PIT_MODE_5 0x0A

#define PIT_COUNTER_INTERNAL 0x00
#define PIT_COUNTER_LSB_ONLY 0x10
#define PIT_COUNTER_MSB_ONLY 0x20
#define PIT_COUNTER_LSB_THEN_MSB 0x30

#define PIT_CHANGE_CHANNEL_0 0x00
#define PIT_CHANGE_CHANNEL_1 0x40
#define PIT_CHANGE_CHANNEL_2 0x80

uint8_t pit_init(uint32_t freq);


#endif // PIT_H
