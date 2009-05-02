#include <avr/io.h>
#include "clock.h"

/*
 * global clock variables
 */
uint8_t 	ticks;		// 125 ticks/second
clock_time_t 	clocktime;	// seconds


void timer_Init(void) {

  // reset clocktime
  clocktime= 0;

  // start Timer0
  OCR0A  = 249;      // Timer0: 0.008s = 250*256/8MHz
  TCCR0B = _BV(CS02);
  TCCR0A = _BV(WGM01);


  TIMSK0 = _BV(OCIE0A);

  // start Timer1
  TCCR1A = 0;
  TCCR1B = _BV(CS11) | _BV(WGM12); // Timer1: 1us = 8/8MHz

  /* Disable Clock Division */
  SetSystemClockPrescaler(0);

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
