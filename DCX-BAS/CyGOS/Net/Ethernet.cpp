// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/Ethernet.cpp_v   1.7   15 Oct 2013 07:37:38   rjamloki  $
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
//
// This module declares Ethernet interface handler task.

// FIXME When boot loader initializes Ethernet for TFTP, app cannot reinitialize it.

#include "Ethernet.h"
#include "Net.h"
#include "PktBuf.h"
#include "FEC.h"
#include "DebugNew.h"
#include "DhcpClient.h"
#include "AutoIP.h"
#include "MII.h"

PtrList<Ethernet> netif_list;

// FIXME Ethernet can only have one IP/mask.
Ethernet::Ethernet(FEC * fec, bool AddtoNetIfList) :
	Task("Eth", 4096)
{
	driver = fec;
	//	hwType = dllHwEthernet;
	//	driver->SetEthernet(this);
	//	input = ArpInput;
	//	linkoutput = low_level_output;
	//	output = etharp_output;
	// TODO NetIf list
	if(AddtoNetIfList)
		netif_list.AddHead(this);		
	mtu = GetMTU();
	dhcp = 0;
	autoip = 0;
	useDhcp = false;
	useAuto = false;
	isRunningDhcpServer = false;
	up = false;
	ipaddr.addr = 0;
	netmask.addr = MakeIP(255, 255, 255, 0);
	gw.addr = 0;
	memset(HostName, 0, sizeof(HostName));
	LinkCallback = 0;
	MBqueue = 0;
	timer = 0;
}

Ethernet::~Ethernet()
{
	POSITION pos = netif_list.Find(this);
	ASSERT(pos);
	netif_list.RemoveAt(pos);
	CRASH("Ethernet is deleted, need to handle server list as well...");
}

void Ethernet::Run()
{
	DelayMs(driver->GetChannel() * 100 + 10);
	// This is Ethernet main loop
	MBMessage MbMsg;
	TcpIpMessage message;
	for(;;)
	{
		// First, wait till link is active, don't do anything else if it is not
		while (!driver->GetLinkStatus())
		{
			driver->Receive(0, 0);
			DelayMs(100);
		}
		// Got cable plugged in, start initialization process
		driver->Init(false);
		if (useDhcp)
			dhcp_start(this);
		else if (useAuto)
			autoip_start(this);
		else
			netif_set_up(this);
		// And when done, go to main loop
		for (;;)
		{
			// This buffer is big enough for any Ethernet frame
			PktBuf * packet = pbuf_alloc(PBUF_RAW, EthMaxFrame, PBUF_RAM);
			int length = driver->Receive(packet->payload, 100);			
			// If nothing received in 100ms, check the link and go inactive if
			// link is down
			if (!driver->GetLinkStatus())
			{
				printf("Link lost\n");
				pbuf_free(packet);
				break;
			}
			if ((length < EthMinFrame) || (length > EthMaxFrame))
			{
				if (length)
					pprintf("Packet of length %d received\n", length);
				pbuf_free(packet);
				continue;
			}
			EthHeader * e = (EthHeader *) packet->payload;
			packet->len = length;
			packet->tot_len = length;
			if((e->type == ETHTYPE_WCDCP) && (MBqueue)){
				MbMsg.packet = packet;
				if (!MBqueue->Write(0, MbMsg, 0))
				{
					pprintf("MB Queue OV\n");
					pbuf_free(packet);
				}
			}
			else
			{
				memset(&message, 0 , sizeof(message));
				message.type = TcpipPacket;
				message.packet = packet;
				message.netif = this;
				if (!queue->Write(0, message, 0))
				{
					pprintf("Tcpip Queue OV\n");
					pbuf_free(packet);
				}
			}
		}
		// Link is lost, shut down Ethernet driver so that next time we can
		// start over
		driver->Stop();
		netif_set_down(this);
	}
}

// FIXME Implement loopback interface.
int netif_loop_output(Ethernet *netif, PktBuf *p, ip_addr *ipaddr)
{
	printf("netif_loop_output failed\n");
	return ERR_RTE;
	ASSERT(netif);
	ASSERT(p);
	ASSERT(ipaddr);
}

/**
 * 	interface to talk to fec driver.
 * 	@param p : packet buffer to be sent
 */
int Ethernet::low_level_output(PktBuf *p)
{
	/* initiate transfer(); */
	if (p->tot_len > (EthMaxFrame - EthCrcLen))
		return ERR_BUF;

	/* signal that packet should be sent(); */
	if (!driver->Transmit(p, p->tot_len))
		return ERR_BUF;

	return ERR_OK;
}

uint8 * Ethernet::GetHardwareAddress() const
{
	return (uint8 *) driver->GetMacAddress();
}

void Ethernet::SetLinkDebouncing(bool EnableDebounce)
{
	if(driver)
		driver->EnableDebounceLinkStatus = EnableDebounce;
}


bool HasLocalIP(ip_addr ipaddr)
{
	POSITION pos = netif_list.GetHeadPosition();
	Ethernet  * eth;
	while (pos)
	{
		eth = netif_list.GetNext(pos);
		if(eth->ipaddr.addr == ipaddr.addr)
			return eth->up;
	}
	return false;
}

void AddToNetIfList(Ethernet * NetIf)
{
	bool Add = true;
	POSITION pos = netif_list.GetHeadPosition();
	Ethernet  * eth;
	while (pos)
	{
		eth = netif_list.GetNext(pos);
		if(eth == NetIf){
			Add = false;
			break;
		}
	}
	if(Add){
		pprintf("\n Add %s", NetIf->ipaddr.ToString());
		CriticalSection cs;
		netif_list.AddHead(NetIf);
	}
}

void RemoveFromNetIfList(Ethernet * NetIf)
{
	pprintf("\n remove %s", NetIf->ipaddr.ToString());
	POSITION pos = netif_list.Find(NetIf);
	ASSERT(pos);
	CriticalSection cs;
	netif_list.RemoveAt(pos);
}

void Ethernet::Tick()
{
	timer += usecPerTick;
	if (timer >= 100000)
	{
		driver->rxQueue.Write(0,0,0);
		timer -= 100000;
	}

}
