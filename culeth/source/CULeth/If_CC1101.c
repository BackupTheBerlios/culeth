#include "If_CC1101.h"
#include "Ethernet.h"
#include "spi.h"
#include "cc1100.h"

#include "IP.h"
#include "UDP.h"

uint16_t FrameCount_If_CC1101_IN = 0;
uint16_t FrameCount_If_CC1101_OUT= 0;


void CC1101_Init(void) {

	ccSpiInit();
     	ccInitChip();
}



bool CC1101_RX(void)
{
     	uint8_t* UDPMessage=	get_UDP_Payload(Frame.FrameData);
     	uint8_t size= 0;

	size= ccRxPacket(UDPMessage);
	if(size) {
		Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData, size);
    		FrameCount_If_CC1101_IN++;
    	}
    	return size;


/*
     	uint8_t* UDPMessage=	get_UDP_Payload(Frame.FrameData);
     	uint8_t size= 0;

     	if(cc1100_readReg(CC1100_MARCSTATE)== 1) {

	  	if(cc1100_readReg(CC1100_RXBYTES)) {

	       		size= cc1100_read(0xbf);

			CC1100_ASSERT;
			cc1100_sendbyte(0xff);

	       		uint8_t b;
	       		for (uint8_t i= 0; i< size; i++) {
		    		b= cc1100_sendbyte(0);
		    		if(i< MAX_PAYLOAD_SIZE) {
		    			UDPMessage[i]= b;
		    		}
		    	}

			CC1100_DEASSERT;

			//UDPMessage[0]= size;
        		//Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData, 1);

        	//size );



        	}

		ccStrobe(CC1100_SFRX);
		ccStrobe(CC1100_SIDLE);
		ccStrobe(CC1100_SNOP);
		ccStrobe(CC1100_SRX);

    		if(size) {
    			Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData, size);
	    		FrameCount_If_CC1101_IN++;
	    	}

		return size;
	;
     	}*/

     	return false;
}

bool CC1101_TX(void)
{
	ccTxPacket(Frame.FrameData, Frame.FrameLength);

	FrameCount_If_CC1101_OUT++;

	return true;

	/*ccStrobe( CC1100_SIDLE );
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

	return true;*/
}

bool CC1101_TX_test(void)
{
/*
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
	ccStrobe( CC1100_STX   );*/
	ccTxPacket(Frame.FrameData, CC1101_TESTPACKETSIZE);

	FrameCount_If_CC1101_OUT++;

	return true;
}
