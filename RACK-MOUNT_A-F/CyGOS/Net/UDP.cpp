// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/UDP.cpp_v   1.1   24 Jul 2015 08:51:08   sesharma  $
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

#include "UDP.h"
#include "Net.h"
#include "PktBuf.h"
#include "ICMP.h"
#include "Ethernet.h"
#include "NetBuf.h"
#include "MailBox.h"
#include "NetConn.h"
#include "DebugNew.h"
#include "DhcpClient.h"

#define UDP_TTL 255

/* The list of UDP PCBs */
UdpSocket *udp_pcbs;

void udp_init()
{
}

/**
 * Process an incoming UDP datagram.
 *
 * Given an incoming UDP datagram (as a chain of pbufs) this function
 * finds a corresponding UDP PCB and hands over the pbuf to the pcbs
 * recv function. If no pcb is found or the datagram is incorrect, the
 * pbuf is freed.
 *
 * @param p pbuf to be demultiplexed to a UDP PCB.
 * @param inp network interface on which the datagram was received.
 *
 */
void udp_input(PktBuf *p, Ethernet *inp)
{
	UdpHeader *udphdr;
	UdpSocket *pcb, *prev;
	UdpSocket *uncon_pcb;
	IPv4Header *iphdr;
	uint16 src, dest;
	uint8 local_match;
	bool broadcast;

	iphdr = (IPv4Header *) p->payload;

	/* Check minimum length (IP header + UDP header)
	 * and move payload pointer to UDP header */
	if (p->tot_len < (iphdr->HL() * 4 + sizeof(UdpHeader)) || pbuf_header(p,
			-(int16) (iphdr->HL() * 4)))
	{
		pbuf_free(p);
		return;
	}

	udphdr = (UdpHeader *) p->payload;

	/* is broadcast packet ? */
	broadcast = ip_addr_isbroadcast(&(iphdr->dest), inp);

	/* convert src and dest ports to host byte order */
	src = ntohs(udphdr->src);
	dest = ntohs(udphdr->dest);

	// DHCP stuff
	pcb = NULL;
	// when DHCP is active, packets to DHCP_CLIENT_PORT may only be processed by
	// the dhcp module, no other UDP pcb may use the local UDP port
	// DHCP_CLIENT_PORT
	if (dest == DHCP_CLIENT_PORT)
	{
		/* all packets for DHCP_CLIENT_PORT not coming from DHCP_SERVER_PORT are dropped! */
		if (src == DHCP_SERVER_PORT)
		{
			if ((inp->dhcp != NULL) && (inp->dhcp->pcb != NULL))
			{
				/* accept the packe if
				 (- broadcast or directed to us) -> DHCP is link-layer-addressed, local ip is always ANY!
				 - inp->dhcp->pcb->remote == ANY or iphdr->src */
				if ((ip_addr_isany(&inp->dhcp->pcb->remote_ip) || ip_addr_cmp(
						&(inp->dhcp->pcb->remote_ip), &(iphdr->src))))
				{
					pcb = inp->dhcp->pcb;
				}
			}
		}
	}
	else
	{
		prev = NULL;
		local_match = 0;
		uncon_pcb = NULL;
		/* Iterate through the UDP pcb list for a matching pcb.
		 * 'Perfect match' pcbs (connected to the remote port & ip address) are
		 * preferred. If no perfect match is found, the first unconnected pcb that
		 * matches the local port and ip address gets the datagram. */
		for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next)
		{
			local_match = 0;
			/* print the PCB local and remote address */
			/* compare PCB local addr+port to UDP destination addr+port */
			if ((pcb->local_port == dest) && ((!broadcast && ip_addr_isany(
					&pcb->local_ip)) || ip_addr_cmp(&(pcb->local_ip),
					&(iphdr->dest)) || ip_addr_ismulticast(&(iphdr->dest)) ||
#if IP_SOF_BROADCAST_RECV
					(broadcast && (pcb->so_options & SOF_BROADCAST))))
					{
#else  /* IP_SOF_BROADCAST_RECV */
					(broadcast)))
			{
#endif /* IP_SOF_BROADCAST_RECV */
				local_match = 1;
				if ((uncon_pcb == NULL) && ((pcb->flags & UDP_FLAGS_CONNECTED)
						== 0))
				{
					/* the first unconnected matching PCB */
					uncon_pcb = pcb;
				}
			}
			/* compare PCB remote addr+port to UDP source addr+port */
			if ((local_match != 0) && (pcb->remote_port == src)
					&& (ip_addr_isany(&pcb->remote_ip) || ip_addr_cmp(
							&(pcb->remote_ip), &(iphdr->src))))
			{
				/* the first fully matching PCB */
				if (prev != NULL)
				{
					/* move the pcb to the front of udp_pcbs so that is
					 found faster next time */
					prev->next = pcb->next;
					pcb->next = udp_pcbs;
					udp_pcbs = pcb;
				}
				break;
			}
			prev = pcb;
		}
		/* no fully matching pcb found? then look for an unconnected pcb */
		if (pcb == NULL)
		{
			pcb = uncon_pcb;
		}
	}

	/* Check checksum if this is a match or if it was directed at us. */
	if (pcb != NULL || ip_addr_cmp(&inp->ipaddr, &iphdr->dest))
	{
		{
			if (udphdr->chksum != 0)
			{
				if (inet_chksum_pseudo(p, (ip_addr *) &(iphdr->src),
						(ip_addr *) &(iphdr->dest), IP_PROTO_UDP, p->tot_len)
						!= 0)
				{
					pbuf_free(p);
					return;
				}
			}
		}
		if (pbuf_header(p, -((int) sizeof(UdpHeader))))
		{
			/* Can we cope with this failing? Just assert for now */
			pbuf_free(p);
			return;
		}
		if (pcb != NULL)
		{
			//			snmp_inc_udpindatagrams();
			/* callback */
			if (pcb->recv != NULL)
			{
				/* now the recv function is responsible for freeing p */
				pcb->recv(pcb->recv_arg, pcb, p, &(iphdr->src), src);
			}
			else
			{
				/* no recv function registered? then we have to free the pbuf! */
				pbuf_free(p);
				return;
			}
		}
		else
		{
			/* No match was found, send ICMP destination port unreachable unless
			 destination address was broadcast/multicast. */
			if (!broadcast && !ip_addr_ismulticast(&iphdr->dest))
			{
				/* move payload pointer back to ip header */
				pbuf_header(p, (iphdr->HL() * 4) + sizeof(UdpHeader));
				ASSERT(p->payload == (uint8 *) iphdr);
				icmp_dest_unreach(p, ICMP_DUR_PORT);
			}
			pbuf_free(p);
		}
	}
	else
	{
		pbuf_free(p);
	}
}

