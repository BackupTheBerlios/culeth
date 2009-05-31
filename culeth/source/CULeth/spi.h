#ifndef _SPI_H
#define _SPI_H

//------------------------------------------------------------------------------

void ccSpiInit(void);
uint8_t ccSpiSend(uint8_t data);
uint8_t ccSpiWrite(uint8_t addr, uint8_t *data, uint16_t length);
uint8_t ccSpiRead(uint8_t addr, uint8_t *data, uint16_t length);
uint8_t ccSpiStrobe(uint8_t cmd);


//------------------------------------------------------------------------------
// see CC1101 documentation, chapter 10, p. 28
//------------------------------------------------------------------------------

#include "board.h"

#define CC1100_SPI_CS_DEASSERT  SET_BIT( CC1100_CS_PORT, CC1100_CS_PIN )
#define CC1100_SPI_CS_ASSERT    CLEAR_BIT( CC1100_CS_PORT, CC1100_CS_PIN )
#define CC1100_SPI_MISO_VAL	(SPI_PORT & _BV(SPI_MISO))
#define CC1100_SPI_BEGIN	{ CC1100_SPI_CS_ASSERT; while(CC1100_SPI_MISO_VAL); }
#define CC1100_SPI_END		{ CC1100_SPI_CS_DEASSERT; }

//------------------------------------------------------------------------------

#endif
