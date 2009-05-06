

#include <LUFA/Drivers/USB/USB.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include "config.h"
#include "clock.h"
#include <avr/interrupt.h>


void prepare_boot(void) {

	// no need for USB_ShutDown();
  	set_config(CFG_OFS_REQBL, CFG_REQBL);

	timer_Done();

  	// infinite loop, the watchdog will take us to reset
  	while (1);
}


void start_bootloader(void)
{
	cli();

  	/* move interrupt vectors to bootloader section and jump to bootloader */
  	MCUCR = _BV(IVCE);
  	MCUCR = _BV(IVSEL);

	#define jump_to_bootloader ((void(*)(void))0x1800)
  	jump_to_bootloader();
}

bool watchdog_caused_reset(void) {

	// check reset source in MCU status register
	return bit_is_set(MCUSR, WDRF);
}

void check_bootloader_request(void)
{
	if(watchdog_caused_reset() && get_config(CFG_OFS_REQBL)==CFG_REQBL) {
		set_config(CFG_OFS_REQBL, CFG_NOREQBL);
		start_bootloader();
  	}
}