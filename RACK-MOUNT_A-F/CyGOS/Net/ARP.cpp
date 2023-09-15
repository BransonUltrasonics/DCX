// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/ARP.cpp_v   1.1   27 Jul 2015 08:49:28   sesharma  $
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

// FIXME ARP requests are sent on first Ethernet even if the second one needed to send ARP.
// This will be a problem when each interface is connected to separate networks.

#include "ARP.h"
#include "Net.h"
#include "Ethernet.h"
#include "PktBuf.h"
#include "IPv4.h"
#include "assert.h"
#include "DebugNew.h"
#include "AutoIP.h"
#include "DebugNew.h"
#define ETHARP_TRUST_IP_MAC 0

struct ethip_hdr
{
	EthHeader eth;
	IPv4Header ip;
};

enum etharp_state
{
	ETHARP_STATE_EMPTY = 0, ETHARP_STATE_PENDING, ETHARP_STATE_STABLE
};

struct ArpQueueEntry
{
	ArpQueueEntry *next;
	PktBuf *p;
};

struct etharp_entry
{
	/**
	 * Pointer to queue of pending outgoing packets on this ARP entry.
	 */
	ArpQueueEntry *q;
	ip_addr ipaddr;
	EthAddr ethaddr;
	etharp_state state;
	uint8 ctime;
	Ethernet *netif;
};

#define ARP_TABLE_SIZE          100

