#ifndef _IF_RNDIS_H_
#define _IF_RNDIS_H_

	#include <stdbool.h>
	#include "Interfaces.h"
	#include "RNDIS.h"
	#include <LUFA/Drivers/USB/USB.h>                // USB Functionality
	#include <LUFA/Scheduler/Scheduler.h>
	#include <LUFA/Drivers/AT90USBXXX/Serial_Stream.h>

	void RNDIS_Notification(void);
	bool RNDIS_RX(void);
	bool RNDIS_TX(void);

	extern uint16_t FrameCount_If_RNDIS_IN;
	extern uint16_t FrameCount_If_RNDIS_OUT;


#endif