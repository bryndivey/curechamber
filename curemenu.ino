#include "utility/twi.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "TwoButtonInput.h"
#include "CureMenu.h"
#include "DHT.h"
#include "icons.h"


/* 

Pins:

5, 9, 11: RGB terminals of LED
6: DHT22
3, 4: buttons
A4, A5: I2C LCD
G/5V - board

*/

#define RED_LED 11
#define GREEN_LED 9
#define BLUE_LED 5
#define FAN_RELAY 10
#define HUMIDIFIER_RELAY 8
#define FRIDGE_RELAY 8

#define CC_RUNNING 0
#define CC_MENU_REQUESTED 1
#define CC_MODE_ACTIVE 0
#define CC_MODE_PASSIVE 1

typedef struct {
	float temperature;
	float humidity;
	bool redraw;
	bool fan_on;
	bool fridge_on;
	bool humidifier_on;
} CureState;

// global volative for interrupt-driven menu trigger
// let's keep this as the only global
volatile uint8_t loop_state = CC_RUNNING; 

void request_menu()
{
	// have to use volatile global
	loop_state = CC_MENU_REQUESTED;
}

void set_led(uint8_t r, uint8_t g, uint8_t b)
{
	analogWrite(RED_LED, r);
	analogWrite(GREEN_LED, g);
	analogWrite(BLUE_LED, b);
}


void get_state(CureConfig config, CureState &state) {
	float t = config.dht->readTemperature();
	float h = config.dht->readHumidity();
	state.redraw = (t != state.temperature || h != state.humidity) ? true : false;

	state.temperature = t;
	state.humidity = h;
}

void compute_activity(CureConfig config, CureState &state)
{
	if(config.mode == CC_MODE_ACTIVE) {
		state.fridge_on = (state.temperature > config.temperature) ? true : false;
		state.humidifier_on = (state.humidity < config.humidity) ? true : false;
		state.fan_on = config.fan_on;
	} else if(config.mode == CC_MODE_PASSIVE) {
		state.fridge_on = config.fridge_on;
		state.humidifier_on = config.humidifier_on;
		state.fan_on = config.fan_on;
	}
}

void print_float(char *buffer, float f)
{
	int int_f = (int)f;
	int frac_f = ((int)(10 * f) - 10 * int_f);
	sprintf(buffer, "%d.%d", int_f, frac_f);
}

void display_config(CureConfig config, CureState state)
{
	char buffer[17];
	sprintf(buffer, "%d.0C %d.0%%", config.temperature, config.humidity);
	config.lcd->setCursor(0, 1);
	config.lcd->print(buffer);
}

void display_readings(CureConfig config, CureState state)
{
	char temp[5];
	char humidity[5];
	print_float(temp, state.temperature);
	print_float(humidity, state.humidity);

	char buffer[17];
	sprintf(buffer, "%sC %s%%", temp, humidity);
	config.lcd->setCursor(0, 0);
	config.lcd->print(buffer);
}

void display_actives(CureConfig config, CureState state)
{
	config.lcd->setCursor(14, 0);
	if(state.humidifier_on) {
		config.lcd->write(ICON_HUMIDIFIER);
	}
	config.lcd->setCursor(15, 0);
	if(state.fridge_on) {
		config.lcd->write(ICON_FRIDGE);
	}
	config.lcd->setCursor(14, 1);
	if(state.fan_on) {
		config.lcd->write(ICON_FAN);
	}
	config.lcd->setCursor(15, 1);
	if(config.mode == CC_MODE_ACTIVE) {
		config.lcd->write(ICON_ACTIVE);
	} else {
		config.lcd->write(ICON_PASSIVE);
	}
}

void update_display(CureConfig config, CureState state)
{
	if(state.redraw) {
		config.lcd->clear();
		display_readings(config, state);
		display_config(config, state);
		display_actives(config, state);
	}
}

void set_relays(CureState state)
{
	digitalWrite(FRIDGE_RELAY, state.fridge_on ? HIGH : LOW);
	digitalWrite(FAN_RELAY, state.fan_on ? HIGH : LOW);
	digitalWrite(HUMIDIFIER_RELAY, state.humidifier_on ? HIGH : LOW);
}

void init(CureConfig config)
{
	Serial.begin(9600);
	Serial.println("Initialization beginning");

	attachInterrupt(1, request_menu, RISING);

	// pin modes
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(FAN_RELAY, OUTPUT);
	pinMode(HUMIDIFIER_RELAY, OUTPUT);

	// setup LED 
	set_led(0, 0, 0);

	// initialize LCD
	config.lcd->init();

	config.lcd->createChar(ICON_FAN, icon_fan);
	config.lcd->createChar(ICON_FRIDGE, icon_fridge);
	config.lcd->createChar(ICON_HUMIDIFIER, icon_humidifier);
	config.lcd->createChar(ICON_ACTIVE, icon_active);
	config.lcd->createChar(ICON_PASSIVE, icon_passive);

	config.lcd->backlight();
	config.lcd->clear();
	config.lcd->home();
	config.lcd->println("Starting");

	Serial.println("Initialization complete");
}

void setup() {}

void loop() {
	// initialization
	DHT dht(6, DHT22);
	LiquidCrystal_I2C lcd(0x20, 16, 2);
	TwoButtonInput tbi(3, 4);

	CureConfig config = {12, 75, 1, 0, 0, 0, &dht, &lcd, &tbi};
	CureState state = {0.0, 0.0, true};

	init(config);

	for(;;) {
		Serial.println("Looping");
		if(loop_state == CC_RUNNING) {
			get_state(config, state);

			// in active, figure out whether we need to turn stuff on
			// in passive, just use the config values
			compute_activity(config, state);

			// update display (only if redraw is set)
			update_display(config, state);
			set_led(0, config.humidifier_on ? 32 : 0, config.fan_on ? 32: 0);

			// turn the things! twiddle the knobs!
			// set_relays(state);
			delay(300);
		} else if(loop_state == CC_MENU_REQUESTED) {
			set_led(128, 128, 0);
			run_menu(&config);
			loop_state = CC_RUNNING;
			state.redraw = true;
		}
	}

}

