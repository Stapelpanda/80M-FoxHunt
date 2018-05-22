#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "DIP.hpp"
#include "DS3231.hpp"
#include "MorseTransmitter.hpp"
#include "ARDF_Mode.hpp"

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
MorseTransmitter morseTransmitter = MorseTransmitter::getInstance();
ARDF_Mode ardfMode;

MorseString_t MT_ARDF = {8, {"MO", "MOE", "MOI", "MOS", "MOH", "MO5", "MO5E", "MO5I"}};
MorseString_t MT_ARDF_SHORT = {8, {"M", "ME", "MI", "MS", "MH", "M5", "M5E", "M5I"}};
MorseString_t MT_CALLSIGN = {8, {"PB0MV", "PB0MV E", "PB0MV I", "PB0MV S", "PB0MV H", "PB0MV 5", "PB0MV 5E", "PB0MV 5I"}};
MorseString_t MT_CALLSIGN2 = {8, {"PI6YRC", "PI6YRC E", "PI6YRC I", "PI6YRC S", "PI6YRC H", "PI6YRC 5", "PI6YRC 5E", "PI6YRC 5I"}};

void setupMode()
{
    // Read out Dipswitches
    DipSettings.init();
    ardfMode.init();
    morseTransmitter.init();

    switch (DipSettings.getNamingScheme())
    {
    case 0:
        morseTransmitter.setStringSet(&MT_ARDF);
        break;
    case 1:
        morseTransmitter.setStringSet(&MT_ARDF_SHORT);
        break;
    case 2:
        morseTransmitter.setStringSet(&MT_CALLSIGN);
        break;
    case 3:
        morseTransmitter.setStringSet(&MT_CALLSIGN2);
        break;
    }
    morseTransmitter.setStringSetIndex(DipSettings.getFoxNum());

    ardfMode.setFoxNum(DipSettings.getFoxNum());
    ardfMode.setMode((ARDF_Mode_t)DipSettings.getMode());

}

int main(void)
{
    // make the LED pin an output for PORTB5
    DDRD |= _BV(DBG1) | _BV(DBG2);

    // Initialize Peripherals
    // - DS3231
    DS3231::getInstance().init();
    DS3231::getInstance().enable32KHz();

    // - Initialize Mode
    setupMode();
    ardfMode.start();

    sei();
    while (1)
    {
        // Loop Functions determine Sleep Mode
        uint8_t sleepModeMode = ardfMode.loop();
        uint8_t sleepModeTransmitter = morseTransmitter.loop();
        
        // Keep lowest sleep Mode, IDLE = 1, Extended Standby = 7 So higher is more risk (timers get shutdown after IDLE)
        uint8_t sleepMode = sleepModeMode < sleepModeTransmitter ? sleepModeMode : sleepModeTransmitter;

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
