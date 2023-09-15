// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/DhcpServer.h_v   1.0   09 Jun 2015 09:13:50   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines DHCP server object.

#ifndef DHCPSERVER_H_
#define DHCPSERVER_H_

#include "Task.h"
#include "IPv4.h"
#include "Ethernet.h"
#include "DHCP.h"

struct sockaddr_in;
struct DNSTableEntry;

struct DhcpServerConfiguration
{
	uint8 id[4];
	uint16 length;
	uint16 poolSize;
	ip_addr poolStart;
	ip_addr netmask;
	ip_addr gateway;
	ip_addr wins;
	ip_addr dns;
	uint32 leaseTime;
	uint32 renewTime;
	uint32 rebindTime;
};

enum LeaseStatus
{
	Unassigned, Offered, Assigned, Declined,Expired,
};

struct DhcpLease
{
	ip_addr ip;
	int time;
	uint8 chaddr[DHCP_CHADDR_LEN];
	LeaseStatus status;
};

class DhcpServer: public Task
{
	enum Configuration
	{
		DhcpCookie = 0x63825363,
	};
public:
	DhcpServer(Ethernet * eth, DhcpServerConfiguration & defaultConfig);
protected:
	virtual void Tick();
	virtual void Run();
	void HandleDiscover(DhcpMsg & message, sockaddr_in & sa);
	void HandleRequest(DhcpMsg & message, sockaddr_in & sa);
	void HandleInform(DhcpMsg & message, sockaddr_in & sa);
	void HandleDecline(DhcpMsg & message, sockaddr_in & sa);
	void HandleRelease(DhcpMsg & message, sockaddr_in & sa);
	void SendNAK(DhcpMsg & message, sockaddr_in & sa);
	DhcpLease * FindLease(uint8 * chaddr);
	DhcpLease * FindLease();
	void ProcessLeaseTable();
	static void ByteOption(uint8 * &opt, DhcpOption option, int value);
	static void StringOption(uint8 * &opt, DhcpOption option,int len, uint8* String);
	static void LongOption(uint8 * &opt, DhcpOption option, int value);
	static void EndOption(uint8 * &opt);
	void GetClientHostName(DhcpMsg & message);
	void AddDNSEntry(DhcpMsg & message,ip_addr IP);
	void RemoveDNSEntry(DhcpMsg & message, ip_addr IP);
	bool VerifyRequestedIPNetmask(DhcpMsg & message);
	Ethernet * netif;
	int fd;
	int tickTimer;
	DhcpServerConfiguration config;
	DhcpLease * leaseTable;
	uint8 ClientHostName[256];
};

#endif
