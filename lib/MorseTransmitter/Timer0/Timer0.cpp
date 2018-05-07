#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "Timer0.hpp"

volatile bool Timer0::interruptOccured = false;
volatile uint32_t Timer0::remainderTime = 0;

ISR(TIMER0_OVF_vect)
{
    if (Timer0::remainderTime >= 0x100)
    {
        Timer0::remainderTime -= 0x100;
    }
    if (Timer0::remainderTime < 0x100)
    {
        OCR0A = Timer0::remainderTime;
        Timer0::remainderTime = 0;
        TIMSK0 = _BV(OCIE0A);
    }
}

ISR(TIMER0_COMPA_vect)
{
    Timer0::interruptOccured = true;
    // Disable Interrupts
    TIMSK0 = 0x00;
}

void Timer0::init()
{
    // Setup PULL-Up on T0
    PORTD |= _BV(PD4); //TODO: ADD EXTERNAL PULLUP

    // Setup Timer
    TCNT0 = 0;
    TCCR0A = 0x00;
    TCCR0B = _BV(CS02) | _BV(CS01); // Setup T1 on Falling Edge (Pull-Up is not that powerfull)
    TIMSK0 = 0x00;
    TIFR0 = 0x00;
}
void Timer0::setTimer(uint32_t offsetFromNow)
{
    // Clear all Pending interrupts
    TIFR0 = 0xFF;
    interruptOccured = 0;

    if (offsetFromNow > 0xFF)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            TCNT0 = 0;
            remainderTime = offsetFromNow;
            TIMSK0 = _BV(TOIE0);
        }
    }
    else
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            TCNT0 = 0;
            OCR0A = offsetFromNow;
            TIMSK0 = _BV(OCIE0A);
        }
    }
}

bool Timer0::hasInterrupt()
{
    return interruptOccured;
}
void Timer0::clearInterrupt()
{
    interruptOccured = false;
}