// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/ICMP.cpp_v   1.0   09 Jun 2015 09:13:56   tayars  $
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

// FIXME Multi-Ethernet configuration - ICMP source IP wrong
// ICMP destination unreachable messages are sent from first Ethernet interface
//even if unreachable port (destination) was on the second interface.

#include "ICMP.h"
#include "Net.h"
#include "PktBuf.h"
#include "Ethernet.h"
#include "IPv4.h"
#include "DebugNew.h"

#define ICMP_TTL                255

/* The amount of data from the original packet to return in a dest-unreachable */
#define ICMP_DEST_UNREACH_DATASIZE 8

void icmp_init()
{
}

/**
 * Processes ICMP input packets, called from ip_input().
 *
 * Currently only processes icmp echo requests and sends
 * out the echo response.
 *
 * @param p the icmp echo request packet, p->payload pointing to the ip header
 * @param inp the netif on which this packet was received
 */
void icmp_input(PktBuf *p, Ethernet *inp)
{
	uint8 type;
	icmp_echo_hdr *iecho;
	IPv4Header *iphdr;
	ip_addr tmpaddr;
	int16 hlen;

	iphdr = (IPv4Header *) p->payload;
	hlen = iphdr->HL() * 4;
	if (pbuf_header(p, -hlen) || (p->tot_len < sizeof(uint16) * 2))
	{
		printf("icmp_input: short ICMP (%d bytes) received\n", p->tot_len);
		goto lenerr;
	}

	type = *((uint8 *) p->payload);
	switch (type)
	{
	case ICMP_ECHO:
		// multicast or broadcast destination address?
		if (ip_addr_ismulticast(&iphdr->dest) || ip_addr_isbroadcast(
				&iphdr->dest, inp))
		{
			printf("icmp_input: Not echoing to multicast or broadcast pings\n");
			pbuf_free(p);
			return;
		}
		if (p->tot_len < sizeof(icmp_echo_hdr))
		{
			printf("icmp_input: bad ICMP echo received\n");
			goto lenerr;
		}
		if (inet_chksum_pbuf(p) != 0)
		{
			printf("icmp_input: checksum failed for received ICMP echo\n");
			pbuf_free(p);
			return;
		}
		if (pbuf_header(p, (PBUF_IP_HLEN + PBUF_LINK_HLEN)))
		{
			/* p is not big enough to contain link headers
			 * allocate a new one and copy p into it
			 */
			PktBuf *r;
			/* switch p->payload to ip header */
			if (pbuf_header(p, hlen))
			{
				goto memerr;
			}
			/* allocate new packet buffer with space for link headers */
			r = pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_RAM);
			if (r == NULL)
			{
				goto memerr;
			}
			ASSERT((r->len >= hlen + sizeof(icmp_echo_hdr)));
			/* copy the whole packet including ip header */
			if (pbuf_copy(r, p) != ERR_OK)
			{
				goto memerr;
			}
			iphdr = (IPv4Header *) r->payload;
			/* switch r->payload back to icmp header */
			if (pbuf_header(r, -hlen))
			{
				goto memerr;
			}
			/* free the original p */
			pbuf_free(p);
			/* we now have an identical copy of p that has room for link headers */
			p = r;
		}
		else
		{
			/* restore p->payload to point to icmp header */
			if (pbuf_header(p, -(int16) (PBUF_IP_HLEN + PBUF_LINK_HLEN)))
			{
				goto memerr;
			}
		}
		/* At this point, all checks are OK. */
		/* We generate an answer by switching the dest and src ip addresses,
		 * setting the icmp type to ECHO_RESPONSE and updating the checksum. */
		iecho = (icmp_echo_hdr *) p->payload;
		tmpaddr.addr = iphdr->src.addr;
		iphdr->src.addr = iphdr->dest.addr;
		iphdr->dest.addr = tmpaddr.addr;
		ICMPH_TYPE_SET(iecho, ICMP_ER);
		/* adjust the checksum */
		if (iecho->chksum >= htons(0xffff - (ICMP_ECHO << 8)))
		{
			iecho->chksum = iecho->chksum + htons(ICMP_ECHO << 8) + 1;
		}
		else
		{
			iecho->chksum = iecho->chksum + htons(ICMP_ECHO << 8);
		}

		/* Set the correct TTL and recalculate the header checksum. */
		iphdr->TTL_SET(ICMP_TTL);
		iphdr->CHKSUM_SET(0);
		iphdr->CHKSUM_SET(inet_chksum(iphdr, sizeof(IPv4Header)));

		if (pbuf_header(p, hlen))
		{
			ASSERT(0); // "Can't move over header in packet", 0);
		}
		else
		{
			int ret = ip_output_if(p, &(iphdr->src), 0, ICMP_TTL, 0,
					IP_PROTO_ICMP, inp);
			if (ret != ERR_OK)
			{
				printf("icmp_input: ip_output_if returned an error: %c.\n", ret);
			}
		}
		break;
	default:
		printf("icmp_input: ICMP type %d not supported.\n", type);
	}
	pbuf_free(p);
	return;
	lenerr: pbuf_free(p);
	return;
	memerr: pbuf_free(p);
	return;
}

