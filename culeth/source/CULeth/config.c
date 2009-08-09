#include "config.h"
#include <avr/eeprom.h>
#include <stdint.h>
#include <avr/pgmspace.h>


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

/*
__attribute__((__noinline__))
void eeprom_wn(uint16_t addr, uint8_t *block, uint8_t size) {
	eeprom_write_block((uint16_t*)addr, block, size);
}

__attribute__((__noinline__))
void eeprom_rn(uint16_t addr, uint8_t *block, uint8_t size) {
	eeprom_read_block(block, (uint16_t*)addr, size);
}
*/

/*
 * Configuration
 */

// Defaults
const PROGMEM prog_uint8_t target_ip[2][4]= {{192,168,108,1},{192,168,108,1}};
const PROGMEM prog_uint16_t target_port0= 2211;
const PROGMEM prog_uint16_t target_port1= 2212;
const PROGMEM prog_uint8_t server_ip[2][4]= {{192,168,108,101},{192,168,108,102}};
const PROGMEM prog_uint16_t server_port = 49152;
const PROGMEM prog_uint8_t server_mac[2][6]= {{0x02, 0x50, 0x8b, 0x00, 0x00, 0x01},
					      {0x02, 0x50, 0x8b, 0x00, 0x00, 0x02}};
const PROGMEM prog_uint8_t adapter_mac[2][6]= {{0x02, 0x00, 0x02, 0x00, 0x02, 0x01},
					       {0x02, 0x00, 0x02, 0x00, 0x02, 0x02}};

void factory_reset(uint8_t cfg) {
	eeprom_w2(CFG_OFS_MAGIC, CFG_MAGIC);		// magic
	eeprom_w2(CFG_OFS_VERSION, CFG_VERSION);	// version

	// don't start bootloader
	eeprom_w1(CFG_OFS_REQBL, CFG_NOREQBL);

	// adapter MAC
	for(int i= 0; i< 6; i++) {
		eeprom_w1(CFG_OFS_AMAC+i, __LPM(adapter_mac[cfg]+i));
	}

	// target IP and port
	for(int i= 0; i< 4; i++) {
		eeprom_w1(CFG_OFS_TIP+i, __LPM(target_ip[cfg]+i));
	}
	if(cfg) {
		eeprom_w2(CFG_OFS_TPORT, target_port1);
	} else {
		eeprom_w2(CFG_OFS_TPORT, target_port0);
	}

	// CULserver IP and port
	for(int i= 0; i< 4; i++) {
		eeprom_w1(CFG_OFS_SIP+i, __LPM(server_ip[cfg]+i));
	}
	eeprom_w2(CFG_OFS_SPORT, server_port);

	// CULserver MAC
	for(int i= 0; i< 6; i++) {
		eeprom_w1(CFG_OFS_SMAC+i, __LPM(server_mac[cfg]+i));
	}

}

void config_Init() {

	uint16_t magic;
	uint16_t version;

	magic= eeprom_r2(CFG_OFS_MAGIC);
	if(magic!=CFG_MAGIC) factory_reset(0);
	version= eeprom_r2(CFG_OFS_VERSION);
	if(version!=CFG_VERSION) factory_reset(0);
}

uint8_t	get_config_byte(uint16_t config) {
	return eeprom_r1(config);
}

uint16_t get_config_word(uint16_t config) {
	return eeprom_r2(config);
}

void get_config_n(uint16_t config, uint8_t *block, uint8_t size) {
	for(uint8_t i= 0; i< size; i++) block[i]= eeprom_r1(config+i);
	//return eeprom_rn(config, block, size);
}

void set_config_byte(uint16_t config, uint8_t value) {
	eeprom_w1(config, value);
}

void set_config_word(uint16_t config, uint16_t value) {
	eeprom_w2(config, value);
}

void set_config_n(uint16_t config, uint8_t *value, uint8_t size) {
	for(uint8_t i= 0; i< size; i++) eeprom_w1(config+i, value[i]);
	//eeprom_wn(config, value, size);
}


