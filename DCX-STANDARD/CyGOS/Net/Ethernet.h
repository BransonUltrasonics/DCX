// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/Ethernet.h_v   1.4   16 Sep 2011 07:44:18   RatneshA  $
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

// FIXME DNS server is too tightly coupled into this code!

#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "Task.h"
#include "Fifo.h"
#include "LinkedList.h"
#include "TcpIp.h"
#include "IPv4.h"
#include "TcpServer.h"
#include "DNSServer.h"
#include "Board.h"

class FEC;

struct PktBuf;
struct DHCP;
struct AutoIP;

/**
 * enum EthernetConfiguration: Ethernet configuration constants.
 */
enum EthernetConfiguration
{
	/**
	 * Ethernet MAC address length.
	 */
	EthAddrLen = 6,
	/**
	 * EthTypeLen: Ethernet frame type length, 2 bytes. This 16-bit value
	 * specifies which Network Layer protocol must handle the frame, such as
	 * IPv4, IPv6, ARP, etc.
	 */
	EthTypeLen = 2,
	/**
	 * EthCrcLen: Ethernet frame CRC length, 4 bytes or 32 bits. Function to
	 * calculate CRC is defined in CRC.h (CalcCrc32_EDB88320).
	 */
	EthCrcLen = 4,
	/**
	 * EthMaxData: Maximum data length in one Ethernet frame. This value does
	 * not include Ethernet header or CRC.
	 */
	EthMaxData = 1500,
	/**
	 * EthMinData: Minimum data length in one Ethernet frame. This value does
	 * not include Ethernet header or CRC.
	 */
	EthMinData = 46,
	/**
	 * EthHeaderLen: Ethernet header length, sum of sizes of two MAC addresses
	 * and  the frame type field.
	 */
	EthHeaderLen = EthAddrLen * 2 + EthTypeLen,
	/**
	 * EthMaxFrame: Maximum Ethernet frame length, including header and CRC.
	 */
	EthMaxFrame = EthHeaderLen + EthMaxData + EthCrcLen,
	/**
	 * EthMinFrame: Minimum Ethernet frame length, including header and CRC.
	 */
	EthMinFrame = EthHeaderLen + EthMinData + EthCrcLen,
	/**
	 * EthMTU: Ethernet Maximum Transfer Unit size, same as EthMaxData.
	 */
	EthMTU = EthMaxData,
};

struct EthAddr
{
	uint8 addr[EthAddrLen];
};

struct EthHeader
{
	EthAddr dest;
	EthAddr src;
	MisAlignedShort type;
};

#define ETHTYPE_ARP       0x0806
#define ETHTYPE_IP        0x0800
#define ETHTYPE_ARTS      0xC605

class Ethernet: public Task
{
public:
	Ethernet(FEC * fec);
	virtual ~Ethernet();
	void SetQueue(Fifo<TcpIpMessage> * q)
	{
		queue = q;
	}
	int GetMTU() const
	{
		return EthMaxData;
	}
	int mtu;
	ip_addr ipaddr;
	ip_addr netmask;
	ip_addr gw;
	char HostName[256];
	int (*igmp_mac_filter)(Ethernet *netif, ip_addr *group, int action);
	uint8 * GetHardwareAddress() const;
	virtual int low_level_output(PktBuf *p);
	void SetHostName(char * Host)
	{
		strcpy(HostName, Host);
	}
	void AddIP(uint32 ip, uint32 nm)
	{
		ipaddr.addr = ip;
		netmask.addr = nm;
	}
	void SetGW(uint32 gateway)
	{
		gw.addr = gateway;
	}
	void SetupInterface(bool useDhcpIP, bool useAutoIP)
	{
		useDhcp = useDhcpIP;
		useAuto = useAutoIP;
	}
	void AddDNSEntryforServer()
	{
		// Add DNS server entry in DNS table.
		DNSTableEntry * DNSEntry = new DNSTableEntry;
		memcpy(DNSEntry->ClientMACAddr,GetHardwareAddress(),6);
		strcpy(DNSEntry->HostName,(const char*)HostName);//,strlen((const char*)ClientHostName));
		memcpy(DNSEntry->Addr,&ipaddr.addr,sizeof(ipaddr.addr));
		DNSTable.AddTail(DNSEntry);
	}
	void AddTcpServer(TcpServer * server);
	PtrList<TcpServer> servers;
	DHCP * dhcp;
	PtrList<DNSTableEntry> DNSTable;
	AutoIP * autoip;
	bool up;
	bool useDhcp;
	bool isRunningDhcpServer;
	void (*LinkCallback)(Ethernet * netif);
protected:
	void Run();
	static void TcpServerLinkCallback(Ethernet * netif);
	FEC * driver;
	Fifo<TcpIpMessage> * queue;
	bool useAuto;
};

extern PtrList<Ethernet> netif_list;
int netif_loop_output(Ethernet *netif, PktBuf *p, ip_addr *ipaddr);

void netif_set_up(Ethernet * netif);
void netif_set_down(Ethernet * netif);
bool HasLocalIP(ip_addr ipaddr);
inline bool eth_addr_cmp(void * a1, void * a2)
{
	return memcmp(a1, a2, sizeof(EthAddr)) == 0;
}

#endif
