// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/IPv4.cpp_v   1.0   18 Aug 2014 09:16:32   amaurya  $
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

// TODO Verify routing on multiple interfaces.
// TODO Verify routing on single interface.

#include "IPv4.h"
#include "Ethernet.h"
#include "PktBuf.h"
#include "Net.h"
#include "ARP.h"
#include "Raw.h"
#include "TCP.h"
#include "UDP.h"
#include "ICMP.h"
#include "IGMP.h"
#include "DhcpClient.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "DebugNew.h"

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

#define IP_REASS_MAX_PBUFS      10
#define IP_REASS_FLAG_LASTFRAG 0x01

#define ip_addr_set(dest, src) (dest)->addr = \
                               ((src) == NULL? 0:\
                               (src)->addr)
#define ip_addr_isany(addr1) ((addr1) == NULL || (addr1)->addr == 0)

#define FOLD_U32T(u)          ((u >> 16) + (u & 0x0000ffffUL))
#define SWAP_BYTES_IN_WORD(w) ((w & 0xff) << 8) | ((w & 0xff00) >> 8)

/* used by IP_ADDR_ANY and IP_ADDR_BROADCAST in ip_addr.h */
struct ip_addr ip_addr_any;// = { IP_ADDR_ANY_VALUE };
struct ip_addr ip_addr_broadcast;// = { IP_ADDR_BROADCAST_VALUE };

/**
 * The interface that provided the packet for the current callback
 * invocation.
 */
static Ethernet *current_netif;

/**
 * Header of the input packet currently being processed.
 */
static const IPv4Header *current_header;
static uint16 ip_reass_pbufcount;
static IpReassembly *reassdatagrams;
static PtrList<RouteTable> routingTable;

char ip_addr::address[16];

//disable debug
//#define printf
//#define pprintf


char * ip_addr::ToString()
{
	uint32 ip = ntohl(addr);
	sprintf(address, "%d.%d.%d.%d", (int)((ip >> 24) & 0xff), (int)((ip >> 16) & 0xff), (int)((ip
			>> 8) & 0xff), (int)(ip & 0xff));
	return address;
}

void ip_init()
{
	ip_addr_any.addr = IP_ADDR_ANY_VALUE;
	ip_addr_broadcast.addr = IP_ADDR_BROADCAST_VALUE;
}

void PrintAddress(ip_addr * addr)
{
	uint8 * ptr = (uint8 *) "\0\0\0\0";
	if (addr)
		ptr = (uint8 *) &addr->addr;
//	printf("%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
}

bool ip_addr_ismulticast(ip_addr * addr1)
{
	return (((addr1)->addr & ntohl(0xf0000000UL)) == ntohl(0xe0000000UL));
}

/**
 * Determine if an address is a broadcast address on a network interface
 *
 * @param addr address to be checked
 * @param netif the network interface against which the address is checked
 * @return returns non-zero if the address is a broadcast address
 */
bool ip_addr_isbroadcast(ip_addr *addr, Ethernet *netif)
{
	uint32 addr2test;

	addr2test = addr->addr;
	/* all ones (broadcast) or all zeroes (old skool broadcast) */
	if ((~addr2test == IP_ADDR_ANY_VALUE) || (addr2test == IP_ADDR_ANY_VALUE))
		return true;
	/* TODO no broadcast support on this network interface? */
	//	if ((netif->flags & NETIF_FLAG_BROADCAST) == 0)
	//		/* the given address cannot be a broadcast address
	//		 * nor can we check against any broadcast addresses */
	//		return 0;
	/* address matches network interface address exactly? => no broadcast */
	// TODO Multiple IP addresses from now on?
	if (addr2test == netif->ipaddr.addr)
		return false;
	/*  on the same (sub) network... */
	if (ip_addr_netcmp(addr, &(netif->ipaddr), &(netif->netmask))
	/* ...and host identifier bits are all ones? =>... */
	&& ((addr2test & ~netif->netmask.addr) == (IP_ADDR_BROADCAST_VALUE
			& ~netif->netmask.addr)))
		/* => network broadcast address */
		return true;
	return false;
}

bool ip_check(Ethernet * netif, IPv4Header * iphdr)
{
	/* interface is up and configured? */
	if (!ip_addr_isany(&netif->ipaddr))
	{
		/* unicast to this interface address? */
		if (ip_addr_cmp(&(iphdr->dest), &(netif->ipaddr)) ||
		/* or broadcast on this interface network address? */
		ip_addr_isbroadcast(&(iphdr->dest), netif))
			return true;
	}
	return false;
}
/**
 * This function is called by the network interface device driver when
 * an IP packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip_forward). The IP checksum is always checked.
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 *
 * @param p the received IP packet (p->payload points to IP header)
 * @param inp the netif on which this packet was received
 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
 *         processed, but currently always returns ERR_OK)
 */
