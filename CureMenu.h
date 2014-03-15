#ifndef CureMenu_h
#define CureMenu_h

#include <inttypes.h>
#include <LiquidCrystal_I2C.h>
#include "TwoButtonInput.h"

typedef struct {
  uint8_t temperature;
  uint8_t humidity;
  bool mode;
  LiquidCrystal_I2C *lcd;
  TwoButtonInput *tbi;
} CureConfig;

int run_menu(CureConfig* config);
int select_number(CureConfig* config);
int select_temperature(CureConfig* config);
int select_humidity(CureConfig* config);

#endif
