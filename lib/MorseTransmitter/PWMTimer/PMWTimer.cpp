#include <avr/io.h>
#include "PWMTimer.hpp"

void PWMTimer::init()
{
#ifdef ENABLE_OC1A
    DDRB |= _BV(PB1); 
    PORTB &= ~_BV(PB1); 
#endif
#ifdef ENABLE_OC1B
    DDRB |= _BV(PB2); 
    PORTB &= ~_BV(PB2); 
#endif

    // Match on 1
    OCR1A = 0x0001;    
    TCCR1A = 0;
    TCCR1B = _BV(WGM12) | _BV(CS10);
    
}

void PWMTimer::enableOutput()
{
    #ifdef ENABLE_OC1A
    TCCR1A |= _BV(COM1A0);
    #endif
}

void PWMTimer::disableOutput()
{
    #ifdef ENABLE_OC1A
    TCCR1A &= ~_BV(COM1A0);
    PORTB &= ~_BV(PB1);
    #endif
}
