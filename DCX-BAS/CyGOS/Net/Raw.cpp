// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/Raw.cpp_v   1.3   04 Sep 2013 13:52:52   ygupta  $
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

#include "Raw.h"
#include "PktBuf.h"
#include "NetBuf.h"
#include "NetConn.h"
#include "MailBox.h"
#include "Ethernet.h"
#include "assert.h"
#include "DebugNew.h"

RawSocket *raw_pcbs;

void raw_init()
{
}

/**
 * Determine if in incoming IP packet is covered by a RAW PCB
 * and if so, pass it to a user-provided receive callback function.
 *
 * Given an incoming IP datagram (as a chain of pbufs) this function
 * finds a corresponding RAW PCB and calls the corresponding receive
 * callback function.
 *
 * @param p pbuf to be demultiplexed to a RAW PCB.
 * @param inp network interface on which the datagram was received.
 * @return - 1 if the packet has been eaten by a RAW PCB receive
 *           callback function. The caller MAY NOT not reference the
 *           packet any longer, and MAY NOT call pbuf_free().
 * @return - 0 if packet is not eaten (pbuf is still referenced by the
 *           caller).
 *
 */
bool raw_input(PktBuf *p, Ethernet *inp)
{
	RawSocket *pcb, *prev;
	IPv4Header *iphdr;
	int16 proto;
	bool eaten = false;

	iphdr = (IPv4Header *) p->payload;
	proto = iphdr->PROTO();

	prev = NULL;
	pcb = raw_pcbs;
	/* loop through all raw pcbs until the packet is eaten by one */
	/* this allows multiple pcbs to match against the packet by design */
	while ((!eaten) && (pcb != NULL))
	{
		if (pcb->protocol == proto)
		{
#if IP_SOF_BROADCAST_RECV
			/* broadcast filter? */
			if ((pcb->so_options & SOF_BROADCAST) || !ip_addr_isbroadcast(&(iphdr->dest), inp))
#endif /* IP_SOF_BROADCAST_RECV */
			{
				/* receive callback function available? */
				if (pcb->recv != NULL)
				{
					/* the receive callback function did not eat the packet? */
					if (pcb->recv(pcb->recv_arg, pcb, p, &(iphdr->src)) != 0)
					{
						/* receive function ate the packet */
						p = NULL;
						eaten = true;
						if (prev != NULL)
						{
							/* move the pcb to the front of raw_pcbs so that is
							 found faster next time */
							prev->next = pcb->next;
							pcb->next = raw_pcbs;
							raw_pcbs = pcb;
						}
					}
				}
				/* no receive callback function was set for this raw PCB */
			}
			/* drop the packet */
		}
		prev = pcb;
		pcb = pcb->next;
	}
	return eaten;
	ASSERT(inp);
}

/**
 * Create a RAW PCB.
 *
 * @return The RAW PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @param proto the protocol number of the IPs payload (e.g. IP_PROTO_ICMP)
 *
 * @see raw_remove()
 */
RawSocket * raw_new(int proto)
{
	RawSocket * pcb = new RawSocket();
	/* could allocate RAW PCB? */
	if (pcb != NULL)
	{
		/* initialize PCB to all zeroes */
		memset(pcb, 0, sizeof(RawSocket));
		pcb->protocol = proto;
		pcb->ttl = RAW_TTL;
		pcb->next = raw_pcbs;
		raw_pcbs = pcb;
	}
	return pcb;
}

/**
 * Receive callback function for RAW netconns.
 * Doesn't 'eat' the packet, only references it and sends it to
 * conn->recvmbox
 *
 * @see raw.h (struct raw_pcb.recv) for parameters and return value
 */
bool recv_raw(void *arg, RawSocket *pcb, PktBuf *p, ip_addr *addr)
{
	PktBuf *q;
	NetBuf *buf;
	NetConn *conn;
	int recv_avail;

	conn = (NetConn *) arg;

	if(conn != NULL)
		recv_avail = conn->recv_avail;
	if ((conn != NULL) && (conn->recvmbox != 0) && ((recv_avail
			+ (int) (p->tot_len)) <= conn->recv_bufsize))
	{
		/* copy the whole packet into new pbufs */
		q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
		if (q != NULL)
		{
			if (pbuf_copy(q, p) != ERR_OK)
			{
				pbuf_free(q);
				q = NULL;
			}
		}

		if (q != NULL)
		{
			buf = new NetBuf();
			if (buf == NULL)
			{
				pbuf_free(q);
				return 0;
			}

			buf->p = q;
			buf->ptr = q;
			buf->addr = &(((IPv4Header*) (q->payload))->src);
			buf->port = pcb->protocol;

			if (!conn->recvmbox->TryPost(buf))
			{
				netbuf_delete(buf);
				return 0;
			}
			else
			{
				conn->recv_avail += q->tot_len;
				/* Register event with callback */
				API_EVENT(conn, NETCONN_EVT_RCVPLUS, q->tot_len);
			}
		}
	}

	return 0; /* do not eat the packet */
	ASSERT(addr);
}

/**
 * Bind a RAW PCB.
 *
 * @param pcb RAW PCB to be bound with a local address ipaddr.
 * @param ipaddr local IP address to bind with. Use IP_ADDR_ANY to
 * bind to all local interfaces.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_USE. The specified IP address is already bound to by
 * another RAW PCB.
 *
 * @see raw_disconnect()
 */
