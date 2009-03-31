#ifndef _IF_CC1101_H_
#define _IF_CC1101_H_

	#include <stdbool.h>
	#include "Interfaces.h"

	bool CC1101_RX(void);
	bool CC1101_TX(void);

	extern uint16_t FrameCount_If_CC1101_IN;
	extern uint16_t FrameCount_If_CC1101_OUT;

#endif