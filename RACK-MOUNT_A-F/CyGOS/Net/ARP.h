// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/ARP.h_v   1.0   09 Jun 2015 09:13:50   tayars  $
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

#ifndef ARP_H_
#define ARP_H_

#include "ArchDefs.h"
#include "Ethernet.h"
#include "IPv4.h"

struct PktBuf;
struct EthAddr;

struct etharp_hdr
{
	EthHeader ethhdr;
	MisAlignedShort hwtype;
	MisAlignedShort proto;
	MisAlignedShort _hwlen_protolen;
	MisAlignedShort opcode;
	EthAddr shwaddr;
	ip_addr sipaddr;
	EthAddr dhwaddr;
	ip_addr dipaddr;
};

#define ARP_REQUEST 1
#define ARP_REPLY   2

extern const EthAddr ethbroadcast;
extern const EthAddr ethzero;

void arp_init();
int arp_input(PktBuf *p, Ethernet *netif);
void etharp_arp_input(Ethernet *netif, EthAddr *ethaddr, PktBuf *p);
int etharp_send_ip(Ethernet *netif, PktBuf *p, EthAddr *src, EthAddr *dst);
int etharp_send_mb(Ethernet *netif, PktBuf *p, EthAddr *src, EthAddr *dst);
int etharp_output(Ethernet *netif, PktBuf *q, ip_addr *ipaddr);
void etharp_tmr();
int etharp_find_addr(Ethernet *netif, ip_addr *ipaddr, EthAddr **eth_ret,
		ip_addr **ip_ret);
int etharp_query(Ethernet *netif, ip_addr *ipaddr, PktBuf *q);
int etharp_request(Ethernet *netif, ip_addr *ipaddr);
void etharp_gratuitous(Ethernet * netif);
int etharp_raw(Ethernet *netif, const EthAddr *ethsrc_addr,
		const EthAddr *ethdst_addr, const EthAddr *hwsrc_addr,
		const ip_addr *ipsrc_addr, const EthAddr *hwdst_addr,
		const ip_addr *ipdst_addr, uint16 opcode);

#endif
