
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "spi.h"
#include "cc1100.h"


//
// see swra141/source_rev_113/app/link_poll/

PROGMEM prog_uint8_t CC1100_PA[] = {

  0x00,0x03,0x0F,0x1E,0x26,0x27,0x00,0x00,      //-10 dBm
  0x00,0x03,0x0F,0x1E,0x25,0x36,0x38,0x67,      // -5 dBm
  0x00,0x03,0x0F,0x25,0x67,0x40,0x60,0x50,      //  0 dBm
  0x00,0x03,0x0F,0x27,0x51,0x88,0x83,0x81,      //  5 dBm
  0x00,0x03,0x0F,0x27,0x50,0xC8,0xC3,0xC2,      // 10 dBm

  0x00,0x27,0x00,0x00,0x00,0x00,0x00,0x00,      //-10 dBm
  0x00,0x67,0x00,0x00,0x00,0x00,0x00,0x00,      // -5 dBm (checked 3 times!)
  0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,      //  0 dBm
  0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,      //  5 dBm
  0x00,0xC2,0x00,0x00,0x00,0x00,0x00,0x00,      // 10 dBm

  0x00,0x32,0x38,0x3f,0x3f,0x3f,0x3f,0x3f
};

/*
	CUL	: 26 MHz crystal
	Betty	: 27 MHz crystal
*/

/* Boop settings adapted to CUL:
	frequency adapted, everything else is the same 	*/
