#ifndef __CONTINUOUS_H__
#define __CONTINUOUS_H__

void CONTINUOUS_Start();
uint8_t CONTINUOUS_WatchDog();
void CONTINUOUS_Stop();

Mode_t CONTINUOUS_Mode = { CONTINUOUS_Start, CONTINUOUS_WatchDog, CONTINUOUS_Stop };

#endif