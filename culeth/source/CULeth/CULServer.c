
/** \file
 *
 *  CUL internal Server packet handling routines.
 */

#include <util/crc16.h>
#include "CULServer.h"
#include "cc1100.h"
#include "Ethernet.h"
#include "IP.h"
#include "If_RNDIS.h"
#include "If_CC1101.h"
#include "led.h"
#include <util/delay.h>

int16_t CULServer_ProcessPacket(void* IPHeaderInStart, void* CULServerRequestStart, void* CULServerReplyStart)
{

        IP_Header_t*   IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
	CULServer_Request_t*	Request= (CULServer_Request_t*)CULServerRequestStart;
	CULServer_Reply_t*	Reply  = (CULServer_Reply_t*)CULServerReplyStart;

	Reply->ExitCode= ACK;
	switch(Request->Operation) {
		case CULSERVER_OP_ECHO:
			if(Reply->Length> MAXLEN) {
				Reply->Length= MAXLEN;
			}
			break;
		case CULSERVER_OP_IP:
			memset(Reply->Result, 0, MAXLEN);
			Reply->Length= 4;
			for(uint8_t i= 0; i<= 3; i++) {
				Reply->Result[i]= ServerIPAddress.Octets[i];
			}
			break;
		case CULSERVER_OP_STAT:
			memset(Reply->Result, 0, MAXLEN);
			Reply->Length= 4;
			Reply->Result[0]= FrameCount_If_RNDIS_IN;
			Reply->Result[1]= FrameCount_If_RNDIS_OUT;
			Reply->Result[2]= FrameCount_If_CC1101_IN;
			Reply->Result[3]= FrameCount_If_CC1101_OUT;
			break;
		case CULSERVER_OP_CC1101INFO:
			memset(Reply->Result, 0, MAXLEN);
			Reply->Length= 12;
			Reply->Result[ 0]= cc1100_readReg(CC1100_PARTNUM);
			Reply->Result[ 1]= cc1100_readReg(CC1100_VERSION);
			Reply->Result[ 2]= cc1100_readReg(CC1100_FREQEST);
			Reply->Result[ 3]= cc1100_readReg(CC1100_LQI);
			Reply->Result[ 4]= cc1100_readReg(CC1100_RSSI);
			Reply->Result[ 5]= cc1100_readReg(CC1100_MARCSTATE);
			Reply->Result[ 6]= cc1100_readReg(CC1100_WORTIME0);
			Reply->Result[ 7]= cc1100_readReg(CC1100_WORTIME1);
			Reply->Result[ 8]= cc1100_readReg(CC1100_PKTSTATUS);
			Reply->Result[ 9]= cc1100_readReg(CC1100_VCO_VC_DAC);
			Reply->Result[10]= cc1100_readReg(CC1100_TXBYTES);
			Reply->Result[11]= cc1100_readReg(CC1100_RXBYTES);
			break;
		case CULSERVER_OP_CC1101SEND:
			memset(Reply->Result, 0, MAXLEN);
			Reply->Length= 1;
			Reply->Result[ 0]= CC1101_TESTPACKETSIZE;
			CC1101_TX_test();
			break;
		case CULSERVER_OP_BLINK:
			memset(Reply->Result, 0, MAXLEN);
			Reply->Length= 1;
			Reply->Result[ 0]= 5;
			for(int i= 0; i< 5; i++) {
				LED_ON();
				for(int j= 0; j< 25; j++) { _delay_ms(10); }
				LED_OFF();
				for(int j= 0; j< 25; j++) { _delay_ms(10); }
			}
			break;
		default:
			Reply->ExitCode= NAK;
	}

	/* Alter the incomming IP packet header so that the corrected IP source and destinations are used - this means that
	   when the response IP header is generated, it will use the corrected addresses and not the null/broatcast addresses */
	IPHeaderIN->DestinationAddress = ServerIPAddress;

	return sizeof(CULServer_Reply_t);
}

