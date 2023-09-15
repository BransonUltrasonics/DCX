// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/DhcpServer.cpp_v   1.4   08 Oct 2013 05:51:02   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines DHCP server object.

#include "DhcpServer.h"
#include "Ethernet.h"
#include "Socket.h"
#include "DHCP.h"
#include "assert.h"
#include "string.h"
#include "DNSServer.h"
#include "DebugNew.h"
//disable debug
#define printf
#define pprintf

int DhcpMsg::GetType()
{
	uint8 * opt = options;
	while (*opt != endOption)
	{
		int type = *opt++;
		int length = *opt++;
		if (type == dhcpMessageType)
			return *opt;
		opt += length;
	}
	return DHCP_ERROR;
}

void DhcpMsg::Print()
{
//	static char * types[] =
//	{ "Unknown", "Discover", "Offer", "Request", "Decline", "ACK", "NAK",
//			"Release", "Inform" };
	//printf("DHCP packet Type %s\n", types[GetType()]);
	//printf("Op code: %d %s\n", op, op == DHCP_BOOTREQUEST ? "Request" : "Reply");
	//printf("Hardware type: %d %s\n", htype, htype == 1 ? "Ethernet" : "Other");
	//printf("Hardware len: %d\n", hlen);
	//printf("Hops: %d\n", hops);
	//printf("XID: %08X\n", (uint32) xid);
	//printf("Time: %d\n", (uint16) secs);
	//printf("Flags: %04X\n", (uint16) flags);
	//printf("CI: %s\n", ciaddr.ToString());
	//printf("YI: %s\n", yiaddr.ToString());
	//printf("SI: %s\n", siaddr.ToString());
	//printf("GI: %s\n", giaddr.ToString());
	//printf("Client MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", chaddr[0], chaddr[1],
	//		chaddr[2], chaddr[3], chaddr[4], chaddr[5]);
	//printf("Host name: '%s'\n", sname);
	//printf("Boot file: '%s'\n", file);
//	//printf("Options:\n");
//	uint8 * opt = options;
//	while (*opt != endOption)
//	{
////		int type = *opt++;
//		*opt++;
//		int length = *opt++;
//		//printf("\tType %2d, length %2d", type, length);
////		for (int i = 0; i < length; i++)
////		{
//			//printf("%s %02X", i ? "" : ", data", opt[i]);
////		}
//		//printf("\n");
//		opt += length;
//	}
}

// FIXME Since lease table is not in EEPROM, we may restart the DHCP server
// FIXME while some leases are active, and we could give same IP out again...

// FIXME Broadcast replies are sent to potentially wrong netif! UDP socket
// FIXME broadcasts must use their bound interface to send the packet.

DhcpServer::DhcpServer(Ethernet * eth, DhcpServerConfiguration & defaultConfig) :
	Task("DhcpServer", 2048*2)
{
	tickTimer = -1;
	netif = eth;
	memcpy(&config, &defaultConfig, sizeof(config));
	leaseTable = new DhcpLease[config.poolSize];
	memset(leaseTable, 0, sizeof(DhcpLease) * config.poolSize);
	for (int i = 0, j = 0; i < config.poolSize; i++,j++)
	{
		if(config.poolStart.addr == netif->ipaddr.addr)
			j += 1;
		DhcpLease * lease = &leaseTable[i];
		lease->ip = config.poolStart.addr + j;
		lease->status = Unassigned;
	}
	netif->isRunningDhcpServer = true;
}