int ip_input(PktBuf *p, Ethernet *inp)
{
	IPv4Header * iphdr = (IPv4Header *) p->payload;
	if (iphdr->V() != 4)
	{
		//printf("Bad IP version %d\n", iphdr->V());
		pbuf_free(p);
		return ERR_OK;
	}
	/* obtain IP header length */
	int iphdr_hlen = iphdr->HL() * 4;
	/* obtain ip length in bytes */
	int iphdr_len = ntohs(iphdr->LEN());

	/* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
	if ((iphdr_hlen > p->len) || (iphdr_len > p->tot_len))
	{
//		printf("Bad IP header length\n");
		pbuf_free(p);
		return ERR_OK;
	}

	/* verify checksum */
	if (inet_chksum(iphdr, iphdr_hlen) != 0)
	{
		printf("Bad IP checksum, header %08X\n", iphdr->CHKSUM());
		pbuf_free(p);
		return ERR_OK;
	}

	/* Trim pbuf. This should have been done at the netif layer,
	 * but we'll do it anyway just to be sure that its done. */
	pbuf_realloc(p, iphdr_len);
	Ethernet * netif;
	/* match packet against an interface, i.e. is this packet for us? */
	if (ip_addr_ismulticast(&(iphdr->dest)))
	{
		if (igmp_lookfor_group(inp, &iphdr->dest))
		{
			netif = inp;
		}
		else
		{
			netif = NULL;
		}
	}
	else
	{
		netif = inp;
		if (!ip_check(netif, iphdr))
		{
			POSITION pos = netif_list.GetHeadPosition();
			while (pos)
			{
				netif = netif_list.GetNext(pos);
				if (ip_check(netif, iphdr))
					break;
				netif = 0;
			}
		}
	}
	// Pass DHCP messages regardless of destination address. DHCP traffic is addressed
	// using link layer addressing (such as Ethernet MAC) so we must not filter on IP.
	// According to RFC 1542 section 3.1.1, referred by RFC 2131).
	bool check_ip_src = true;
	if (!netif)
	{
		/* remote port is DHCP server? */
		if (iphdr->PROTO() == IP_PROTO_UDP)
		{
//			 printf("ip_input: UDP packet to DHCP client port %d\n", ntohs(
//			 ((UdpHeader *) ((uint8 *) iphdr + iphdr_hlen))->dest));
			if (ntohs(((UdpHeader *) ((uint8 *) iphdr + iphdr_hlen))->dest)
					== DHCP_CLIENT_PORT)
			{
//				 printf("ip_input: DHCP packet accepted.\n");
				netif = inp;
				check_ip_src = false;
			}
		}
	}
	/* broadcast or multicast packet source address? Compliant with RFC 1122: 3.2.1.3 */
	// DHCP-enabled - don't check source address if false
	if (check_ip_src)
	{
		if ((ip_addr_isbroadcast(&(iphdr->src), inp)) || (ip_addr_ismulticast(
				&(iphdr->src))))
		{
			// DHCP server will receive UDP datagram from a broadcast source
			// address since the address for the sender is not configured yet.
			// Handle this case separately
			int destport = ntohs(
					((UdpHeader *) ((uint8 *) iphdr + iphdr_hlen))->dest);
			if ((iphdr->PROTO() == IP_PROTO_UDP) && (destport
					== DHCP_SERVER_PORT))
			{
				 //printf("DHCP server packet received\n");
			}
			else
			{
//				printf("Broadcast/multicast address\n");
				pbuf_free(p);
				return ERR_OK;
			}
		}
	}
	// packet not for us?
	if (netif == NULL)
	{
		/* non-broadcast packet? */
		// if (!ip_addr_isbroadcast(&(iphdr->dest), inp))
		// Ethernet header is in p+SIZEOF_STRUCT_PBUF
		EthHeader * ehdr = (EthHeader *) (((uint8 *) p) + SIZEOF_STRUCT_PBUF);
		if (memcmp(&ehdr->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)
		{
			//before forwarding perform the subnet check
			//the check makes sure that we do forward the packets on same interface
			//if it is not destined for us
			if (ip_addr_netcmp(&(inp->ipaddr), &(iphdr->dest), &(inp->netmask)))
			{
				//ip_forward(p, iphdr, inp);/* try to forward IP packet*/
			}
		}
		pbuf_free(p);
		return ERR_OK;
	}
	/* packet consists of multiple fragments? */
	if ((iphdr->OFFSET() & htons(IP_OFFMASK | IP_MF)) != 0)
	{
		pbuf_free(p);
		return ERR_OK;
		/* reassemble the packet*/
		p = ip_reass(p);
		/* packet not fully reassembled yet? */
		if (!p)
		{
			pprintf("Reassembly problems\n");
			return ERR_OK;
		}
		iphdr = (IPv4Header *) p->payload;
	}

	current_netif = inp;
	current_header = iphdr;

	/* raw input did not eat the packet? */
	if (!raw_input(p, inp))
	{
		switch (iphdr->PROTO())
		{
		case IP_PROTO_UDP:
			//pprintf(" UI ");
			udp_input(p, inp);
			break;
		case IP_PROTO_TCP:
			//pprintf(" TI ");
			tcp_input(p, inp);
			break;
		case IP_PROTO_ICMP:
			//pprintf(" IC ");
			icmp_input(p, inp);
			break;
		case IP_PROTO_IGMP:
			//pprintf(" IG ");
			igmp_input(p, inp, &(iphdr->dest));
			break;
		default:
			//pprintf(" DF ");
//			printf("Undefined protocol %d\n", iphdr->PROTO());
			/* send ICMP destination protocol unreachable unless is was a broadcast */
			if (!ip_addr_isbroadcast(&(iphdr->dest), inp)
					&& !ip_addr_ismulticast(&(iphdr->dest)))
			{
				//pprintf(" DUN ");
				p->payload = (uint8 *) iphdr;
				icmp_dest_unreach(p, ICMP_DUR_PROTO);
			}
			//pprintf(" FDF ");
			pbuf_free(p);
		}
	}
	current_netif = NULL;
	current_header = NULL;

	return ERR_OK;
}

/**
 * lwip checksum
 *
 * @param dataptr points to start of data to be summed at any boundary
 * @param len length of data to be summed
 * @return host order (!) lwip checksum (non-inverted Internet sum)
 *
 * @note accumulator size limits summable length to 64k
 * @note host endianess is irrelevant (p3 RFC1071)
 */
static int lwip_standard_chksum(void *dataptr, int len)
{
	uint32 acc;
	uint16 src;
	uint8 *octetptr;

	acc = 0;
	/* dataptr may be at odd or even addresses */
	octetptr = (uint8*) dataptr;
	while (len > 1)
	{
		/* declare first octet as most significant
		 thus assume network order, ignoring host order */
		src = (*octetptr) << 8;
		octetptr++;
		/* declare second octet as least significant */
		src |= (*octetptr);
		octetptr++;
		acc += src;
		len -= 2;
	}
	if (len > 0)
	{
		/* accumulate remaining octet */
		src = (*octetptr) << 8;
		acc += src;
	}
	/* add deferred carry bits */
	acc = (acc >> 16) + (acc & 0x0000ffffUL);
	if ((acc & 0xffff0000UL) != 0)
	{
		acc = (acc >> 16) + (acc & 0x0000ffffUL);
	}
	/* This maybe a little confusing: reorder sum using htons()
	 instead of ntohs() since it has a little less call overhead.
	 The caller must invert bits for Internet sum ! */
	return htons(acc);
}

/* inet_chksum:
 *
 * Calculates the Internet checksum over a portion of memory. Used primarily for IP
 * and ICMP.
 *
 * @param dataptr start of the buffer to calculate the checksum (no alignment needed)
 * @param len length of the buffer to calculate the checksum
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
int inet_chksum(void *dataptr, int len)
{
	return (~lwip_standard_chksum(dataptr, len)) & 0xFFFF;
}

bool IP_ADDRESSES_AND_ID_MATCH(IPv4Header * iphdrA, IPv4Header *iphdrB)
{
	if (!ip_addr_cmp(&iphdrA->src, &iphdrB->src))
		return false;
	if (!ip_addr_cmp(&iphdrA->dest, &iphdrB->dest))
		return false;
	return iphdrA->ID() == iphdrB->ID();
}

/**
 * Dequeues a datagram from the datagram queue. Doesn't deallocate the pbufs.
 * @param ipr points to the queue entry to dequeue
 */
static void ip_reass_dequeue_datagram(IpReassembly *ipr, IpReassembly *prev)
{

	/* dequeue the reass struct  */
	if (reassdatagrams == ipr)
	{
		/* it was the first in the list */
		reassdatagrams = ipr->next;
	}
	else
	{
		/* it wasn't the first, so it must have a valid 'prev' */
		prev->next = ipr->next;
	}
	/* now we can free the ip_reass struct */
	delete ipr;
}

/**
 * Free a datagram (struct ip_reassdata) and all its pbufs.
 * Updates the total count of enqueued pbufs (ip_reass_pbufcount),
 * SNMP counters and sends an ICMP time exceeded packet.
 *
 * @param ipr datagram to free
 * @param prev the previous datagram in the linked list
 * @return the number of pbufs freed
 */
static int ip_reass_free_complete_datagram(IpReassembly *ipr,
		IpReassembly *prev)
{
	int pbufs_freed = 0;
	PktBuf *p;
	ip_reass_helper *iprh;

	ASSERT(prev != ipr);
	if (prev != NULL)
	{
		ASSERT(prev->next == ipr);
	}

	//	snmp_inc_ipreasmfails();
	iprh = (ip_reass_helper *) ipr->p->payload;
	if (iprh->start == 0)
	{
		/* The first fragment was received, send ICMP time exceeded. */
		/* First, de-queue the first pbuf from r->p. */
		p = ipr->p;
		ipr->p = iprh->next_pbuf;
		/* Then, copy the original header into it. */
		memcpy(p->payload, &ipr->iphdr, sizeof(IPv4Header));
		icmp_time_exceeded(p, ICMP_TE_FRAG);
		pbufs_freed += pbuf_clen(p);
		pbuf_free(p);
	}

	/* First, free all received pbufs.  The individual pbufs need to be released
	 separately as they have not yet been chained */
	p = ipr->p;
	while (p != NULL)
	{
		PktBuf *pcur;
		iprh = (ip_reass_helper *) p->payload;
		pcur = p;
		/* get the next pointer before freeing */
		p = iprh->next_pbuf;
		pbufs_freed += pbuf_clen(pcur);
		pbuf_free(pcur);
	}
	/* Then, unchain the struct ip_reassdata from the list and free it. */
	if(prev)//TODO:Test It
		ip_reass_dequeue_datagram(ipr, prev);
	ASSERT(ip_reass_pbufcount >= pbufs_freed);
	ip_reass_pbufcount -= pbufs_freed;

	return pbufs_freed;
}

/**
 * Free the oldest datagram to make room for enqueueing new fragments.
 * The datagram 'fraghdr' belongs to is not freed!
 *
 * @param fraghdr IP header of the current fragment
 * @param pbufs_needed number of pbufs needed to enqueue
 *        (used for freeing other datagrams if not enough space)
 * @return the number of pbufs freed
 */
static int ip_reass_remove_oldest_datagram(IPv4Header *fraghdr,
		int pbufs_needed)
{
	/* @todo Can't we simply remove the last datagram in the
	 *       linked list behind reassdatagrams?
	 */
	IpReassembly *r, *oldest, *prev;
	int pbufs_freed = 0, pbufs_freed_current;
	int other_datagrams;

	/* Free datagrams until being allowed to enqueue 'pbufs_needed' pbufs,
	 * but don't free the datagram that 'fraghdr' belongs to! */
	do
	{
		oldest = NULL;
		prev = NULL;
		other_datagrams = 0;
		r = reassdatagrams;
		while (r != NULL)
		{
			if (!IP_ADDRESSES_AND_ID_MATCH(&r->iphdr, fraghdr))
			{
				/* Not the same datagram as fraghdr */
				other_datagrams++;
				if (oldest == NULL)
				{
					oldest = r;
				}
				else if (r->timer <= oldest->timer)
				{
					/* older than the previous oldest */
					oldest = r;
				}
			}
			if (r->next != NULL)
			{
				prev = r;
			}
			r = r->next;
		}
		if (oldest != NULL)
		{
			pbufs_freed_current = ip_reass_free_complete_datagram(oldest, prev);
			pbufs_freed += pbufs_freed_current;
		}
	} while ((pbufs_freed < pbufs_needed) && (other_datagrams > 1));
	return pbufs_freed;
}

/**
 * Enqueues a new fragment into the fragment queue
 * @param fraghdr points to the new fragments IP hdr
 * @param clen number of pbufs needed to enqueue (used for freeing other datagrams if not enough space)
 * @return A pointer to the queue location into which the fragment was enqueued
 */
static IpReassembly * ip_reass_enqueue_new_datagram(IPv4Header *fraghdr,
		int clen)
{
	/* No matching previous fragment found, allocate a new reassdata struct */
	IpReassembly * ipr = new IpReassembly();
	if (ipr == NULL)
	{
		if (ip_reass_remove_oldest_datagram(fraghdr, clen) >= clen)
			ipr = new IpReassembly();
		if (!ipr)
			return NULL;
	}
	memset(ipr, 0, sizeof(IpReassembly));
	ipr->timer = IP_REASS_MAXAGE;

	/* enqueue the new structure to the front of the list */
	ipr->next = reassdatagrams;
	reassdatagrams = ipr;
	/* copy the ip header for later tests and input */
	/* @todo: no ip options supported? */
	memcpy(&(ipr->iphdr), fraghdr, sizeof(IPv4Header));
	return ipr;
}

/**
 * Chain a new pbuf into the pbuf list that composes the datagram.  The pbuf list
 * will grow over time as  new pbufs are rx.
 * Also checks that the datagram passes basic continuity checks (if the last
 * fragment was received at least once).
 * @param root_p points to the 'root' pbuf for the current datagram being assembled.
 * @param new_p points to the pbuf for the current fragment
 * @return 0 if invalid, >0 otherwise
 */
static int ip_reass_chain_frag_into_datagram_and_validate(IpReassembly *ipr,
		PktBuf *new_p)
{
	ip_reass_helper *iprh, *iprh_tmp, *iprh_prev = NULL;
	PktBuf *q;
	uint16 offset, len;
	IPv4Header *fraghdr;
	int valid = 1;

	/* Extract length and fragment offset from current fragment */
	fraghdr = (IPv4Header*) new_p->payload;
	len = ntohs(fraghdr->LEN()) - fraghdr->HL() * 4;
	offset = (ntohs(fraghdr->OFFSET()) & IP_OFFMASK) * 8;

	/* overwrite the fragment's ip header from the pbuf with our helper struct,
	 * and setup the embedded helper structure. */
	iprh = (ip_reass_helper*) new_p->payload;
	iprh->next_pbuf = NULL;
	iprh->start = offset;
	iprh->end = offset + len;

	/* Iterate through until we either get to the end of the list (append),
	 * or we find on with a larger offset (insert). */
	for (q = ipr->p; q != NULL;)
	{
		iprh_tmp = (ip_reass_helper*) q->payload;
		if (iprh->start < iprh_tmp->start)
		{
			/* the new pbuf should be inserted before this */
			iprh->next_pbuf = q;
			if (iprh_prev != NULL)
			{
				/* not the fragment with the lowest offset */
				if ((iprh->start < iprh_prev->end) || (iprh->end
						> iprh_tmp->start))
				{
					/* fragment overlaps with previous or following, throw away */
					goto freepbuf;
				}
				iprh_prev->next_pbuf = new_p;
			}
			else
			{
				/* fragment with the lowest offset */
				ipr->p = new_p;
			}
			break;
		}
		else if (iprh->start == iprh_tmp->start)
		{
			/* received the same datagram twice: no need to keep the datagram */
			goto freepbuf;
		}
		else if (iprh->start < iprh_tmp->end)
		{
			/* overlap: no need to keep the new datagram */
			goto freepbuf;
		}
		else
		{
			/* Check if the fragments received so far have no wholes. */
			if (iprh_prev != NULL)
			{
				if (iprh_prev->end != iprh_tmp->start)
				{
					/* There is a fragment missing between the current
					 * and the previous fragment */
					valid = 0;
				}
			}
		}
		q = iprh_tmp->next_pbuf;
		iprh_prev = iprh_tmp;
	}

	/* If q is NULL, then we made it to the end of the list. Determine what to do now */
	if (q == NULL)
	{
		if (iprh_prev != NULL)
		{
			/* this is (for now), the fragment with the highest offset:
			 * chain it to the last fragment */
			ASSERT(iprh_prev->end <= iprh->start);
			iprh_prev->next_pbuf = new_p;
			if (iprh_prev->end != iprh->start)
			{
				valid = 0;
			}
		}
		else
		{
			ASSERT(ipr->p == NULL);
			/* this is the first fragment we ever received for this ip datagram */
			ipr->p = new_p;
		}
	}

	/* At this point, the validation part begins: */
	/* If we already received the last fragment */
	if ((ipr->flags & IP_REASS_FLAG_LASTFRAG) != 0)
	{
		/* and had no wholes so far */
		if (valid)
		{
			/* then check if the rest of the fragments is here */
			/* Check if the queue starts with the first datagram */
			if (((ip_reass_helper*) ipr->p->payload)->start != 0)
			{
				valid = 0;
			}
			else
			{
				/* and check that there are no wholes after this datagram */
				iprh_prev = iprh;
				q = iprh->next_pbuf;
				while (q != NULL)
				{
					iprh = (ip_reass_helper*) q->payload;
					if (iprh_prev->end != iprh->start)
					{
						valid = 0;
						break;
					}
					iprh_prev = iprh;
					q = iprh->next_pbuf;
				}
				/* if still valid, all fragments are received
				 * (because to the MF==0 already arrived */
				if (valid)
				{
					ASSERT(ipr->p != NULL);
					ASSERT(((ip_reass_helper*) ipr->p->payload) != iprh);
					ASSERT(iprh->next_pbuf == NULL);
					ASSERT(iprh->end == ipr->datagram_len);
				}
			}
		}
		/* If valid is 0 here, there are some fragments missing in the middle
		 * (since MF == 0 has already arrived). Such datagrams simply time out if
		 * no more fragments are received... */
		return valid;
	}
	/* If we come here, not all fragments were received, yet! */
	return 0; /* not yet valid! */
	freepbuf: ip_reass_pbufcount -= pbuf_clen(new_p);
	pbuf_free(new_p);
	return 0;
}

/**
 * Reassembles incoming IP fragments into an IP datagram.
 *
 * @param p points to a pbuf chain of the fragment
 * @return NULL if reassembly is incomplete, ? otherwise
 */
PktBuf * ip_reass(PktBuf *p)
{
	PktBuf *r;
	IPv4Header *fraghdr;
	IpReassembly *ipr;
	ip_reass_helper *iprh;
	uint16 offset, len;
	uint8 clen;
	IpReassembly *ipr_prev = NULL;

	fraghdr = (IPv4Header*) p->payload;

	if ((fraghdr->HL() * 4) != sizeof(IPv4Header))
	{
//		printf("ip_reass: IP options currently not supported!\n");
		goto nullreturn;
	}

	offset = (ntohs(fraghdr->OFFSET()) & IP_OFFMASK) * 8;
	len = ntohs(fraghdr->LEN()) - fraghdr->HL() * 4;

	/* Check if we are allowed to enqueue more datagrams. */
	clen = pbuf_clen(p);
	if ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS)
	{
		if (!ip_reass_remove_oldest_datagram(fraghdr, clen)
				|| ((ip_reass_pbufcount + clen) > IP_REASS_MAX_PBUFS))
		{
			/* No datagram could be freed and still too many pbufs enqueued */
//			printf("ip_reass: Overflow: pbufct=%d, clen=%d, MAX=%d\n",
//					ip_reass_pbufcount, clen, IP_REASS_MAX_PBUFS);
			/* @todo: send ICMP time exceeded here? */
			/* drop this pbuf */
			goto nullreturn;
		}
	}

	/* Look for the datagram the fragment belongs to in the current datagram queue,
	 * remembering the previous in the queue for later dequeueing. */
	for (ipr = reassdatagrams; ipr != NULL; ipr = ipr->next)
	{
		/* Check if the incoming fragment matches the one currently present
		 in the reassembly buffer. If so, we proceed with copying the
		 fragment into the buffer. */
		if (IP_ADDRESSES_AND_ID_MATCH(&ipr->iphdr, fraghdr))
			break;
		ipr_prev = ipr;
	}

	if (!ipr)
	{
		/* Enqueue a new datagram into the datagram queue */
		ipr = ip_reass_enqueue_new_datagram(fraghdr, clen);
		/* Bail if unable to enqueue */
		if (!ipr)
			goto nullreturn;
	}
	else
	{
		if (((ntohs(fraghdr->OFFSET()) & IP_OFFMASK) == 0) && ((ntohs(
				ipr->iphdr.OFFSET()) & IP_OFFMASK) != 0))
		{
			/* ipr->iphdr is not the header from the first fragment, but fraghdr is
			 * -> copy fraghdr into ipr->iphdr since we want to have the header
			 * of the first fragment (for ICMP time exceeded and later, for copying
			 * all options, if supported)*/
			memcpy(&ipr->iphdr, fraghdr, sizeof(IPv4Header));
		}
	}
	/* Track the current number of pbufs current 'in-flight', in order to limit
	 the number of fragments that may be enqueued at any one time */
	ip_reass_pbufcount += clen;

	/* At this point, we have either created a new entry or pointing
	 * to an existing one */

	/* check for 'no more fragments', and update queue entry*/
	if ((ntohs(fraghdr->OFFSET()) & IP_MF) == 0)
	{
		ipr->flags |= IP_REASS_FLAG_LASTFRAG;
		ipr->datagram_len = offset + len;
	}
	/* find the right place to insert this pbuf */
	/* @todo: trim pbufs if fragments are overlapping */
	if (ip_reass_chain_frag_into_datagram_and_validate(ipr, p))
	{
		/* the totally last fragment (flag more fragments = 0) was received at least
		 * once AND all fragments are received */
		ipr->datagram_len += sizeof(IPv4Header);

		/* save the second pbuf before copying the header over the pointer */
		r = ((ip_reass_helper*) ipr->p->payload)->next_pbuf;

		/* copy the original ip header back to the first pbuf */
		fraghdr = (IPv4Header*) (ipr->p->payload);
		memcpy(fraghdr, &ipr->iphdr, sizeof(IPv4Header));
		fraghdr->LEN_SET(htons(ipr->datagram_len));
		fraghdr->OFFSET_SET(0);
		fraghdr->CHKSUM_SET(0);
		/* @todo: do we need to set calculate the correct checksum? */
		fraghdr->CHKSUM_SET(inet_chksum(fraghdr, sizeof(IPv4Header)));

		p = ipr->p;

		/* chain together the pbufs contained within the reass_data list. */
		while (r != NULL)
		{
			iprh = (ip_reass_helper*) r->payload;

			/* hide the ip header for every succeding fragment */
			pbuf_header(r, -((int) sizeof(IPv4Header)));
			pbuf_cat(p, r);
			r = iprh->next_pbuf;
		}
		/* release the sources allocate for the fragment queue entry */
		if(ipr_prev)//TODO:Test It
			ip_reass_dequeue_datagram(ipr, ipr_prev);

		/* and adjust the number of pbufs currently queued for reassembly. */
		ip_reass_pbufcount -= pbuf_clen(p);

		/* Return the pbuf chain */
		return p;
	}
	/* the datagram is not (yet?) reassembled completely */
	return NULL;

	nullreturn: pbuf_free(p);
	return NULL;
}

