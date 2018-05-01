#include <stdint.h>
#include <avr/sleep.h>

#include "ARDF.hpp"

#define LARGEST_DELAY (6 * 60)

uint8_t ARDF::HandleI2C_Error()
{
    //// WHAT TO DO???
    return 1;
}

uint8_t ARDF::getFrameNum(DS3231_Time *currentTime, uint8_t offset)
{
    uint8_t nextFrame = 0;
    //TODO Filter Corner casing of alarm being ran at this moment => alarm = 1:00 vs time = 1:00
    while (nextFrame < sizeof(FrameStart) && (FrameStart[nextFrame] + offset) < currentTime->Minutes)
        nextFrame++;

    if (nextFrame == sizeof(FrameStart))
    {
        nextFrame = 0;
    }
    return nextFrame;
}

uint8_t ARDF::getFutureAlarm(DS3231_Time *currentTime, uint8_t offset)
{
    return FrameStart[getFrameNum(currentTime, offset)] + offset;
}

uint8_t ARDF::setNewAlarm(DS3231_Time *currentTime)
{
    switch (ARDF_State)
    {
    default:
        // FAILSAFE: Set State to waiting if outside of normal behaviour
        ARDF_State = ARDF_STATE_WAIT_FOR_TRANSMIT;
        return setNewAlarm(currentTime);
    case ARDF_STATE_WAIT_FOR_TRANSMIT:
        NextAlarm.Seconds = 0;
        NextAlarm.Minutes = getFutureAlarm(currentTime, FoxNum);
        return rtc->setAlarm(1, &NextAlarm, ALARM_MATCH_MINUTES_SECONDS);

    case ARDF_STATE_TRANSMIT:
        NextAlarm.Seconds = 59;
        NextAlarm.Minutes = getFutureAlarm(currentTime, FoxNum);
        return rtc->setAlarm(1, &NextAlarm, ALARM_MATCH_MINUTES_SECONDS);
    }
}
void ARDF::Start()
{

    // Set State To Initialize, StateMachine will set first Alarm
    ARDF_State = ARDF_STATE_INIT;

    // Setup ALARM 2 => Each Minute; (Kind of safeguard) !!ALARM TIME DOES NOT MATTER IN ALARM MODE ALARM_EACH_MINUTE!!
    if (rtc->setAlarm(2, &NextAlarm, ALARM_EACH_MINUTE) == 0)
    {
        rtc->enableAlarm(2);
    }
}

/**
 * 
 * @returns SLEEP_MODE
 */
uint8_t ARDF::StateMachine(DS3231_Time *currentTime)
{
    switch (ARDF_State)
    {
    case ARDF_STATE_INIT:
        ARDF_State = ARDF_STATE_WAIT_FOR_TRANSMIT;
        if (setNewAlarm(currentTime) != 0)
        {
            HandleI2C_Error();
        }
    case ARDF_STATE_TRANSMIT:
        // TODO Enable TX
    case ARDF_STATE_WAIT_FOR_TRANSMIT:
        // Get diff with Current Time, if timer Expired, set new Alarm
        if (DS3231_Time::getDiffInSeconds(currentTime, &NextAlarm) > LARGEST_DELAY)
        {
            if (setNewAlarm(currentTime) == 0)
            {
                HandleI2C_Error();
            }
        }
        break;
    }
}

uint8_t ARDF::Loop(DS3231_Time *currentTime)
{
    uint8_t rtcStatus = 0;
    uint8_t sleepMode = SLEEP_MODE_EXT_STANDBY;
    if (rtc->hasInterrupt() || ARDF_State == ARDF_STATE_INIT)
    {
        if (ARDF_State == ARDF_STATE_INIT)
        {
            return StateMachine(currentTime);
        }

        if (rtc->getStatus(&rtcStatus) == 0)
        {
            if (rtcStatus && DS3231_BIT_A1IF)
            {
                // If the Alarm went off this means we get onto the next state..
                ARDF_State = static_cast<ARDF_State_e>(ARDF_State + 1);

                // Reset Alarm
                rtc->resetAlarm(1);

                // Do StateMachine stuff
                uint8_t stateMachineSleepMode = StateMachine(currentTime);
                if (stateMachineSleepMode < sleepMode)
                {
                    sleepMode = stateMachineSleepMode;
                }
            }

            // If alarm 2 goes, Check the alarm, if not in Future lets do some StateMachine Stuff
            if (rtcStatus && DS3231_BIT_A2IF)
            {
                DS3231_Time nextAlarm;
                if (rtc->getAlarm(1, &nextAlarm) == 0)
                {
                    if (DS3231_Time::getDiffInSeconds(currentTime, &nextAlarm) > LARGEST_DELAY)
                    {
                        //TODO: Disable TX Mode

                        //Set to WAIT TX
                        ARDF_State = ARDF_STATE_WAIT_FOR_TRANSMIT;
                        setNewAlarm(currentTime);
                    }
                }
                rtc->resetAlarm(2);
            }
        }
        rtc->clearInterrupt();
    }
    return SLEEP_MODE_EXT_STANDBY;
}

void ARDF::Stop()
{
    rtc->disableAlarm(1);
    rtc->disableAlarm(2);
}
