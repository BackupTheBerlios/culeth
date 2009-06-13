
/** \file
 *
 *  General Ethernet protocol constants and type defines, for use by
 *  all network protocol portions of the TCP/IP stack.
 */

#ifndef _ETHERNET_PROTOCOLS_H_
#define _ETHERNET_PROTOCOLS_H_

	/* Macros: */
		#define ETHERTYPE_IPV4                   0x0800
		#define ETHERTYPE_ARP                    0x0806
		#define ETHERTYPE_RARP                   0x8035
		#define ETHERTYPE_APPLETALK              0x809b
		#define ETHERTYPE_APPLETALKARP           0x80f3
		#define ETHERTYPE_IEEE8021Q              0x8100
		#define ETHERTYPE_NOVELLIPX              0x8137
		#define ETHERTYPE_NOVELL                 0x8138
		#define ETHERTYPE_IPV6                   0x86DD
		#define ETHERTYPE_COBRANET               0x8819
		#define ETHERTYPE_PROVIDERBRIDGING       0x88a8
		#define ETHERTYPE_MPLSUNICAST            0x8847
		#define ETHERTYPE_MPLSMULTICAST          0x8848
		#define ETHERTYPE_PPPoEDISCOVERY         0x8863
		#define ETHERTYPE_PPPoESESSION           0x8864
		#define ETHERTYPE_EAPOVERLAN             0x888E
		#define ETHERTYPE_HYPERSCSI              0x889A
		#define ETHERTYPE_ATAOVERETHERNET        0x88A2
		#define ETHERTYPE_ETHERCAT               0x88A4
		#define ETHERTYPE_SERCOSIII              0x88CD
		#define ETHERTYPE_CESoE                  0x88D8
		#define ETHERTYPE_MACSECURITY            0x88E5
		#define ETHERTYPE_FIBRECHANNEL           0x8906
		#define ETHERTYPE_QINQ                   0x9100
		#define ETHERTYPE_VLLT                   0xCAFE

		#define PROTOCOL_ICMP                    1
		#define PROTOCOL_IGMP                    2
		#define PROTOCOL_TCP                     6
		#define PROTOCOL_UDP                     17
		#define PROTOCOL_OSPF                    89
		#define PROTOCOL_SCTP                    132

#endif