const EthAddr ethbroadcast =
{
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
const EthAddr ethzero =
{
{ 0, 0, 0, 0, 0, 0 } };
static etharp_entry arp_table[ARP_TABLE_SIZE];
static uint8 etharp_cached_entry;

/**
 * Try hard to create a new entry - we want the IP address to appear in
 * the cache (even if this means removing an active entry or so). */
#define ETHARP_TRY_HARD 1
#define ETHARP_FIND_ONLY  2

#define HWTYPE_ETHERNET 1

/** the time an ARP entry stays pending after first request,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (2 * 5) seconds = 10 seconds.
 *
 *  @internal Keep this number at least 2, otherwise it might
 *  run out instantly if the timeout occurs directly after a request.
 */
#define ARP_MAXPENDING 4

/** the time an ARP entry stays valid after its last update,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (24 * 5) seconds = 2 minutes.
 */
#define ARP_MAXAGE 24

void arp_init()
{
}

/**
 * Free a complete queue of etharp entries
 *
 * @param q a qeueue of etharp_q_entry's to free
 */
void free_etharp_q(ArpQueueEntry *q)
{
	ArpQueueEntry *r;
	ASSERT(q != NULL);
	ASSERT(q->p != NULL);
	while (q)
	{
		r = q;
		q = q->next;
		ASSERT(r->p != NULL);
		pbuf_free(r->p);
		delete r;
	}
}

/**
 * Search the ARP table for a matching or new entry.
 *
 * If an IP address is given, return a pending or stable ARP entry that matches
 * the address. If no match is found, create a new entry with this address set,
 * but in state ETHARP_EMPTY. The caller must check and possibly change the
 * state of the returned entry.
 *
 * If ipaddr is NULL, return a initialized new entry in state ETHARP_EMPTY.
 *
 * In all cases, attempt to create new entries from an empty entry. If no
 * empty entries are available and ETHARP_TRY_HARD flag is set, recycle
 * old entries. Heuristic choose the least important entry for recycling.
 *
 * @param ipaddr IP address to find in ARP cache, or to add if not found.
 * @param flags
 * - ETHARP_TRY_HARD: Try hard to create a entry by allowing recycling of
 * active (stable or pending) entries.
 *
 * @return The ARP entry index that matched or is created, ERR_MEM if no
 * entry is found or could be recycled.
 */
int8 find_entry(ip_addr *ipaddr, uint8 flags)
{
	int8 old_pending = ARP_TABLE_SIZE, old_stable = ARP_TABLE_SIZE;
	int8 empty = ARP_TABLE_SIZE;
	uint8 i = 0, age_pending = 0, age_stable = 0;
	/* oldest entry with packets on queue */
	int8 old_queue = ARP_TABLE_SIZE;
	/* its age */
	uint8 age_queue = 0;

	/* First, test if the last call to this function asked for the
	 * same address. If so, we're really fast! */
	if (ipaddr)
	{
		/* ipaddr to search for was given */
		if (arp_table[etharp_cached_entry].state == ETHARP_STATE_STABLE)
		{
			/* the cached entry is stable */
			if (ip_addr_cmp(ipaddr, &arp_table[etharp_cached_entry].ipaddr))
			{
				/* cached entry was the right one! */
				return etharp_cached_entry;
			}
		}
	}

	/**
	 * a) do a search through the cache, remember candidates
	 * b) select candidate entry
	 * c) create new entry
	 */

	/* a) in a single search sweep, do all of this
	 * 1) remember the first empty entry (if any)
	 * 2) remember the oldest stable entry (if any)
	 * 3) remember the oldest pending entry without queued packets (if any)
	 * 4) remember the oldest pending entry with queued packets (if any)
	 * 5) search for a matching IP entry, either pending or stable
	 *    until 5 matches, or all entries are searched for.
	 */

	for (i = 0; i < ARP_TABLE_SIZE; ++i)
	{
		/* no empty entry found yet and now we do find one? */
		if ((empty == ARP_TABLE_SIZE) && (arp_table[i].state
				== ETHARP_STATE_EMPTY))
		{
			//			LWIP_DEBUGF(ETHARP_DEBUG, ("find_entry: found empty entry %"U16_F"\n", (uint16)i));
			/* remember first empty entry */
			empty = i;
		}
		/* pending entry? */
		else if (arp_table[i].state == ETHARP_STATE_PENDING)
		{
			/* if given, does IP address match IP address in ARP entry? */
			if (ipaddr && ip_addr_cmp(ipaddr, &arp_table[i].ipaddr))
			{
				//				LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: found matching pending entry %"U16_F"\n", (uint16)i));
				/* found exact IP address match, simply bail out */
				etharp_cached_entry = i;
				return i;
				/* pending with queued packets? */
			}
			else if (arp_table[i].q != NULL)
			{
				if (arp_table[i].ctime >= age_queue)
				{
					old_queue = i;
					age_queue = arp_table[i].ctime;
				}
				/* pending without queued packets? */
			}
			else
			{
				if (arp_table[i].ctime >= age_pending)
				{
					old_pending = i;
					age_pending = arp_table[i].ctime;
				}
			}
		}
		/* stable entry? */
		else if (arp_table[i].state == ETHARP_STATE_STABLE)
		{
			/* if given, does IP address match IP address in ARP entry? */
			if (ipaddr && ip_addr_cmp(ipaddr, &arp_table[i].ipaddr))
			{
				//				LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: found matching stable entry %"U16_F"\n", (uint16)i));
				/* found exact IP address match, simply bail out */
				etharp_cached_entry = i;
				return i;
				/* remember entry with oldest stable entry in oldest, its age in maxtime */
			}
			else if (arp_table[i].ctime >= age_stable)
			{
				old_stable = i;
				age_stable = arp_table[i].ctime;
			}
		}
	}
	/* { we have no match } => try to create a new entry */

	/* no empty entry found and not allowed to recycle? */
	if (((empty == ARP_TABLE_SIZE) && ((flags & ETHARP_TRY_HARD) == 0))
	/* or don't create new entry, only search? */
	|| ((flags & ETHARP_FIND_ONLY) != 0))
	{
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: no empty entry found and not allowed to recycle\n"));
		return (int8) ERR_MEM;
	}

	/* b) choose the least destructive entry to recycle:
	 * 1) empty entry
	 * 2) oldest stable entry
	 * 3) oldest pending entry without queued packets
	 * 4) oldest pending entry with queued packets
	 *
	 * { ETHARP_TRY_HARD is set at this point }
	 */

	/* 1) empty entry available? */
	if (empty < ARP_TABLE_SIZE)
	{
		i = empty;
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: selecting empty entry %"U16_F"\n", (uint16)i));
	}
	/* 2) found recyclable stable entry? */
	else if (old_stable < ARP_TABLE_SIZE)
	{
		/* recycle oldest stable*/
		i = old_stable;
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: selecting oldest stable entry %"U16_F"\n", (uint16)i));
		/* no queued packets should exist on stable entries */
		ASSERT(arp_table[i].q == NULL);
		/* 3) found recyclable pending entry without queued packets? */
	}
	else if (old_pending < ARP_TABLE_SIZE)
	{
		/* recycle oldest pending */
		i = old_pending;
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: selecting oldest pending entry %"U16_F" (without queue)\n", (uint16)i));
		/* 4) found recyclable pending entry with queued packets? */
	}
	else if (old_queue < ARP_TABLE_SIZE)
	{
		/* recycle oldest pending */
		i = old_queue;
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("find_entry: selecting oldest pending entry %"U16_F", freeing packet queue %p\n", (uint16)i, (void *)(arp_table[i].q)));
		free_etharp_q(arp_table[i].q);
		arp_table[i].q = NULL;
		/* no empty or recyclable entries found */
	}
	else
	{
		return (int8) ERR_MEM;
	}

	/* { empty or recyclable entry found } */
	ASSERT(i < ARP_TABLE_SIZE);

	if (arp_table[i].state != ETHARP_STATE_EMPTY)
	{
		//SNMP		snmp_delete_arpidx_tree(arp_table[i].Ethernet, &arp_table[i].ipaddr);
	}
	/* recycle entry (no-op for an already empty entry) */
	arp_table[i].state = ETHARP_STATE_EMPTY;

	/* IP address given? */
	if (ipaddr)
	{
		ip_addr_set(&arp_table[i].ipaddr, ipaddr);
	}
	arp_table[i].ctime = 0;
	etharp_cached_entry = i;
	return (int) i;
}

