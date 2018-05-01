#include <avr/io.h>
#include <string.h>

#include "State.h"
#include "PWMTimer/PWMTimer.h"
#include "Timer0/Timer0.h"

#include "MorseTransmitter.hpp"
#include "MorseTable.h"

extern State_t State;
enum {
    MT_Start,
    MT_Transmit,

    MT_Dah, // -
    MT_Dit, // .
    MT_Pause, // Pause After Dah or Dit
    MT_CharPause, // Pause After Char

    MT_WordPause // Pause after word
};

void MorseTransmitter::init()
{
    PWMTimer_Init();
    Timer0_Init();

    // Default Settings
    wpm = 12;
}

void MorseTransmitter::setSpeed(uint8_t wpm)
{
    wpm = wpm;
}
void MorseTransmitter::setStringSet(MorseString_t* set){
    morseStrings = set;
}
void MorseTransmitter::setStringSetIndex(uint8_t idx){
    if(morseStrings != 0){
        if(morseStrings->count > idx){
            morseStringIndex = idx;
        }
    }
}

void MorseTransmitter::setStart(){
    PWMTimer_DisableOutput();
    state = MT_Start;
}

void MorseTransmitter_StateMachine(){
    // Tricky Bit, State machine is !!FALL THROUGH CASE STATEMENT!!
    switch(MTVars.state){
        case MT_Start:
            // Set CharIdx to 0, start Transmit if all variables are set
            MTVars.morseStringCharIndex = 0;
            MTVars.morsePattern = 0;
            if(MTVars.morseStrings == 0){
                return;
            }
            MTVars.state = MT_Transmit;
        case MT_Transmit:
            // Morse pattern is built like this:
            //  Z => ..--
            // So in binary this is 0011, reverse and add 1 in front: 1 1100. 
            // Check last bit and divide by 2, if 1 is reached the morse is over
            if(MTVars.morsePattern == 0){
                // Resolve Errors by setting them to default
                if(MTVars.morseStringIndex >= MTVars.morseStrings->count)
                    MTVars.morseStringIndex = 0;
                if(MTVars.morseStringCharIndex >= strlen(MTVars.morseStrings->elements[MTVars.morseStringIndex]))
                    MTVars.morseStringCharIndex = 0;

                char morseStringChar = MTVars.morseStrings->elements[MTVars.morseStringIndex][MTVars.morseStringCharIndex];
                if(morseStringChar == '\0'){
                    MTVars.state = MT_WordPause;
                }else{
                    for(int i = 0; i < morsetab_SIZE; i++){
                        if(morseStringChar == morsetab[i].c){
                            MTVars.morsePattern = morsetab[i].pat;
                            break;
                        }
                    }
                }
            }
            // Dont do else, Above can set to right MorseTab
            if(MTVars.morsePattern == 1){
                MTVars.state = MT_CharPause;
                MTVars.morsePattern = 0;
            }else if(MTVars.morsePattern > 1){
                if(MTVars.morsePattern & 0x01){
                    MTVars.state = MT_Dah;
                }else{
                    MTVars.state = MT_Dit;
                }
                PWMTimer_EnableOutput();
                // Shift pattern
                MTVars.morsePattern >>= 1;
            }
            break;
        
        case MT_Dah:
        case MT_Dit:
            PWMTimer_DisableOutput();
            MTVars.state = MT_Pause;
            break;
        
        case MT_Pause:
        case MT_CharPause:
            MTVars.state = MT_Transmit;
            break;
            
        case MT_WordPause:
            MTVars.state = MT_Start;
            break;
    }
}

uint8_t MorseTransmitter_SetTimerByState(){
    // ElementsPerSecond = (PARIS * WPM)
    // Second = 32 * 1024
    // elementDelay = Second / ElementsPerSecond
    uint16_t elementDelay = (32UL * 1024UL) / (50UL * MTVars.wpm);

    /* Timing Scheme:
     *  DI              => 1 Element
     *  DAH             => 3 Elements
     *  (Pause)         => 1 Element
     *  [Character End] => 3 Elements
     *  #Word End#      => 7 Elements
     * 
     *  .-- .-.- => 1 (1) 3 (1) 3 (1) [3] 1 (1) 3 (1) 1 (1) 3 (1) #7#
     */

    switch(MTVars.state){
        case MT_CharPause:
        case MT_Dah:
            Timer0_SetTimer(elementDelay * 3);
            return 1;
        case MT_Pause:
        case MT_Dit:
            Timer0_SetTimer(elementDelay * 1);
            return 1;        
        case MT_WordPause:
            // Since CharPause is always included WordPause = 4
            Timer0_SetTimer(elementDelay * 4);
            return 1;
    }
    return 0;
}

uint8_t MorseTransmitter_Loop()
{
    // Call State Machine
    if(MTVars.state == MT_Start || Timer1_HasInterrupt()) {
        MorseTransmitter_StateMachine();
        if(Timer1_HasInterrupt()){
            Timer1_ClearInterrupt();
        }

        // Call SetTimerByState
        if(MorseTransmitter_SetTimerByState() == 1){
            return SLEEP_MODE_IDLE;
        }
    }    
    return SLEEP_MODE_EXT_STANDBY;
}