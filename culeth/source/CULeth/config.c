#include "config.h"
#include <avr/eeprom.h>
#include <stdint.h>


/*
 * EEPROM functionality
 */

__attribute__((__noinline__))
void eeprom_w1(uint16_t addr, uint8_t byte) {
	eeprom_write_byte((uint8_t*)addr, byte);
}

__attribute__((__noinline__))
uint8_t eeprom_r1(uint16_t addr) {
	return eeprom_read_byte((uint8_t*)addr);
}

__attribute__((__noinline__))
void eeprom_w2(uint16_t addr, uint16_t word) {
	eeprom_write_word((uint16_t*)addr, word);
}

__attribute__((__noinline__))
uint16_t eeprom_r2(uint16_t addr) {
	return eeprom_read_word((uint16_t*)addr);
}

__attribute__((__noinline__))
void eeprom_wn(uint16_t addr, uint8_t *block, uint8_t size) {
	eeprom_write_block(block, (uint16_t*)addr, size);
}

__attribute__((__noinline__))
void eeprom_rn(uint16_t addr, uint8_t *block, uint8_t size) {
	eeprom_read_block(block, (uint16_t*)addr, size);
}

/*
 * Configuration
 */

void factory_reset() {
	eeprom_w2(CFG_OFS_MAGIC, CFG_MAGIC);	// magic
	eeprom_w2(CFG_OFS_VERSION, CFG_VERSION);// version
	eeprom_w1(CFG_OFS_REQBL, CFG_NOREQBL);	// do not start bootloader
}

void config_Init() {

	uint16_t magic;
	uint16_t version;

	magic= eeprom_r2(CFG_OFS_MAGIC);
	if(magic!=CFG_MAGIC) factory_reset();
	version= eeprom_r2(CFG_OFS_VERSION);
	if(version!=CFG_VERSION) factory_reset();
}

uint8_t	get_config(uint16_t config) {
	return eeprom_r1(config);
}

void set_config(uint16_t config, uint8_t value) {
	eeprom_w1(config, value);
}

