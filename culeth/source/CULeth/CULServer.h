

/** \file
 *
 *  Header file for CULServer.c.
 */

#ifndef _CULSERVER_H_
#define _CULSERVER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

	/* Macros: */

		/** CULServer operation constant, indicating a request from a host */
		#define CULSERVER_OP_ECHO       'e'

		#define CULSERVER_OP_IP         'i'

		#define CULSERVER_OP_STAT       's'

		#define ACK			6
		#define NAK			21

		#define MAXLEN			16


	/* Type Defines: */
		/** Type define for a CULServer request packet inside an Ethernet frame. */
		typedef struct
		{
			uint8_t Operation; /**< operation  */
			char Operand[MAXLEN];
		} CULServer_Request_t;

		/** Type define for a CULServer reply packet inside an Ethernet frame. */
		typedef struct
		{
			uint8_t ExitCode; /**< exit code  */
			char Result[MAXLEN];
		} CULServer_Reply_t;

	/* Function Prototypes: */
		int16_t CULServer_ProcessPacket(void* IPHeaderInStart, void* CULServerRequestStart, void* CULServerReplyStart);

#endif
