#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

/*
 * configuration constants
 */

#define CFG_MAGIC		0x1969	// eeprom magic
#define CFG_VERSION		0x0001	// config version
#define CFG_REQBL		0xb0	// request bootloader
#define CFG_NOREQBL		0x00	// do not request bootloader

/*
 * configuration offsets
 */

#define CFG_OFS_CC1101		0x100

#define CFG_OFS_REQBL		0x1fb
#define CFG_OFS_VERSION       	0x1fc
#define CFG_OFS_MAGIC		0x1fe


void factory_reset(void);
void config_Init(void);
uint8_t	get_config(uint16_t config);
void set_config(uint16_t config, uint8_t value);


#endif