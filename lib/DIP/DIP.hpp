#ifndef __DIP_H__
#define __DIP_H__

#include <avr/io.h>

#define DIP_Read_1() (1 - ((PIND & _BV(PD6)) >> PD6))
#define DIP_Read_2() (1 - ((PIND & _BV(PD7)) >> PD7))
#define DIP_Read_3() (1 - ((PIND & _BV(PD1)) >> PD1))
#define DIP_Read_4() (1 - ((PIND & _BV(PD0)) >> PD0))

#define DIP_Read_5() (1 - ((PINC & _BV(PC3)) >> PC3))
#define DIP_Read_6() (1 - ((PINC & _BV(PC2)) >> PC2))
#define DIP_Read_7() (1 - ((PINC & _BV(PC1)) >> PC1))
#define DIP_Read_8() (1 - ((PINC & _BV(PC0)) >> PC0))

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
        uint8_t temp;
        temp = _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3);
        DDRC = DDRC & ~temp;
        PORTC |= temp;

        temp = _BV(PD6) | _BV(PD7) | _BV(PD1) | _BV(PD0);
        DDRD = DDRD & ~(temp);
        PORTD |= temp;
        
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