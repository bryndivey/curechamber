#ifndef CureChamber_h
#define CureChamber_h

#include <inttypes.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "TwoButtonInput.h"

/*
 * Application configuration
 */

typedef struct {
  uint8_t temperature;
  uint8_t humidity;
  
  bool fridge_on;
  bool humidifier_on;
  bool light_on;
  bool fan_on;
  
  bool mode;

  DHT *dht;
  LiquidCrystal_I2C *lcd;
  TwoButtonInput *tbi;
} CureConfig;


/*
 * Current application state
 */

typedef struct {
  float temperature;
  float humidity;
  bool redraw;
  
  bool fridge_on;
  bool humidifier_on;
  bool fan_on;
  bool light_on;
} CureState;

#endif
