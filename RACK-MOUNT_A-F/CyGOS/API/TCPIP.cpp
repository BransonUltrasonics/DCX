 // $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/API/TCPIP.cpp_v   1.1   24 Jul 2015 09:01:48   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements CyGOS TCP/IP API.

#include "CyGOS.h"
#include "RTOS.h"
#include "TcpIp.h"
#include "Board.h"
#include "MII.h"
#include "FEC.h"
#include "Ethernet.h"
#include "TftpClient.h"
#include "DhcpServer.h"
#include "TcpServer.h"
#include "HttpServer.h"
#include "DNSServer.h"
#include "assert.h"


extern TcpIp * tcpip;

const char * CyGOS::GetInterfaceName(int intf)
{
   MII * mii = Board::GetMII(intf);
   if (mii)
      return mii->PHYName;
   return 0;
}
void CyGOS::CreateTcpipTasks(int & priority)
{
	RTOS::Create(new TcpIp(), priority++);
}

void CyGOS::AddTcpInterface(int intf, int & priority, unsigned char * macAddr, bool addtonetiflist)
{
   ASSERT((intf >= 0) && (intf < Board::EthCount()));

   MII * mii = Board::GetMII(intf);
   FEC * fec;
   Ethernet * ethernet;

   fec = new FEC(intf, mii, Board::rmii[intf], macAddr);
   ethernet = new Ethernet(fec, addtonetiflist);

   RTOS::Create(ethernet, priority++);
   tcpip->AddNetIf(ethernet);
   ASSERT(!Board::eth[intf]);
   Board::eth[intf] = ethernet;
}

void CyGOS::SetupInterface(int intf, bool useDhcpIP, bool useAutoIP)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	Board::eth[intf]->SetupInterface(useDhcpIP, useAutoIP);
}
void CyGOS::SetupHostName(int intf, char* HostName)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	//pprintf("\n %s :%s",__FUNCTION__,HostName);
	Board::eth[intf]->SetHostName(HostName);
}
void CyGOS::SetupAddress(int intf, uint32 address, uint32 netmask)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	Board::eth[intf]->AddIP(address, netmask);
}

bool CyGOS::IsNetworkConfigured(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	if (!Board::eth[intf])
		return false;
	return Board::eth[intf]->ipaddr.addr != 0;
}

void CyGOS::SetGW(int intf, uint32 gateway)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	Board::eth[intf] -> SetGW(gateway);
}

void CyGOS::CreateDhcpServerTask(int intf, DhcpServerConfiguration & cfg,
		int & priority)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	RTOS::Create(new DhcpServer(Board::eth[intf], cfg), priority++);
}

void CyGOS::CreateDNSServerTask(int intf, int & priority)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	RTOS::Create(new DNSServer(Board::eth[intf]), priority++);
}

char * CyGOS::GetIPAddressStr(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> ipaddr.ToString();
}

char * CyGOS::GetSubnetMaskStr(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> netmask.ToString();
}

char * CyGOS::GetGatewayAddressStr(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> gw.ToString();
}

bool CyGOS::IsDhcpEnabled(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> useDhcp;
}

ip_addr CyGOS::GetIPAddress(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> ipaddr;
}

ip_addr CyGOS::GetSubnetMask(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> netmask;
}

ip_addr CyGOS::GetGatewayAddress(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> gw;
}

unsigned char * CyGOS::GetMacAddress(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf] -> GetHardwareAddress();
}

Ethernet * CyGOS::GetNetworkInterface(int intf)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	return Board::eth[intf];
}

void CyGOS::SetEthernetLinkDebouncing(bool flag)
{
	int EthCount = Board::EthCount();
	for(int i = 0; i < EthCount; i++)
		Board::eth[i]->SetLinkDebouncing(flag);
}

NetworkConfiguration CyGOS::GetNetworkConfiguration(int intf)
{
	NetworkConfiguration netConfig;
	Ethernet * netif = GetNetworkInterface(intf);
	if(netif -> useDhcp)
		netConfig = configDhcpClient;
	else if(netif -> isRunningDhcpServer)
		netConfig = configDhcpServer;
	else
		netConfig = configStatic;

	return netConfig;
}