void DhcpServer::Tick()
{
	if (tickTimer >= 0)
		tickTimer += usecPerTick;
	if (tickTimer >= 1000000)
	{
		tickTimer -= 1000000;
		for (int i = 0; i < config.poolSize; i++)
		{
			DhcpLease * lease = &leaseTable[i];
			if ((lease->status != Unassigned) && (lease->time > 0))
			{
				lease->time--;
			}
			if ((lease->status == Assigned) && (lease->time <= 0))
			{
				lease->status = Expired;
				lease->time = 0;
			}
		}
	}
}
//void UpdateTPVal(int);
void DhcpServer::Run()
{
	DelayMs(1000);
	//printf("DHCP Server running...\n");
	//printf("Pool IP   %s\n", config.poolStart.ToString());
	//printf("Pool size %d\n", config.poolSize);
	//printf("Netmask   %s\n", config.netmask.ToString());
	//printf("Gateway   %s\n", config.gateway.ToString());
	//printf("DNS       %s\n", config.dns.ToString());
	//printf("WINS      %s\n", config.wins.ToString());
	//printf("Lease     %d\n", config.leaseTime);
	//printf("Renew     %d\n", config.renewTime);
	//printf("Rebind    %d\n", config.rebindTime);
	//printf("\n");
	// Now it's time to check that we are not running DHCP server on DHCP-
	// enabled interface...
	ASSERT(!netif->dhcp);
	fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(fd != INVALID_SOCKET);
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(DHCP_SERVER_PORT);
	//printf("\n netif->ipaddr.addr %08X",(int)netif->ipaddr.addr);
	sa.sin_addr.s_addr = netif->ipaddr.addr;
	if (bind(fd, (sockaddr *) &sa, sizeof(sa)) < 0)
		CRASH("Bind DHCP server socket");
	netif->AddDNSEntryforServer();
	for (;;)
	{
		DhcpMsg message;
		int flags = 0;
		int sa_len = sizeof(sa);
		memset(&message, 0, sizeof(message));
		int len = recvfrom(fd, &message, sizeof(message), flags,
				(sockaddr *) &sa, &sa_len);
		if (len < 0)
		{
			DelayMs(10);
			continue;
		}
		if (ntohl(message.cookie) != DhcpCookie){
			continue;
		}
		message.Print();
		int type = message.GetType();
		//GetClientHostName(message);
		switch (type)
		{
		case DHCP_DISCOVER:
			HandleDiscover(message, sa);
			break;
		case DHCP_REQUEST:
			HandleRequest(message, sa);
			break;
		case DHCP_INFORM:
			HandleInform(message, sa);
			break;
		case DHCP_DECLINE:
			HandleDecline(message, sa);
			break;
		case DHCP_RELEASE:
			HandleRelease(message, sa);
			break;
		}
		ProcessLeaseTable();
	}
}

void DhcpServer::HandleDiscover(DhcpMsg & message, sockaddr_in & sa)
{
	// Offer the first IP that we think is available. Also, check for client
	// MAC and XID to see if this is a duplicate discover.
	// First, check all leases for duplicates.
	DhcpLease * lease = FindLease(message.chaddr);
	// Now, if we did not find an active lease for that node, look for leases
	// that are still unassigned.
	if (!lease)
		lease = FindLease();
	// If still no lease, we're fresh out of them. We even checked all
	// unassigned leases.
	// Can't do a thing here, other than probably send a NAK.
	if (!lease)
	{
		SendNAK(message,sa);
		return;
	}
	DhcpMsg response;
	memcpy(&response, &message, sizeof(response));
	response.op = DHCP_BOOTREPLY;
	response.ciaddr = 0;
	response.yiaddr = lease->ip;
	response.siaddr = netif->ipaddr;
	//	printf("\n lease ip = %08X",(int)lease->ip.addr);
	memset(response.options, 0, sizeof(response.options));
	uint8 * opt = response.options;
	// TODO We'll ignore the options that were requested and just send a bunch of them
	ByteOption(opt, dhcpMessageType, DHCP_OFFER);
	LongOption(opt, dhcpServerIdentifier, ntohl(netif->ipaddr.addr));

	LongOption(opt, subnetMask, ntohl(config.netmask.addr));
	LongOption(opt, routersOnSubnet, ntohl(config.gateway.addr));
	LongOption(opt, dns, ntohl(config.dns.addr));
	LongOption(opt, logServer, 0);
	LongOption(opt, netBIOSnameServer, ntohl(config.wins.addr));
	LongOption(opt, dhcpIPaddrLeaseTime, config.leaseTime);
	LongOption(opt, dhcpT1value, config.renewTime);
	LongOption(opt, dhcpT2value, config.rebindTime);
	//StringOption(opt, domainName, strlen("Branson") , (uint8*)"Branson");
	EndOption(opt);
	// Response is ready, but need to tweak destination address first because
	// it should be empty in most cases. Replace empty address with broadcast.
	if (sa.sin_addr.s_addr == 0)
		sa.sin_addr.s_addr = 0xFFFFFFFF;

	sendto(fd, &response, sizeof(response), 0, (sockaddr *) &sa, sizeof(sa));
	lease->status = Offered;
	//Lease expiration timer should be maybe 5-10% higher 
	//than the time that is sent to the client, so that we don't
	//have a situation where lease expired on server before client 
	//could renew it because of clock mismatch.	 
	lease->time = config.leaseTime + (config.leaseTime/10);
	memcpy(lease->chaddr, message.chaddr, sizeof(lease->chaddr));
}

