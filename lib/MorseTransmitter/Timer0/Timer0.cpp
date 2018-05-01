#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "Timer0.hpp"

bool Timer0_Interrupt = false;
uint32_t Timer0_Remainder = 0;
ISR(TIMER0_COMPA_vect)
{
    if(Timer0_Remainder > 0x100){
        Timer0_Remainder -= 0x100;
        if(Timer0_Remainder < 0x100){
            OCR0A = Timer0_Remainder;
            Timer0_Remainder = 0;
        }
    }else{
        Timer0_Interrupt = true;
        // Disable Interrupt
        TIMSK0 &= ~_BV(OCIE0A);
    }
}

void Timer0::init()
{
    // Setup PULL-Up on T0
    PORTD |= _BV(PD4); //TODO: ADD EXTERNAL PULLUP

    // Setup Timer
    TCCR0A = 0x00;
    TCCR0B = _BV(CS02) | _BV(CS01); // Setup T1 on Falling Edge (Pull-Up is not that powerfull)
}

void Timer0::setCompare(uint8_t offsetFromNow){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        OCR0A = TCNT0 + offsetFromNow;
    }
}
void Timer0::setTimer(uint32_t offsetFromNow)
{
    // Clear Interrupt, just to be save
    TIFR0 |= _BV(OCF0A);
    
    Timer0_Remainder = offsetFromNow;
    if(Timer0_Remainder > 0xFF){
        TCNT0 = 0x00;
        OCR0A = 0xFF;
    }else{
        Timer0_Remainder = 0;
        setCompare(offsetFromNow & 0xFF);
    }

    // Enable Interrupt
    TIMSK0 |= _BV(OCIE0A);
}
uint8_t Timer0::hasInterrupt()
{
    return Timer0_Interrupt;
}
void Timer0::clearInterrupt()
{
    Timer0_Interrupt = false;
}