/**
 * Update (or insert) a IP/MAC address pair in the ARP cache.
 *
 * If a pending entry is resolved, any queued packets will be sent
 * at this point.
 *
 * @param ipaddr IP address of the inserted ARP entry.
 * @param ethaddr Ethernet address of the inserted ARP entry.
 * @param flags Defines behaviour:
 * - ETHARP_TRY_HARD Allows ARP to insert this as a new item. If not specified,
 * only existing ARP entries will be updated.
 *
 * @return
 * - ERR_OK Succesfully updated ARP cache.
 * - ERR_MEM If we could not add a new ARP entry when ETHARP_TRY_HARD was set.
 * - ERR_ARG Non-unicast address given, those will not appear in ARP cache.
 *
 * @see pbuf_free()
 */
int update_arp_entry(Ethernet *netif, ip_addr *ipaddr, EthAddr *ethaddr,
		uint8 flags)
{
	int8 i;
	uint8 k;
	/* non-unicast address? */
	if (ip_addr_isany(ipaddr) || ip_addr_isbroadcast(ipaddr, netif)
			|| ip_addr_ismulticast(ipaddr))
	{
		return ERR_ARG;
	}
	/* find or create ARP entry */
	i = find_entry(ipaddr, flags);
	/* bail out if no entry could be found */
	if (i < 0)
	{
		return (int) i;
	}

	/* mark it stable */
	arp_table[i].state = ETHARP_STATE_STABLE;
	/* record network interface */
	arp_table[i].netif = netif;

	/* insert in SNMP ARP index tree */
	//SNMP	snmp_insert_arpidx_tree(Ethernet, &arp_table[i].ipaddr);

	//	LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("update_arp_entry: updating stable entry %"S16_F"\n", (int16)i));
	/* update address */
	k = EthAddrLen;
	while (k > 0)
	{
		k--;
		arp_table[i].ethaddr.addr[k] = ethaddr->addr[k];
	}
	/* reset time stamp */
	arp_table[i].ctime = 0;
	/* this is where we will send out queued packets! */
	while (arp_table[i].q != NULL)
	{
		PktBuf *p;
		/* remember remainder of queue */
		ArpQueueEntry *q = arp_table[i].q;
		/* pop first item off the queue */
		arp_table[i].q = q->next;
		/* get the packet pointer */
		p = q->p;
		/* now queue entry can be freed */
		delete q;
		/* send the queued IP packet */
		etharp_send_ip(netif, p, (EthAddr*) (netif->GetHardwareAddress()),
				ethaddr);
		/* free the queued IP packet */
		pbuf_free(p);
	}
	return ERR_OK;
}

/**
 * Send an IP packet on the network using netif->linkoutput
 * The ethernet header is filled in before sending.
 *
 * @params netif the lwIP network interface on which to send the packet
 * @params p the packet to send, p->payload pointing to the (uninitialized) ethernet header
 * @params src the source MAC address to be copied into the ethernet header
 * @params dst the destination MAC address to be copied into the ethernet header
 * @return ERR_OK if the packet was sent, any other int on failure
 */
int etharp_send_ip(Ethernet *netif, PktBuf *p, EthAddr *src, EthAddr *dst)
{
	EthHeader *ethhdr = (EthHeader *) p->payload;
	uint8 k;

	k = EthAddrLen;
	while (k > 0)
	{
		k--;
		ethhdr->dest.addr[k] = dst->addr[k];
		ethhdr->src.addr[k] = src->addr[k];
	}
	ethhdr->type = htons(ETHTYPE_IP);
	/* send the packet */
	return netif->low_level_output(p);
}

/**
 * Send an IP packet on the network using netif->linkoutput
 * The ethernet header is filled in before sending.
 *
 * @params netif the lwIP network interface on which to send the packet
 * @params p the packet to send, p->payload pointing to the (uninitialized) ethernet header
 * @params src the source MAC address to be copied into the ethernet header
 * @params dst the destination MAC address to be copied into the ethernet header
 * @return ERR_OK if the packet was sent, any other int on failure
 */
int etharp_send_mb(Ethernet *netif, PktBuf *p, EthAddr *src, EthAddr *dst)
{
	EthHeader *ethhdr = (EthHeader *) p->payload;
	uint8 k;

	k = EthAddrLen;
	while (k > 0)
	{
		k--;
		ethhdr->dest.addr[k] = dst->addr[k];
		ethhdr->src.addr[k] = src->addr[k];
	}
	ethhdr->type = htons(ETHTYPE_WCDCP);

	/* send the packet */
	return netif->low_level_output(p);
}