/**
 * Send an icmp packet in response to an incoming packet.
 *
 * @param p the input packet for which the 'unreachable' should be sent,
 *          p->payload pointing to the IP header
 * @param type Type of the ICMP header
 * @param code Code of the ICMP header
 */
void icmp_send_response(PktBuf *p, uint8 type, uint8 code)
{
	PktBuf *q;
	IPv4Header *iphdr;
	/* we can use the echo header here */
	icmp_echo_hdr *icmphdr;

	/* ICMP header + IP header + 8 bytes of data */
	q = pbuf_alloc(PBUF_IP, sizeof(icmp_echo_hdr) + sizeof(IPv4Header)
			+ ICMP_DEST_UNREACH_DATASIZE, PBUF_RAM);
	if (q == NULL)
	{
		printf("icmp_time_exceeded: failed to allocate pbuf for ICMP packet.\n");
		return;
	}
	ASSERT((q->len >= (sizeof(icmp_echo_hdr) + sizeof(IPv4Header) + ICMP_DEST_UNREACH_DATASIZE)));

	iphdr = (IPv4Header *) p->payload;

	icmphdr = (icmp_echo_hdr *) q->payload;
	icmphdr->type = type;
	icmphdr->code = code;
	icmphdr->id = 0;
	icmphdr->seqno = 0;

	/* copy fields from original packet */
	memcpy((uint8 *) q->payload + sizeof(icmp_echo_hdr), (uint8 *) p->payload,
			sizeof(IPv4Header) + ICMP_DEST_UNREACH_DATASIZE);

	/* calculate checksum */
	icmphdr->chksum = 0;
	icmphdr->chksum = inet_chksum(icmphdr, q->len);
	ip_output(q, NULL, &(iphdr->src), ICMP_TTL, 0, IP_PROTO_ICMP);
	pbuf_free(q);
}

/**
 * Send an icmp 'destination unreachable' packet, called from ip_input() if
 * the transport layer protocol is unknown and from udp_input() if the local
 * port is not bound.
 *
 * @param p the input packet for which the 'unreachable' should be sent,
 *          p->payload pointing to the IP header
 * @param t type of the 'unreachable' packet
 */
void icmp_dest_unreach(PktBuf *p, icmp_dur_type t)
{
	icmp_send_response(p, ICMP_DUR, t);
}

/**
 * Send a 'time exceeded' packet, called from ip_forward() if TTL is 0.
 *
 * @param p the input packet for which the 'time exceeded' should be sent,
 *          p->payload pointing to the IP header
 * @param t type of the 'time exceeded' packet
 */
void icmp_time_exceeded(PktBuf *p, icmp_te_type t)
{
	icmp_send_response(p, ICMP_TE, t);
}
