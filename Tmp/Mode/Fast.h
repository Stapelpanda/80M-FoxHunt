#ifndef __FAST_H__
#define __FAST_H__

void FAST_Start();
uint8_t FAST_WatchDog();
void FAST_Stop();

Mode_t FAST_Mode = { FAST_Start, FAST_WatchDog, FAST_Stop };

#endif