const PROGMEM uint8_t CC1100_CFG[CC1100_CONFIG_SIZE_W] =
{
0x29 ,	// IOCFG2	GDO2_INV= 0, GDO2_CFG= 0x29 (CHP_RDYn)
0x2E ,	// IOCFG1	GDO_DS= 0, GDO1_INV= 0, GDO1_CFG= 0x2E (3-state)
0x06 ,	// IOCFG0	TEMP_SENSOR_ENABLE= 0, GDO0_INV= 0, GDO0_CFG= 0x06
	//		(Asserts when sync word has been sent / received, and
	//		de-asserts at the end of the packet. In RX, the pin will
	//		de-assert when the optional address check fails or the
	//		RX FIFO overflows. In TX the pin will de-assert if the
	//		TX FIFO underflows.
0x47 ,	// FIFOTHR	ADC_RETENTION = 1,
	//		CLOSE_IN_RX= FIFO_THR= 0x07 (33 bytes in TX FIFO,
	//		32 Bytes in RX FIFO)
0xD3 ,	// SYNC1	SYNC= 0xD391 (16-bit sync word)
0x91 ,	// SYNC0
0x3E ,	// PKTLEN	PACKET_LENGTH= 0x3E (fixed or maximum packet length)
0x1A ,	// PKTCTRL1	PQT= 0, CRC_AUTOFLUSH= 0, APPEND_STATUS= 0, ADR_CHK= 0x02
	//		(address check and 0x00 broadcast)

0x45 ,	// PKTCTRL0	WHITE_DATA= 1 (whitening on), PKT_FORMAT= 0x00 (use FIFOs
	//		for RX and TX), CRC_EN= 1 (CRC calculation in RX and CRC
	//		check in RX), LENGTH_CONFIG= 0x01 (variable packet length
	//		mode)
0x01 ,	// ADDR		DEVICE_ADDR= 0x01 (address for packet filtration)
0x01 ,	// CHANNR	CHAN= 0x01 (channel number)
0x06 ,	// FSCTRL1	FREQ_IF= 0x06 (27 MHz/2^10*6= 158.203 kHz)
	//		(26 MHz/2^10*6= 152.344 kHz)
0x00 ,	// FSCTRL0	FREQOFF= 0
0x10 ,	// FREQ2	FREQ= 0x10A9D6 (1,092,054 * 26 MHz / 65536= 433.249 MHz)
0xA9 ,	// FREQ1
0xD6 ,	// FREQ0

0x8A ,	// MDMCFG4	CHANBW_E= 0x10, CHANBW_M= 0x00, DRATE_E= 0x0A
0x75 ,	// MDMCFG3	DRATE_M= 0x75
0x13 ,	// MDMCFG2	DEM_DCFILT_OFF= 0, MOD_FORMAT= 0x01 (GFSK)
	//		MANCHESTER= 0, SYNC_MODE= 0x03 (30/32 sync word bits)
0x22 ,	// MDMCFG1	FEC_EN= 0, NUM_PREAMBLE= 0x02 (minimum number of preamble
	//		bits), CHSNSPC_E= 0x02 (2 bit exponent of channel spacing)
0xC1 ,	// MDMCFG0	CHANSPC_M= 0xC1
0x35 ,	// DEVIATN	DEVIATION_E= 0x03, DEVIATION_M= 0x05
0x04 ,	// MSCM2	RX_TIME_RSSI= 0, RX_TIME_QUAL= 0, RX_TIME= 0x04
0x0C ,	// MSCM1	CCA_MODE= 0x0 (always), RXOFF_MODE= 0x3 (stay in RX after
	//		finishing packet reception), TXOFF_MODE= 0x0 (IDLE after
	//		a packet has been sent)
/*
0x0F ,	// MSCM1	CCA_MODE= 0x0 (always), RXOFF_MODE= 0x3 (stay in RX after
	//		finishing packet reception), TXOFF_MODE= 0x3 (RX after
	//		a packet has been sent)
*/
0x38 ,	// MSCM0	FS_AUTOCAL= 0x3, PO_TIMEOUT= 0x2, PIN_CTRL_EN= 0,
	//		XOS_FORCE_ON= 0
0x16 ,	// FOCCFG	FOC_BS_CS_GATE= 0, FOC_PRE_K= 0x2, FOC_POST_K= 1,
	//		FOC_LIMIT= 0x2
0x6C ,	// BSCFG
0x43 ,	// AGCCTRL2
0x40 ,	// AGCCTRL1
0x91 ,	// AGCCTRL0
0x46 ,	// WOREVT1
0x50 ,	// WOREVT0

0x78 ,	// WORCTRL
0x56 ,  // FREND1
0x10 ,	// FREND0
0xA9 ,	// FSCAL3
0x0A ,	// FSCAL2
0x00 ,	// FSCAL1
0x11 ,	// FSCAL0
0x41 ,	// RCCTRL1	RCCTRL= 0x41 (RC oscillator configuration)

0x00 ,	// RCCTRL0
0x57 ,	// FSTEST
0x7F ,	// PTEST
0x3F ,	// AGCTEST
0x98 ,	// TEST2
0x31 ,	// TEST1
0x0B	// TEST0
};


 /*
 Conf1: SmartRF Studio:
   Xtal: 26Mhz, RF out: 0dB, PA ramping, Dev:5kHz, Data:1kHz, Modul: ASK/OOK,
   RF: 868.35MHz, Chan:350kHz, RX Filter: 325kHz
   SimpleRX: Async, SimpleTX: Async+Unmodulated
 */

