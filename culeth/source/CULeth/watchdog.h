#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <LUFA/Scheduler/Scheduler.h>

void enable_watchdog(void);
void disable_watchdog(void);
TASK(Watchdog_Task);

#endif