/**
 * Simple interface to ip_output_if. It finds the outgoing network
 * interface and calls upon ip_output_if to do the actual work.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
 protocol header; if dest == IP_HDRINCL, p already includes an IP
 header and p->payload points to that IP header)
 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
 *         IP  address of the netif used to send is used as source address)
 * @param dest the destination IP address to send the packet to
 * @param ttl the TTL value to be set in the IP header
 * @param tos the TOS value to be set in the IP header
 * @param proto the PROTOCOL to be set in the IP header
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
int ip_output(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto)
{
	Ethernet *netif;

	if ((netif = ip_route(dest, src)) == NULL)
	{
//		printf("ip_output: No route to 0x%08X\n", (uint32) dest->addr);
		//		IP_STATS_INC(ip.rterr);
		return ERR_RTE;
	}
	return ip_output_if(p, src, dest, ttl, tos, proto, netif);
}

/**
 * Finds the appropriate network interface for a given IP address. It
 * searches the list of network interfaces linearly. A match is found
 * if the masked IP address of the network interface equals the masked
 * IP address given to the function.
 *
 * @param dest the destination IP address for which to find the route
 * @return the netif on which to send to reach dest
 */
Ethernet * ip_route(ip_addr *dest, ip_addr * src)
{
	//	printf("ip_route: dst ");
	//	PrintAddress(dest);
	//	printf(" src ");
	//	PrintAddress(src);
	//	printf("\n");
	POSITION pos = netif_list.GetHeadPosition();
	Ethernet *netif = 0;
	while (pos)
	{
		netif = netif_list.GetNext(pos);
		//		printf("NetIf: IP ");
		//		PrintAddress(&netif->ipaddr);
		//		printf("... ");
		// If source is not specified, just check the destination to be in our
		// netmask. This is a special case to determine MSS for the interface.
		if ((!src) && (ip_addr_netcmp(dest, &netif->ipaddr, &netif->netmask)))
		{
			//			printf("OK (no src)\n");
			break;
		}
		// Any other call to this function specifies the source address for
		// the interface. So, we only need to check that.
		if (src)
		{
			if (ip_addr_netcmp(src, &netif->ipaddr, &netif->netmask))
				break;
		}

		netif = 0;
		//		printf("Nope\n");
	}
	if (!netif)
	{
		//printf("No route to ");
		//PrintAddress(dest);
		//printf(", using first interface...\n");
		return netif_list.GetHead();
	}
	// TODO default is the same as the only one
	/* iterate through netifs
	 for (netif = netif_list; netif != NULL; netif = netif->next)
	 {
	 // network mask matches?
	 if (netif_is_up(netif))
	 {
	 if (ip_addr_netcmp(dest, &(netif->ip_addr), &(netif->netmask)))
	 {
	 // return netif on which to forward IP packet
	 return netif;
	 }
	 }
	 }
	 if ((netif_default == NULL) || (!netif_is_up(netif_default)))
	 {
	 LWIP_DEBUGF(IP_DEBUG | 2, ("ip_route: No route to 0x%"X32_F"\n", dest->addr));
	 IP_STATS_INC(ip.rterr);
	 snmp_inc_ipoutnoroutes();
	 return NULL;
	 }
	 // no matching netif found, use default netif
	 return netif_default;
	 */
	return netif;
}

