

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

		/** CULServer operation constants, indicating a request from a host */
		#define CULSERVER_OP_GETSTAT    's'
		#define CULSERVER_OP_GETCC1101STATUS 'u'
		#define CULSERVER_OP_GETCC1101CONFIG 'c'
		#define CULSERVER_OP_GETVERSION 'v'
		#define CULSERVER_OP_GETTIME    't'
		#define CULSERVER_OP_BLINK      'b'
		#define CULSERVER_OP_TESTPACKETONAIR       'a'
		#define CULSERVER_OP_TESTPACKETTOTARGET       'p'
		#define CULSERVER_OP_BOOTLOADER '>'
		#define CULSERVER_OP_EEPROM     'e'
		#define CULSERVER_OP_EEPROMRESET     'E'
		#define CULSERVER_OP_REBOOT '0'
		#define CULSERVER_OP_SETADAPTERMAC	'A'
		#define CULSERVER_OP_SETSERVERMAC	'M'
		#define CULSERVER_OP_SETSERVERIPPORT	'I'
		#define CULSERVER_OP_SETTARGETIPPORT	'T'

		#define ACK			6
		#define NAK			21

		#define MAXLEN			128


	/* Type Defines: */
		/** Type define for a CULServer request packet inside an Ethernet frame. */
		typedef struct
		{
			uint8_t Operation; /**< operation  */
			uint8_t Token;
			uint8_t Filler;
			uint8_t Length;
			uint8_t Operand[MAXLEN];
		} CULServer_Request_t;

		/** Type define for a CULServer reply packet inside an Ethernet frame. */
		typedef struct
		{
			uint8_t Operation; /**< operation  */
			uint8_t Token;
			uint8_t ExitCode; /**< exit code  */
			uint8_t Length;
			uint8_t Result[MAXLEN];
		} CULServer_Reply_t;

	/* Function Prototypes: */
		int16_t CULServer_ProcessPacket(void* IPHeaderInStart, void* CULServerRequestStart, void* CULServerReplyStart);

#endif
