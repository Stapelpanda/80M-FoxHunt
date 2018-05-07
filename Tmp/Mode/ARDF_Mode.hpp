#ifndef ARDF_MODE_HPP
#define ARDF_MODE_HPP

#include <stdint.h>
#include "DS3231.hpp"

typedef enum {
    ARDF,
    ARDF_Short,
    Continuous,
} ARDF_Mode_t;

class ARDF_Mode
{
  protected:
    DS3231 *rtc;
    DS3231_Time NextAlarm;
    uint8_t FoxNum = 0;
    ARDF_Mode_t Mode = ARDF;
  public:
    ARDF_Mode(DS3231 *rtcIn);
    void Start();
    void SetMode(ARDF_Mode_t ardfMode);
    void SetFoxNum(uint8_t foxNum);
    uint8_t Loop();
    void Stop();
};

#endif