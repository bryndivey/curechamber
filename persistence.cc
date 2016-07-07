#include <EEPROM.h>
#include "persistence.h"

#define CC_PERSISTENCE_OFFSET 0
#define MAGIC 101
#define FRIDGE_MASK 0
#define HUMIDIFIER_MASK 1
#define LIGHT_MASK 2
#define FAN_MASK 3
#define MODE_MASK 4



void update(int loc, uint8_t val) {
    loc = loc + CC_PERSISTENCE_OFFSET;
    if(loc >= EEPROM.length()) {
	// what now?
    }
    EEPROM.write(loc, val);
}

uint8_t read(int loc) {
    return EEPROM.read(loc + CC_PERSISTENCE_OFFSET);
}

uint8_t get_mask(CureConfig *config) {
    return config->fridge_on << FRIDGE_MASK |
	config->humidifier_on << HUMIDIFIER_MASK |
	config->light_on << LIGHT_MASK |
	config->fan_on << FAN_MASK |
	config->mode << MODE_MASK;
}

void save_settings(CureConfig *config) {
    // some kind of versioning?
    update(0, MAGIC);
    update(1, config->temperature);
    update(2, config->humidity);
    update(3, get_mask(config));

    debug("Saved magic as %d and temp as %d", EEPROM.read(0), EEPROM.read(1));
}

int load_settings(CureConfig *config) {
    // don't want random data here
    if(read(0) != MAGIC) {
	debug("Can't find magic marker");
	return 1;
    }
    
    config->temperature = read(1);
    config->humidity = read(2);
    uint8_t mask = read(3);
    config->fridge_on = mask & FRIDGE_MASK;
    config->humidifier_on = mask & HUMIDIFIER_MASK;
    config->light_on = mask & LIGHT_MASK;
    config->fan_on = mask & FAN_MASK;
    config->mode = mask & MODE_MASK;
    debug("Loaded to %dC %d%%", config->temperature, config->humidity);
    
    return 0;
}



