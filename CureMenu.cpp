#include "CureMenu.h"
#include <avr/pgmspace.h>

#define NO_SELECTION -123

#define OPT_TEMPERATURE 0
#define OPT_HUMIDITY 1
#define OPT_MODE 2

prog_char s0[] PROGMEM = "Temperature";
prog_char s1[] PROGMEM = "Humidity";
prog_char s2[] PROGMEM = "Mode";
prog_char s3[] PROGMEM = "Active";
prog_char s4[] PROGMEM = "Passive";

const char *options[] PROGMEM = {s0, s1, s2, s3, s4};


void get_string(const char** options, uint8_t index, char *buffer) {
  strcpy_P(buffer, (char*)pgm_read_word(&(options[index])));
}

int generic_selector(CureConfig *config,
		     uint8_t low,
		     uint8_t high,
		     uint8_t start,
		     uint8_t step,
		     void (*display_callback)(LiquidCrystal_I2C*, int))
{
  int i = start;
  int prev = -1;
  
  for(;;) {
    int r = config->tbi->read();

    if (r & TBI_BUTTON1 && r & TBI_LONG) {
      return(i);
    } else if (r & TBI_BUTTON2 && r & TBI_LONG) {
      return(NO_SELECTION);
    } else if (r & TBI_BUTTON1) {
      i = i + step;
    } else if (r & TBI_BUTTON2) {
      i = i - step;
    }
    
    if(i > high) {
      i = low;
    } else if(i < low) {
      i = high;
    }

    if(i != prev) {
      display_callback(config->lcd, i);
      prev = i;
    }
  }
}

void number_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[10];
  sprintf(buffer, "%d", i);
  lcd->clear();
  lcd->print(buffer);
}

int select_number(CureConfig *config) {
  int number = generic_selector(config, 0, 10, 3, 1, number_displayer);
}


void temperature_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[4];
  sprintf(buffer, "%dC", i);
  lcd->clear();
  lcd->print(buffer);
}

int select_temperature(CureConfig *config) {
  int temp = generic_selector(config,
			      10,
			      25,
			      config->temperature,
			      1,
			      temperature_displayer);
  if(temp != NO_SELECTION) {
    config->temperature = temp;
  }
  return(config->temperature);
}

void humidity_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[4];
  sprintf(buffer, "%d%%", i);
  lcd->clear();
  lcd->print(buffer);
}

int select_humidity(CureConfig *config) {
  int temp = generic_selector(config,
			      50,
			      95,
			      config->humidity,
			      5,
			      humidity_displayer);
  if(temp != NO_SELECTION) {
    config->humidity = temp;
  }
  return(config->humidity);
}

void mode_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[12];
  get_string(options, i+3, buffer);
  lcd->clear();
  lcd->print(buffer);
}

int select_mode(CureConfig *config) {
  int temp = generic_selector(config, 0, 1, config->mode, 1, mode_displayer);
  if(temp != NO_SELECTION) {
    config->mode = temp;
  }
  return(config->mode);
}

void menu_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[12];
  get_string(options, i, buffer);
  lcd->clear();
  lcd->print(buffer);
}

int run_menu(CureConfig *config)
{
  int temp = generic_selector(config, 0, 2, 0, 1, menu_displayer);
  if(temp == OPT_TEMPERATURE) {
    select_temperature(config);
  } else if (temp == OPT_HUMIDITY) {
    select_humidity(config);
  } else if (temp == OPT_MODE) {
    select_mode(config);
  } else if (temp == NO_SELECTION) {
    return(0);
  }
  run_menu(config);
}
