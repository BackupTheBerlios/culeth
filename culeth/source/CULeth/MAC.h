#ifndef _MAC_H_
#define _MAC_H_

#include <avr/io.h>

/** Type define for a physical MAC address of a device on a network */
typedef struct {
	uint8_t       Octets[6]; /**< Individual bytes of a MAC address */
} MAC_Address_t;


void get_CULServer_MAC(MAC_Address_t *MACAddress);
void set_CULServer_MAC(MAC_Address_t *MACAddress);
void get_Adapter_MAC(MAC_Address_t *MACAddress);
void set_Adapter_MAC(MAC_Address_t *MACAddress);

#endif