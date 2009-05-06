#ifndef _IF_CC1101_H_
#define _IF_CC1101_H_

	#include <stdbool.h>
	#include "Interfaces.h"

	void CC1101_Init(void);
	bool CC1101_RX(void);
	bool CC1101_TX(void);
	bool CC1101_TX_test(void);
	uint16_t build_UDP_Packet(uint8_t* Data);

	#define CC1101_TESTPACKETSIZE	16

	extern uint16_t FrameCount_If_CC1101_IN;
	extern uint16_t FrameCount_If_CC1101_OUT;

#endif