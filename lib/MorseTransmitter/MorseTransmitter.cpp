#include <avr/io.h>
#include <string.h>

#include "MorseTransmitter.hpp"
#include "MorseTable.h"

#include "PWMTimer/PWMTimer.hpp"
#include "Timer0/Timer0.hpp"

PWMTimer pwmOutput;
Timer0 delayTimer;

enum
{
    MT_Start,
    MT_Transmit,

    MT_Dah,       // -
    MT_Dit,       // .
    MT_Pause,     // Pause After Dah or Dit
    MT_CharPause, // Pause After Char

    MT_WordPause, // Pause after word

    MT_Stop
};

void MorseTransmitter::init()
{
    setSpeed(12);
    DDRB |= _BV(PB1);

    pwmOutput.init();
    delayTimer.init();

    delayTimerActive = false;
    stop();
}

void MorseTransmitter::setSpeed(uint8_t wpm)
{
    // ElementsPerSecond = (PARIS * WPM)    PARIS = 50 spots
    // Second = 32 * 1024
    // elementDelay = Second / ElementsPerSecond
    elementDelay = (32UL * 1024UL * 60UL) / (50UL * wpm);
}

void MorseTransmitter::setStringSet(MorseString_t *set)
{
    morseStrings = set;
}

void MorseTransmitter::setStringSetIndex(uint8_t idx)
{
    if (morseStrings != 0)
    {
        if (morseStrings->count > idx)
        {
            morseStringIndex = idx;
        }
    }
}

void MorseTransmitter::start()
{
    pwmOutput.disableOutput();
    delayTimer.clearInterrupt();
    state = MT_Start;
}

void MorseTransmitter::stop()
{
    pwmOutput.disableOutput();
    state = MT_Stop;    
}

void MorseTransmitter::stateMachine()
{
    // Tricky Bit, State machine is !!FALL THROUGH CASE STATEMENT!!
    switch (state)
    {
    case MT_WordPause:
        state = MT_Start;
    case MT_Start:
        // Set CharIdx to 0, start Transmit if all variables are set
        morseStringCharIndex = 0;
        morsePattern = 0;
        if (morseStrings == 0)
        {
            return;
        }
    case MT_Pause:
    case MT_CharPause:
        state = MT_Transmit;
    case MT_Transmit:
        // Morse pattern is built like this:
        //  Z => ..--
        // So in binary this is 0011, reverse and add 1 in front: 1 1100.
        // Check last bit and divide by 2, if 1 is reached the morse is over
        if (morsePattern == 0)
        {
            // Resolve Errors by setting them to default
            if (morseStringIndex >= morseStrings->count)
                morseStringIndex = 0;
            if (morseStringCharIndex > strlen(morseStrings->elements[morseStringIndex])){
                morseStringCharIndex = 0;
            }

            char morseStringChar = morseStrings->elements[morseStringIndex][morseStringCharIndex];
            if (morseStringChar == '\0')
            {
                state = MT_WordPause;
            }
            else
            {
                for (uint8_t i = 0; i < morsetab_SIZE; i++)
                {
                    if (morseStringChar == morsetab[i].c)
                    {
                        morsePattern = morsetab[i].pat;
                        break;
                    }
                }
            }
        }
        // Dont do else, Above can set to right MorseTab
        if (morsePattern == 1)
        {
            state = MT_CharPause;
            morsePattern = 0;
            morseStringCharIndex++;
        }
        else if (morsePattern > 1)
        {
            
            if (morsePattern & 0x01)
            {
                state = MT_Dah;
            }
            else
            {
                state = MT_Dit;
            }
            pwmOutput.enableOutput();
            // Shift pattern
            morsePattern >>= 1;            
        }
        break;

    case MT_Dah:
    case MT_Dit:
        pwmOutput.disableOutput();
        state = MT_Pause;
        break;
    }
}

uint8_t MorseTransmitter::setTimerByState()
{
    /* Timing Scheme:
     *  DI              => 1 Element
     *  DAH             => 3 Elements
     *  (Pause)         => 1 Element
     *  [Character End] => 3 Elements
     *  #Word End#      => 7 Elements
     * 
     *  .-- .-.- => 1 (1) 3 (1) 3 (1) [3] 1 (1) 3 (1) 1 (1) 3 (1) #7#
     */
    switch (state)
    {
    case MT_CharPause:
    case MT_Dah:
        delayTimer.setTimer(elementDelay * 3);
        return 1;
    case MT_Pause:
    case MT_Dit:
        delayTimer.setTimer(elementDelay * 1);
        return 1;
    case MT_WordPause:
        // Since CharPause is always included WordPause = 4
        delayTimer.setTimer(elementDelay * 4);
        return 1;
    }
    return 0;
}

uint8_t MorseTransmitter::loop()
{
    // Call State Machine
    if (state == MT_Start || delayTimer.hasInterrupt())
    {
        stateMachine();
        if (delayTimer.hasInterrupt())
        {
            delayTimerActive = false;
            delayTimer.clearInterrupt();
        }

        // Call SetTimerByState, we need Idle for Timer1
        if (setTimerByState() == 1)
        {
            delayTimerActive = true;
            return SLEEP_MODE_IDLE;
        }
    }
    if(delayTimerActive){
        return SLEEP_MODE_IDLE;
    }else{
        return SLEEP_MODE_EXT_STANDBY;
    }
}