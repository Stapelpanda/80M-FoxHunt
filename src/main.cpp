#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "DS3231.hpp"
#include "DIP.hpp"
// #include "MorseTransmitter.hpp"

#include "State.hpp"
//#include "Mode.h"

// Preflight Checks
#ifndef __AVR_ATmega328P__
#error Built for 328p, please rewrite for other AVRs
#endif

#if F_CPU < 14000000 || F_CPU > 15000000
#error F_CPU Should be around 14 MHz for 80m Operation
#endif

// Debug Pins
#define DBG1 PD3
#define DBG2 PD5

State_t State;
// MorseTransmitter morseTransmitter;

void initConfig()
{
    // Check DIP Switches for Mode
    /*  Mode_t* dipMode;
    switch(DIP_Read_Mode()){
        case 0:
            dipMode = &ARDF_Mode;
        break;
        case 1:
            dipMode = &ARDF7_Mode;
        break;
        case 2:
            dipMode = &FAST_Mode;
        break;
        case 3:
            dipMode = &CONTINUOUS_Mode;
        break;
    }
    if(dipMode != currentMode){
        currentMode->stop();
        currentMode = dipMode;
        currentMode->start();
    }*/
}

int main(void)
{
    // make the LED pin an output for PORTB5
    DDRD |= _BV(DBG1) | _BV(DBG2);

    // Init all Components
    DIP_Init();
    State.foxNum = DIP_Read_FoxNum();
    State.namingScheme = DIP_Read_Mode();

    State.rtc.init();
    // morseTransmitter.init();

    // morseTransmitter.setStringSet(MT_ARDF);
    // morseTransmitter.setStringSetIndex(0);

    initConfig();

    sei();
    uint8_t i = 0;
    DDRB |= _BV(PB1);
    if (State.rtc.setAlarm(DS3231_ALARM_1, &State.modeAlarm, ALARM_EACH_MINUTE) == 0)
    {
        PORTB |= _BV(PB1);
    }
    if (State.rtc.enableAlarm(DS3231_ALARM_1) == 0)
    {
        PORTB &= ~_BV(PB1);
    }
    bool noSleep = 0;
    
    while (1)
    {
        
        if (State.rtc.hasInterrupt())
        {
            State.rtc.clearInterrupt();
            if (State.rtc.getStatus(&i) == 0)
            {
                if (i & DS3231_ALARM_1)
                {
                    State.rtc.resetAlarm(DS3231_ALARM_1);
                }
                if (i & DS3231_ALARM_2)
                {
                    State.rtc.resetAlarm(DS3231_ALARM_2);
                    
                }
            }
        }
        
        // morseTransmitter.loop();
        /*
        // Loop Functions determine Sleep Mode
        uint8_t sleepModeTransmitMode = currentMode->loop();
        uint8_t sleepModeTimer = MorseTransmitter_Loop();
        */
        // Keep lowest sleep Mode, IDLE = 1, Extended Standby = 7 So higher is more risk (timers get shutdown after IDLE)
        uint8_t sleepMode = SLEEP_MODE_PWR_DOWN;

        // Atomic operation of setting Sleep mode
        set_sleep_mode(sleepMode);
        cli();
        if (sleepMode)
        {
            sleep_enable();
            sei();
            sleep_cpu();
            sleep_disable();
        }
        sei();
    }
    return 0;
}
