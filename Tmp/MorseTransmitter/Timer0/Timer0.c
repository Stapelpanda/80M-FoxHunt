#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "Timer0.h"

void Timer0_Init(){
    // Setup PULL-Up on T1
    PORTD |= _BV(PD4); //TODO: ADD EXTERNAL PULLUP
    // Setup Timer
    TCCR1A = 0x00;
    TCCR1B = _BV(CS12) | _BV(CS11); // Setup T1 on Falling Edge (Pull-Up is not that powerfull)

    // Enable RTC 32Khz Output
}

void Timer0_SetTimer(uint16_t offsetFromNow){
    // Clear Interrupt
    TIFR1 |= _BV(OCFA);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        OCR1A = TCNT1 + offsetFromNow;
    }

    // Enable Interrupt
    TIMSK1 |= _BV(OCIE1A);    
}
uint8_t Timer0_HasInterrupt(){
    return (TIFR1 & _BV(OCFA));
}
void Timer0_ClearInterrupt(){
    // Clear Interrupt & Disable Interrupt
    TIFR1 |= _BV(OCFA);
    TIMSK1 &= ~_BV(OCIE1A);
}