void DhcpServer::HandleRequest(DhcpMsg & message, sockaddr_in & sa)
{
	//	printf("\n %s",__FUNCTION__);
	//netmask of requested IP doesn't match with the one we have
	if(!VerifyRequestedIPNetmask(message))
	{
		SendNAK(message, sa);
		return;
	}

	// Lease was offered, so try to find it...
	DhcpLease * lease = FindLease(message.chaddr);
	// FIXME WTH are we supposed to do if nothing was offered?
	if (!lease)
		lease = FindLease();
	// If still no lease, we're fresh out of them. We even checked all
	// unassigned leases.
	// Can't do a thing here, other than probably send a NAK.
	if (!lease)
	{
		SendNAK(message,sa);
		return;
	}	

	// OK, we got a request and we have a lease, send the ACK reply...
	DhcpMsg response;
	memcpy(&response, &message, sizeof(response));
	response.op = DHCP_BOOTREPLY;
	response.ciaddr = lease->ip;
	response.yiaddr = lease->ip;
	response.siaddr = netif->ipaddr;
	memset(response.options, 0, sizeof(response.options));
	uint8 * opt = response.options;
	// TODO We'll ignore the options that were requested and just send a bunch of them
	ByteOption(opt, dhcpMessageType, DHCP_ACK);
	LongOption(opt, dhcpServerIdentifier, ntohl(netif->ipaddr.addr));
	LongOption(opt, subnetMask, ntohl(config.netmask.addr));
	LongOption(opt, routersOnSubnet, ntohl(config.gateway.addr));
	LongOption(opt, dns, ntohl(config.dns.addr));
	LongOption(opt, logServer, 0);
	LongOption(opt, netBIOSnameServer, ntohl(config.wins.addr));
	LongOption(opt, dhcpIPaddrLeaseTime, config.leaseTime);
	LongOption(opt, dhcpT1value, config.renewTime);
	LongOption(opt, dhcpT2value, config.rebindTime);
	//StringOption(opt, domainName, strlen("Branson") , (uint8*)"Branson");
	EndOption(opt);
	// Response is ready, but need to tweak destination address first because
	// it should be empty in most cases. Replace empty address with broadcast.
	if (sa.sin_addr.s_addr == 0)
		sa.sin_addr.s_addr = 0xFFFFFFFF;

	//	printf("\n \n Calling1 sendto function.");
	sendto(fd, &response, sizeof(response), 0, (sockaddr *) &sa, sizeof(sa));
	lease->status = Assigned;
	//Lease expiration timer should be maybe 5-10% higher 
	//than the time that is sent to the client, so that we don't
	//have a situation where lease expired on server before client 
	//could renew it because of clock mismatch.	 
	lease->time = config.leaseTime + (config.leaseTime/10);
	//pprintf("\n lease IP%04X ", (uint32)lease->ip.addr);
	AddDNSEntry(message, lease->ip);
	memcpy(lease->chaddr, message.chaddr, sizeof(lease->chaddr));
}

