#ifndef ARDF_MODE_HPP
#define ARDF_MODE_HPP

#include <stdint.h>
#include "DS3231.hpp"

typedef enum {
  ARDF,
  ARDF_Short,
  Continuous,
} ARDF_Mode_t;

typedef enum ARDF_State_e
{
  ARDF_STATE_INIT,
  ARDF_STATE_WAIT_FOR_TRANSMIT,
  ARDF_STATE_TRANSMIT,
  ARDF_STATE_END_TRANSMIT,
} ARDF_State_t;

class ARDF_Mode
{
private:
  DS3231_Time NextAlarm;
  uint8_t FoxNum;
  ARDF_Mode_t Mode;
  ARDF_State_t State;

public:
  void init();

  void setMode(ARDF_Mode_t ardfMode);
  void setFoxNum(uint8_t foxNum);

  void start();
  void stop();
  uint8_t loop();
};

#endif