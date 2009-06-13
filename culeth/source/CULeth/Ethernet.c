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
 *  Ethernet frame packet handling routines. This protocol handles the processing of raw Ethernet
 *  frames sent and receieved, deferring the processing of subpacket protocols to the appropriate
 *  protocol handlers, such as DHCP or ARP.
 */

#include "Ethernet.h"
#include "ARP.h"

/* Global Variables: */
/** Ethernet Frame buffer structure, to hold the Ethernet frame. */
Ethernet_Frame_Info_t Frame;


/** Constant for convenience when checking against or setting a MAC address to the virtual server MAC address. */
//const MAC_Address_t ServerMACAddress    = {SERVER_MAC_ADDRESS};

/** Constant for convenience when checking against or setting an IP address to the virtual server IP address. */
//const IP_Address_t  ServerIPAddress     = {SERVER_IP_ADDRESS};

/** Constant for convenience when checking against or setting a MAC address to the broadcast MAC address. */
const MAC_Address_t BroadcastMACAddress = {BROADCAST_MAC_ADDRESS};

/** Constant for convenience when checking against or setting a IP address to the broadcast IP address. */
const IP_Address_t  BroadcastIPAddress  = {BROADCAST_IP_ADDRESS};

/** Constant for convenience when checking against or setting an IP address to the client (host) IP address. */
//const IP_Address_t  ClientIPAddress     = {CLIENT_IP_ADDRESS};



bool isServerMACAddress(void) {

	Ethernet_Frame_Header_t* FrameHeader  = (Ethernet_Frame_Header_t*)&Frame.FrameData;

	MAC_Address_t ServerMACAddress;
	get_CULServer_MAC(&ServerMACAddress);
	/* Ensure frame is addressed to either all (broadcast) or the virtual server, and is a type II frame */
	return MAC_COMPARE(&FrameHeader->Destination, &ServerMACAddress);

}

bool isBroadcastMACAddress(void) {

	Ethernet_Frame_Header_t* FrameHeader  = (Ethernet_Frame_Header_t*)&Frame.FrameData;
	/* Ensure frame is addressed to either all (broadcast) or the virtual server, and is a type II frame */
	return MAC_COMPARE(&FrameHeader->Destination, &BroadcastMACAddress);

}

/** Processes an incomming Ethernet frame, and writes the appropriate response to the output Ethernet
 *  frame buffer if the sub protocol handlers create a valid response.
 */
bool Ethernet_ProcessPacket(void)
{

	Ethernet_Frame_Header_t* FrameHeader    = (Ethernet_Frame_Header_t*)&Frame.FrameData;
	int16_t                  RetSize        = NO_RESPONSE;

	/* Ensure frame is a type II frame */
	if((SwapEndian_16(Frame.FrameLength) > ETHERNET_VER2_MINSIZE))
	{
		/* Process the packet depending on its protocol */
		switch (SwapEndian_16(FrameHeader->EtherType))
		{
			case ETHERTYPE_ARP:
				RetSize = ARP_ProcessARPPacket(&Frame.FrameData[sizeof(Ethernet_Frame_Header_t)],
				                               &Frame.FrameData[sizeof(Ethernet_Frame_Header_t)]);
				break;
			case ETHERTYPE_IPV4:
				RetSize = IP_ProcessIPPacket(&Frame.FrameData[sizeof(Ethernet_Frame_Header_t)],
				                             &Frame.FrameData[sizeof(Ethernet_Frame_Header_t)]);
				break;
		}

		/* Protcol processing routine has filled a response, complete the ethernet frame header */
		if (RetSize > 0)
		{
			/* Fill out the response Ethernet frame header */
			FrameHeader->Destination     = FrameHeader->Source;
			get_CULServer_MAC(&FrameHeader->Source);

			/* Set the response length in the buffer and indicate that a response is ready to be sent */
			Frame.FrameLength            = (sizeof(Ethernet_Frame_Header_t) + RetSize);

			return true;
		}
	}

	return false;

}

/** Calculates the appropriate ethernet checksum, consisting of the addition of the one's
 *  compliment of each word, complimented.
 *
 *  \param Data   Pointer to the packet buffer data whose checksum must be calculated
 *  \param Bytes  Number of bytes in the data buffer to process
 *
 *  \return A 16-bit Ethernet checksum value
 */
uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes)
{
	uint16_t* Words    = (uint16_t*)Data;
	uint32_t  Checksum = 0;

	for (uint8_t CurrWord = 0; CurrWord < (Bytes >> 1); CurrWord++)
	  Checksum += Words[CurrWord];

	while (Checksum & 0xFFFF0000)
	  Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

	return ~Checksum;
}
