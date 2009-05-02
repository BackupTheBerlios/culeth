/*
 * extras.c
 *
 * This file contains routines that might interfere with the answers from
 * CULServer and therefore are executed after the Ethernet_Task has finished.
 *
 */


#include <util/delay.h>

#include "extras.h"
#include "If_RNDIS.h"
#include "Ethernet.h"
#include "UDP.h"


uint8_t extraflag= EXTRA_NONE;



#define TEST_LEN 0x2f


PROGMEM prog_uint8_t test[TEST_LEN] =
{
0x29 , 0x2E , 0x06 , 0x47 , 0xD3 , 0x91 , 0x3E , 0x1A ,
0x45 , 0x01 , 0x01 , 0x06 , 0x00 , 0x10 , 0xA9 , 0xD6 ,
0x8A , 0x75 , 0x13 , 0x22 , 0xC1 , 0x35 , 0x04 , 0x0C ,
0x38 , 0x16 , 0x6C , 0x43 , 0x40 , 0x91 , 0x46 , 0x50 ,
0x78 , 0x56 , 0x10 , 0xA9 , 0x0A , 0x00 , 0x11 , 0x41 ,
0x00 , 0x57 , 0x7F , 0x3F , 0x98 , 0x31 , 0x0B
};

TASK(Extras_Task)
{

	uint8_t* Payload;

	switch(extraflag) {
		case EXTRA_NONE:
			break;
		case EXTRA_TEST:
			/*
			for(int i= 0; i< 5; i++) {
				LED_ON();
				for(int j= 0; j< 10; j++) { _delay_ms(10); }
				LED_OFF();
				for(int j= 0; j< 10; j++) { _delay_ms(10); }
			}
			*/
			Payload= get_UDP_Payload(Frame.FrameData);
			// build UDP message
			for(int i= 0; i< TEST_LEN; i++) {
				Payload[i]= __LPM(test+i);
			}
			Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData,
TEST_LEN);
			RNDIS_TX();
			break;
		default:
			{
			}
	}


	extraflag= EXTRA_NONE;
}
