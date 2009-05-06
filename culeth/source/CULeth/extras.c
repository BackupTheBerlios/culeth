/*
 * extras.c
 *
 * This file contains routines that might interfere with the answers from
 * CULServer and therefore are executed after the Ethernet_Task has finished.
 *
 */


#include <util/delay.h>

#include "extras.h"
#include "boot.h"
#include "If_RNDIS.h"
#include "Ethernet.h"
#include "UDP.h"
#include "clock.h"


uint8_t extraflag= EXTRA_NONE;
uint8_t extraarg = 0;

void send_msg(void) {

	/*
	uint8_t* Payload= get_UDP_Payload(Frame.FrameData);
	strcpy((char*)(Payload), msg);
	Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData, strlen(msg));
	RNDIS_TX();
	*/
	clock_time_t clocktime= clock_time();
	uint8_t* Payload= get_UDP_Payload(Frame.FrameData);
	memcpy(Payload, &clocktime, 4);
	Payload[4]= extraarg;
	memcpy(Payload+5, "test\0", 5);
	Frame.FrameLength= finalize_UDP_Packet(Frame.FrameData, 10);
	RNDIS_TX();
}

TASK(Extras_Task)
{

	switch(extraflag) {
		case EXTRA_NONE:
			break;
		case EXTRA_TEST:
			send_msg();
			break;
		case EXTRA_PACKET:
			send_msg();
			break;
		case EXTRA_BOOT:
			prepare_boot();
			break;
		default:
			{
			}
	}

	extraflag= EXTRA_NONE;
	extraarg = 0;
}
