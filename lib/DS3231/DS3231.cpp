#include <avr/io.h>
#include <avr/interrupt.h>
#include "ds3231.hpp"
#include "i2c_master/i2c_master.h"

uint8_t __attribute__((noinline)) bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
uint8_t __attribute__((noinline)) bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

bool INT0_Interrupt = 0;
//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
    INT0_Interrupt = 1;
}

i2c_status_t DS3231::init()
{
    i2c_init();
    i2c_status_t res = 0;
    // Reset all stuff
    res |= disableAlarm(DS3231_ALARM_1);
    res |= disableAlarm(DS3231_ALARM_2);
    res |= resetAlarm(DS3231_ALARM_1);
    res |= resetAlarm(DS3231_ALARM_2);
    res |= disable32KHz();

    // This should not be here :(
    PORTD |= _BV(PD2);
    EICRA |= _BV(ISC01); // Setup Interrupt on Falling edge
    EIMSK |= _BV(INT0);

    return res;
}

bool DS3231::hasInterrupt()
{
    return INT0_Interrupt ;
}
void DS3231::clearInterrupt()
{
    INT0_Interrupt = 0;
}

i2c_status_t DS3231::changeBit(uint8_t reg, uint8_t bit, uint8_t value)
{
    uint8_t readTemp;
    if (i2c_readReg(DS3231_ADDRESS, reg, &readTemp, 1) == 0)
    {
        if (value)
        {
            readTemp |= _BV(bit);
        }
        else
        {
            readTemp &= ~_BV(bit);
        }
        return i2c_writeReg(DS3231_ADDRESS, reg, &readTemp, 1);
    }
    else
    {
        return 1;
    }
}

i2c_status_t DS3231::setBit(uint8_t reg, uint8_t bit)
{
    return changeBit(reg, bit, 1);
}

i2c_status_t DS3231::clearBit(uint8_t reg, uint8_t bit)
{
    return changeBit(reg, bit, 0);
}

i2c_status_t DS3231::enable32KHz()
{
    return setBit(DS3231_REG_STATUS, DS3231_BIT_32KHZ);
}

i2c_status_t DS3231::disable32KHz()
{
    return clearBit(DS3231_REG_STATUS, DS3231_BIT_32KHZ);
}

i2c_status_t DS3231::getHoursFromByte(uint8_t hourByte)
{
    // BIT_24 => 0 = 24 Hour, 1 = 12 Hour
    if ((hourByte & DS3231_BIT_24) == 0)
    {
        return bin2bcd(hourByte & 0x3F);
    }
    else
    {
        // 12 AM => 00
        // 1 AM -> 11 AM => 1 -> 11
        // 12 PM => 12
        // 1 PM -> 11 PM => 13 -> 23
        // Return = (isPM ? 12 : 0) + (hour == 12 ? 0 : hour)
        return ((hourByte & DS3231_BIT_AMPM) != 0 ? 12 : 0) +
               (bcd2bin(hourByte & 0x1F) == 12 ? 0 : bcd2bin(hourByte & 0x1F));
    }
}
i2c_status_t DS3231::getTime(DS3231_Time *timeStruct)
{
    uint8_t tempBytes[3];
    // TODO: Implement Days??
    if (i2c_readReg(DS3231_ADDRESS, DS3231_REG_SECONDS, tempBytes, 3) == 0)
    {
        timeStruct->Seconds = bcd2bin(tempBytes[0]);
        timeStruct->Minutes = bcd2bin(tempBytes[1]);
        timeStruct->Hours = getHoursFromByte(tempBytes[2]);
        return 0;
    }
    else
    {
        return 1;
    }
}

i2c_status_t DS3231::getStatus(uint8_t *status)
{
    return i2c_readReg(DS3231_ADDRESS, DS3231_REG_STATUS, status, 1);
}

i2c_status_t DS3231::getAlarm(uint8_t alarmNum, DS3231_Time *timeStruct)
{
    uint8_t tempBytes[4];
    // Read out Alarm REGS
    if (alarmNum == DS3231_ALARM_1 && i2c_readReg(DS3231_ADDRESS, DS3231_REG_ALARM1_SECONS, tempBytes, 4) == 0)
    {
        timeStruct->Seconds = bcd2bin(tempBytes[0] & 0x7F);
        timeStruct->Minutes = bcd2bin(tempBytes[1] & 0x7F);
        timeStruct->Hours = getHoursFromByte(tempBytes[2] & 0x7F);

        return 0;
    }
    else if (alarmNum == DS3231_ALARM_2 && i2c_readReg(DS3231_ADDRESS, DS3231_REG_ALARM2_MINUTES, tempBytes, 3) == 0)
    {
        timeStruct->Seconds = 0;
        timeStruct->Minutes = bcd2bin(tempBytes[0] & 0x7F);
        timeStruct->Hours = getHoursFromByte(tempBytes[1] & 0x7F);

        return 0;
    }
    return 1;
}

