#ifndef __MORSE_TRANSMITTER_HPP__
#define __MORSE_TRANSMITTER_HPP__ 1

#define maxElements 8
typedef struct
{
  uint8_t count;
  const char *elements[maxElements];
} MorseString_t;

class MorseTransmitter
{
private:
  bool delayTimerActive;
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
  void start();
  void stop();
  uint8_t loop();

  static MorseTransmitter &getInstance()
  {
    static MorseTransmitter INSTANCE;
    return INSTANCE;
  }
};

#endif