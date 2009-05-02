
/** \file
 *
 *  User Datagram Protocol (UDP) packet handling routines. This protocol handles high throughput, low
 *  reliability packets which are typically used to encapsulate streaming data.
 */

#define  INCLUDE_FROM_UDP_C
#include "UDP.h"
#include "CULServer.h"
#include "RNDIS.h" // -> ADAPTER_MAC_ADDRESS

/** Processes a UDP packet inside an Ethernet frame, and writes the appropriate response
 *  to the output Ethernet frame if a subprotocol handler has created a response packet.
 *
 *  \param IPHeaderInStart    Pointer to the start of the incomming packet's IP header
 *  \param UDPHeaderInStart   Pointer to the start of the incomming packet's UDP header
 *  \param UDPHeaderOutStart  Pointer to the start of the outgoing packet's UDP header
 *
 *  \return The number of bytes written to the out Ethernet frame if any, NO_RESPONSE otherwise
 */
int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart, void* UDPHeaderInStart, void* UDPHeaderOutStart)
{
	UDP_Header_t* UDPHeaderIN  = (UDP_Header_t*)UDPHeaderInStart;
	UDP_Header_t* UDPHeaderOUT = (UDP_Header_t*)UDPHeaderOutStart;

	int16_t RetSize = NO_RESPONSE;

	/* Check to see if the UDP packet is a DHCP packet */
	if (SwapEndian_16(UDPHeaderIN->DestinationPort) == UDP_PORT_DHCP_REQUEST)
	{
		RetSize = NO_RESPONSE;
	}

	if (SwapEndian_16(UDPHeaderIN->DestinationPort) == 7073)
	{
                RetSize = CULServer_ProcessPacket(IPHeaderInStart,
                             &((uint8_t*)UDPHeaderInStart)[sizeof(UDP_Header_t)],
                             &((uint8_t*)UDPHeaderOutStart)[sizeof(UDP_Header_t)]);
	}


	/* Check to see if the protocol processing routine has filled out a response */
	if (RetSize > 0)
	{
		/* Fill out the response UDP packet header */
		uint16_t Port= UDPHeaderOUT->SourcePort;

		UDPHeaderOUT->SourcePort      = UDPHeaderIN->DestinationPort;
		UDPHeaderOUT->DestinationPort = Port;
		UDPHeaderOUT->Checksum        = 0;
		UDPHeaderOUT->Length          = SwapEndian_16(sizeof(UDP_Header_t) + RetSize);

		/* Return the size of the response so far */
		return (sizeof(UDP_Header_t) + RetSize);
	}

	return NO_RESPONSE;
}



const IP_Address_t  TargetIPAddress     = {TARGET_IP_ADDRESS};


