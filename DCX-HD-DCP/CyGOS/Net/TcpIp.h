// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/TcpIp.h_v   1.0.1.0   19 Sep 2014 07:39:42   amaurya  $
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

#ifndef TCPIP_H_
#define TCPIP_H_

#include "Task.h"
#include "Fifo.h"
#include "LinkedList.h"

class Ethernet;

struct PktBuf;
struct api_msg;

enum TcpipMessageType
{
	TcpipTick, TcpipPacket, TcpipApiMessage, TcpipCallbackMessage,
};

struct TcpIpMessage
{
	TcpipMessageType type;
	Ethernet * netif;
	PktBuf * packet;
	api_msg * apimsg;
	void (*f)(void *ctx);
	void *ctx;
};

//typedef init,receive and tick handlers
typedef int (*ProtocolReceiveHandler)(PktBuf *p, Ethernet *netif);
typedef void (*ProtocolInitHandler)(void);
typedef void (*ProtocolTickHandler)(void);

//All handlers will go in one structure
struct protocolHandler
{
	ProtocolReceiveHandler recHandler;
	ProtocolInitHandler initHandler;
	ProtocolTickHandler tickHandler;
};

//The protocol to be added
struct CustomProtocol
{
	protocolHandler * handler; //All handlers (init,tick,receive) of protocol
	uint16 protocolNumber; //protocol to be added in stack
	PtrList<Ethernet> netif_list;//The interfaces where protocol is enabled
};

class TcpIp: public Task
{
public:
	TcpIp();
	virtual ~TcpIp();
	void AddNetIf(Ethernet * eth);
	void SendApiMessage(api_msg * apimsg);
	void SendCallbackMessage(void(*f)(void *ctx), void *ctx);
	void AddCustomProtocol(uint16 protocolNumber, protocolHandler * handler,
			Ethernet * netif);
	void InitCustom();
protected:
	void Run();
	void Tick();
	void Tick100ms();
	Fifo<TcpIpMessage> queue;
};

extern TcpIp * tcpip;
extern PtrList<CustomProtocol> customProtocols;

#endif
