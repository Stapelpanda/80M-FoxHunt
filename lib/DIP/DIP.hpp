#ifndef __DIP_H__
#define __DIP_H__

#include <avr/io.h>

#define DIP_Init() { DDRC = DDRC & 0xF0; PORTC = 0x0F; DDRD = DDRD & 0x3C; PORTD = 0xC3; }

#define DIP_Read_1() ((PIND & 0x40) > 0)
#define DIP_Read_2() ((PIND & 0x80) > 0)
#define DIP_Read_3() ((PIND & 0x02) > 0)
#define DIP_Read_4() ((PIND & 0x01) > 0)

#define DIP_Read_5() ((PIND & 0x08) > 0)
#define DIP_Read_6() ((PIND & 0x04) > 0)
#define DIP_Read_7() ((PIND & 0x02) > 0)
#define DIP_Read_8() ((PIND & 0x01) > 0)

#define DIP_Read_FoxNum() (DIP_Read_5() << 1 | DIP_Read_4())
#define DIP_Read_Mode() (DIP_Read_3() << 2 | DIP_Read_2() << 1 | DIP_Read_1())
#endif