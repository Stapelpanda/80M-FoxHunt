#ifndef __MORSE_TRANSMITTER_HPP__
#define __MORSE_TRANSMITTER_HPP__ 1

#define maxElements 8
typedef struct
{
  uint8_t count;
  const char* elements[maxElements];
} MorseString_t;


// MorseString_t MT_ARDF_SHORT = {8, {"M", "ME", "MI", "MS", "MH", "M5", "M5E", "M5I"}};
// MorseString_t MT_CALLSIGN = {8, {"PB0MV", "PB0MV E", "PB0MV I", "PB0MV S", "PB0MV H", "PB0MV 5", "PB0MV 5E", "PB0MV 5I"}};
// MorseString_t MT_CALLSIGN2 = {8, {"PI6YRC", "PI6YRC E", "PI6YRC I", "PI6YRC S", "PI6YRC H", "PI6YRC 5", "PI6YRC 5E", "PI6YRC 5I"}};

class MorseTransmitter
{
private:
  uint8_t state;
  uint32_t elementDelay;

  MorseString_t *morseStrings;
  uint8_t morseStringIndex;
  uint8_t morseStringCharIndex;

  uint8_t morsePattern;

  void stateMachine();
  uint8_t setTimerByState();

public:
  void init();

  void setSpeed(uint8_t wpm);
  void setStringSet(MorseString_t *set);
  void setStringSetIndex(uint8_t idx);
  void setStart();
  uint8_t loop();
};

#endif