/**
 * Updates the ARP table using the given IP packet.
 *
 * Uses the incoming IP packet's source address to update the
 * ARP cache for the local network. The function does not alter
 * or free the packet. This function must be called before the
 * packet p is passed to the IP layer.
 *
 * @param netif The lwIP network interface on which the IP packet pbuf arrived.
 * @param p The IP packet that arrived on netif.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
void etharp_ip_input(Ethernet *netif, PktBuf *p)
{
	ethip_hdr *hdr;
	/* Only insert an entry if the source IP address of the
	 incoming IP packet comes from a host on the local network. */
	hdr = (ethip_hdr *) p->payload;
	/* source is not on the local network? */
	if (!ip_addr_netcmp(&(hdr->ip.src), &(netif->ipaddr), &(netif->netmask)))
	{
		printf("Not in the local mask: src ");
		PrintAddress(&hdr->ip.src);
		printf("our ");
		PrintAddress(&netif->ipaddr);
		printf(" mask ");
		PrintAddress(&netif->netmask);
		printf("\n");
		/* do nothing */
		return;
	}
	/* update ARP table */
	/* @todo We could use ETHARP_TRY_HARD if we think we are going to talk
	 * back soon (for example, if the destination IP address is ours. */
	update_arp_entry(netif, &(hdr->ip.src), &(hdr->eth.src), 0);
}

/**
 * Process received ethernet frames. Using this function instead of directly
 * calling ip_input and passing ARP frames through etharp in ethernetif_input,
 * the ARP cache is protected from concurrent access.
 *
 * @param p the recevied packet, p->payload pointing to the ethernet header
 * @param netif the network interface on which the packet was received
 */
int arp_input(PktBuf *p, Ethernet *netif)
{
	EthHeader* ethhdr;

	/* points to packet payload, which starts with an Ethernet header */
	ethhdr = (EthHeader *) p->payload;
	switch (htons(ethhdr->type))
	{
	/* IP packet? */
	case ETHTYPE_IP:
#if ETHARP_TRUST_IP_MAC
		/* update ARP table */
		etharp_ip_input(netif, p);
#endif /* ETHARP_TRUST_IP_MAC */
		/* skip Ethernet header */

		if (pbuf_header(p, -(int16) sizeof(EthHeader)))
		{
			//pprintf("\nDUMP");
			pbuf_free(p);
		}
		else
		{
			//pprintf("\nIPIN");
			ip_input(p, netif);
		}

		break;

	case ETHTYPE_ARP:
		//pprintf("\nAR");
		/* pass p to ARP module */

		etharp_arp_input(netif, (EthAddr *) netif->GetHardwareAddress(), p);

		break;
	default:



//		POSITION pos = customProtocols.GetHeadPosition();
//		while (pos)
//		{
//			CustomProtocol * protocol = customProtocols.GetNext(pos);
//			if (protocol->protocolNumber == htons(ethhdr->type))
//			{
//				POSITION pos1 = protocol->netif_list.GetHeadPosition();
//				while (pos1)
//				{
//					Ethernet * eth = protocol->netif_list.GetNext(pos1);
//					if (eth == netif){
//
//						return protocol->handler->recHandler(p, netif);
//					}
//				}
//				break;//current interface is not configured for this protocol;
//			}
//		}
		pbuf_free(p);
//
	}
	/* This means the pbuf is freed or consumed,
	 so the caller doesn't have to free it again */
	return ERR_OK;
}

