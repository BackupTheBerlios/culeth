
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "board.h"
#include "cc1100.h"

const PROGMEM uint8_t CC1100_CFG[60] = {

     0x00, 0x02,

     0x02, 0x06,

     0x03, 0x0D,

     0x04, 0xE9,

     0x05, 0xCA,

     0x07, 0x0C,

     0x0B, 0x06,

    // 0x0D, 0x21,
     //0x0D, 0x10,
     //0x0D, 0x21,

     //0x0E, 0x65,
     0x0E, 0xA9,
     //0x0E, 0x65,

     //0x0F, 0x6A,
     0x0F, 0xD6,
     //0x0F, 0xE8,

     0x10, 0xC8,

     0x11, 0x93,

     0x12, 0x03,

     0x15, 0x34,

     0x17, 0x00,
//     0x17, 0x03,
//     0x17, 0x0f,

     0x18, 0x18,

     0x19, 0x16,

     0x1B, 0x43,

     0x21, 0x56,

     0x25, 0x00,

     0x26, 0x11,

     0x2D, 0x35,

     0x3e, 0xc3,

     0xff
};

uint8_t cc1100_sendbyte(uint8_t data) {

     SPDR = data;		        // send byte

     while (!(SPSR & _BV (SPIF)));	// wait until transfer finished

     return SPDR;
}

uint8_t cc1100_read(uint8_t data) {

     CC1100_ASSERT;

     cc1100_sendbyte( data );
     uint8_t temp = cc1100_sendbyte( 0 );

     CC1100_DEASSERT;

     return temp;
}

uint8_t cc1100_readReg(uint8_t addr) {
     return cc1100_read( addr | CC1100_READ_BURST );
}

void ccStrobe(uint8_t strobe) {

     CC1100_ASSERT;
     cc1100_sendbyte( strobe );
     CC1100_DEASSERT;

}


//----------------------------------------------------------------------------------
//  void ccResetChip(void)
//
//  DESCRIPTION:
//    Resets the chip using the procedure described in the datasheet.
//----------------------------------------------------------------------------------
void ccResetChip(void) {

//     printf_P( PSTR("Reset CC1100 chip ...\r\n" ) );

     // Toggle chip select signal
     CC1100_DEASSERT;
     _delay_us(30);
     CC1100_ASSERT;
     _delay_us(30);
     CC1100_DEASSERT;
     _delay_us(45);

     // Send SRES command
     ccStrobe( CC1100_SRES );

     _delay_us(100);

     // load configuration
     for (uint8_t i = 0; i<60; i += 2) {

	  if (CC1100_CFG[i]>0x40)
	       break;

	  CC1100_ASSERT;
	  cc1100_sendbyte( CC1100_CFG[i] );
	  cc1100_sendbyte( CC1100_CFG[i+1] );
	  CC1100_DEASSERT;
     }

     ccStrobe( CC1100_SCAL );

     for (uint8_t i=0; i<10; i++)
	  _delay_ms(200);
}

void ccInitChip(void) {

//     printf_P( PSTR("Init CC1100 chip ...\r\n" ) );

     SET_BIT( CC1100_CS_DDR, CC1100_CS_PIN ); // CS as output

     CC1100_DEASSERT;

     ccResetChip();

}



/*

void ccTX(void) {

     GICR  &= ~_BV(INT1);

     while ((cc1100_readReg( CC1100_MARCSTATE ) & 0x1f) != 19)
	  ccStrobe( CC1100_STX );

}

void ccRX(void) {

     while ((cc1100_readReg( CC1100_MARCSTATE ) & 0x1f) != 13)
	  ccStrobe( CC1100_SRX );

     GIFR  |= _BV(INTF1);
     GICR  |= _BV(INT1);

}

*/
