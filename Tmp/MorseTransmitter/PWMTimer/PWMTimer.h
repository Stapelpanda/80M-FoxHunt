#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__

#define ENABLE_OC1A 
#define PWM_TIMER_FREQ_DIV 2

void PWMTimer_Init();

void PWMTimer_EnableOutput();
void PWMTimer_DisableOutput();

#endif