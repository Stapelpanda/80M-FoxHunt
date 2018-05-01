#ifndef __ARDF7_H__
#define __ARDF7_H__

void ARDF7_Start();
uint8_t ARDF7_WatchDog();
void ARDF7_Stop();

Mode_t ARDF7_Mode = { ARDF7_Start, ARDF7_WatchDog, ARDF7_Stop };

#endif