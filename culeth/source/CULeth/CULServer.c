
/** \file
 *
 *  CUL internal Server packet handling routines.
 */

#include "CULServer.h"
#include "Ethernet.h"
#include "IP.h"
#include "If_RNDIS.h"
#include "If_CC1101.h"


int16_t CULServer_ProcessPacket(void* IPHeaderInStart, void* CULServerRequestStart, void* CULServerReplyStart)
{

        IP_Header_t*   IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
	CULServer_Request_t*	Request= (CULServer_Request_t*)CULServerRequestStart;
	CULServer_Reply_t*	Reply  = (CULServer_Reply_t*)CULServerReplyStart;

	switch(Request->Operation) {
		case CULSERVER_OP_ECHO:
			Reply->ExitCode= ACK;
			//strncpy(Reply->Result, Request->Operand, MAXLEN);
			break;
		case CULSERVER_OP_IP:
			Reply->ExitCode= ACK;
			memset(Reply->Result, 0, MAXLEN);
			strncpy(Reply->Result, "10.0.0.2", MAXLEN);
			break;
		case CULSERVER_OP_STAT:
			Reply->ExitCode= ACK;
			memset(Reply->Result, 0, MAXLEN);
			sprintf(Reply->Result, "%d %d %d %d", FrameCount_If_RNDIS_IN,
							      FrameCount_If_RNDIS_OUT,
							      FrameCount_If_CC1101_IN,
							      FrameCount_If_CC1101_OUT);
			break;
		default:
			Reply->ExitCode= NAK;
	}

	/* Alter the incomming IP packet header so that the corrected IP source and destinations are used - this means that
	   when the response IP header is generated, it will use the corrected addresses and not the null/broatcast addresses */
	IPHeaderIN->DestinationAddress = ServerIPAddress;

	return sizeof(CULServer_Reply_t);
}