void DhcpServer::AddDNSEntry(DhcpMsg & message, ip_addr IP)
{
	GetClientHostName(message);
	if (strlen((const char*)ClientHostName) > 0)
	{
		POSITION pos = netif->DNSTable.GetHeadPosition();
		while (pos)
		{
			DNSTableEntry * DNS = netif->DNSTable.GetNext(pos);
			if (memcmp(DNS->ClientMACAddr, message.chaddr, 6)==0)
			{
				strcpy(DNS->HostName, (const char*)ClientHostName);
				memcpy(DNS->Addr, &IP.addr, sizeof(IP.addr));
				return;
			}
		}
		DNSTableEntry * DNSEntry = new DNSTableEntry;
		memcpy(DNSEntry->ClientMACAddr, message.chaddr, 6);
		strcpy(DNSEntry->HostName, (const char*)ClientHostName);//,strlen((const char*)ClientHostName));
		memcpy(DNSEntry->Addr, &IP.addr, sizeof(IP.addr));
		netif->DNSTable.AddTail(DNSEntry);
	}
}

void DhcpServer::RemoveDNSEntry(DhcpMsg & message, ip_addr IP)
{
	POSITION pos = netif->DNSTable.GetHeadPosition();
	while (pos)
	{
		POSITION last = pos;
		DNSTableEntry * DNS = netif->DNSTable.GetNext(pos);
		if (memcmp(DNS->ClientMACAddr, message.chaddr, 6)==0)
		{
			if (memcmp(DNS->Addr, &IP.addr, sizeof(IP.addr))==0)
				netif->DNSTable.DeleteAt(last);
		}
	}
}
void DhcpServer::HandleInform(DhcpMsg & message, sockaddr_in & sa)
{
	// TODO Inform
   return;
   if( message.GetType() != DHCP_INFORM)
      ASSERT(sa.sin_port);
}

void DhcpServer::HandleDecline(DhcpMsg & message, sockaddr_in & sa)
{
	// Lease was offered, so try to find it...
	DhcpLease * lease = FindLease(message.chaddr);
	if (!lease)
		return;
	//Found a previously assigned lease, Now Release it.
	lease->status = Declined;
	lease->time = 0;
	RemoveDNSEntry(message, lease->ip);
	memset(lease->chaddr, 0, sizeof(lease->chaddr));
	return;
   if( message.GetType() != DHCP_DECLINE)
      ASSERT(sa.sin_port);
}

void DhcpServer::HandleRelease(DhcpMsg & message, sockaddr_in & sa)
{
	// Lease was offered, so try to find it...
	DhcpLease * lease = FindLease(message.chaddr);
	if (!lease)
		return;
	//Found a previously assigned lease, Now Release it.
	lease->status = Unassigned;
	lease->time = 0;
	RemoveDNSEntry(message, lease->ip);
	memset(lease->chaddr, 0, sizeof(lease->chaddr));
   return;
   if( message.GetType() != DHCP_RELEASE)
      ASSERT(sa.sin_port);
}

void DhcpServer::ProcessLeaseTable()
{
	// TODO Process lease table
}
void DhcpServer::StringOption(uint8 * &opt, DhcpOption option, int len,
		uint8* String)
{
	*opt++ = option;
	*opt++ = len;
	for (int x =0; x<len; x++)
		*opt++ = *String++;
}
void DhcpServer::ByteOption(uint8 * &opt, DhcpOption option, int value)
{
	*opt++ = option;
	*opt++ = 1;
	*opt++ = value;
}