/**
 * Receive callback function for UDP netconns.
 * Posts the packet to conn->recvmbox or deletes it on memory error.
 *
 * @see udp.h (struct udp_pcb.recv) for parameters
 */
void recv_udp(void *arg, UdpSocket *pcb, PktBuf *p, ip_addr *addr, int port)
{
	ASSERT(pcb);
	ASSERT(arg);
	NetConn * conn = (NetConn *) arg;
	ASSERT(conn->pcb.udp == pcb);

	int recv_avail = conn->recv_avail;
	if ((conn == NULL) || (conn->recvmbox == 0) || ((recv_avail
			+ (int) (p->tot_len)) > conn->recv_bufsize))
	{
		pbuf_free(p);
		return;
	}

	NetBuf * buf = new NetBuf();
	if (buf == NULL)
	{
		pbuf_free(p);
		return;
	}
	else
	{
		buf->p = p;
		buf->ptr = p;
		buf->addr = addr;
		buf->port = port;
	}

	if (!conn->recvmbox->TryPost(buf))
	{
		netbuf_delete(buf);
		return;
	}
	else
	{
		conn->recv_avail += p->tot_len;
		// Register event with callback
		API_EVENT(conn, NETCONN_EVT_RCVPLUS, p->tot_len);
	}
}

/**
 * Remove an UDP PCB.
 *
 * @param pcb UDP PCB to be removed. The PCB is removed from the list of
 * UDP PCB's and the data structure is freed from memory.
 *
 * @see udp_new()
 */
void udp_remove(UdpSocket *pcb)
{
	UdpSocket *pcb2;

	//	snmp_delete_udpidx_tree(pcb);
	// pcb to be removed is first in list?
	if (udp_pcbs == pcb)
	{
		// make list start at 2nd pcb
		udp_pcbs = udp_pcbs->next;
		// pcb not 1st in list
	}
	else
	{
		for (pcb2 = udp_pcbs; pcb2 != NULL; pcb2 = pcb2->next)
		{
			// find pcb in udp_pcbs list
			if (pcb2->next != NULL && pcb2->next == pcb)
			{
				// remove pcb from list
				pcb2->next = pcb->next;
			}
		}
	}
	delete pcb;
}

