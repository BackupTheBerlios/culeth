#include "If_RNDIS.h"

uint16_t FrameCount_If_RNDIS_IN = 0;
uint16_t FrameCount_If_RNDIS_OUT= 0;


/** Task to manage the sending and receiving of encapsulated RNDIS data and notifications. This removes the RNDIS
 *  wrapper from recieved Ethernet frames and places them in the FrameIN global buffer, or adds the RNDIS wrapper
 *  to a frame in the FrameOUT global before sending the buffer contents to the host.
 */
void RNDIS_Notification(void)
{
	/* Select the notification endpoint */
	Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPNUM);

	/* Check if a message response is ready for the host */
	if (Endpoint_ReadWriteAllowed() && ResponseReady)
	{
		USB_Notification_t Notification = (USB_Notification_t)
			{
				bmRequestType: (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
				bNotification: NOTIF_RESPONSE_AVAILABLE,
				wValue:        0,
				wIndex:        0,
				wLength:       0,
			};

		/* Indicate that a message response is ready for the host */
		Endpoint_Write_Stream_LE(&Notification, sizeof(Notification));

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearCurrentBank();

		/* Indicate a response is no longer ready */
		ResponseReady = false;
	}
}


bool RNDIS_RX(void)
{
	/* Don't process the data endpoints until the system is in the data initialized state, and the buffer is free */
	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		/* Create a new packet header for reading/writing */
		RNDIS_PACKET_MSG_t RNDISPacketHeader;

		/* Select the data OUT endpoint */
		Endpoint_SelectEndpoint(CDC_RX_EPNUM);

		/* Check if the data OUT endpoint contains data, and that the IN buffer is empty */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Read in the packet message header */
			Endpoint_Read_Stream_LE(&RNDISPacketHeader, sizeof(RNDIS_PACKET_MSG_t));

			/* Stall the request if the data is too large */
			if (RNDISPacketHeader.MessageLength > ETHERNET_FRAME_SIZE_MAX)
			{
				Endpoint_StallTransaction();
				return false;
			}

			/* Read in the Ethernet frame into the buffer */
			Endpoint_Read_Stream_LE(Frame.FrameData, RNDISPacketHeader.DataLength);

			/* Finalize the stream transfer to send the last packet */
			Endpoint_ClearCurrentBank();

			/* Store the size of the Ethernet frame */
			Frame.FrameLength = RNDISPacketHeader.DataLength;

			FrameCount_If_RNDIS_IN++;

			return true;
		}

	}

	return false;

}

bool RNDIS_TX(void)
{
	/* Don't process the data endpoints until the system is in the data initialized state, and the buffer is free */
	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		/* Create a new packet header for reading/writing */
		RNDIS_PACKET_MSG_t RNDISPacketHeader;

		/* Select the data IN endpoint */
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);

		/* Check if the data IN endpoint is ready for more data, and that the IN buffer is full */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Clear the packet header with all 0s so that the relevant fields can be filled */
			memset(&RNDISPacketHeader, 0, sizeof(RNDIS_PACKET_MSG_t));

			/* Construct the required packet header fields in the buffer */
			RNDISPacketHeader.MessageType   = REMOTE_NDIS_PACKET_MSG;
			RNDISPacketHeader.MessageLength = (sizeof(RNDIS_PACKET_MSG_t) + Frame.FrameLength);
			RNDISPacketHeader.DataOffset    = (sizeof(RNDIS_PACKET_MSG_t) - sizeof(RNDIS_Message_Header_t));
			RNDISPacketHeader.DataLength    = Frame.FrameLength;

			/* Send the packet header to the host */
			Endpoint_Write_Stream_LE(&RNDISPacketHeader, sizeof(RNDIS_PACKET_MSG_t));

			/* Send the Ethernet frame data to the host */
			Endpoint_Write_Stream_LE(Frame.FrameData, RNDISPacketHeader.DataLength);

			/* Finalize the stream transfer to send the last packet */
			Endpoint_ClearCurrentBank();

			FrameCount_If_RNDIS_OUT++;

			return true;

		}
	}

	return false;

}
