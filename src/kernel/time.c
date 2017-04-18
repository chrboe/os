#include "time.h"

static void cmos_select(uint8_t reg, uint8_t nmi_disabled_bit)
{
    outb(0x70, (nmi_disabled_bit << 7) | reg);
}

static uint8_t cmos_read(uint8_t reg, uint8_t nmi_disabled_bit)
{
    cmos_select(reg, nmi_disabled_bit);
    return inb(0x71);
}

static void cmos_write(uint8_t reg, uint8_t val, uint8_t nmi_disabled_bit)
{
    cmos_select(reg, nmi_disabled_bit);
    outb(0x71, val);
}

static uint8_t rtc_update_in_progess()
{
    outb(0x70, 0x0A);
    return (inb(0x71) & 0x80);
}

void get_time(struct datetime *result)
{
    uint8_t nmi_disabled_bit = is_nmi_disabled();

    while(rtc_update_in_progess());


    uint8_t seconds = cmos_read(TIME_REG_SECONDS, nmi_disabled_bit);
    uint8_t minutes = cmos_read(TIME_REG_MINUTES, nmi_disabled_bit);
    uint8_t hours = cmos_read(TIME_REG_HOURS, nmi_disabled_bit);
    uint8_t day = cmos_read(TIME_REG_DAY, nmi_disabled_bit);
    uint8_t month = cmos_read(TIME_REG_MONTH, nmi_disabled_bit);
    uint8_t year = cmos_read(TIME_REG_YEAR, nmi_disabled_bit);
    uint8_t century = cmos_read(TIME_REG_CENTURY, nmi_disabled_bit);

    /*
     * status register B:
     * if bit 2 (0x04) is set, the values are read in regular binary mode.
     * if bit 2 (0x04) is not set, the values are read in BCD.
     * e.g. if the time is "2017-04-17 14:59:13", then
     *      century = 0x20
     *      year    = 0x17
     *      month   = 0x04
     *      day     = 0x17
     *
     *      hours   = 0x14
     *      minutes = 0x59
     *      seconds = 0x13
     */
    uint8_t statusb = cmos_read(TIME_REG_STATUS_B, nmi_disabled_bit);
    if (!(statusb & 0x04)) {
        /* convert from BCD to binary */
        seconds = (seconds & 0x0F) + ((seconds / 16) * 10);
        minutes = (minutes & 0x0F) + ((minutes / 16) * 10);
        hours = ( (hours & 0x0F) + (((hours & 0x70) / 16) * 10) ) | (hours & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        century = (century & 0x0F) + ((century / 16) * 10);
    }

    result->seconds = seconds;
    result->minutes = minutes;
    result->hours = hours;
    result->day = day;
    result->month = month;
    result->year = year;
    result->century = century;
}