/**
 * Bind an UDP PCB.
 *
 * @param pcb UDP PCB to be bound with a local address ipaddr and port.
 * @param ipaddr local IP address to bind with. Use IP_ADDR_ANY to
 * bind to all local interfaces.
 * @param port local UDP port to bind with. Use 0 to automatically bind
 * to a random port between UDP_LOCAL_PORT_RANGE_START and
 * UDP_LOCAL_PORT_RANGE_END.
 *
 * ipaddr & port are expected to be in the same byte order as in the pcb.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_USE. The specified ipaddr and port are already bound to by
 * another UDP PCB.
 *
 * @see udp_disconnect()
 */
int udp_bind(UdpSocket *pcb, ip_addr *ipaddr, int port)
{
	UdpSocket *ipcb;
	uint8 rebind;

	rebind = 0;
	// Check for double bind and rebind of the same pcb
	for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next)
	{
		// is this UDP PCB already on active list?
		if (pcb == ipcb)
		{
			// pcb may occur at most once in active list
			ASSERT(rebind == 0);
			// pcb already in list, just rebind
			rebind = 1;
		}

		// this code does not allow upper layer to share a UDP port for
		// listening to broadcast or multicast traffic (See SO_REUSE_ADDR and
		// SO_REUSE_PORT under *BSD). TODO: See where it fits instead, OR
		// combine with implementation of UDP PCB flags. Leon Woestenberg.
#ifdef LWIP_UDP_TODO
		// port matches that of PCB in list?

		else
		if ((ipcb->local_port == port) &&
				// IP address matches, or one is IP_ADDR_ANY?
				(ip_addr_isany(&(ipcb->local_ip)) ||
						ip_addr_isany(ipaddr) ||
						ip_addr_cmp(&(ipcb->local_ip), ipaddr)))
		{
			// other PCB already binds to this local IP and port
			LWIP_DEBUGF(UDP_DEBUG,
					("udp_bind: local port %"U16_F" already bound by another pcb\n", port));
			return ERR_USE;
		}
#endif
	}

	ip_addr_set(&pcb->local_ip, ipaddr);

	// no port specified?
	if (!port)
	{
#ifndef UDP_LOCAL_PORT_RANGE_START
#define UDP_LOCAL_PORT_RANGE_START 4096
#define UDP_LOCAL_PORT_RANGE_END   0x7fff
#endif
		port = UDP_LOCAL_PORT_RANGE_START;
		ipcb = udp_pcbs;
		while ((ipcb != NULL) && (port != UDP_LOCAL_PORT_RANGE_END))
		{
			if (ipcb->local_port == port)
			{
				// port is already used by another udp_pcb
				port++;
				// restart scanning all udp pcbs
				ipcb = udp_pcbs;
			}
			else
				// go on with next udp pcb
				ipcb = ipcb->next;
		}
		if (ipcb != NULL)
		{
			// no more ports available in local range
			printf("udp_bind: out of free UDP ports\n");
			return ERR_USE;
		}
	}
	pcb->local_port = port;
	//	snmp_insert_udpidx_tree(pcb);
	// pcb not active yet?
	if (rebind == 0)
	{
		// place the PCB on the active list if not already there
		pcb->next = udp_pcbs;
		udp_pcbs = pcb;
	}
	return ERR_OK;
}

/**
 * Send data using UDP.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 *
 * The datagram will be sent to the current remote_ip & remote_port
 * stored in pcb. If the pcb is not bound to a port, it will
 * automatically be bound to a random port.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_MEM. Out of memory.
 * - ERR_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
 *
 * @see udp_disconnect() udp_sendto()
 */
int udp_send(UdpSocket *pcb, PktBuf *p)
{
	// send to the packet using remote ip and port stored in the pcb
	return udp_sendto(pcb, p, &pcb->remote_ip, pcb->remote_port);
}

/**
 * Send data to a specified address using UDP.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 * @param dst_ip Destination IP address.
 * @param dst_port Destination UDP port.
 *
 * dst_ip & dst_port are expected to be in the same byte order as in the pcb.
 *
 * If the PCB already has a remote address association, it will
 * be restored after the data is sent.
 *
 * @return lwIP error code (@see udp_send for possible error codes)
 *
 * @see udp_disconnect() udp_send()
 */
