// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/IPv4.h_v   1.0   09 Jun 2015 09:13:56   tayars  $
//
// Original file Copyright (c) 2007 Dominik Spies <kontakt@dspies.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Changes to this file copyright (c) Cyber Group, Inc 2009
//
// This file is part of CyGOS-3.

#ifndef IPV4_H_
#define IPV4_H_

#include "ArchDefs.h"

class Ethernet;

struct PktBuf;

enum IpProtocols
{
	IP_PROTO_ICMP = 1, IP_PROTO_IGMP = 2, IP_PROTO_UDP = 17, IP_PROTO_TCP = 6,
};

struct ip_addr
{
	MisAlignedLong addr;
	char * ToString();
	void operator =(uint32 value)
	{
		addr = value;
	}
private:
	static char address[16];
};

extern ip_addr ip_addr_any;
extern ip_addr ip_addr_broadcast;

/** IP_ADDR_ can be used as a fixed IP address
 *  for the wildcard and the broadcast address
 */
#define IP_ADDR_ANY         (&ip_addr_any)
#define IP_ADDR_BROADCAST   (&ip_addr_broadcast)

struct IPv4Header
{
	/** version / header length / type of service */
	MisAlignedShort _v_hl_tos;
	/** total length */
	MisAlignedShort _len;
	/** identification */
	MisAlignedShort _id;
	/** fragment offset field */
	MisAlignedShort _offset;
	/** time to live / protocol*/
	MisAlignedShort _ttl_proto;
	/** checksum */
	MisAlignedShort _chksum;
	/* source IP address */
	ip_addr src;
	/* destination IP address */
	ip_addr dest;
	int V()
	{
		return ntohs(_v_hl_tos) >> 12;
	}
	int HL()
	{
		return (ntohs(_v_hl_tos) >> 8) & 0x0f;
	}
	int TOS()
	{
		return ntohs(_v_hl_tos) & 0xff;
	}
	int LEN()
	{
		return _len;
	}
	int ID()
	{
		return _id;
	}
	int OFFSET()
	{
		return _offset;
	}
	int TTL()
	{
		return ntohs(_ttl_proto) >> 8;
	}
	int PROTO()
	{
		return ntohs(_ttl_proto) & 0xff;
	}
	int CHKSUM()
	{
		return _chksum;
	}

	void VHLTOS_SET(int v, int hl, int tos)
	{
		_v_hl_tos = (htons(((v) << 12) | ((hl) << 8) | (tos)));
	}
	void LEN_SET(int len)
	{
		_len = len;
	}
	void ID_SET(int id)
	{
		_id = id;
	}
	void OFFSET_SET(int off)
	{
		_offset = off;
	}
	void TTL_SET(int ttl)
	{
		_ttl_proto = htons(PROTO() | (ttl << 8));
	}
	void PROTO_SET(int proto)
	{
		_ttl_proto = htons(proto | (TTL() << 8));
	}
	void CHKSUM_SET(int chksum)
	{
		_chksum = chksum;
	}
};

#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */

struct IpReassembly
{
	IpReassembly *next;
	PktBuf *p;
	IPv4Header iphdr;
	uint16 datagram_len;
	uint8 flags;
	uint8 timer;
};

/** This is a helper struct which holds the starting
 * offset and the ending offset of this fragment to
 * easily chain the fragments.
 * It has to be packed since it has to fit inside the IP header.
 */
struct ip_reass_helper
{
	PktBuf *next_pbuf;
	uint16 start;
	uint16 end;
};

struct RouteTable//All entries should be network byte ordered
{
	ip_addr destinationIP;
	ip_addr sourceIP;
	ip_addr DstnetMask;
	ip_addr SrcnetMask;
	ip_addr gatewayIP;
};

inline bool ip_addr_isany(ip_addr * addr1)
{
	return (addr1 == 0) || (addr1->addr == 0);
}

#define ip_addr_set(dest, src) (dest)->addr = \
                               ((src) == NULL? 0:\
                               (src)->addr)
#define ip4_addr1(ipaddr) ((uint16)(ntohl((ipaddr)->addr) >> 24) & 0xff)
#define ip4_addr2(ipaddr) ((uint16)(ntohl((ipaddr)->addr) >> 16) & 0xff)
#define ip4_addr3(ipaddr) ((uint16)(ntohl((ipaddr)->addr) >> 8) & 0xff)
#define ip4_addr4(ipaddr) ((uint16)(ntohl((ipaddr)->addr)) & 0xff)
#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = htonl(((uint32)((a) & 0xff) << 24) | \
                               ((uint32)((b) & 0xff) << 16) | \
                               ((uint32)((c) & 0xff) << 8) | \
                                (uint32)((d) & 0xff))
#define ip_addr_debug_print(ipaddr) \
  printf("%d.%d.%d.%d",              \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 24) & 0xff : 0,  \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 16) & 0xff : 0,  \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 8) & 0xff : 0,   \
                      ipaddr != NULL ?                                  \
                      (uint16)ntohl((ipaddr)->addr) & 0xff : 0)

inline bool ip_addr_cmp(ip_addr * addr1, ip_addr * addr2)
{
	return addr1->addr == addr2->addr;
}

inline bool ip_addr_netcmp(ip_addr * addr1, ip_addr * addr2, ip_addr * mask)
{
	return ((addr1->addr & mask->addr) == (addr2->addr & mask->addr));
}

int inet_chksum(void *dataptr, int len);
int inet_chksum_pseudo(PktBuf *p, ip_addr *src, ip_addr *dest, int proto,
		int proto_len);
PktBuf * ip_reass(PktBuf *p);
bool ip_addr_isbroadcast(ip_addr *addr, Ethernet *netif);
bool ip_addr_ismulticast(ip_addr * addr1);
int ip_output_if(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto, Ethernet *netif);
int ip_output_if_opt(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto, Ethernet *netif, void *ip_options, int optlen);
int ip_output(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto);
void set_route(uint32 destination, uint32 source, uint32 dstnetMask,
		uint32 srcnetMask, uint32 gateway);
Ethernet * ip_route(ip_addr *dest, ip_addr * src);
int ip_frag(PktBuf *p, Ethernet *netif, ip_addr *dest);
void ip_reass_tmr();
Ethernet * ip_forward(PktBuf *p, IPv4Header *iphdr, Ethernet *inp);
ip_addr * ResolveGateway(ip_addr * destination, ip_addr * source,
		Ethernet * netif);
void ip_init();
int ip_input(PktBuf *p, Ethernet *inp);
void PrintAddress(ip_addr * addr);
void PrintRoutingTable();

#endif