/**
 * Responds to ARP requests to us. Upon ARP replies to us, add entry to cache
 * send out queued IP packets. Updates cache with snooped address pairs.
 *
 * Should be called for incoming ARP packets. The pbuf in the argument
 * is freed by this function.
 *
 * @param netif The lwIP network interface on which the ARP packet pbuf arrived.
 * @param ethaddr Ethernet address of netif.
 * @param p The ARP packet that arrived on netif. Is freed by this function.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
void etharp_arp_input(Ethernet *netif, EthAddr *ethaddr, PktBuf *p)
{
	etharp_hdr *hdr;
	/* these are aligned properly, whereas the ARP header fields might not be */
	ip_addr sipaddr, dipaddr;
	uint8 i;
	bool for_us;
	const uint8 * ethdst_hwaddr;

	/* drop short ARP packets: we have to check for p->len instead of p->tot_len here
	 since a struct etharp_hdr is pointed to p->payload, so it musn't be chained! */
	if (p->len < sizeof(etharp_hdr))
	{
		pbuf_free(p);
		printf("short\n");
		return;
	}

	hdr = (etharp_hdr *) p->payload;

	/* RFC 826 "Packet Reception": */
	if ((hdr->hwtype != htons(HWTYPE_ETHERNET)) || (hdr->_hwlen_protolen
			!= htons((EthAddrLen << 8) | sizeof(ip_addr))) || (hdr->proto
			!= htons(ETHTYPE_IP)) || (hdr->ethhdr.type != htons(ETHTYPE_ARP)))
	{
		pbuf_free(p);
		printf("826\n");
		return;
	}

	/* We have to check if a host already has configured our random
	 * created link local address and continously check if there is
	 * a host with this IP-address so we can detect collisions */
	autoip_arp_reply(netif, hdr);

	/* Copy struct ip_addr2 to aligned ip_addr, to support compilers without
	 * structure packing (not using structure copy which breaks strict-aliasing rules). */
	memcpy(&sipaddr, &hdr->sipaddr, sizeof(sipaddr));
	memcpy(&dipaddr, &hdr->dipaddr, sizeof(dipaddr));

	/* this interface is not configured? */
	if (netif->ipaddr.addr == 0)
	{
		for_us = false;
	}
	else
	{
		/* ARP packet directed to us? */
		for_us = ip_addr_cmp(&dipaddr, &(netif->ipaddr));
	}
	/* ARP message directed to us? */
	if (for_us)
	{
		/* add IP address in ARP cache; assume requester wants to talk to us.
		 * can result in directly sending the queued packets for this host. */
		update_arp_entry(netif, &sipaddr, &(hdr->shwaddr), ETHARP_TRY_HARD);
		/* ARP message not directed to us? */
	}
	//	else no Sniffing of ARP messages
	//	{
	//		/* update the source IP address in the cache, if present */
	//		update_arp_entry(netif, &sipaddr, &(hdr->shwaddr), 0);
	//	}

	/* now act on the message itself */
	switch (htons(hdr->opcode))
	{
	/* ARP request? */
	case ARP_REQUEST:
		/* ARP request. If it asked for our address, we send out a
		 * reply. In any case, we time-stamp any existing ARP entry,
		 * and possiby send out an IP packet that was queued on it. */

		//		LWIP_DEBUGF (ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_arp_input: incoming ARP request\n"));
		/* ARP request for our address? */
		if (for_us)
		{
			/* Re-use pbuf to send ARP reply.
			 Since we are re-using an existing pbuf, we can't call etharp_raw since
			 that would allocate a new pbuf. */
			hdr->opcode = htons(ARP_REPLY);

			hdr->dipaddr = hdr->sipaddr;
			memcpy(&hdr->sipaddr, &netif->ipaddr, sizeof(hdr->sipaddr));

			i = EthAddrLen;
			/* If we are using Link-Local, ARP packets must be broadcast on the
			 * link layer. (See RFC3927 Section 2.5) */
			ethdst_hwaddr = ((netif->autoip != NULL) && (netif->autoip->state
					!= AUTOIP_STATE_OFF)) ? (uint8*) (ethbroadcast.addr)
					: hdr->shwaddr.addr;

			while (i > 0)
			{
				i--;
				hdr->dhwaddr.addr[i] = hdr->shwaddr.addr[i];
				hdr->ethhdr.dest.addr[i] = ethdst_hwaddr[i];
				// not auto				hdr->ethhdr.dest.addr[i] = hdr->shwaddr.addr[i];
				hdr->shwaddr.addr[i] = ethaddr->addr[i];
				hdr->ethhdr.src.addr[i] = ethaddr->addr[i];
			}

			/* hwtype, hwaddr_len, proto, protolen and the type in the ethernet header
			 are already correct, we tested that before */

			/* return ARP reply */
			netif->low_level_output(p);
			/* we are not configured? */
		}
		else if (netif->ipaddr.addr == 0)
		{
			/* { for_us == 0 and netif->ip_addr.addr == 0 } */
			//			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_arp_input: we are unconfigured, ARP request ignored.\n"));
			/* request was not directed to us */
		}
		else
		{
			/* { for_us == 0 and netif->ip_addr.addr != 0 } */
			//			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_arp_input: ARP request was not for us.\n"));
		}
		break;
	case ARP_REPLY:
		/* ARP reply. We already updated the ARP cache earlier. */
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_arp_input: incoming ARP reply\n"));
#if (LWIP_DHCP && DHCP_DOES_ARP_CHECK)
		/* DHCP wants to know about ARP replies from any host with an
		 * IP address also offered to us by the DHCP server. We do not
		 * want to take a duplicate IP address on a single network.
		 * @todo How should we handle redundant (fail-over) interfaces? */
		dhcp_arp_reply(Ethernet, &sipaddr);
#endif
		break;
	default:
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_arp_input: ARP unknown opcode type %"S16_F"\n", htons(hdr->opcode)));
		break;
	}
	/* free ARP packet */
	pbuf_free(p);
}

