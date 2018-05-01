#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__

#define ENABLE_OC1A
#define PWM_TIMER_FREQ_DIV 2

class PWMTimer
{
  public:
    void init();

    void enableOutput();
    void disableOutput();

    static PWMTimer &getInstance()
    {
        static PWMTimer INSTANCE;
        return INSTANCE;
    }
};

#endif