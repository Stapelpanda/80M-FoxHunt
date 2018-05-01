#include <stdint.h>
#include <avr/sleep.h>
#include "State.h"

extern State_t State;

void CONTINUOUS_Start(){
    ;
}
uint8_t CONTINUOUS_WatchDog(){

    return SLEEP_MODE_IDLE;
}
void CONTINUOUS_Stop(){
    ;
}

