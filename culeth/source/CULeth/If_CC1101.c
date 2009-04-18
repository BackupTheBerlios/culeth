#include "If_CC1101.h"
#include "Ethernet.h"
#include "cc1100.h"


uint16_t FrameCount_If_CC1101_IN = 0;
uint16_t FrameCount_If_CC1101_OUT= 0;


void CC1101_Init(void) {

     // SPI
     // set SCK to Hi
     SPI_PORT |= 1<<SPI_SCLK;
     // MOSI, SCK, SS as output
     SPI_DDR  |= 1<<SPI_MOSI | 1<<SPI_SCLK | 1<<SPI_SS; // mosi, sck, ss output
     // MISO as input
     SPI_DDR  &= ~( 1<<SPI_MISO ); // miso input

     // master mode
     SPCR = 1<<MSTR | 1<<SPE;

     SPSR |= _BV(SPI2X);

     ccInitChip();
}



bool CC1101_RX(void)
{
     if (cc1100_readReg( CC1100_MARCSTATE ) == 1) {

	  if (cc1100_readReg( CC1100_RXBYTES )) {
	       Frame.FrameLength = cc1100_read(0xbf);

	       CC1100_ASSERT;
	       cc1100_sendbyte(0xff);

	       for (uint8_t i=0; i<Frame.FrameLength; i++) {
		    Frame.FrameData[i]= cc1100_sendbyte(0);
	       }

	       CC1100_DEASSERT;

//	       printf_P( PSTR("\r\n" ) );
//	  printf_P( PSTR("State: %02X\r\n" ), cc1100_readReg( CC1100_MARCSTATE ) );

//	       printf_P( PSTR("State: %02X\r\n" ), cc1100_readReg( CC1100_MARCSTATE ) );
	  }

	  ccStrobe( CC1100_SFRX  );
	  ccStrobe( CC1100_SIDLE );
	  ccStrobe( CC1100_SNOP  );
	  ccStrobe( CC1100_SRX   );

    	  FrameCount_If_CC1101_IN++;

	  return true;
     }

     return false;
}

bool CC1101_TX(void)
{
	ccStrobe( CC1100_SIDLE );
	ccStrobe( CC1100_SFRX  );
	ccStrobe( CC1100_SFTX  );

	CC1100_ASSERT;

	cc1100_sendbyte(0x7f);

	cc1100_sendbyte(Frame.FrameLength);

	for(uint8_t i= 0; i< Frame.FrameLength; i++) {
		cc1100_sendbyte(Frame.FrameData[i]);
	}

	CC1100_DEASSERT;

	ccStrobe( CC1100_SIDLE );
	ccStrobe( CC1100_SFRX  );
	ccStrobe( CC1100_STX   );

	FrameCount_If_CC1101_OUT++;

	return true;
}

bool CC1101_TX_test(void)
{

	ccStrobe( CC1100_SIDLE );
	ccStrobe( CC1100_SFRX  );
	ccStrobe( CC1100_SFTX  );

	CC1100_ASSERT;

	cc1100_sendbyte(0x7f);

	cc1100_sendbyte(CC1101_TESTPACKETSIZE);

	for(uint8_t i= 0; i< CC1101_TESTPACKETSIZE; i++) {
		cc1100_sendbyte(i);
	}

	CC1100_DEASSERT;

	ccStrobe( CC1100_SIDLE );
	ccStrobe( CC1100_SFRX  );
	ccStrobe( CC1100_STX   );

	FrameCount_If_CC1101_OUT++;

	return true;
}
