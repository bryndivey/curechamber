#include "utility/twi.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "TwoButtonInput.h"
#include "CureMenu.h"
#include "DHT.h"

/* 

Pins:

9, 10, 11: RGB terminals of LED
7: DHT22
2, 3: buttons
A4, A5: I2C LCD
G/5V - board

*/


DHT dht(7, DHT22);
LiquidCrystal_I2C lcd(0x20, 16, 2);
TwoButtonInput tbi(2, 3);
CureConfig config = {12, 75, 0, &lcd, &tbi};

#define CC_RUNNING 0
#define CC_MENU_REQUESTED 1
#define CC_MODE_ACTIVE 0
#define CC_MODE_PASSIVE 1

// menu request state
volatile uint8_t state = CC_RUNNING;

// fridge + humidifier states
bool fridge_on = 0;
bool humidifier_on = 0;

// current temps
float humidity = 0.0;
float temperature = 0.0;
bool redraw = true;

void request_menu()
{
	state = CC_MENU_REQUESTED;
}

void setup() {
	lcd.init();
	lcd.backlight();
	Serial.begin(9600);
	attachInterrupt(0, request_menu, RISING);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	analogWrite(10, 0);
	analogWrite(11, 0);
}

void print_config()
{
	char buffer[16];
	sprintf(buffer, "%d.0C %d.0%% %s", config.temperature, config.humidity, config.mode == 0 ? "A" : "P");
	lcd.print(buffer);
}

void print_state()
{
	int inttemp = (int)temperature;
	int inthumidity = (int)humidity;

	int tempd = ((int)(10 * temperature)) - 10 * inttemp;
	int humidityd = ((int)(10 * humidity)) - 10 * inthumidity;

	char buffer[17];
	sprintf(buffer, "%d.%dC %d.%d%% F%sH%s", 
		inttemp, tempd, inthumidity, humidityd,
		fridge_on ? "y" : "n",
		humidifier_on ? "y" : "n");
	lcd.clear();
	lcd.print(buffer);
}

void get_state() {
	float t = dht.readTemperature();
	float h = dht.readHumidity();
	redraw = (t != temperature || h != humidity) ? true : false;

	temperature = t;
	humidity = h;

	fridge_on = (temperature > config.temperature && config.mode == CC_MODE_ACTIVE) ? true : false;
	humidifier_on = (humidity < config.humidity && config.mode == CC_MODE_ACTIVE) ? true : false;
}

void set_led() 
{
	analogWrite(10, humidifier_on ? 32 : 0);
	analogWrite(11, fridge_on ? 32 : 0);
}

void loop() {
	if(state == CC_RUNNING) {
		get_state();
		if(redraw) {
			print_state();
			lcd.setCursor(0, 1);
			print_config();
			set_led();
		}
		delay(300);
	} else if(state == CC_MENU_REQUESTED) {
		run_menu(&config);
		state = CC_RUNNING;
		redraw = true;
	}

}
