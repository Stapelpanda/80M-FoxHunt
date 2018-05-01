#ifndef __TIMER0_H__
#define __TIMER0_H__

#include <stdint.h>

class Timer0
{
private:
  void setCompare(uint8_t offsetFromNow);

public:
  void init();

  void setTimer(uint32_t offsetFromNow);
  uint8_t hasInterrupt();
  void clearInterrupt();

  static Timer0 &getInstance()
  {
    static Timer0 INSTANCE;
    return INSTANCE;
  }
};

#endif