i2c_status_t DS3231::setAlarm(uint8_t alarmNum, DS3231_Time *time, alarmtype_t alarmType)
{
    uint8_t alarmRegs[4];
    alarmRegs[0] = (bin2bcd(time->Seconds) & 0x7F) | (alarmType & 0x01) << 7;
    alarmRegs[1] = (bin2bcd(time->Minutes) & 0x7F) | (alarmType & 0x02) << 6;
    alarmRegs[2] = (bin2bcd(time->Hours) & 0x3F) | (alarmType & 0x04) << 5;
    alarmRegs[3] = (time->DayOfWeek & 0x0F) | (alarmType & 0x08) << 4;

    if (alarmNum == DS3231_ALARM_1)
    {
        return i2c_writeReg(DS3231_ADDRESS, DS3231_REG_ALARM1_SECONS, alarmRegs, 4);
    }
    else if (alarmNum == DS3231_ALARM_2)
    {
        return i2c_writeReg(DS3231_ADDRESS, DS3231_REG_ALARM2_MINUTES, &alarmRegs[1], 3);
    }
    return 1;
}

i2c_status_t DS3231::enableAlarm(uint8_t alarmNum)
{
    if (alarmNum == DS3231_ALARM_1)
    {
        return setBit(DS3231_REG_CONTROL, DS3231_BIT_A1IE);
    }
    else if (alarmNum == DS3231_ALARM_2)
    {
        return setBit(DS3231_REG_CONTROL, DS3231_BIT_A2IE);
    }
    return 1;
}

i2c_status_t DS3231::disableAlarm(uint8_t alarmNum)
{
    if (alarmNum == DS3231_ALARM_1)
    {
        return clearBit(DS3231_REG_CONTROL, DS3231_BIT_A1IE);
    }
    else if (alarmNum == DS3231_ALARM_2)
    {
        return clearBit(DS3231_REG_CONTROL, DS3231_BIT_A2IE);
    }
    return 1;
}

i2c_status_t DS3231::resetAlarm(uint8_t alarmNum)
{
    if (alarmNum == DS3231_ALARM_1)
    {
        return clearBit(DS3231_REG_STATUS, DS3231_BIT_A1IF);
    }
    else if (alarmNum == DS3231_ALARM_2)
    {
        return clearBit(DS3231_REG_STATUS, DS3231_BIT_A2IF);
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////// DS3231_Time
/////////////////////////////////////////////////////////////////////////////////////////////////////

void DS3231_Time::addDays(uint8_t days)
{
    DayOfWeek = (DayOfWeek + days) % 7;
}
void DS3231_Time::addHours(uint8_t hours)
{
    Hours += hours;
    addDays(Hours / 24);
    Hours = Hours % 24;
}

void DS3231_Time::addMinutes(uint8_t minutes)
{
    Minutes += minutes;
    addHours(Minutes / 60);
    Minutes = Minutes % 60;
}

void DS3231_Time::addSeconds(uint8_t seconds)
{
    Seconds += seconds;
    addMinutes(Seconds / 60);
    Seconds = Seconds % 60;
}

uint8_t DS3231_Time::timesEqual(DS3231_Time *time1, DS3231_Time *time2, uint8_t checkUntill)
{
    return time1->Seconds == time2->Seconds &&
           (checkUntill < 2 || time1->Minutes == time2->Minutes) &&
           (checkUntill < 3 || time1->Hours == time2->Hours) &&
           (checkUntill < 4 || time1->DayOfWeek == time2->DayOfWeek);
}
uint32_t DS3231_Time::getDiffInSeconds(DS3231_Time *current, DS3231_Time *next)
{
    uint32_t currentTime = current->Hours * 3600 + current->Minutes * 60 + current->Seconds;
    uint32_t nextTime = next->Hours * 3600 + next->Minutes * 60 + next->Seconds;

    /*
      Calculate diff in seconds: A = current, B = next
        0                                                23:59:59
            A       B                                                 => return B - A
            B                                          A              => return (24 * 3600) - A + B        

    */
    if (nextTime >= currentTime)
    {
        return nextTime - currentTime;
    }
    else
    {
        uint32_t remainder = (24UL * 3600UL) - currentTime;
        return nextTime + remainder;
    }
}