#include <avr/sleep.h>
#include "ARDF_Mode.hpp"
#include "MorseTransmitter.hpp"

DS3231 rtc = DS3231::getInstance();
extern MorseTransmitter morseTransmitter;

void ARDF_Mode::init()
{
    FoxNum = 0;
    Mode = ARDF;
    State = ARDF_STATE_INIT;
    NextAlarm.Seconds = 0;
    NextAlarm.Minutes = 0;
    NextAlarm.Hours = 0;
    NextAlarm.DayOfWeek = 0;
    NextAlarmSet = false;
}

void ARDF_Mode::setFoxNum(uint8_t foxNum)
{
    FoxNum = foxNum;
}
void ARDF_Mode::setMode(ARDF_Mode_t ardfMode)
{
    Mode = ardfMode;
    if (Mode == ARDF_Short || Mode == ARDF7_Short)
    {
        morseTransmitter.setSpeed(15);
    }
    else
    {
        morseTransmitter.setSpeed(10);
    }
}

void ARDF_Mode::start()
{
    // Set Safeguard Alarm each minute
    rtc.setAlarm(DS3231_ALARM_2, &NextAlarm, ALARM_EACH_MINUTE);
    rtc.enableAlarm(DS3231_ALARM_2);
    rtc.clearInterrupt();

    State = ARDF_STATE_INIT;
    stateMachine();
}

void ARDF_Mode::stateMachine()
{
    switch (State)
    {
    case ARDF_STATE_INIT:
        if (FoxNum < 2) // Fox 0, 1 Start Transmit
        {
            morseTransmitter.start();
            State = ARDF_STATE_TRANSMIT;
            if (FoxNum == 1) // Fox 1 Corner case, when booting up start transmission immediatly
            {
                addSecondsAndSetAlarm(transmitDelay());
            }
        }
        else
        {
            // Set Fox num Delay
            addSecondsAndSetAlarm(initialDelay());
            State = ARDF_STATE_WAIT_FOR_TRANSMIT;
        }
        break;
    case ARDF_STATE_WAIT_FOR_TRANSMIT:
        morseTransmitter.start();
        State = ARDF_STATE_TRANSMIT;
        addSecondsAndSetAlarm(transmitDelay());
        break;
    case ARDF_STATE_TRANSMIT:
        if (FoxNum > 0)
        {
            morseTransmitter.stop();
            State = ARDF_STATE_WAIT_FOR_TRANSMIT;
            addSecondsAndSetAlarm(waitDelay());
        }
        break;
    }
}

uint8_t ARDF_Mode::loop()
{
    if (rtc.hasInterrupt())
    {
        uint8_t status;
        DS3231_Time currentTime;
        if (rtc.getStatus(&status) == I2C_ok && rtc.getTime(&currentTime) == I2C_ok)
        {
            rtc.clearInterrupt();
            // Watchdog
            if (status & DS3231_ALARM_2)
            {
                rtc.resetAlarm(DS3231_ALARM_2);
                if (NextAlarmSet && !(status & DS3231_ALARM_1))
                {
                    if (DS3231_Time::getDiffInSeconds(&currentTime, &NextAlarm) > largestDelay())
                    {
                        // We skipped our Alarm, reset!
                        // start();
                    }
                }
            }
            // State Machine
            if (status & DS3231_ALARM_1)
            {
                rtc.resetAlarm(DS3231_ALARM_1);
                NextAlarmSet = false;
                stateMachine();
                if (NextAlarmSet)
                {
                    // Check Alarm
                }
            }
        }
    }
    return SLEEP_MODE_EXT_STANDBY;
}

bool ARDF_Mode::addSecondsAndSetAlarm(uint16_t seconds)
{
    NextAlarm.addSeconds(seconds);
    NextAlarmSet = true;
    return (rtc.setAlarm(DS3231_ALARM_1, &NextAlarm, ALARM_MATCH_MINUTES_SECONDS) == I2C_ok &&
            rtc.enableAlarm(DS3231_ALARM_1) == I2C_ok);
}

void ARDF_Mode::stop()
{
}

uint16_t ARDF_Mode::largestDelay()
{
    return transmitDelay() + waitDelay();
}

void ARDF_Mode::roundAlarmToFrame(DS3231_Time *currentTime, DS3231_Time *alarm)
{
}
uint16_t ARDF_Mode::initialDelay()
{
    if (FoxNum > 0)
    {
        return transmitDelay() * (FoxNum - 1);
    }
    else
    {
        return 0;
    }
}
uint16_t ARDF_Mode::transmitDelay()
{
    switch (Mode)
    {
    case ARDF:
    case ARDF7:
        return 60;
    case ARDF_Short:
    case ARDF7_Short:
        return 20;
    }
    return 0;
}
uint16_t ARDF_Mode::waitDelay()
{
    switch (Mode)
    {
    case ARDF:
    case ARDF_Short:
        return 4 * transmitDelay();
    case ARDF7:
    case ARDF7_Short:
        return 6 * transmitDelay();
    }
    return 0;
}