#ifndef icons_h
#define icons_h

// icons
uint8_t icon_fan[8] = {
  B00000,
  B11001,
  B01011,
  B00100,
  B11010,
  B10011,
};

uint8_t icon_humidifier[8] = {
  B00110,
  B01000,
  B00100,
  B00010,
  B00100,
  B11111,
  B11111,
};

uint8_t icon_fridge[8] = {
  B11111,
  B10001,
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
};

uint8_t icon_active[8] = {
  B00000,
  B11111,
  B10001,
  B11111,
  B10001,
  B10001,
  B00000,
};

uint8_t icon_passive[8] = {
  B00000,
  B11111,
  B10001,
  B11111,
  B10000,
  B10000,
  B00000,
};

#define ICON_FAN 0
#define ICON_HUMIDIFIER 1
#define ICON_FRIDGE 2
#define ICON_ACTIVE 3
#define ICON_PASSIVE 4

#endif
