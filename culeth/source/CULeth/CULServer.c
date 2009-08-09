
/** \file
 *
 *  CUL internal Server packet handling routines.
 */

#include <util/crc16.h>
#include "CULServer.h"
#include "cc1100.h"
#include "MAC.h"
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
		case CULSERVER_OP_TESTPACKETTOTARGET:
			Reply->Length= 0;
			extraflag= EXTRA_TEST;
			extraarg = 0xff;
			break;
		case CULSERVER_OP_TESTPACKETONAIR:
			Reply->Length= 1;
			Reply->Result[ 0]= CC1101_TESTPACKETSIZE;
			CC1101_TX_test();
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
			ccGetStatusRegisters(Reply->Result);
			break;
		case CULSERVER_OP_GETCC1101CONFIG:
			Reply->Length= CC1100_CONFIG_SIZE_R;
			ccGetConfig(Reply->Result);
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
		case CULSERVER_OP_REBOOT:
			Reply->Length= 1;
			Reply->Result[0]= CFG_NOREQBL;
			extraflag= EXTRA_REBOOT;
			break;
		case CULSERVER_OP_BOOTLOADER:
			Reply->Length= 1;
			Reply->Result[0]= CFG_REQBL;
			extraflag= EXTRA_BOOTLOADER;
			break;
		case CULSERVER_OP_EEPROM:
			Reply->Length= CFG_LENGTH;
			for(int i= 0; i< Reply->Length; i++)
				Reply->Result[i]= get_config_byte(CFG_START+i);
			break;
		case CULSERVER_OP_EEPROMRESET1:
			Reply->Length= 0;
			factory_reset(0);
			extraflag= EXTRA_REBOOT;
			break;
		case CULSERVER_OP_EEPROMRESET2:
			Reply->Length= 0;
			factory_reset(1);
			extraflag= EXTRA_REBOOT;
			break;
		case CULSERVER_OP_SETTARGETIPPORT:
			set_Target_IP((IP_Address_t*)Request->Operand);
			set_config_byte(CFG_OFS_TPORT  , Request->Operand[4]);
			set_config_byte(CFG_OFS_TPORT+1, Request->Operand[5]);
			Reply->Length= sizeof(IP_Address_t);
			get_Target_IP((IP_Address_t*)Reply->Result);
			break;
		case CULSERVER_OP_SETSERVERIPPORT:
			set_CULServer_IP((IP_Address_t*)Request->Operand);
			set_config_byte(CFG_OFS_SPORT  , Request->Operand[4]);
			set_config_byte(CFG_OFS_SPORT+1, Request->Operand[5]);
			Reply->Length= sizeof(IP_Address_t);
			get_CULServer_IP((IP_Address_t*)Reply->Result);
			break;
		case CULSERVER_OP_SETSERVERMAC:
			set_CULServer_MAC((MAC_Address_t*)Request->Operand);
			Reply->Length= sizeof(MAC_Address_t);
			get_CULServer_MAC((MAC_Address_t*)Reply->Result);
			extraflag= EXTRA_REBOOT;
			break;
		case CULSERVER_OP_SETADAPTERMAC:
			set_Adapter_MAC((MAC_Address_t*)Request->Operand);
			Reply->Length= sizeof(MAC_Address_t);
			get_Adapter_MAC((MAC_Address_t*)Reply->Result);
			extraflag= EXTRA_REBOOT;
			break;
		default:
			Reply->ExitCode= NAK;
	}

	/* Alter the incoming IP packet header so that the corrected IP source and destinations are used - this means that
	   when the response IP header is generated, it will use the corrected addresses and not the null/broadcast addresses */
	get_CULServer_IP(&IPHeaderIN->DestinationAddress);

	return 4+Reply->Length; //sizeof(CULServer_Reply_t);
}

