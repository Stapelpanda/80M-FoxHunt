#ifndef DS3231_HPP
#define DS3231_HPP

#include <stdint.h>

#define DS3231_ADDRESS 0xD0

#define DS3231_REG_SECONDS 0
#define DS3231_REG_MINUTES 1
#define DS3231_REG_HOURS 2
#define DS3231_REG_DAYOFWEEK 3
#define DS3231_REG_DAY 4
#define DS3231_REG_MONTH_CENTURY 5
#define DS3231_REG_YEAR 6

#define DS3231_REG_ALARM1_SECONS 7
#define DS3231_REG_ALARM1_MINUTES 8
#define DS3231_REG_ALARM1_HOURS 9
#define DS3231_REG_ALARM1_DAY 10

#define DS3231_REG_ALARM2_MINUTES 11
#define DS3231_REG_ALARM2_HOURS 12
#define DS3231_REG_ALARM2_DAY 13

#define DS3231_REG_CONTROL 14
#define DS3231_REG_STATUS 15

#define DS3231_REG_AGING 16
#define DS3231_REG_MSB 17
#define DS3231_REG_LSB 18

#define DS3231_BIT_24 6
#define DS3231_BIT_AMPM 5
#define DS3231_BIT_32KHZ 3

#define DS3231_BIT_A1IE 0
#define DS3231_BIT_A2IE 1
#define DS3231_BIT_A1IF 0
#define DS3231_BIT_A2IF 1

#define DS3231_ALARM_1 1
#define DS3231_ALARM_2 2

class DS3231_Time
{
public:
  uint8_t Seconds;
  uint8_t Minutes;
  uint8_t Hours;
  uint8_t DayOfWeek;

  void addDays(uint8_t days);
  void addHours(uint8_t hours);
  void addMinutes(uint8_t minutes);
  void addSeconds(uint8_t seconds);

  static uint8_t timesEqual(DS3231_Time *time1, DS3231_Time *time2, uint8_t checkUntill);
  static uint32_t getDiffInSeconds(DS3231_Time *current, DS3231_Time *next);
};

typedef enum {
  ALARM_EACH_MINUTE = 0x0F,
  ALARM_EACH_SECOND = 0x0F,
  ALARM_MATCH_SECONDS = 0x0E,
  ALARM_MATCH_MINUTES_SECONDS = 0x0C,
  ALARM_MATCH_HOURS_MINUTES_SECONDS = 0x08,
  ALARM_MATCH_DAYS_HOURS_MINUTES_SECONDS = 0x00,
} alarmtype_t;

typedef bool i2c_status_t;

class DS3231
{
private:
  i2c_status_t changeBit(uint8_t reg, uint8_t bit, uint8_t value);
  i2c_status_t setBit(uint8_t reg, uint8_t bit);
  i2c_status_t clearBit(uint8_t reg, uint8_t bit);
  i2c_status_t getHoursFromByte(uint8_t hourByte);

public:
  i2c_status_t init();
  i2c_status_t enable32KHz();
  i2c_status_t disable32KHz();
  i2c_status_t getTime(DS3231_Time *time);

  bool hasInterrupt();
  void clearInterrupt();

  i2c_status_t getStatus(uint8_t *alarmNum);
  i2c_status_t getAlarm(uint8_t alarmNum, DS3231_Time *time);
  i2c_status_t setAlarm(uint8_t alarmNum, DS3231_Time *time, alarmtype_t alarmType);
  i2c_status_t enableAlarm(uint8_t alarmNum);
  i2c_status_t disableAlarm(uint8_t alarmNum);
  i2c_status_t resetAlarm(uint8_t alarmNum);

  static DS3231& getInstance()
  {
    static DS3231 INSTANCE;
    return INSTANCE;
  }
};
#endif