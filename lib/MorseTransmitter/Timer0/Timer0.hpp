#ifndef __TIMER0_H__
#define __TIMER0_H__

#include <stdint.h>

class Timer0
{
  
public:
  static volatile bool interruptOccured;
  static volatile uint32_t remainderTime;

  void init();

  void setTimer(uint32_t offsetFromNow);
  bool hasInterrupt();
  void clearInterrupt();
};

#endif