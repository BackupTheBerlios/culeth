

#include <LUFA/Drivers/USB/USB.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include "config.h"
#include <avr/interrupt.h>


void prepare_boot(void) {

	USB_ShutDown();

  	set_config(CFG_OFS_REQBL, CFG_REQBL);

	TIMSK0 = 0;        // Disable the clock which resets the watchdog

  	// infinite loop, the watchdog will take us to reset
  	while (1);
}


void start_bootloader(void)
{
	cli();

  	/* move interrupt vectors to bootloader section and jump to bootloader */
  	MCUCR = _BV(IVCE);
  	MCUCR = _BV(IVSEL);

	#define jump_to_bootloader ((void(*)(void))0x3000)
  	jump_to_bootloader();
}

void check_bootloader_request(void)
{
	if(bit_is_set(MCUSR,WDRF) && get_config(CFG_OFS_REQBL)==CFG_REQBL) {
		set_config(CFG_OFS_REQBL, CFG_NOREQBL);
		start_bootloader();
  	}

}