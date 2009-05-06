#ifndef _INTERFACES_H_
#define _INTERFACES_H_

	#include <avr/io.h>

	typedef uint8_t If_t;

	/** enumeration of CUL interfaces */
	#define If_NONE		0x00
	#define If_RNDIS	0x01
	#define If_CC1101	0x02
	#define If_INTERNAL	0x08
	#define If_ALL		(If_RNDIS | If_CC1101 | If_INTERNAL)


#endif