#ifndef ARDF_MODE_HPP
#define ARDF_MODE_HPP

#include <stdint.h>
#include "DS3231.hpp"

typedef enum {
  ARDF = 0,
  ARDF_Short = 1,
  ARDF7 = 2,
  ARDF7_Short = 3,
} ARDF_Mode_t;

typedef enum ARDF_State_e {
  ARDF_STATE_INIT,
  ARDF_STATE_WAIT_FOR_TRANSMIT,
  ARDF_STATE_TRANSMIT
} ARDF_State_t;

class ARDF_Mode
{
private:
  DS3231_Time NextAlarm;
  bool NextAlarmSet;

  uint8_t FoxNum;
  ARDF_Mode_t Mode;
  ARDF_State_t State;

  void stateMachine();

  void roundAlarmToFrame(DS3231_Time *currentTime, DS3231_Time *alarm);
  
  uint16_t initialDelay();
  uint16_t transmitDelay();
  uint16_t waitDelay();  
  uint16_t largestDelay();
  
  bool addSecondsAndSetAlarm(uint16_t seconds);

public:
  void init();

  void setMode(ARDF_Mode_t ardfMode);
  void setFoxNum(uint8_t foxNum);

  void start();
  void stop();
  uint8_t loop();
};

#endif