/**
 * Send a raw ARP packet (opcode and all addresses can be modified)
 *
 * @param netif the lwip network interface on which to send the ARP packet
 * @param ethsrc_addr the source MAC address for the ethernet header
 * @param ethdst_addr the destination MAC address for the ethernet header
 * @param hwsrc_addr the source MAC address for the ARP protocol header
 * @param ipsrc_addr the source IP address for the ARP protocol header
 * @param hwdst_addr the destination MAC address for the ARP protocol header
 * @param ipdst_addr the destination IP address for the ARP protocol header
 * @param opcode the type of the ARP packet
 * @return ERR_OK if the ARP packet has been sent
 *         ERR_MEM if the ARP packet couldn't be allocated
 *         any other int on failure
 */
int etharp_raw(Ethernet *netif, const EthAddr *ethsrc_addr,
		const EthAddr *ethdst_addr, const EthAddr *hwsrc_addr,
		const ip_addr *ipsrc_addr, const EthAddr *hwdst_addr,
		const ip_addr *ipdst_addr, uint16 opcode)
{
	PktBuf *p;
	int result = ERR_OK;
	uint8 k; /* ARP entry index */
	etharp_hdr *hdr;
	const uint8 * ethdst_hwaddr;

	/* allocate a pbuf for the outgoing ARP request packet */
	p = pbuf_alloc(PBUF_RAW, sizeof(etharp_hdr), PBUF_RAM);
	/* could allocate a pbuf for an ARP request? */
	if (!p)
		return ERR_MEM;
	ASSERT(p->len >= sizeof(etharp_hdr));

	hdr = (etharp_hdr *) p->payload;
	//printf("etharp_raw: sending raw ARP packet.\n");
	hdr->opcode = htons(opcode);

	k = EthAddrLen;
	/* If we are using Link-Local, ARP packets must be broadcast on the
	 * link layer. (See RFC3927 Section 2.5) */
	ethdst_hwaddr = ((netif->autoip != NULL) && (netif->autoip->state
			!= AUTOIP_STATE_OFF)) ? (uint8*) (ethbroadcast.addr)
			: ethdst_addr->addr;
	/* Write MAC-Addresses (combined loop for both headers) */
	while (k > 0)
	{
		k--;
		/* Write the ARP MAC-Addresses */
		hdr->shwaddr.addr[k] = hwsrc_addr->addr[k];
		hdr->dhwaddr.addr[k] = hwdst_addr->addr[k];
		/* Write the Ethernet MAC-Addresses */
		hdr->ethhdr.dest.addr[k] = ethdst_hwaddr[k];
		// not auto		hdr->ethhdr.dest.addr[k] = ethdst_addr->addr[k];
		hdr->ethhdr.src.addr[k] = ethsrc_addr->addr[k];
	}
	hdr->sipaddr.addr = ipsrc_addr->addr;
	hdr->dipaddr.addr = ipdst_addr->addr;

	hdr->hwtype = htons(HWTYPE_ETHERNET);
	hdr->proto = htons(ETHTYPE_IP);
	/* set hwlen and protolen together */
	hdr->_hwlen_protolen = htons((EthAddrLen << 8) | sizeof(struct ip_addr));

	hdr->ethhdr.type = htons(ETHTYPE_ARP);
	/* send ARP query */
	result = netif->low_level_output(p);
	/* free ARP query packet */
	pbuf_free(p);
	p = NULL;
	/* could not allocate pbuf for ARP request */

	return result;
}

/**
 * Send an ARP request packet asking for ipaddr.
 *
 * @param netif the lwip network interface on which to send the request
 * @param ipaddr the IP address for which to ask
 * @return ERR_OK if the request has been sent
 *         ERR_MEM if the ARP packet couldn't be allocated
 *         any other int on failure
 */
int etharp_request(Ethernet *netif, ip_addr *ipaddr)
{
	//printf("etharp_request: sending ARP request.\n");
	return etharp_raw(netif, (EthAddr *) netif->GetHardwareAddress(),
			&ethbroadcast, (EthAddr *) netif->GetHardwareAddress(),
			&netif->ipaddr, &ethzero, ipaddr, ARP_REQUEST);
}