/*
PROGMEM prog_uint8_t CC1100_CFG[CC1100_CONFIG_SIZE_W] = {
//Active                  Our FT  SmartRF Studio
   0x0D, //*00 IOCFG2      0D  0D *0B // GDO2 as serial output
   0x2E, // 01 IOCFG1      2E  2E  2E
   0x2D, //*02 IOCFG0      2D  2D *0C // GDO0 for input
   0x07, // 03 FIFOTHR     47 *07  47
   0xD3, // 04 SYNC1       D3  D3  D3
   0x91, // 05 SYNC0       91  91  91
   0x3D, // 06 PKTLEN      ff *3D *3D
   0x04, // 07 PKTCTRL1    04  04  04
   0x32, // 08 PKTCTRL0    32  32  32
   0x00, // 09 ADDR        00  00  00
   0x00, // 0A CHANNR      00  00  00
   0x06, // 0B FSCTRL1     06  06  06
   0x00, // 0C FSCTRL0     00  00  00
   0x10, // 0D FREQ2       21  21  21
   0xA9, // 0E FREQ1       65  65  65
   0xD6, // 0F FREQ0       E8  e8  e8
   0x55, // 10 MDMCFG4     55  55  55
   0xe4, // 11 MDMCFG3     43 *e4  43
   0x30, //*12 MDMCFG2     30 *30 *B0  // DEM_DCFILT_OFF
   0x23, // 13 MDMCFG1     23  23  23
   0xb9, // 14 MDMCFG0     B9  b9  b9
   0x00, // 15 DEVIATN     00  00  00
   0x07, // 16 MCSM2       07  07  07
   0x30, // 17 MCSM1       30  30  30
   0x18, // 18 MCSM0       18  18  18
   0x14, // 19 FOCCFG      14  14  14
   0x6C, // 1A BSCFG       6C  6C  6C
   0x07, //*1B AGCCTRL2    07  05 *03 // 42 dB instead of 33dB
   0x00, // 1C AGCCTRL1    40 *00  40
   0x90, //*1D AGCCTRL0    91 *90 *92 // 16 samples instead of 32
   0x87, // 1E WOREVT1     87  87  87
   0x6B, // 1F WOREVT0     6B  6B  6B
   0xF8, // 20 WORCTRL     F8  F8  F8
   0x56, // 21 FREND1      56  56  56
   0x17, // 22 FREND0      17  17  17 // 0x11 for no PA ramping
   0xE9, // 23 FSCAL3      E9  E9  E9   EF
   0x2A, // 24 FSCAL2      2A  2A  2A   2F
   0x00, // 25 FSCAL1      00  00  00   18
   0x1F, // 26 FSCAL0      1F  1F  1F
   0x41, // 27 RCCTRL1     41  41  41
   0x00, // 28 RCCTRL0     00  00  00
};
*/

//--------------------------------------------------------------------

// see CC1101 documentation, chapter 29, p. 82
void ccSetConfig(void) {

	CC1100_SPI_BEGIN;                             // upload configuration
  	ccSpiSend(0 | CC1100_WRITE_BURST);
  	for(uint8_t i= 0; i< CC1100_CONFIG_SIZE_W; i++) {
    		ccSpiSend(__LPM(CC1100_CFG+i));
  	}
  	CC1100_SPI_END;

}

//--------------------------------------------------------------------

void ccGetConfig(uint8_t* data) {

	ccSpiRead(0 | CC1100_READ_BURST, data, CC1100_CONFIG_SIZE_R);

}

//--------------------------------------------------------------------

uint8_t ccReadReg(uint8_t addr) {

	uint8_t data;

	CC1100_SPI_BEGIN;
    	ccSpiSend(addr | CC1100_READ_STATUS);
    	data= ccSpiSend(0);
	CC1100_SPI_END;
	return(data);

}

//--------------------------------------------------------------------

void ccGetStatusRegisters(uint8_t* data) {

  	CC1100_SPI_BEGIN;
  	for(uint8_t i= 0; i< CC1100_STATUS_SIZE_R; i++) {
    		ccSpiSend((CC1100_PARTNUM+i) | CC1100_READ_STATUS);
    		data[i]= ccSpiSend(0);
  	}
  	CC1100_SPI_END;

}

//--------------------------------------------------------------------

void ccResetChip(void)
{

	//
	// reset
	//
  	CC1100_SPI_CS_DEASSERT;	// Toggle chip select signal
  	_delay_us(30);
  	CC1100_SPI_CS_ASSERT;
  	_delay_us(30);
  	CC1100_SPI_CS_DEASSERT;
  	_delay_us(45);

  	ccSpiStrobe(CC1100_SRES);	// Send SRES command
  	_delay_us(100);

	CC1100_SPI_CS_DEASSERT;
}


//--------------------------------------------------------------------
void ccInitChip(void)
{

	SET_BIT(CC1100_CS_DDR, CC1100_CS_PIN);     // CS as output
	CC1100_SPI_CS_DEASSERT;

	ccResetChip();

	ccSetConfig();                  // load configuration

	ccSpiStrobe(CC1100_SCAL);
  	_delay_ms(1);

}

//--------------------------------------------------------------------

uint8_t ccGetStatus(uint8_t strobe) {

	uint8_t status1;
	uint8_t status2;

	status1= ccSpiStrobe(strobe);
	status2= ccSpiStrobe(strobe);
	while(status1 != status2) {
		status1= status2;
		status2= ccSpiStrobe(strobe);
	}
	return status2;
}

//--------------------------------------------------------------------