int udp_sendto(UdpSocket *pcb, PktBuf *p, ip_addr *dst_ip, int dst_port)
{
	Ethernet *netif;

	// find the outgoing network interface for this packet
	netif = ip_route((ip_addr_ismulticast(dst_ip)) ? &pcb->multicast_ip
			: dst_ip, 0);

	// no outgoing network interface could be found?
	if (!netif)
	{
		printf("udp_send: No route ");
		return ERR_RTE;
	}
	return udp_sendto_if(pcb, p, dst_ip, dst_port, netif);
}

/**
 * Send data to a specified address using UDP.
 * The netif used for sending can be specified.
 *
 * This function exists mainly for DHCP, to be able to send UDP packets
 * on a netif that is still down.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 * @param dst_ip Destination IP address.
 * @param dst_port Destination UDP port.
 * @param netif the netif used for sending.
 *
 * dst_ip & dst_port are expected to be in the same byte order as in the pcb.
 *
 * @return lwIP error code (@see udp_send for possible error codes)
 *
 * @see udp_disconnect() udp_send()
 */
int udp_sendto_if(UdpSocket *pcb, PktBuf *p, ip_addr *dst_ip, int dst_port,
		Ethernet *netif)
{
	UdpHeader *udphdr;
	ip_addr *src_ip;
	int err;
	PktBuf *q; // q will be sent down the stack

#if IP_SOF_BROADCAST
	// broadcast filter?
	if ( ((pcb->so_options & SOF_BROADCAST) == 0) && ip_addr_isbroadcast(dst_ip, Ethernet) )
	{
		LWIP_DEBUGF(UDP_DEBUG | 1, ("udp_sendto_if: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
		return ERR_VAL;
	}
#endif // IP_SOF_BROADCAST
	// if the PCB is not yet bound to a port, bind it here
	if (pcb->local_port == 0)
	{
		printf("udp_send: not yet bound to a port, binding now\n");
		err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
		if (err != ERR_OK)
		{
			printf("udp_send: forced port bind failed\n");
			return err;
		}
	}
	// not enough space to add an UDP header to first pbuf in given p chain?
	if (pbuf_header(p, sizeof(UdpHeader)))
	{
		// allocate header in a separate new pbuf
		q = pbuf_alloc(PBUF_IP, sizeof(UdpHeader), PBUF_RAM);
		// new header pbuf could not be allocated?
		if (q == NULL)
		{
			printf("udp_send: could not allocate header\n");
			return ERR_MEM;
		}
		// chain header q in front of given pbuf p
		pbuf_chain(q, p);
		// first pbuf q points to header pbuf
	}
	else
	{
		// adding space for header within p succeeded
		// first pbuf q equals given pbuf
		q = p;
	}
	ASSERT(q->len >= sizeof(UdpHeader));
	// q now represents the packet to be sent
	udphdr = (UdpHeader *) q->payload;
	udphdr->src = htons(pcb->local_port);
	udphdr->dest = htons(dst_port);
	// in UDP, 0 checksum means 'no checksum'
	udphdr->chksum = 0x0000;

	// PCB local address is IP_ANY_ADDR?
	if (ip_addr_isany(&pcb->local_ip))
	{
		// use outgoing network interface IP address as source address
		src_ip = &(netif->ipaddr);
	}
	else
	{
		// check if UDP PCB local IP address is correct: this could be an old
		// address if netif->ip_addr has changed
		if (!ip_addr_cmp(&(pcb->local_ip), &(netif->ipaddr)))
		{
			// local_ip doesn't match, drop the packet
			if (q != p)
			{
				// free the header pbuf
				pbuf_free(q);
				q = NULL;
				// p is still referenced by the caller, and will live on
			}
			return ERR_VAL;
		}
		// use UDP PCB local IP address as source address
		src_ip = &(pcb->local_ip);
	}

	udphdr->len = htons(q->tot_len);
	// calculate checksum
	if ((pcb->flags & UDP_FLAGS_NOCHKSUM) == 0)
	{
		udphdr->chksum = inet_chksum_pseudo(q, src_ip, dst_ip, IP_PROTO_UDP,
				q->tot_len);
		// chksum zero must become 0xffff, as zero means 'no checksum'
		if (udphdr->chksum == 0x0000)
			udphdr->chksum = 0xffff;
	}
	// output to IP
	err = ip_output_if(q, src_ip, dst_ip, pcb->ttl, pcb->tos, IP_PROTO_UDP,
			netif);
	// TODO: must this be increased even if error occured?
	//	snmp_inc_udpoutdatagrams();

	// did we chain a separate header pbuf earlier?
	if (q != p)
	{
		// free the header pbuf
		pbuf_free(q);
		q = NULL;
		// p is still referenced by the caller, and will live on
	}
	return err;
}

/**
 * Connect an UDP PCB.
 *
 * This will associate the UDP PCB with the remote address.
 *
 * @param pcb UDP PCB to be connected with remote address ipaddr and port.
 * @param ipaddr remote IP address to connect with.
 * @param port remote UDP port to connect with.
 *
 * @return lwIP error code
 *
 * ipaddr & port are expected to be in the same byte order as in the pcb.
 *
 * The udp pcb is bound to a random local port if not already bound.
 *
 * @see udp_disconnect()
 */
int udp_connect(UdpSocket *pcb, ip_addr *ipaddr, int port)
{
	UdpSocket *ipcb;

	if (pcb->local_port == 0)
	{
		int err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
		if (err != ERR_OK)
			return err;
	}

	ip_addr_set(&pcb->remote_ip, ipaddr);
	pcb->remote_port = port;
	pcb->flags |= UDP_FLAGS_CONNECTED;
	// TODO: this functionality belongs in upper layers
#ifdef LWIP_UDP_TODO
	// Nail down local IP for netconn_addr()/getsockname()
	if (ip_addr_isany(&pcb->local_ip) && !ip_addr_isany(&pcb->remote_ip))
	{
		struct Ethernet *Ethernet;

		if ((Ethernet = ip_route(&(pcb->remote_ip))) == NULL)
		{
			LWIP_DEBUGF(UDP_DEBUG, ("udp_connect: No route to 0x%lx\n", pcb->remote_ip.addr));
			UDP_STATS_INC(udp.rterr);
			return ERR_RTE;
		}
		TODO: this will bind the udp pcb locally, to the interface which
		is used to route output packets to the remote address. However, we
		might want to accept incoming packets on any interface!
		pcb->local_ip = Ethernet->ip_addr;
	}
	else if (ip_addr_isany(&pcb->remote_ip))
	{
		pcb->local_ip.addr = 0;
	}
#endif
	//	printf("udp_connect: connected to %s, port %d\n",
	//			pcb->remote_ip.ToString(), pcb->remote_port);
	// Insert UDP PCB into the list of active UDP PCBs.
	for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next)
	{
		if (pcb == ipcb)
		{
			// already on the list, just return
			return ERR_OK;
		}
	}
	// PCB not yet on the list, add PCB now
	pcb->next = udp_pcbs;
	udp_pcbs = pcb;
	return ERR_OK;
}

/**
 * Disconnect a UDP PCB
 *
 * @param pcb the udp pcb to disconnect.
 */
void udp_disconnect(UdpSocket *pcb)
{
	// reset remote address association
	ip_addr_set(&pcb->remote_ip, IP_ADDR_ANY);
	pcb->remote_port = 0;
	// mark PCB as unconnected
	pcb->flags &= ~UDP_FLAGS_CONNECTED;
}

/**
 * Set a receive callback for a UDP PCB
 *
 * This callback will be called when receiving a datagram for the pcb.
 *
 * @param pcb the pcb for wich to set the recv callback
 * @param recv function pointer of the callback function
 * @param recv_arg additional argument to pass to the callback function
 */
void udp_recv(UdpSocket *pcb, void(* recv)(void *arg, UdpSocket *upcb,
		PktBuf *p, ip_addr *addr, int port), void *recv_arg)
{
	/* remember recv() callback and user data */
	pcb->recv = recv;
	pcb->recv_arg = recv_arg;
}

/**
 * Create a UDP PCB.
 *
 * @return The UDP PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @see udp_remove()
 */
UdpSocket * udp_new()
{
	UdpSocket * pcb = new UdpSocket();
	/* could allocate UDP PCB? */
	if (pcb != NULL)
	{
		/* UDP Lite: by initializing to all zeroes, chksum_len is set to 0
		 * which means checksum is generated over the whole datagram per default
		 * (recommended as default by RFC 3828). */
		/* initialize PCB to all zeroes */
		memset(pcb, 0, sizeof(UdpSocket));
		pcb->ttl = UDP_TTL;
	}
	return pcb;
}
