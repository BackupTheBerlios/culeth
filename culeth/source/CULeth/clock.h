#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <LUFA/Scheduler/Scheduler.h>

typedef uint32_t clock_time_t;	// seconds
clock_time_t clock_time(void);

void timer_Init(void);

TASK(Timer_Task);

#endif
