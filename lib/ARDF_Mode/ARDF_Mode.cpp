#include <avr/sleep.h>
#include "ARDF_Mode.hpp"

DS3231 rtc = DS3231::getInstance();
void ARDF_Mode::init()
{
    FoxNum = 0;
    Mode = ARDF;
    State = ARDF_STATE_INIT;
}

void ARDF_Mode::setFoxNum(uint8_t foxNum)
{
    FoxNum = foxNum;
}
void ARDF_Mode::setMode(ARDF_Mode_t ardfMode)
{
    Mode = ardfMode;
}

void ARDF_Mode::start()
{
    // Set Safeguard Alarm each minute
    rtc.setAlarm(DS3231_ALARM_1, 0, ALARM_EACH_MINUTE);
    rtc.enableAlarm(DS3231_ALARM_1);
}

uint8_t ARDF_Mode::loop()
{
    if (rtc.hasInterrupt())
    {
        uint8_t status;
        if (rtc.getStatus(&status) == I2C_ok)
        {
            if(status & DS3231_ALARM_2){
                // Watchdog
                PORTB ^= _BV(PB1);
                rtc.resetAlarm(DS3231_ALARM_2);
            }
            if(status & DS3231_ALARM_1){
                // State Machine
                PORTB ^= _BV(PB1);
                rtc.resetAlarm(DS3231_ALARM_1);
            }
        }
    }
    return SLEEP_MODE_EXT_STANDBY;
}

void ARDF_Mode::stop()
{
}