uint16_t build_UDP_TestPacket(uint8_t* Data) {

	/*
	UDP_Packet_t*	UDPPacket= (UDP_Packet_t*) Data;

	const MAC_Address_t  AdapterMACAddress     = {ADAPTER_MAC_ADDRESS};

	// build Ethernet Frame
	memcpy(&UDPPacket->FrameHeader.Source, &ServerMACAddress, sizeof(MAC_Address_t));
	memcpy(&UDPPacket->FrameHeader.Destination, &AdapterMACAddress, sizeof(MAC_Address_t));
	UDPPacket->FrameHeader.EtherType= SwapEndian_16(ETHERTYPE_IPV4);

	// build IP Header
	UDPPacket->IPHeader.TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + 8+ TEST_PAYLOAD_SIZE);
	UDPPacket->IPHeader.TypeOfService      = 0;
	UDPPacket->IPHeader.HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
	UDPPacket->IPHeader.Version            = 4;
	UDPPacket->IPHeader.Flags              = 0;
	UDPPacket->IPHeader.FragmentOffset     = 0;
	UDPPacket->IPHeader.Identification     = 0;
	UDPPacket->IPHeader.HeaderChecksum     = 0;
	UDPPacket->IPHeader.Protocol           = PROTOCOL_UDP;
	UDPPacket->IPHeader.TTL                = DEFAULT_TTL;
	UDPPacket->IPHeader.SourceAddress      = ServerIPAddress;
	UDPPacket->IPHeader.DestinationAddress = TargetIPAddress;
	UDPPacket->IPHeader.HeaderChecksum     = Ethernet_Checksum16(&UDPPacket->IPHeader, sizeof(IP_Header_t));

	// build UDP header
	UDPPacket->UDPHeader.SourcePort= 0;
	UDPPacket->UDPHeader.DestinationPort= SwapEndian_16(7073);
	UDPPacket->UDPHeader.Length= SwapEndian_16(8+TEST_PAYLOAD_SIZE);
	UDPPacket->UDPHeader.Checksum= 0; // no checksum

	// build UDP message
	for(int i= 0; i<= TEST_PAYLOAD_SIZE; i++) {
		UDPPacket->UDPMessage[i]= i;
	}

	return sizeof(UDP_Packet_t)+TEST_PAYLOAD_SIZE-MAX_PAYLOAD_SIZE;
	*/
	uint8_t* Payload= get_UDP_Payload(Frame.FrameData);
	// build UDP message
	for(int i= 0; i< TEST_PAYLOAD_SIZE; i++) {
		Payload[i]= i;
	}
	return finalize_UDP_Packet(Frame.FrameData, TEST_PAYLOAD_SIZE);
}

uint8_t* get_UDP_Payload(uint8_t* Data) {

	return ((UDP_Packet_t*)Data)->UDPMessage;
}


uint16_t finalize_UDP_Packet(uint8_t* Data, uint8_t payloadsize) {

	UDP_Packet_t*	UDPPacket= (UDP_Packet_t*) Data;

	const MAC_Address_t  AdapterMACAddress     = {ADAPTER_MAC_ADDRESS};

	// sanity check
	if(payloadsize> MAX_PAYLOAD_SIZE) {
		payloadsize= MAX_PAYLOAD_SIZE;
	}

	// build Ethernet Frame
	memcpy(&UDPPacket->FrameHeader.Source, &ServerMACAddress, sizeof(MAC_Address_t));
	memcpy(&UDPPacket->FrameHeader.Destination, &AdapterMACAddress, sizeof(MAC_Address_t));
	UDPPacket->FrameHeader.EtherType= SwapEndian_16(ETHERTYPE_IPV4);

	// build IP Header
	UDPPacket->IPHeader.TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + 8+ payloadsize);
	UDPPacket->IPHeader.TypeOfService      = 0;
	UDPPacket->IPHeader.HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
	UDPPacket->IPHeader.Version            = 4;
	UDPPacket->IPHeader.Flags              = 0;
	UDPPacket->IPHeader.FragmentOffset     = 0;
	UDPPacket->IPHeader.Identification     = 0;
	UDPPacket->IPHeader.HeaderChecksum     = 0;
	UDPPacket->IPHeader.Protocol           = PROTOCOL_UDP;
	UDPPacket->IPHeader.TTL                = DEFAULT_TTL;
	UDPPacket->IPHeader.SourceAddress      = ServerIPAddress;
	UDPPacket->IPHeader.DestinationAddress = TargetIPAddress;
	UDPPacket->IPHeader.HeaderChecksum     = Ethernet_Checksum16(&UDPPacket->IPHeader, sizeof(IP_Header_t));

	// build UDP header
	UDPPacket->UDPHeader.SourcePort= 0;
	UDPPacket->UDPHeader.DestinationPort= SwapEndian_16(7073);
	UDPPacket->UDPHeader.Length= SwapEndian_16(8+payloadsize);
	UDPPacket->UDPHeader.Checksum= 0; // no checksum

	return sizeof(UDP_Packet_t)+payloadsize-MAX_PAYLOAD_SIZE;
}