int raw_bind(RawSocket *pcb, ip_addr *ipaddr)
{
	ip_addr_set(&pcb->local_ip, ipaddr);
	return ERR_OK;
}

/**
 * Set the callback function for received packets that match the
 * raw PCB's protocol and binding.
 *
 * The callback function MUST either
 * - eat the packet by calling pbuf_free() and returning non-zero. The
 *   packet will not be passed to other raw PCBs or other protocol layers.
 * - not free the packet, and return zero. The packet will be matched
 *   against further PCBs and/or forwarded to another protocol layers.
 *
 * @return non-zero if the packet was free()d, zero if the packet remains
 * available for others.
 */
void raw_recv(RawSocket *pcb, bool(* recv)(void *arg, RawSocket *upcb,
		PktBuf *p, ip_addr *addr), void *recv_arg)
{
	/* remember recv() callback and user data */
	pcb->recv = recv;
	pcb->recv_arg = recv_arg;
}

/**
 * Send the raw IP packet to the given address. Note that actually you cannot
 * modify the IP headers (this is inconsistent with the receive callback where
 * you actually get the IP headers), you can only specify the IP payload here.
 * It requires some more changes in lwIP. (there will be a raw_send() function
 * then.)
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 * @param ipaddr the destination address of the IP packet
 *
 */
int raw_sendto(RawSocket *pcb, PktBuf *p, ip_addr *ipaddr)
{
	int err;
	Ethernet * netif;
	ip_addr *src_ip;
	PktBuf *q; /* q will be sent down the stack */

	/* not enough space to add an IP header to first pbuf in given p chain? */
	if (pbuf_header(p, sizeof(IPv4Header)))
	{
		/* allocate header in new pbuf */
		q = pbuf_alloc(PBUF_IP, 0, PBUF_RAM);
		/* new header pbuf could not be allocated? */
		if (!q)
			return ERR_MEM;
		/* chain header q in front of given pbuf p */
		pbuf_chain(q, p);
		/* { first pbuf q points to header pbuf } */
	}
	else
	{
		/* first pbuf q equals given pbuf */
		q = p;
		if (pbuf_header(q, -((int) sizeof(IPv4Header))))
		{
			ASSERT(0);
			return ERR_MEM;
		}
	}

	if ((netif = ip_route(ipaddr, 0)) == NULL)
	{
//		("raw_sendto: No route to 0x%08X\n", ipaddr->addr);
		/* free any temporary header pbuf allocated by pbuf_header() */
		if (q != p)
		{
			pbuf_free(q);
		}
		return ERR_RTE;
	}

#if IP_SOF_BROADCAST
	/* broadcast filter? */
	if ( ((pcb->so_options & SOF_BROADCAST) == 0) && ip_addr_isbroadcast(ipaddr, NetIf) )
	{
		LWIP_DEBUGF(RAW_DEBUG | 1, ("raw_sendto: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
		/* free any temporary header pbuf allocated by pbuf_header() */
		if (q != p)
		{
			pbuf_free(q);
		}
		return ERR_VAL;
	}
#endif /* IP_SOF_BROADCAST */

	if (ip_addr_isany(&pcb->local_ip))
	{
		/* use outgoing network interface IP address as source address */
		src_ip = &(netif->ipaddr);
	}
	else
	{
		/* use RAW PCB local IP address as source address */
		src_ip = &(pcb->local_ip);
	}

	err = ip_output_if(q, src_ip, ipaddr, pcb->ttl, pcb->tos, pcb->protocol,
			netif);

	/* did we chain a header earlier? */
	if (q != p)
	{
		/* free the header */
		pbuf_free(q);
	}
	return err;
}

/**
 * Send the raw IP packet to the address given by raw_connect()
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 *
 */
int raw_send(RawSocket *pcb, PktBuf *p)
{
	return raw_sendto(pcb, p, &pcb->remote_ip);
}

/**
 * Remove an RAW PCB.
 *
 * @param pcb RAW PCB to be removed. The PCB is removed from the list of
 * RAW PCB's and the data structure is freed from memory.
 *
 * @see raw_new()
 */
void raw_remove(RawSocket *pcb)
{
	RawSocket *pcb2;
	/* pcb to be removed is first in list? */
	if (raw_pcbs == pcb)
	{
		/* make list start at 2nd pcb */
		raw_pcbs = raw_pcbs->next;
		/* pcb not 1st in list */
	}
	else
	{
		for (pcb2 = raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next)
		{
			/* find pcb in raw_pcbs list */
			if (pcb2->next != NULL && pcb2->next == pcb)
			{
				/* remove pcb from list */
				pcb2->next = pcb->next;
			}
		}
	}
	delete pcb;
}

/**
 * Connect an RAW PCB. This function is required by upper layers
 * of lwip. Using the raw api you could use raw_sendto() instead
 *
 * This will associate the RAW PCB with the remote address.
 *
 * @param pcb RAW PCB to be connected with remote address ipaddr and port.
 * @param ipaddr remote IP address to connect with.
 *
 * @return lwIP error code
 *
 * @see raw_disconnect() and raw_sendto()
 */
int raw_connect(RawSocket *pcb, ip_addr *ipaddr)
{
	ip_addr_set(&pcb->remote_ip, ipaddr);
	return ERR_OK;
}

