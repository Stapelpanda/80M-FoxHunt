#ifndef __ARDF_H__
#define __ARDF_H__

#include "ARDF_Mode.hpp"
#include "DS3231.hpp"

class ARDF : ARDF_Mode
{
  private:
    const uint8_t FrameStart[12] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
    enum ARDF_State_e
    {
        ARDF_STATE_INIT,
        ARDF_STATE_WAIT_FOR_TRANSMIT,
        ARDF_STATE_TRANSMIT,
        ARDF_STATE_END_TRANSMIT,
    } ARDF_State;

    uint8_t HandleI2C_Error();
    uint8_t getFrameNum(DS3231_Time *currentTime, uint8_t offset);
    uint8_t getFutureAlarm(DS3231_Time *currentTime, uint8_t offset);
    uint8_t setNewAlarm(DS3231_Time *currentTime);
    uint8_t StateMachine(DS3231_Time *currentTime);

  public:
    void Start();
    void Loop(DS3231_Time *currentTime);
    void Stop();
};

#endif