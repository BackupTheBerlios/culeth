#ifndef _EXTRAS_H_
#define _EXTRAS_H_

#include <LUFA/Scheduler/Scheduler.h>

extern uint8_t extraflag;


#define EXTRA_NONE	0
#define EXTRA_TEST	1


TASK(Extras_Task);


#endif