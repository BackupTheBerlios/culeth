#include <avr/io.h>
#include "led.h"

int main(void) {

  uint32_t  i;

  LED_Init();

  while(1) {
    LED_ON();
    for(i= 0; i< 8000000; i++)
      asm volatile ( "NOP" );
    LED_OFF();
    for(i= 0; i< 8000000; i++)
      asm volatile ( "NOP" );
  }
  return 0;
}