/**
 * Same as ip_output_if() but with the possibility to include IP options:
 *
 * @ param ip_options pointer to the IP options, copied into the IP header
 * @ param optlen length of ip_options
 */
int ip_output_if_opt(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto, Ethernet *netif, void *ip_options, int optlen)
{
	IPv4Header *iphdr;
	static uint16 ip_id = 0;

	//	snmp_inc_ipoutrequests();

	/* Should the IP header be generated or is it already included in p? */
	if (dest)
	{
		uint16 ip_hlen = sizeof(IPv4Header);
		uint16 optlen_aligned = 0;
		if (optlen != 0)
		{
			/* round up to a multiple of 4 */
			optlen_aligned = ((optlen + 3) & ~3);
			ip_hlen += optlen_aligned;
			/* First write in the IP options */
			if (pbuf_header(p, optlen_aligned))
			{
//				printf("not enough room for IP options in pbuf\n");
				return ERR_BUF;
			}
			memcpy(p->payload, ip_options, optlen);
			if (optlen < optlen_aligned)
			{
				/* zero the remaining bytes */
				memset(((char*) p->payload) + optlen, 0, optlen_aligned
						- optlen);
			}
		}
		/* generate IP header */
		if (pbuf_header(p, sizeof(IPv4Header)))
		{
//			printf("ip_output: not enough room for IP header in pbuf\n");
			return ERR_BUF;
		}

		iphdr = (IPv4Header *) p->payload;
		ASSERT(p->len >= sizeof(IPv4Header));

		iphdr->TTL_SET(ttl);
		iphdr->PROTO_SET(proto);

		ip_addr_set(&(iphdr->dest), dest);

		iphdr->VHLTOS_SET(4, ip_hlen / 4, tos);
		iphdr->LEN_SET(htons(p->tot_len));
		iphdr->OFFSET_SET(0);
		iphdr->ID_SET(htons(ip_id));
		++ip_id;

		if (ip_addr_isany(src))
		{
			ip_addr_set(&(iphdr->src), &(netif->ipaddr));
		}
		else
		{
			ip_addr_set(&(iphdr->src), src);
		}
		iphdr->CHKSUM_SET(0);
		iphdr->CHKSUM_SET(inet_chksum(iphdr, ip_hlen));
	}
	else
	{
		/* IP header already included in p */
		iphdr = (IPv4Header *) p->payload;
		dest = &(iphdr->dest);
	}

	/* don't fragment if interface has mtu set to 0 [loopif] */
	if (netif->mtu && (p->tot_len > netif->mtu))
		return ip_frag(p, netif, dest);
	if (ip_addr_cmp(dest, &netif->ipaddr))
	{
		/* Packet to self, enqueue it for loopback */
//		printf("netif_loop_output()");
		return netif_loop_output(netif, p, dest);
	}
	else
	{
		ip_addr * gateway = ResolveGateway(&iphdr->dest, &iphdr->src, netif);
		if (gateway)
		{
			//pprintf("\nGWF");
			//pprintf("\n gateway for srcIP %08X dstIP %08X is %08X", (int)iphdr->src.addr, (int)iphdr->dest.addr, (int)gateway->addr);
			return etharp_output(netif, p, gateway);
		}
		else
		{
			//pprintf("\nGWNF");
//			printf(" gateway not found for IP %08X", (uint32) iphdr->dest.addr);
		}
	}
	return ERR_BUF;
}

