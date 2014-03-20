#ifndef CureMenu_h
#define CureMenu_h

#include <inttypes.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "TwoButtonInput.h"

typedef struct {
  uint8_t temperature;
  uint8_t humidity;
  
  bool fan_on;
  bool fridge_on;
  bool humidifier_on;
  
  bool mode;

  DHT *dht;
  LiquidCrystal_I2C *lcd;
  TwoButtonInput *tbi;
} CureConfig;


int run_menu(CureConfig *config);

#endif
