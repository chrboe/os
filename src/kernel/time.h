#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include "util.h"
#include "idt.h"

#define TIME_REG_SECONDS 0x00
#define TIME_REG_MINUTES 0x02
#define TIME_REG_HOURS 0x04
#define TIME_REG_DAY 0x07
#define TIME_REG_MONTH 0x08
#define TIME_REG_YEAR 0x09
#define TIME_REG_CENTURY 0x32
#define TIME_REG_STATUS_A 0x0A
#define TIME_REG_STATUS_B 0x0B

struct datetime {
    uint8_t century;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

void get_time(struct datetime *result);

#endif // TIME_H