/**
 * Sends an IP packet on a network interface. This function constructs
 * the IP header and calculates the IP header checksum. If the source
 * IP address is NULL, the IP address of the outgoing network
 * interface is filled in as source address.
 * If the destination IP address is IP_HDRINCL, p is assumed to already
 * include an IP header and p->payload points to it instead of the data.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
 protocol header; if dest == IP_HDRINCL, p already includes an IP
 header and p->payload points to that IP header)
 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
 *         IP  address of the netif used to send is used as source address)
 * @param dest the destination IP address to send the packet to
 * @param ttl the TTL value to be set in the IP header
 * @param tos the TOS value to be set in the IP header
 * @param proto the PROTOCOL to be set in the IP header
 * @param netif the netif on which to send this packet
 * @return ERR_OK if the packet was sent OK
 *         ERR_BUF if p doesn't have enough space for IP/LINK headers
 *         returns errors returned by netif->output
 *
 * @note ip_id: RFC791 "some host may be able to simply use
 *  unique identifiers independent of destination"
 */
int ip_output_if(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl, int tos,
		int proto, Ethernet *netif)
{
	return ip_output_if_opt(p, src, dest, ttl, tos, proto, netif, NULL, 0);
}

/**
 * Fragment an IP datagram if too large for the netif.
 *
 * Chop the datagram in MTU sized chunks and send them in order
 * by using a fixed size static memory buffer (PBUF_REF) or
 * point PBUF_REFs into p (depending on IP_FRAG_USES_STATIC_BUF).
 *
 * @param p ip packet to send
 * @param netif the netif on which to send
 * @param dest destination ip address to which to send
 *
 * @return ERR_OK if sent successfully, int otherwise
 */