/**
 * Send an ARP request for the given IP address and/or queue a packet.
 *
 * If the IP address was not yet in the cache, a pending ARP cache entry
 * is added and an ARP request is sent for the given address. The packet
 * is queued on this entry.
 *
 * If the IP address was already pending in the cache, a new ARP request
 * is sent for the given address. The packet is queued on this entry.
 *
 * If the IP address was already stable in the cache, and a packet is
 * given, it is directly sent and no ARP request is sent out.
 *
 * If the IP address was already stable in the cache, and no packet is
 * given, an ARP request is sent out.
 *
 * @param netif The lwIP network interface on which ipaddr
 * must be queried for.
 * @param ipaddr The IP address to be resolved.
 * @param q If non-NULL, a pbuf that must be delivered to the IP address.
 * q is not freed by this function.
 *
 * @note q must only be ONE packet, not a packet queue!
 *
 * @return
 * - ERR_BUF Could not make room for Ethernet header.
 * - ERR_MEM Hardware address unknown, and no more ARP entries available
 *   to query for address or queue the packet.
 * - ERR_MEM Could not queue packet due to memory shortage.
 * - ERR_RTE No route to destination (no gateway to external networks).
 * - ERR_ARG Non-unicast address given, those will not appear in ARP cache.
 *
 */
int etharp_query(Ethernet *netif, ip_addr *ipaddr, PktBuf *q)
{
	EthAddr * srcaddr = (EthAddr *) netif->GetHardwareAddress();
	int result = ERR_MEM;

	/* non-unicast address? */
	if (ip_addr_isbroadcast(ipaddr, netif) || ip_addr_ismulticast(ipaddr)
			|| ip_addr_isany(ipaddr))
	{
		//		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_query: will not add non-unicast IP address to ARP cache\n"));
		return ERR_ARG;
	}

	/* find entry in ARP cache, ask to create entry if queueing packet */
	int i = find_entry(ipaddr, ETHARP_TRY_HARD);
	/* could not find or create entry? */
	if (i < 0)
		return i;

	/* mark a fresh entry as pending (we just sent a request) */
	if (arp_table[i].state == ETHARP_STATE_EMPTY)

		arp_table[i].state = ETHARP_STATE_PENDING;

	/* { i is either a STABLE or (new or existing) PENDING entry } */
	ASSERT((arp_table[i].state == ETHARP_STATE_PENDING) || (arp_table[i].state
					== ETHARP_STATE_STABLE));

	/* do we have a pending entry? or an implicit query request? */
	if ((arp_table[i].state == ETHARP_STATE_PENDING) || (q == NULL))
	{
		/* try to resolve it; send out ARP request */
		result = etharp_request(netif, ipaddr);
		if (result != ERR_OK)
		{
			/* ARP request couldn't be sent */
			/* We don't re-send arp request in etharp_tmr, but we still queue packets,
			 since this failure could be temporary, and the next packet calling
			 etharp_query again could lead to sending the queued packets. */
		}
	}

	/* packet given? */
	if (q != NULL)
	{
		/* stable entry? */
		if (arp_table[i].state == ETHARP_STATE_STABLE)
		{
			/* we have a valid IP->Ethernet address mapping */
			/* send the packet */
			result = etharp_send_ip(netif, q, srcaddr, &(arp_table[i].ethaddr));
			/* pending entry? (either just created or already pending */
		}
		else if (arp_table[i].state == ETHARP_STATE_PENDING)
		{
			PktBuf *p;
			int copy_needed = 0;
			/* IF q includes a PBUF_REF, PBUF_POOL or PBUF_RAM, we have no choice but
			 * to copy the whole queue into a new PBUF_RAM (see bug #11400)
			 * PBUF_ROMs can be left as they are, since ROM must not get changed. */
			p = q;
			while (p)
			{
				ASSERT((p->len != p->tot_len) || (p->next == 0));
				if (p->type != PBUF_ROM)
				{
					copy_needed = 1;
					break;
				}
				p = p->next;
			}
			if (copy_needed)
			{
				/* copy the whole packet into new pbufs */
				p = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
				if (p != NULL)
				{
					if (pbuf_copy(p, q) != ERR_OK)
					{
						pbuf_free(p);
						p = NULL;
					}
				}
			}
			else
			{
				/* referencing the old pbuf is enough */
				p = q;
				pbuf_ref(p);
			}
			/* packet could be taken over? */
			if (p != NULL)
			{
				/* queue packet ... */
				/* allocate a new arp queue entry */
				ArpQueueEntry * new_entry = new ArpQueueEntry();
				if (new_entry != NULL)
				{
					new_entry->next = 0;
					new_entry->p = p;
					if (arp_table[i].q != NULL)
					{
						/* queue was already existent, append the new entry to the end */
						ArpQueueEntry *r;
						r = arp_table[i].q;
						while (r->next != NULL)
						{
							r = r->next;
						}
						r->next = new_entry;
					}
					else
					{
						/* queue did not exist, first item in queue */
						arp_table[i].q = new_entry;
					}
					result = ERR_OK;
				}
				else
				{
					pbuf_free(p);
					printf("etharp_query: could not queue a copy of PBUF_REF"
						" packet %p (out of memory)\n", q);
					/* { result == ERR_MEM } through initialization */
				}
			}
			else
			{
				printf("etharp_query: could not queue a copy of PBUF_REF"
					"packet %p (out of memory)\n", q);
				/* { result == ERR_MEM } through initialization */
			}
		}
	}
	return result;
}