uint8_t ccGetRxStatus(void) {

//
// Status byte:
//
// ----------------------------------------------------------------------------
// |          |            |                                                  |
// | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (available bytes in RX FIFO |
// |          |            |                                                  |
// ----------------------------------------------------------------------------
//
	return ccGetStatus(CC1100_SNOP | CC1100_READ_SINGLE);
}


//--------------------------------------------------------------------

uint8_t ccGetTxStatus(void) {

//
// Status byte:
//
// ---------------------------------------------------------------------------
// |          |            |                                                 |
// | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (free bytes in the TX FIFO |
// |          |            |                                                 |
// ---------------------------------------------------------------------------

	return(ccGetStatus(CC1100_SNOP));
}

//--------------------------------------------------------------------

uint8_t ccWriteFifo(uint8_t *data, uint8_t length)
{
    return(ccSpiWrite(CC1100_TXFIFO | CC1100_WRITE_BURST, data, length));
}

//----------------------------------------------------------------------------------

uint8_t ccReadFifo(uint8_t *data, uint8_t length)
{
    return(ccSpiRead(CC1100_RXFIFO | CC1100_READ_BURST, data, length));
}

//--------------------------------------------------------------------

void ccTxPacket(uint8_t *data, uint8_t count) {

	uint8_t status;		// TX status
	uint8_t nsent= 0;	// bytes sent
	uint8_t n;

	while(1) {

		status= ccGetTxStatus();

		switch(status & CC1100_STATUS_STATE_BM) {
			case CC1100_STATE_IDLE:
				// all sent?
				if(nsent==count) {
					return;
				} else {
					// TX command strobe
					ccSpiStrobe(CC1100_STX);
					// fall through (no break)
				}
			case CC1100_STATE_CALIBRATE:
			case CC1100_STATE_TX:
				n= status & CC1100_STATUS_FIFO_BYTES_AVAILABLE_BM;
				// in variable length packet mode, first byte is packet length
				if(!nsent) { ccWriteFifo(&count, 1); n--; }
				// send at most remaining number of bytes
				if(n>count-nsent) { n= count-nsent; }
				if(n) {
					ccWriteFifo(&data[nsent], n);
					nsent+= n;
				}
				break;
			case CC1100_STATE_TX_UNDERFLOW:
				ccSpiStrobe(CC1100_SFTX); // clear underflow
				break;
			default:
				nsent= count;
				break;
		}
	}
}

//--------------------------------------------------------------------

uint8_t ccRxPacket(uint8_t *data) {

	uint8_t count= 0;	// length byte
	uint8_t status;		// RX status
	uint8_t nrcvd= 0;	// bytes received
	uint8_t n;

	while(1) {

		status= ccGetRxStatus();

		switch(status & CC1100_STATUS_STATE_BM) {
			case CC1100_STATE_IDLE:
			case CC1100_STATE_RX:
				// If there's anything in the RX FIFO....
				if((n= (status & CC1100_STATUS_FIFO_BYTES_AVAILABLE_BM))) {
					if(!count) {
		                	    // Make sure that the RX FIFO is not emptied
	                		    // (see the CC1100 or 2500 Errata Note)
        	        		    if(n>1) {
                			    	count= ccReadReg(CC1100_RXFIFO);
                			    	n--;
                			    } else {
	                		    	break;
        	        		    }
                			}
	   			        // Make sure that the RX FIFO is not emptied
        	        		// (see the CC1100 or 2500 Errata Note)
	                		if((n>1) && (n!= count-nrcvd)) {
        	        			// Leave one byte in FIFO
						n--;
	                		} else if((n<=1) && (n!= count-nrcvd)) {
		                	    	// Need more data in FIFO before reading additional bytes
                	    			break;
		                	}
			                // Read from RX FIFO and store the data in rxBuffer
                			ccReadFifo(&(data[nrcvd]), n);
                			nrcvd+= n;
					// Done?
					if(!count && (nrcvd==count)) return count;
					break;
                		} else if(!count) return 0; // nothing to receive
			case CC1100_STATE_RX_OVERFLOW:
				ccSpiStrobe(CC1100_SFRX); // clear overflow
				return 0;
		}
	}

}