int ip_frag(PktBuf *p, Ethernet *netif, ip_addr *dest)
{
	PktBuf *rambuf;
	PktBuf *newpbuf;
	IPv4Header *original_iphdr;
	IPv4Header *iphdr;
	uint16 nfb;
	uint16 left, cop;
	uint16 mtu = netif->mtu;
	uint16 ofo, omf;
	uint16 last;
	uint16 poff = sizeof(IPv4Header);
	uint16 tmp;
	uint16 newpbuflen = 0;
	uint16 left_to_copy;

	/* Get a RAM based MTU sized pbuf */
	original_iphdr = (IPv4Header *) p->payload;
	iphdr = original_iphdr;

	/* Save original offset */
	tmp = ntohs(iphdr->OFFSET());
	ofo = tmp & IP_OFFMASK;
	omf = tmp & IP_MF;

	left = p->tot_len - sizeof(IPv4Header);

	nfb = (mtu - sizeof(IPv4Header)) / 8;

	while (left)
	{
		last = (left <= mtu - sizeof(IPv4Header));

		/* Set new offset and MF flag */
		tmp = omf | (IP_OFFMASK & (ofo));
		if (!last)
			tmp = tmp | IP_MF;

		/* Fill this fragment */
		cop = last ? left : nfb * 8;

		/* When not using a static buffer, create a chain of pbufs.
		 * The first will be a PBUF_RAM holding the link and IP header.
		 * The rest will be PBUF_REFs mirroring the pbuf chain to be fragged,
		 * but limited to the size of an mtu.
		 */
		rambuf = pbuf_alloc(PBUF_LINK, sizeof(IPv4Header), PBUF_RAM);
		if (rambuf == NULL)
		{
			return ERR_MEM;
		}
		ASSERT(p->len >= sizeof(IPv4Header));
		memcpy(rambuf->payload, original_iphdr, sizeof(IPv4Header));
		iphdr = (IPv4Header *) rambuf->payload;

		/* Can just adjust p directly for needed offset. */
		p->payload = (uint8 *) p->payload + poff;
		p->len -= poff;

		left_to_copy = cop;
		while (left_to_copy)
		{
			newpbuflen = (left_to_copy < p->len) ? left_to_copy : p->len;
			/* Is this pbuf already empty? */
			if (!newpbuflen)
			{
				p = p->next;
				continue;
			}
			newpbuf = pbuf_alloc(PBUF_RAW, 0, PBUF_REF);
			if (newpbuf == NULL)
			{
				pbuf_free(rambuf);
				return ERR_MEM;
			}
			/* Mirror this pbuf, although we might not need all of it. */
			newpbuf->payload = p->payload;
			newpbuf->len = newpbuf->tot_len = newpbuflen;
			/* Add it to end of rambuf's chain, but using pbuf_cat, not pbuf_chain
			 * so that it is removed when pbuf_dechain is later called on rambuf.
			 */
			pbuf_cat(rambuf, newpbuf);
			left_to_copy -= newpbuflen;
			if (left_to_copy)
				p = p->next;
		}
		poff = newpbuflen;

		/* Correct header */
		iphdr->OFFSET_SET(htons(tmp));
		iphdr->LEN_SET(htons(cop + sizeof(IPv4Header)));
		iphdr->CHKSUM_SET(0);
		iphdr->CHKSUM_SET(inet_chksum(iphdr, sizeof(IPv4Header)));

		/* No need for separate header pbuf - we allowed room for it in rambuf
		 * when allocated.
		 */
		ip_addr * gateway = ResolveGateway(&iphdr->dest, &iphdr->src, netif);
		if (gateway)
		{
			//pprintf("\nIPGWF");
			etharp_output(netif, rambuf, gateway);
		}
		else
		{
			//pprintf("\nIPNGWF");
//			printf(" gateway not found for IP %08X", (uint32) iphdr->dest.addr);
		}

		/* Unfortunately we can't reuse rambuf - the hardware may still be
		 * using the buffer. Instead we free it (and the ensuing chain) and
		 * recreate it next time round the loop. If we're lucky the hardware
		 * will have already sent the packet, the free will really free, and
		 * there will be zero memory penalty.
		 */

		pbuf_free(rambuf);
		left -= cop;
		ofo += nfb;
	}
	//	snmp_inc_ipfragoks();
	return ERR_OK;
	ASSERT(dest);
}

