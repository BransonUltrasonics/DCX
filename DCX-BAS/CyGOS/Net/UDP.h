// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/UDP.h_v   1.0   21 Mar 2012 12:43:34   PDwivedi  $
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

#ifndef UDP_H_
#define UDP_H_

#include "ArchDefs.h"
#include "Net.h"

class Ethernet;

struct PktBuf;

struct UdpHeader
{
	MisAlignedShort src;
	MisAlignedShort dest; /* src/dest UDP ports */
	MisAlignedShort len;
	MisAlignedShort chksum;
};

class UdpSocket: public Socket
{
public:
	/* Protocol specific PCB members */
	UdpSocket *next;

	int flags;
	/* ports are in host byte order */
	int local_port, remote_port;

	/* outgoing network interface for multicast packets */
	ip_addr multicast_ip;

	/* receive callback function
	 * addr and port are in same byte order as in the pcb
	 * The callback is responsible for freeing the pbuf
	 * if it's not used any more.
	 *
	 * @param arg user supplied argument (udp_pcb.recv_arg)
	 * @param pcb the udp_pcb which received data
	 * @param p the packet buffer that was received
	 * @param addr the remote IP address from which the packet was received
	 * @param port the remote port from which the packet was received
	 */
	void
	(* recv)(void *arg, UdpSocket *pcb, PktBuf *p, ip_addr *addr, int port);
	/* user-supplied argument for the recv callback */
	void *recv_arg;
};

#define UDP_FLAGS_NOCHKSUM 0x01U
#define UDP_FLAGS_CONNECTED  0x04U

#define          udp_flags(pcb)  ((pcb)->flags)
#define          udp_setflags(pcb, f)  ((pcb)->flags = (f))

UdpSocket * udp_new();
void udp_recv(UdpSocket *pcb, void(* recv)(void *arg, UdpSocket *upcb,
		PktBuf *p, ip_addr *addr, int port), void *recv_arg);
void udp_remove(UdpSocket *pcb);
int udp_bind(UdpSocket *pcb, ip_addr *ipaddr, int port);
int udp_connect(UdpSocket *pcb, ip_addr *ipaddr, int port);
void udp_disconnect(UdpSocket *pcb);
int udp_sendto_if(UdpSocket *pcb, PktBuf *p, ip_addr *dst_ip, int dst_port,
		Ethernet *netif);
int udp_sendto(UdpSocket *pcb, PktBuf *p, ip_addr *dst_ip, int dst_port);
int udp_send(UdpSocket *pcb, PktBuf *p);
void udp_recv(UdpSocket *pcb, void(* recv)(void *arg, UdpSocket *upcb,
		PktBuf *p, ip_addr *addr, int port), void *recv_arg);

#define          udp_flags(pcb)  ((pcb)->flags)
#define          udp_setflags(pcb, f)  ((pcb)->flags = (f))

void recv_udp(void *arg, UdpSocket *pcb, PktBuf *p, ip_addr *addr, int port);
void udp_init();
void udp_input(PktBuf *p, Ethernet *inp);

#endif
