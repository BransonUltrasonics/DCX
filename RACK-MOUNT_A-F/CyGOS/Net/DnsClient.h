// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/DnsClient.h_v   1.0   09 Jun 2015 09:13:50   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines DNS resolver (client) object.

#ifndef DNS_H_
#define DNS_H_

#define LWIP_DNS 1

#define DNS_MAX_SERVERS                 2

struct ip_addr;

void dns_setserver(int numdns, ip_addr * dnsserver);

#endif