/**
 * Calculate a checksum over a chain of pbufs (without pseudo-header, much like
 * inet_chksum only pbufs are used).
 *
 * @param p pbuf chain over that the checksum should be calculated
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
int inet_chksum_pbuf(PktBuf *p)
{
	bool swapped = false;

	uint32 acc = 0;
	for (PktBuf * q = p; q; q = q->next)
	{
		acc += lwip_standard_chksum(q->payload, q->len);
		acc = FOLD_U32T(acc);
		if (q->len % 2 != 0)
		{
			swapped = !swapped;
			acc = SWAP_BYTES_IN_WORD(acc);
		}
	}

	if (swapped)
		acc = SWAP_BYTES_IN_WORD(acc);
	return (uint16) ~(acc & 0xffffUL);
}

/* inet_chksum_pseudo:
 *
 * Calculates the pseudo Internet checksum used by TCP and UDP for a pbuf chain.
 * IP addresses are expected to be in network byte order.
 *
 * @param p chain of pbufs over that a checksum should be calculated (ip data part)
 * @param src source ip address (used for checksum of pseudo header)
 * @param dst destination ip address (used for checksum of pseudo header)
 * @param proto ip protocol (used for checksum of pseudo header)
 * @param proto_len length of the ip data part (used for checksum of pseudo header)
 * @return checksum (as u16_t) to be saved directly in the protocol header
 */
int inet_chksum_pseudo(PktBuf *p, ip_addr *src, ip_addr *dest, int proto,
		int proto_len)
{
	uint32 acc;
	PktBuf *q;
	uint8 swapped;

	acc = 0;
	swapped = 0;
	/* iterate through all pbuf in chain */
	for (q = p; q != NULL; q = q->next)
	{
		if (((int) q->payload) == (int)0xCCCCCCCC)
		{
//			printf("CCCC\n");
		}
		acc += lwip_standard_chksum(q->payload, q->len);
		/*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): unwrapped lwip_chksum()=%"X32_F" \n", acc));*/
		/* just executing this next line is probably faster that the if statement needed
		 to check whether we really need to execute it, and does no harm */
		acc = FOLD_U32T(acc);
		if (q->len % 2 != 0)
		{
			swapped = 1 - swapped;
			acc = SWAP_BYTES_IN_WORD(acc);
		}
		/*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): wrapped lwip_chksum()=%"X32_F" \n", acc));*/
	}

	if (swapped)
	{
		acc = SWAP_BYTES_IN_WORD(acc);
	}
	acc += (src->addr & 0xffffUL);
	acc += ((src->addr >> 16) & 0xffffUL);
	acc += (dest->addr & 0xffffUL);
	acc += ((dest->addr >> 16) & 0xffffUL);
	acc += (uint32) htons((uint16) proto);
	acc += (uint32) htons(proto_len);

	/* Fold 32-bit sum to 16 bits
	 calling this twice is propably faster than if statements... */
	acc = FOLD_U32T(acc);
	acc = FOLD_U32T(acc);
	return (uint16) ~(acc & 0xffffUL);
}

/**
 * Reassembly timer base function
 *
 * Should be called every 1000 msec (defined by IP_TMR_INTERVAL).
 */
void ip_reass_tmr()
{
	IpReassembly *r, *prev = NULL;

	r = reassdatagrams;
	while (r != NULL)
	{
		/* Decrement the timer. Once it reaches 0,
		 * clean up the incomplete fragment assembly */
		if (r->timer > 0)
		{
			r->timer--;
//			printf("ip_reass_tmr: timer dec %d\n", r->timer);
			prev = r;
			r = r->next;
		}
		else
		{
			/* reassembly timed out */
			IpReassembly *tmp;
//			printf("ip_reass_tmr: timer timed out\n");
			tmp = r;
			/* get the next pointer before freeing */
			r = r->next;
			/* free the helper struct and all enqueued pbufs */
			ip_reass_free_complete_datagram(tmp, prev);
		}
	}
}

