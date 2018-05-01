#include <stdint.h>
#include <avr/sleep.h>
#include "State.h"

extern State_t State;

void FAST_Start(){
    ;
}
uint8_t FAST_WatchDog(){

    return SLEEP_MODE_IDLE;
}
void FAST_Stop(){
    ;
}

