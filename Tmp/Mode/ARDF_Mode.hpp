#ifndef ARDF_MODE_HPP
#define ARDF_MODE_HPP

#include <stdint.h>
#include "DS3231.hpp"

class ARDF_Mode {
    protected:
        DS3231* rtc;
        DS3231_Time NextAlarm;
        uint8_t FoxNum = 0;
    public:
        ARDF_Mode(DS3231* rtcIn);
        void Start();
        uint8_t Loop();        
        void Stop();
};

#endif