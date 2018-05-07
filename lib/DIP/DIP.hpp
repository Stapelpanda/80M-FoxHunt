#ifndef __DIP_H__
#define __DIP_H__

#include <avr/io.h>

#define DIP_Init()          \
    {                       \
        DDRC = DDRC & 0xF0; \
        PORTC = 0x0F;       \
        DDRD = DDRD & 0x3C; \
        PORTD = 0xC3;       \
    }

#define DIP_Read_1() ((PIND & 0x40) > 0)
#define DIP_Read_2() ((PIND & 0x80) > 0)
#define DIP_Read_3() ((PIND & 0x02) > 0)
#define DIP_Read_4() ((PIND & 0x01) > 0)

#define DIP_Read_5() ((PINC & 0x08) > 0)
#define DIP_Read_6() ((PINC & 0x04) > 0)
#define DIP_Read_7() ((PINC & 0x02) > 0)
#define DIP_Read_8() ((PINC & 0x01) > 0)

class DIP
{
  private:
    uint8_t foxNum;
    uint8_t namingScheme;
    uint8_t mode;
    uint8_t debug;

  public:
    void init()
    {
        readSwitches();
    }
    void readSwitches()
    {
        mode = (DIP_Read_5() << 1 | DIP_Read_4());
        foxNum = (DIP_Read_3() << 2 | DIP_Read_2() << 1 | DIP_Read_1());
        namingScheme = (DIP_Read_7() << 1 | DIP_Read_6());
        debug = (DIP_Read_8());
    }
    uint8_t getFoxNum()
    {
        return (foxNum & 0x07);
    }
    uint8_t getMode()
    {
        return (mode & 0x03);
    }
    uint8_t getNamingScheme()
    {
        return (namingScheme & 0x03);
    }
    uint8_t getDebug()
    {
        return (debug & 0x01);
    }
};

#endif