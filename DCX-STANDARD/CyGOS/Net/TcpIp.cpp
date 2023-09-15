// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/TcpIp.cpp_v   1.2   28 Aug 2011 11:41:38   ygupta  $
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

#include "TcpIp.h"
#include "Ethernet.h"
#include "IPv4.h"
#include "ARP.h"
#include "Raw.h"
#include "TCP.h"
#include "UDP.h"
#include "ICMP.h"
#include "IGMP.h"
#include "Socket.h"
#include "NetConn.h"
#include "assert.h"
#include "DebugNew.h"
#include "RTOS.h"
#include "Event.h"
#include "DhcpCLient.h"
#include "AutoIP.h"
#include "trace.h"

TcpIp * tcpip;
PtrList<CustomProtocol> customProtocols;

TcpIp::TcpIp() :
	Task("TCP/IP", 16384), queue(10000, "TCP/IP")
{
	ASSERT(!tcpip);
	tcpip = this;
	ip_init();
	arp_init();
	raw_init();
	udp_init();
	tcp_init();
	icmp_init();
	igmp_init();
	autoip_init();
	dhcp_init();
	lwip_socket_init();
}

TcpIp::~TcpIp()
{
}

void TcpIp::AddNetIf(Ethernet * eth)
{
	eth->SetQueue(&queue);
}

void TcpIp::Run()
{
	TcpIpMessage message;
	for (;;)
	{
		//TcpIpMessage message;
		queue.Read(this, message, -1);

		switch (message.type)
		{
		case TcpipTick:
			//pprintf("^");
			Tick100ms();
			break;
		case TcpipPacket:
			//pprintf("*");
			arp_input(message.packet, message.netif);
			break;
		case TcpipApiMessage:
			//pprintf("%%");
			message.apimsg->function(&message.apimsg->msg);
			break;
		case TcpipCallbackMessage:
			//pprintf("$");
			message.f(message.ctx);
			break;
		default:
			ASSERT(0);
		}
	}
}

void TcpIp::Tick()
{
	static int timer = 0;
	timer += usecPerTick;
	if (timer >= 100000)
	{
		TcpIpMessage message;
		memset(&message, 0 , sizeof(message));
		message.type = TcpipTick;
		if (queue.Write(0, message, 0))
			timer -= 100000;
	}
}

void TcpIp::Tick100ms()
{
	// once in 100ms
	igmp_tmr();
	autoip_tmr();
	// etharp_tmr every 5 seconds
	static int etharp_timer = 0;
	etharp_timer++;
	if (etharp_timer >= 50)
	{
		etharp_timer = 0;
		etharp_tmr();
	}
	// ip_reass_timer every second
	static int ip_reass_timer = 0;
	ip_reass_timer++;
	if (ip_reass_timer >= 10)
	{
		ip_reass_timer = 0;
		ip_reass_tmr();
	}
	// once in 100ms
	tcp_tmr();
	POSITION pos = customProtocols.GetHeadPosition();
	while (pos)
	{
		CustomProtocol * protocol = customProtocols.GetNext(pos);
		if (protocol->handler->tickHandler)
			protocol->handler->tickHandler();
	}
	static int dhcp_timer_coarse = 0;
	dhcp_timer_coarse += 100;
	if (dhcp_timer_coarse >= ((int)DHCP_COARSE_TIMER_MSECS))
	{
		dhcp_timer_coarse = 0;
		dhcp_coarse_tmr();
	}
	static int dhcp_timer_fine = 0;
	dhcp_timer_fine += 100;
	if (dhcp_timer_fine >= DHCP_FINE_TIMER_MSECS)
	{
		dhcp_timer_fine = 0;
		dhcp_fine_tmr();
	}
}

void TcpIp::SendApiMessage(api_msg * apimsg)
{
	TcpIpMessage message;
	memset(&message, 0 , sizeof(message));
	message.type = TcpipApiMessage;
	message.apimsg = apimsg;
	ASSERT(queue.Write(0, message, 0));
}

/**
 * Call a specific function in the thread context of
 * tcpip_thread for easy access synchronization.
 * A function called in that way may access lwIP core code
 * without fearing concurrent access.
 * Error codes, if any, are returned through the function parameter 'ctx'
 * and this is handled by the code that calls this function.
 *
 * @param f the function to call
 * @param ctx parameter passed to f
 */
void TcpIp::SendCallbackMessage(void(*f)(void *ctx), void *ctx)
{
	TcpIpMessage message;
	memset(&message, 0 , sizeof(message));
	message.type = TcpipCallbackMessage;
	message.f = f;
	message.ctx = ctx;
	queue.Write(RTOS::GetCurrentTask(), message, -1);
}

//plug the protocol using this function
void TcpIp::AddCustomProtocol(uint16 protocolNumber, protocolHandler * handler,
		Ethernet * netif)
{
	POSITION pos = customProtocols.GetHeadPosition();
	while (pos)
	{
		CustomProtocol * protocol = customProtocols.GetNext(pos);
		if (protocol->protocolNumber == protocolNumber)
		{
			//not checking whether already added for this netif
			protocol->handler = handler;//one handler regardless of interfaces
			protocol->netif_list.AddTail(netif);
			return;
		}
	}
	CustomProtocol * protocol = new CustomProtocol;
	protocol->protocolNumber = protocolNumber;
	protocol->handler = handler;
	protocol->netif_list.AddTail(netif);
	customProtocols.AddTail(protocol);
}

/* Initialize all custom protocols: To be called from main()*/
void TcpIp::InitCustom()
{
	POSITION pos = customProtocols.GetHeadPosition();
	while (pos)
	{
		CustomProtocol * protocol = customProtocols.GetNext(pos);
		if (protocol->handler->initHandler)
			protocol->handler->initHandler();
	}
}
