#include <stdint.h>
#include <avr/sleep.h>

#include "DS3231.h"
#include <stdint.h>
#include <avr/io.h>

#include "State.h"

extern State_t State;

void ARDF7_Start(){

}
uint8_t ARDF7_WatchDog()
{
    return SLEEP_MODE_EXT_STANDBY;
}
void ARDF7_Stop(){

}