#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include "config.h"
#include <avr/interrupt.h>
#include "watchdog.h"

void enable_watchdog(void) {

	MCUSR &= ~(1 << WDRF);
  	wdt_enable(WDTO_2S);
}

void disable_watchdog(void) {

	MCUSR &= ~(1 << WDRF);
	wdt_disable();
}