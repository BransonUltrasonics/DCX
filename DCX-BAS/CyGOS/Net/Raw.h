// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/Raw.h_v   1.0   21 Mar 2012 12:42:20   PDwivedi  $
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

#ifndef RAW_H_
#define RAW_H_

#include "Net.h"

class Ethernet;

struct PktBuf;
struct ip_addr;

#define RAW_TTL                        255

class RawSocket: public Socket
{
public:
	RawSocket *next;

	int protocol;

	/* receive callback function
	 * @param arg user supplied argument (raw_pcb.recv_arg)
	 * @param pcb the raw_pcb which received data
	 * @param p the packet buffer that was received
	 * @param addr the remote IP address from which the packet was received
	 * @return 1 if the packet was 'eaten' (aka. deleted),
	 *         0 if the packet lives on
	 * If returning 1, the callback is responsible for freeing the pbuf
	 * if it's not used any more.
	 */
	bool (* recv)(void *arg, RawSocket *pcb, PktBuf *p, ip_addr *addr);
	/* user-supplied argument for the recv callback */
	void *recv_arg;
};

RawSocket * raw_new(int proto);
bool recv_raw(void *arg, RawSocket *pcb, PktBuf *p, ip_addr *addr);
void raw_recv(RawSocket *pcb, bool(* recv)(void *arg, RawSocket *upcb,
		PktBuf *p, ip_addr *addr), void *recv_arg);
RawSocket * raw_new(int proto);
void raw_remove(RawSocket *pcb);
int raw_bind(RawSocket *pcb, ip_addr *ipaddr);
int raw_connect(RawSocket *pcb, ip_addr *ipaddr);
int raw_sendto(RawSocket *pcb, PktBuf *p, ip_addr *ipaddr);
int raw_send(RawSocket *pcb, PktBuf *p);
void raw_init();
bool raw_input(PktBuf *p, Ethernet *inp);

#endif
