#ifndef _EXTRAS_H_
#define _EXTRAS_H_

#include <LUFA/Scheduler/Scheduler.h>

extern uint8_t extraflag;
extern uint8_t extraarg;


#define EXTRA_NONE		0x00
#define EXTRA_START		0x01
#define EXTRA_TEST		0x7e
#define EXTRA_BOOTLOADER	0xfe
#define EXTRA_REBOOT		0xff
#define EXTRA_PACKET		0x05


TASK(Extras_Task);


#endif