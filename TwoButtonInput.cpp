#include "TwoButtonInput.h"
#include <inttypes.h>

#define TBI_LONG_TIME 500
#define TBI_BOUNCE_TIME 30

TwoButtonInput::TwoButtonInput(uint8_t b1, uint8_t b2)
{
  _b1 = b1;
  _b2 = b2;
}

void TwoButtonInput::init(void) 
{
  pinMode(_b1, INPUT);
  pinMode(_b2, INPUT);
}

uint8_t TwoButtonInput::_longPress(uint8_t b)
{
  unsigned long start = millis();
  for(;;) {
    int r = digitalRead(b);
    if(r==0 && millis() - start > TBI_BOUNCE_TIME) {
      break;
    }
  }
  if(millis() - start > TBI_LONG_TIME) {
    return(TBI_LONG);
  } else {
    return(TBI_SHORT);
  }
}

uint8_t TwoButtonInput::read(void)
{
  int b1 = digitalRead(_b1);
  int b2 = digitalRead(_b2);
  if(!(b1 || b2)) {
    return(TBI_NO_BUTTON);
  }
  if(b1) {
    return(TBI_BUTTON1 ^ _longPress(_b1));
  } else {
    return(TBI_BUTTON2 ^ _longPress(_b2));
  }
}
