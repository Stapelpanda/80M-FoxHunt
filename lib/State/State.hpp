#ifndef STATE_HPP
#define STATE_HPP
#include <stdint.h>
#include "DS3231.hpp"

typedef struct
{
    uint8_t foxNum;
    uint8_t namingScheme;
    
    uint8_t debug;
    
    DS3231_Time modeAlarm;
    DS3231 rtc;
} State_t;

#endif