void DhcpServer::LongOption(uint8 * &opt, DhcpOption option, int value)
{
	*opt++ = option;
	*opt++ = 4;
	*opt++ = value >> 24;
	*opt++ = value >> 16;
	*opt++ = value >> 8;
	*opt++ = value >> 0;
}

void DhcpServer::EndOption(uint8 * &opt)
{
	*opt++ = endOption;
}

DhcpLease * DhcpServer::FindLease(uint8 * chaddr)
{
	for (int i = 0; i < config.poolSize; i++)
	{
		DhcpLease * lease = &leaseTable[i];
		if (memcmp(lease->chaddr, chaddr, sizeof(lease->chaddr)) == 0)
		{
			return lease;
		}
	}
	return 0;
}

DhcpLease * DhcpServer::FindLease()
{
	//First search for unassigned lease.
	for (int i = 0; i < config.poolSize; i++)
	{
		DhcpLease * lease = &leaseTable[i];
		if (lease->status == Unassigned)
		{
			// Found unassigned one.
			return lease;
		}
	}
	// If still here then search for expired lease. 
	for (int i = 0; i < config.poolSize; i++)
	{
		DhcpLease * lease = &leaseTable[i];
		if (lease->status == Expired)
		{
			// Found expired one.
			return lease;
		}
	}
	return 0;
}

void DhcpServer::GetClientHostName(DhcpMsg & message)
{
	uint8 * opt = message.options;
	memset(&ClientHostName, 0, 256);
	while (*opt != endOption)
	{
		int type = *opt++;
		int length = *opt++;
		if (type == hostName)
		{
			memcpy(ClientHostName, opt, length);
			ClientHostName[length] = '\0';
			return;
		}
		opt += length;
	}
}

void DhcpServer::SendNAK(DhcpMsg & message, sockaddr_in & sa)
{
	// Send the NAK reply...
	DhcpMsg response;
	memcpy(&response, &message, sizeof(response));
	response.op = DHCP_BOOTREPLY;
	memset(response.options, 0, sizeof(response.options));
	uint8 * opt = response.options;
	// TODO We'll ignore the options that were requested and just send a bunch of them
	ByteOption(opt, dhcpMessageType, DHCP_NAK);
	LongOption(opt, dhcpServerIdentifier, ntohl(netif->ipaddr.addr));
	LongOption(opt, subnetMask, ntohl(config.netmask.addr));
	LongOption(opt, routersOnSubnet, ntohl(config.gateway.addr));
	LongOption(opt, dns, ntohl(config.dns.addr));
	LongOption(opt, logServer, 0);
	LongOption(opt, netBIOSnameServer, ntohl(config.wins.addr));
	LongOption(opt, dhcpIPaddrLeaseTime, config.leaseTime);
	LongOption(opt, dhcpT1value, config.renewTime);
	LongOption(opt, dhcpT2value, config.rebindTime);
	EndOption(opt);
	// Response is ready, but need to tweak destination address first because
	// it should be empty in most cases. Replace empty address with broadcast.
	if (sa.sin_addr.s_addr == 0)
		sa.sin_addr.s_addr = 0xFFFFFFFF;
	//	printf("\n \n Calling1 sendto function.");
	sendto(fd, &response, sizeof(response), 0, (sockaddr *) &sa, sizeof(sa));
}


bool DhcpServer::VerifyRequestedIPNetmask(DhcpMsg & message)
{
	bool retVal = true;
	uint8 * opt = message.options;
	unsigned int reqIPAddr = 0;
	while (*opt != endOption)
	{
		int type = *opt++;
		int length = *opt++;
		if(type == dhcpRequestedIPaddr)
		{
			memcpy(&reqIPAddr , opt , length);
		}
		opt += length;
	}
	//pprintf("\n reqIP %08X", reqIPAddr);
	if((reqIPAddr != 0))
	{
		if((reqIPAddr & netif->netmask.addr) != (netif->ipaddr.addr & netif->netmask.addr))
			retVal = false;
	}

	return retVal;

}

