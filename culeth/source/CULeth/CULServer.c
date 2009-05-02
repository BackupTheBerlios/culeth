
/** \file
 *
 *  CUL internal Server packet handling routines.
 */

#include <util/crc16.h>
#include "CULServer.h"
#include "cc1100.h"
#include "Ethernet.h"
#include "IP.h"
#include "UDP.h"
#include "If_RNDIS.h"
#include "If_CC1101.h"
#include "led.h"
#include "version.h"
#include "config.h"
#include "clock.h"
#include "boot.h"
#include "extras.h"
#include <util/delay.h>

int16_t CULServer_ProcessPacket(void* IPHeaderInStart, void* CULServerRequestStart, void* CULServerReplyStart)
{

        IP_Header_t*   IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
	CULServer_Request_t*	Request= (CULServer_Request_t*)CULServerRequestStart;
	CULServer_Reply_t*	Reply  = (CULServer_Reply_t*)CULServerReplyStart;
	clock_time_t	elapsed_seconds;



	Reply->ExitCode= ACK;
	Reply->Length= 0;
	memset(Reply->Result, 0, MAXLEN);
	switch(Request->Operation) {
		case CULSERVER_OP_GETVERSION:
			Reply->Length= 2;
			Reply->Result[0]= VERSION >> 8;
			Reply->Result[1]= VERSION && 0xff;
			break;
		case CULSERVER_OP_GETTIME:
			elapsed_seconds= clock_time();
			Reply->Length= sizeof(elapsed_seconds);
			memcpy(Reply->Result, &elapsed_seconds, Reply->Length);
			break;
		case CULSERVER_OP_TEST:
			Reply->Length= 0;
			extraflag= EXTRA_TEST;
			break;
		case CULSERVER_OP_SETDFLTCFG:
			Reply->Length= 0;
			factory_reset();
			break;
		case CULSERVER_OP_GETIP:
			Reply->Length= 4;
			for(uint8_t i= 0; i<= 3; i++) {
				Reply->Result[i]= ServerIPAddress.Octets[i];
			}
			break;
		case CULSERVER_OP_GETSTAT:
			Reply->Length= 4;
			Reply->Result[0]= FrameCount_If_RNDIS_IN;
			Reply->Result[1]= FrameCount_If_RNDIS_OUT;
			Reply->Result[2]= FrameCount_If_CC1101_IN;
			Reply->Result[3]= FrameCount_If_CC1101_OUT;
			break;
		case CULSERVER_OP_GETCC1101STATUS:
			Reply->Length= CC1100_STATUS_SIZE_R;
			ccGetStatus(Reply->Result);
			break;
		case CULSERVER_OP_GETCC1101CONFIG:
			Reply->Length= CC1100_CONFIG_SIZE_R;
			ccGetConfig(Reply->Result);
			break;
		case CULSERVER_OP_CC1101SEND:
			Reply->Length= 1;
			Reply->Result[ 0]= CC1101_TESTPACKETSIZE;
			CC1101_TX_test();
			break;
		case CULSERVER_OP_BLINK:
			Reply->Length= 1;
			Reply->Result[ 0]= 5;
			for(int i= 0; i< 5; i++) {
				LED_ON();
				for(int j= 0; j< 25; j++) { _delay_ms(10); }
				LED_OFF();
				for(int j= 0; j< 25; j++) { _delay_ms(10); }
			}
			break;
		case CULSERVER_OP_BOOTLOADER:
			Reply->Length= 0;
			prepare_boot();
			break;
		default:
			Reply->ExitCode= NAK;
	}

	/* Alter the incoming IP packet header so that the corrected IP source and destinations are used - this means that
	   when the response IP header is generated, it will use the corrected addresses and not the null/broadcast addresses */
	IPHeaderIN->DestinationAddress = ServerIPAddress;

	return 4+Reply->Length; //sizeof(CULServer_Reply_t);
}

