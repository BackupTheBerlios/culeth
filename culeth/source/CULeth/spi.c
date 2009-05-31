#include <avr/io.h>
#include "spi.h"
#include "board.h"

//------------------------------------------------------------------------------

void ccSpiInit(void) {

  PRR0 &= ~_BV(PRSPI);
  SPI_PORT |= _BV(SPI_SCLK);
  SPI_DDR  |= (_BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(SPI_SS));
  SPI_DDR  &= ~_BV(SPI_MISO);

  SPCR = (_BV(MSTR) | _BV(SPE));
  SPSR |= _BV(SPI2X);
}

//------------------------------------------------------------------------------

uint8_t ccSpiSend(uint8_t data) {

	SPDR = data;
  	while(!(SPSR & _BV(SPIF)));
  	return SPDR;
}

//------------------------------------------------------------------------------

uint8_t ccSpiWrite(uint8_t addr, uint8_t *data, uint16_t length) {

    uint16_t i;
    uint8_t rc;

    CC1100_SPI_BEGIN;
    rc= ccSpiSend(addr);
    for(i= 0; i< length; i++) {
    	ccSpiSend(data[i]);
    }
    CC1100_SPI_END;
    return(rc);

}

//------------------------------------------------------------------------------

uint8_t ccSpiRead(uint8_t addr, uint8_t *data, uint16_t length) {

    uint16_t i;
    uint8_t rc;

    CC1100_SPI_BEGIN;
    rc= ccSpiSend(addr);
    for(i= 0; i< length; i++) {
    	data[i]= ccSpiSend(0);	// Dummy write to read data byte
    }
    CC1100_SPI_END;
    return(rc);
}

//------------------------------------------------------------------------------

uint8_t ccSpiStrobe(uint8_t cmd) {

    uint8_t rc;

    CC1100_SPI_BEGIN;
    rc= ccSpiSend(cmd);
    CC1100_SPI_END;
    return(rc);
}

//------------------------------------------------------------------------------





