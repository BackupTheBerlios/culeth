#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

/*
 * configuration constants
 */

#define CFG_MAGIC		(uint16_t)0x1969	// eeprom magic
#define CFG_VERSION		(uint16_t)0x0003	// config version
#define CFG_REQBL		0xb0			// request bootloader
#define CFG_NOREQBL		0x00			// do not request bootloader

/*
 * configuration offsets
 *
 * notice: the processor is little-endian, i.e. we have 0x69 at 0x1fe
 */

#define CFG_OFS_CC1101		0x100

#define CFG_START		0x1e2
#define CFG_LENGTH		(0x1ff-CFG_START+1)

#define CFG_OFS_TIP		0x1e2	// 1e2, 1e3, 1e4, 1e5
#define CFG_OFS_TPORT		0x1e6	// 1e6, 1e7
#define CFG_OFS_SIP		0x1e8	// 1e8, 1e9, 1ea, 1eb
#define CFG_OFS_SPORT		0x1ec	// 1ec, 1ed
#define CFG_OFS_SMAC		0x1ee	// 1ee, 1ef, 1f0, 1f1, 1f2, 1f3
#define CFG_OFS_AMAC		0x1f4	// 1f4, 1f5, 1f6, 1f7, 1f8, 1f9
#define CFG_OFS_RESERVED2	0x1fa	// 1fa
#define CFG_OFS_REQBL		0x1fb	// 1fb
#define CFG_OFS_VERSION       	0x1fc	// 1fc
#define CFG_OFS_MAGIC		0x1fe	// 1fe, 1ff


void factory_reset(uint8_t cfg);
void config_Init(void);

uint8_t	get_config_byte(uint16_t config);
uint16_t get_config_word(uint16_t config);
void get_config_n(uint16_t config, uint8_t *block, uint8_t size);

void set_config_byte(uint16_t config, uint8_t value);
void set_config_word(uint16_t config, uint16_t value);
void set_config_n(uint16_t config, uint8_t *value, uint8_t size);

#endif