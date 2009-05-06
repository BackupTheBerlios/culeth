#include <avr/io.h>
#include "clock.h"
#include <avr/wdt.h>

/*
 * global clock variables
 */
uint8_t 	ticks;		// 125 ticks/second
clock_time_t 	clocktime;	// seconds


void timer_Init(void) {

	// reset clocktime
  	clocktime= 0;

	// disable clock division
  	SetSystemClockPrescaler(0);

	// start Timer0: 0.008s = 250*256/8MHz
  	OCR0A  = 250;     	// count from 0 to 250
  	TCCR0B = _BV(CS02);	// bit mask 100: Prescaler= 256
  	TCCR0A = _BV(WGM01);  	// wave form generation mode 01
  	TIMSK0 = _BV(OCIE0A); 	// Timer 0 Output Compare A Match Interrupt Enable

}

void timer_Done(void) {
	TIMSK0 = 0;
}

clock_time_t clock_time(void) {
	return clocktime;
}


ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	ticks++;
	if(ticks >= 125) {
 		clocktime++;
    		ticks= 0;
  	}
}

TASK(Timer_Task)
{
	// empty
}
