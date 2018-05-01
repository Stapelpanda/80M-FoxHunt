#ifndef __TIMER0_H__
#define __TIMER0_H__

#include <stdint.h>

void Timer0_Init();

void Timer0_SetTimer(uint16_t offsetFromNow);
uint8_t Timer0_HasInterrupt();
void Timer0_ClearInterrupt();

#endif