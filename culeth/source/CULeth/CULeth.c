/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.

  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the RNDISEthernet demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "CULeth.h"
#include "board.h"
#include "led.h"
#include "clock.h"
#include "watchdog.h"
#include "config.h"
#include "cc1100.h"
#include "If_RNDIS.h"
#include "If_CC1101.h"
#include "Ethernet.h"
#include "UDP.h"
#include "extras.h"
#include "boot.h"


/* Scheduler Task List */
TASK_LIST
{
	{ Task: Watchdog_Task         , TaskStatus: TASK_RUN },
	{ Task: USB_USBTask           , TaskStatus: TASK_STOP },
	{ Task: RNDIS_NotificationTask, TaskStatus: TASK_STOP },
	{ Task: Ethernet_Task         , TaskStatus: TASK_STOP },
	{ Task: Timer_Task	      , TaskStatus: TASK_STOP },
	{ Task: Extras_Task	      , TaskStatus: TASK_RUN  },
};

/** Main program entry point. This routine configures the hardware required by the application, then
 *  starts the scheduler to run the USB management task.
 */
int main(void)
{
	/* Configuration setup */
	config_Init();

	/* start bootloader if requested */
	check_bootloader_request();

	/* Disable watchdog if enabled by bootloader/fuses */
	disable_watchdog(); // bootloader does not work if this comes earlier

	/* Hardware Initialization */
	timer_Init();
	LED_Init();
	CC1101_Init();
	SerialStream_Init(9600, false);

	/* Enable watchdog */
	enable_watchdog();

	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);

	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Indicate USB enumerating */
	UpdateStatus(Status_USBEnumerating);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops all the relevent tasks.
 */
EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running TCP/IP and USB management tasks */
	Scheduler_SetTaskMode(RNDIS_NotificationTask, TASK_STOP);
	Scheduler_SetTaskMode(Ethernet_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the RNDIS device endpoints and starts the relevent tasks.
 */
EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup CDC Notification, Rx and Tx Endpoints */
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	UpdateStatus(Status_USBReady);

	/* Start IP tasks */
	Scheduler_SetTaskMode(RNDIS_NotificationTask, TASK_RUN);
	Scheduler_SetTaskMode(Ethernet_Task, TASK_RUN);
}

/** Event handler for the USB_UnhandledControlPacket event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the RNDIS control commands,
 *  which set up the USB RNDIS network adapter), so that they can be handled appropriately for the application.
 */
EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Discard the unused wValue parameter */
	Endpoint_Discard_Word();

	/* Discard the unused wIndex parameter */
	Endpoint_Discard_Word();

	/* Read in the wLength parameter */
	uint16_t wLength = Endpoint_Read_Word_LE();

	/* Process RNDIS class commands */
	switch (bRequest)
	{
		case SEND_ENCAPSULATED_COMMAND:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Clear the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();

				/* Read in the RNDIS message into the message buffer */
				Endpoint_Read_Control_Stream_LE(RNDISMessageBuffer, wLength);

				/* Finalize the stream transfer to clear the last packet from the host */
				Endpoint_ClearSetupIN();

				/* Process the RNDIS message */
				ProcessRNDISControlMessage();
			}

			break;
		case GET_ENCAPSULATED_RESPONSE:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Check if a response to the last message is ready */
				if (!(MessageHeader->MessageLength))
				{
					/* Set the response to a single 0x00 byte to indicate that no response is ready */
					RNDISMessageBuffer[0] = 0;
					MessageHeader->MessageLength = 1;
				}

				/* Check if less than the requested number of bytes to transfer */
				if (MessageHeader->MessageLength < wLength)
				  wLength = MessageHeader->MessageLength;

				/* Clear the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();

				/* Write the message response data to the endpoint */
				Endpoint_Write_Control_Stream_LE(RNDISMessageBuffer, wLength);

				/* Finalize the stream transfer to send the last packet or clear the host abort */
				Endpoint_ClearSetupOUT();

				/* Reset the message header once again after transmission */
				MessageHeader->MessageLength = 0;
			}

			break;
	}
}

/** Function to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the RNDISEthernet_StatusCodes_t enum
 */
void UpdateStatus(uint8_t CurrentStatus)
{
	/* Set the LED mask to the appropriate LED mask based on the given status code */
	switch (CurrentStatus)
	{
		case Status_USBNotReady:
			LED_OFF();
			break;
		case Status_USBEnumerating:
			LED_ON();
			break;
		case Status_USBReady:
                        LED_OFF();
			break;
		case Status_ProcessingEthernetFrame:
                        LED_ON();
			break;
	}

}


TASK(RNDIS_NotificationTask)
{
	RNDIS_Notification();
}


If_t RX(void) {

	if(RNDIS_RX()) return If_RNDIS;
	if(CC1101_RX()) return If_CC1101;

	return If_NONE;
}

void TX(If_t Destination) {

	if(Destination & If_RNDIS) RNDIS_TX();
	if(Destination & If_CC1101) CC1101_TX();

}



/** Ethernet frame processing task. This task checks to see if a frame has been received, and if so hands off the processing
 *  of the frame to the Ethernet processing routines.
 */
TASK(Ethernet_Task)
{
	uint8_t	If_Src= If_NONE;
	uint8_t If_Dst= If_NONE;

	/* receive ethernet frame from source interface */
	If_Src= RX();

	/* process frame in buffer */
	if(If_Src != If_NONE) {

		/* update status LED */
		UpdateStatus(Status_ProcessingEthernetFrame);

		/* process destinations */
		if(isServerMACAddress()) {
			If_Dst= If_INTERNAL;
		} else {
			if(isBroadcastMACAddress()) {
				If_Dst= If_ALL & ~If_Src;
			} else {
				if(If_Src== If_CC1101) {
					If_Dst= If_RNDIS;
				} else {
					If_Dst= If_CC1101;
				}
			}
		}

		/* forward frame to destination interfaces */
		TX(If_Dst);

		extraflag= EXTRA_PACKET;
  		extraarg = 16*If_Src+If_Dst;

		if(If_Dst & If_INTERNAL) {
			if(Ethernet_ProcessPacket()) {
				If_Dst= If_Src;
				If_Src= If_INTERNAL;
				TX(If_Dst);
			}
		}


		UpdateStatus(Status_USBReady);

	}

}
