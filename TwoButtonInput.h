#ifndef TwoButtonInput_h
#define TwoButtonInput_h

#include <inttypes.h>
#include "Arduino.h"

#define TBI_NO_BUTTON 0
#define TBI_SHORT 0
#define TBI_LONG 128
#define TBI_BUTTON1 1
#define TBI_BUTTON2 2

class TwoButtonInput {
 public:
  TwoButtonInput(uint8_t b1, uint8_t b2);
  void init(void);
  uint8_t read(void);

 private:
  uint8_t _longPress(uint8_t b);
  uint8_t _b1;
  uint8_t _b2;
};

#endif
