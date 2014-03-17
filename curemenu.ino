#include "utility/twi.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "TwoButtonInput.h"
#include "CureMenu.h"
#include "DHT.h"


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

DHT dht(6, DHT22);
LiquidCrystal_I2C lcd(0x20, 16, 2);
TwoButtonInput tbi(3, 4);
CureConfig config = {12, 75, 0, &lcd, &tbi};

#define CC_RUNNING 0
#define CC_MENU_REQUESTED 1
#define CC_MODE_ACTIVE 0
#define CC_MODE_PASSIVE 1

// menu request state
volatile uint8_t state = CC_RUNNING;

// current temps
float humidity = 0.0;
float temperature = 0.0;
bool redraw = true;

void request_menu()
{
	state = CC_MENU_REQUESTED;
}

void set_led(uint8_t r, uint8_t g, uint8_t b)
{
	analogWrite(RED_LED, r);
	analogWrite(GREEN_LED, g);
	analogWrite(BLUE_LED, b);
}

void setup() {
	lcd.init();
	lcd.backlight();
	Serial.begin(9600);
	attachInterrupt(1, request_menu, RISING);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(FAN_RELAY, OUTPUT);
	pinMode(HUMIDIFIER_RELAY, OUTPUT);
	set_led(0, 0, 0);
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
		config.fan_on ? "y" : "n",
		config.humidifier_on ? "y" : "n");
	lcd.clear();
	lcd.print(buffer);
}

void get_state() {
	float t = dht.readTemperature();
	float h = dht.readHumidity();
	redraw = (t != temperature || h != humidity) ? true : false;

	temperature = t;
	humidity = h;

	config.fan_on = (temperature > config.temperature && config.mode == CC_MODE_ACTIVE) ? true : false;
	config.humidifier_on = (humidity < config.humidity && config.mode == CC_MODE_ACTIVE) ? true : false;
}

void set_led() 
{
	analogWrite(10, config.humidifier_on ? 32 : 0);
	analogWrite(11, config.fan_on ? 32 : 0);
}

void set_relays()
{
	digitalWrite(FRIDGE_RELAY, config.fan_on ? HIGH : LOW);
	digitalWrite(FAN_RELAY, config.fan_on ? HIGH : LOW);
	digitalWrite(HUMIDIFIER_RELAY, config.humidifier_on ? HIGH : LOW);
}

void loop() {
	if(state == CC_RUNNING) {
		get_state();
		if(redraw) {
			print_state();
			lcd.setCursor(0, 1);
			print_config();
			set_led(0, config.humidifier_on ? 32 : 0, config.fan_on ? 32: 0);
		}
		set_relays();
		delay(300);
	} else if(state == CC_MENU_REQUESTED) {
		set_led(128, 128, 0);
		run_menu(&config);
		state = CC_RUNNING;
		redraw = true;
	}

}