/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
int etharp_output(Ethernet *netif, PktBuf *q, ip_addr *ipaddr)
{
	EthAddr *dest, mcastaddr;

	/* make room for Ethernet header - should not fail */
	if (pbuf_header(q, sizeof(EthHeader)) != 0)
		return ERR_BUF;

	/* assume unresolved Ethernet address */
	dest = NULL;
	/* Determine on destination hardware address. Broadcasts and multicasts
	 * are special, other IP addresses are looked up in the ARP table. */

	/* broadcast destination IP address? */
	if (ip_addr_isbroadcast(ipaddr, netif))
	{
		/* broadcast on Ethernet also */
		dest = (EthAddr *) &ethbroadcast;
		/* multicast destination IP address? */
	}
	else if (ip_addr_ismulticast(ipaddr))
	{
		/* Hash IP multicast address to MAC address.*/
		mcastaddr.addr[0] = 0x01;
		mcastaddr.addr[1] = 0x00;
		mcastaddr.addr[2] = 0x5e;
		mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
		mcastaddr.addr[4] = ip4_addr3(ipaddr);
		mcastaddr.addr[5] = ip4_addr4(ipaddr);
		/* destination Ethernet address is multicast */
		dest = &mcastaddr;
		/* unicast destination IP address? */
	}
	else
	{
		/* outside local network? */
		if (!ip_addr_netcmp(ipaddr, &(netif->ipaddr), &(netif->netmask)))
		{
			/* interface has default gateway? */
			if (netif->gw.addr != 0)
			{
				/* send to hardware address of default gateway IP address */
				ipaddr = &(netif->gw);
				/* no default gateway available */
			}
			else
			{
				/* no route to destination error (default gateway missing) */
				return ERR_RTE;
			}
		}
		/* queue on destination Ethernet address belonging to ipaddr */
		return etharp_query(netif, ipaddr, q);
	}

	/* continuation for multicast/broadcast destinations */
	/* obtain source Ethernet address of the given interface */
	/* send packet directly on the link */
	return etharp_send_ip(netif, q, (EthAddr*) (netif->GetHardwareAddress()),
			dest);
}

/**
 * Clears expired entries in the ARP table.
 *
 * This function should be called every ETHARP_TMR_INTERVAL microseconds (5 seconds),
 * in order to expire entries in the ARP table.
 */
void etharp_tmr()
{
	/* remove expired entries from the ARP table */
	for (int i = 0; i < ARP_TABLE_SIZE; ++i)
	{
		arp_table[i].ctime++;
		if (((arp_table[i].state == ETHARP_STATE_STABLE) && (arp_table[i].ctime
				>= ARP_MAXAGE))
				|| ((arp_table[i].state == ETHARP_STATE_PENDING)
						&& (arp_table[i].ctime >= ARP_MAXPENDING)))
		{
			/* pending or stable entry has become old! */
			/* clean up entries that have just been expired */
			/* remove from SNMP ARP index tree */
			//			snmp_delete_arpidx_tree(arp_table[i].Ethernet, &arp_table[i].ipaddr);
			/* and empty packet queue */
			if (arp_table[i].q != NULL)
			{
				/* remove all queued packets */
				free_etharp_q(arp_table[i].q);
				arp_table[i].q = NULL;
			}
			/* recycle entry for re-use */
			arp_table[i].state = ETHARP_STATE_EMPTY;
		}
		/* still pending entry? (not expired) */
		if (arp_table[i].state == ETHARP_STATE_PENDING)
		{
			/* resend an ARP query here? */
		}
	}
}

/**
 * Finds (stable) ethernet/IP address pair from ARP table
 * using interface and IP address index.
 * @note the addresses in the ARP table are in network order!
 *
 * @param netif points to interface index
 * @param ipaddr points to the (network order) IP address index
 * @param eth_ret points to return pointer
 * @param ip_ret points to return pointer
 * @return table index if found, -1 otherwise
 */
int etharp_find_addr(Ethernet *netif, ip_addr *ipaddr, EthAddr **eth_ret,
		ip_addr **ip_ret)
{
	int8 i = find_entry(ipaddr, ETHARP_FIND_ONLY);
	if ((i >= 0) && arp_table[i].state == ETHARP_STATE_STABLE)
	{
		*eth_ret = &arp_table[i].ethaddr;
		*ip_ret = &arp_table[i].ipaddr;
		return i;
	}
	return -1;
	netif = 0;
}

void etharp_gratuitous(Ethernet * netif)
{
	etharp_request(netif, &netif->ipaddr);
}
