#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "DIP.hpp"

#include "DS3231.hpp"
// #include "MorseTransmitter.hpp"
#include "Timer0/Timer0.hpp"

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

DIP DipSettings;
// MorseTransmitter morseTransmitter;
Timer0 timer;

void setupMode()
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

// MorseString_t MT_ARDF = {8, {"MO", "MOE", "MOI", "MOS", "MOH", "MO5", "MO5E", "MO5I"}};

int main(void)
{
    // make the LED pin an output for PORTB5
    DDRD |= _BV(DBG1) | _BV(DBG2);

    // Init all Components
    DipSettings.init();

    // Initialize Peripherals
    // - DS3231
    DS3231::getInstance().init();
    DS3231::getInstance().enable32KHz();

    // Initialize Mode
    setupMode();
    // morseTransmitter.init();

    // morseTransmitter.setStringSet(&MT_ARDF);
    // morseTransmitter.setStringSetIndex(0);
    // morseTransmitter.setStart();

    timer.init();
    timer.setTimer(0x100);
    sei();
    DDRB |= _BV(PB1);

    while (1)
    {
        if (timer.hasInterrupt())
        {
            timer.clearInterrupt();
            timer.setTimer(0x100);
            PORTB ^= _BV(PB1);
        }
        /*
        // Loop Functions determine Sleep Mode
        uint8_t sleepModeTransmitMode = currentMode->loop();
        */
        // uint8_t sleepModeTransmitter = morseTransmitter.loop();
        // Keep lowest sleep Mode, IDLE = 1, Extended Standby = 7 So higher is more risk (timers get shutdown after IDLE)

        uint8_t sleepMode = 0; //sleepModeTransmitter;

        if (sleepMode > 0)
        {
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
    }
    return 0;
}