/**
 * Forwards an IP packet. It finds an appropriate route for the
 * packet, decrements the TTL value of the packet, adjusts the
 * checksum and outputs the packet on the appropriate interface.
 *
 * @param p the packet to forward (p->payload points to IP header)
 * @param iphdr the IP header of the input packet
 * @param inp the netif on which this packet was received
 * @return the netif on which the packet was sent (NULL if it wasn't sent)
 */
Ethernet * ip_forward(PktBuf *p, IPv4Header *iphdr, Ethernet *inp)
{
	Ethernet *netif;

	/* Find network interface where to forward this IP packet to. */
	netif = ip_route(&iphdr->dest, &iphdr->src);
	if (!netif)
	{
//		printf("ip_forward: no forwarding route for 0x%08X found\n",
//				(uint32) iphdr->dest.addr);
		//		snmp_inc_ipoutnoroutes();
		return 0;
	}
	/* Do not forward packets onto the same network interface on which
	 * they arrived. */
	if (netif == inp)
	{
		// TODO Fix this, we want to bounce back...
		//printf("ip_forward: not bouncing packets back on incoming interface.\n");
		//		snmp_inc_ipoutnoroutes();
		//printf(" forward on same interface");
		//return 0;
	}
	else
	{
//		printf(" forward on different interface");
	}

	/* decrement TTL */
	iphdr->TTL_SET(iphdr->TTL() - 1);
	/* send ICMP if TTL == 0 */
	if (iphdr->TTL() == 0)
	{
		//		snmp_inc_ipinhdrerrors();
		/* Don't send ICMP messages in response to ICMP messages */
		if (iphdr->PROTO() != IP_PROTO_ICMP)
		{
			icmp_time_exceeded(p, ICMP_TE_TTL);
		}
		return 0;
	}

	/* Incrementally update the IP checksum. */
	if (iphdr->CHKSUM() >= htons(0xffff - 0x100))
	{
		iphdr->CHKSUM_SET(iphdr->CHKSUM() + htons(0x100) + 1);
	}
	else
	{
		iphdr->CHKSUM_SET(iphdr->CHKSUM() + htons(0x100));
	}

	/* transmit pbuf on chosen interface */
	ip_addr * gateway = ResolveGateway(&iphdr->dest, &iphdr->src, netif);
	if (gateway)
	{
		//printf("ip_forward: forwarding packet to 0x%08X\n",
		//(uint32) gateway->addr);
		etharp_output(netif, p, gateway);
	}
	else
	{
//		printf(" gateway not found for IP-%08X", (uint32) iphdr->dest.addr);
	}
	return netif;
}

//All IP parameters should be network byte ordered.
//We ourselves don't use htonl call on parameters for ordering the bytes.
//Because we can't determine that the whether we have received the network
//byte ordered parameters or not.Let's make it the caller's responsibility.
//Right now set_route gets called from arts or main()
//arts receive everything in network byte order.From main()
//MakeIP() call automatically order the bytes.
void set_route(uint32 destination, uint32 source, uint32 dstnetMask,
		uint32 srcnetMask, uint32 gateway)
{
	//	ASSERT(srcnetMask == 0xFFFFFFFF);
	//	ASSERT(dstnetMask == 0xFFFFFFFF);//Even windows doesn't allow mask other than all F's
	// (dest & gateway) == dest is must.
	POSITION pos = routingTable.GetHeadPosition();
	while (pos)
	{
		RouteTable * entry = routingTable.GetNext(pos);
		if ((entry->destinationIP.addr == destination) && (entry->sourceIP.addr
				== source) && (entry->DstnetMask.addr == dstnetMask)
				&& (entry->SrcnetMask.addr == srcnetMask))
		{ // entry for set of dst and src  already exist, simply update gateway and exit.
			entry->gatewayIP.addr = gateway;
			return;
		}
	}
//	printf("New route\n");
	RouteTable * entry = new RouteTable();
	entry->destinationIP.addr = destination;
	entry->sourceIP.addr = source;
	entry->DstnetMask.addr = dstnetMask;
	entry->SrcnetMask.addr = srcnetMask;
	entry->gatewayIP.addr = gateway;
	routingTable.AddTail(entry);
}

ip_addr * ResolveGateway(ip_addr * destination, ip_addr * source,
		Ethernet * netif)
{
	// First, the routing table. Packet destination anded with netmask must be
	// same as route destination anded with netmask, use gateway to route.
	POSITION pos = routingTable.GetHeadPosition();
	// Now for different sources of packet(with same destination) the gateways may be
	// different. There should be unique entry for a set of destination and source.
	while (pos)
	{
		RouteTable * entry = routingTable.GetNext(pos);
		if (((entry->destinationIP.addr & entry->DstnetMask.addr)
				== (destination->addr & entry->DstnetMask.addr))
				&& ((entry->sourceIP.addr & entry->SrcnetMask.addr)
						== (source->addr & entry->SrcnetMask.addr)))
		{
			return &entry->gatewayIP;
		}
	}
	// Next, check if the target address matches up our IP with the netmask,
	// in this case, route directly.
	if (((netif->ipaddr.addr & netif->netmask.addr) == (destination->addr
			& netif->netmask.addr)) && ((netif->ipaddr.addr
			& netif->netmask.addr) == (source->addr & netif->netmask.addr)))
	{
		return destination;
	}
	if((unsigned int)destination->addr == 0xFFFFFFFF)
		return destination;
	if((unsigned int)destination->addr == 0x00)
			return 0;

	return &netif->gw;
}

void PrintRoutingTable()
{
//	printf("\n\n    Routing table\n");
	POSITION pos = routingTable.GetHeadPosition();
	while (pos)
	{
		RouteTable * entry = routingTable.GetNext(pos);
		PrintAddress(&entry->destinationIP);
//		printf("  ");
		PrintAddress(&entry->sourceIP);
//		printf("  ");
		PrintAddress(&entry->DstnetMask);
//		printf("  ");
		PrintAddress(&entry->SrcnetMask);
//		printf("  ");
		PrintAddress(&entry->gatewayIP);
	}
//	printf("\n\n");
}
