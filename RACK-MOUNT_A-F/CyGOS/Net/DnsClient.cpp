// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/DnsClient.cpp_v   1.0   09 Jun 2015 09:13:50   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines DNS resolver (client) object.

#include "DnsClient.h"
#include "UDP.h"

ip_addr dns_servers[DNS_MAX_SERVERS];
UdpSocket * dns_pcb;

void dns_setserver(int numdns, ip_addr * dnsserver)
{
	if ((numdns < DNS_MAX_SERVERS) && (dns_pcb != NULL) && (dnsserver != NULL)
			&& (dnsserver->addr != 0))
	{
		dns_servers[numdns] = (*dnsserver);
	}
}
