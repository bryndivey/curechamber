#include "CureMenu.h"
#include <avr/pgmspace.h>
#include <stdarg.h>

#define NO_SELECTION -123

#define OPT_TEMPERATURE 0
#define OPT_HUMIDITY 1
#define OPT_MODE 2
#define OPT_CONTROL 3

prog_char s0[] PROGMEM = "Temperature";
prog_char s1[] PROGMEM = "Humidity";
prog_char s2[] PROGMEM = "Mode";
prog_char s3[] PROGMEM = "Control";
prog_char s4[] PROGMEM = "Active";
prog_char s5[] PROGMEM = "Passive";
prog_char s6[] PROGMEM = "Fridge";
prog_char s7[] PROGMEM = "Fan";
prog_char s8[] PROGMEM = "Humidifier";
prog_char s9[] PROGMEM = "Off";
prog_char s10[] PROGMEM = "On";
const char *options[] PROGMEM = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10};

#define OPTION_OFFSET_MAIN 0
#define OPTION_OFFSET_MODE 4
#define OPTION_OFFSET_CONTROL 6
#define OPTION_OFFSET_ENABLED 9
#define MAX_OPTION 10


void get_string(const char** options, uint8_t index, char *buffer) {
  if(index > MAX_OPTION) {
    strcpy(buffer, "error");
  } else {
    strcpy_P(buffer, (char*)pgm_read_word(&(options[index])));
  }
}

void debug(const char *fmt, ...) {
  char buffer[32];
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  Serial.println(buffer);
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

  display_callback(config->lcd, i);
  
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
  get_string(options, i+OPTION_OFFSET_MODE, buffer);
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

void control_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[12];
  get_string(options, i+OPTION_OFFSET_CONTROL, buffer);
  lcd->clear();
  lcd->print(buffer);
}

void enabled_displayer(LiquidCrystal_I2C *lcd, int i)
{
  char buffer[12];
  get_string(options, i+OPTION_OFFSET_ENABLED, buffer);
  lcd->clear();
  lcd->print(buffer);
}

int select_control(CureConfig *config) {
  int temp = generic_selector(config, 0, 2, 0, 1, control_displayer);
  bool *dest;
  if(temp == NO_SELECTION) {
    return(0);
  } else if(temp == 0) {
    dest = &config->fridge_on;
  } else if(temp == 1) {
    dest = &config->fan_on;
  } else if(temp == 2) {
    dest = &config->humidifier_on;
  }
  temp = generic_selector(config, 0, 1, *dest, 1, enabled_displayer);
  if(temp != *dest) {
    *dest = temp;
  }
  select_control(config);
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
  Serial.println("HERE");
  int temp = generic_selector(config, 0, 3, 0, 1, menu_displayer);
  if(temp == OPT_TEMPERATURE) {
    select_temperature(config);
  } else if (temp == OPT_HUMIDITY) {
    select_humidity(config);
  } else if (temp == OPT_MODE) {
    select_mode(config);
  } else if (temp == OPT_CONTROL) {
    select_control(config);
  } else if (temp == NO_SELECTION) {
    return(0);
  }
  run_menu(config);
}
