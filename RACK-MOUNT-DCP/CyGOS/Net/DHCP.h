// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/DHCP.h_v   1.0   18 Aug 2014 09:16:26   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines DHCP protocol constants and data structures common to
// both server and client.

#ifndef DHCP_H_
#define DHCP_H_

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67

#define DHCP_CHADDR_LEN 16
#define DHCP_SNAME_LEN 64
#define DHCP_FILE_LEN 128

#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY 2

#define DHCP_MIN_OPTIONS_LEN 68
// First implementation on CyGOS had it 312
#define DHCP_OPTIONS_LEN 312

/** make sure user does not configure this too small */
#if ((defined(DHCP_OPTIONS_LEN)) && (DHCP_OPTIONS_LEN < DHCP_MIN_OPTIONS_LEN))
#  undef DHCP_OPTIONS_LEN
#endif
/** allow this to be configured in lwipopts.h, but not too small */
#if (!defined(DHCP_OPTIONS_LEN))
/** set this to be sufficient for your options in outgoing DHCP msgs */
#  define DHCP_OPTIONS_LEN DHCP_MIN_OPTIONS_LEN
#endif

enum DhcpMsgType
{
	DHCP_ERROR,
	DHCP_DISCOVER,
	DHCP_OFFER,
	DHCP_REQUEST,
	DHCP_DECLINE,
	DHCP_ACK,
	DHCP_NAK,
	DHCP_RELEASE,
	DHCP_INFORM,
};

/** minimum set of fields of any DHCP message */
struct DhcpMsg
{
	/**
	 * op: Message op code / message type. 1 = BOOTREQUEST, 2 = BOOTREPLY
	 */
	uint8 op;
	/**
	 * htype: Hardware address type, see ARP section in "Assigned Numbers"
	 * RFC; e.g., '1' = 10mb Ethernet. Probably should use 6 - IEEE-802.
	 */
	uint8 htype;
	/**
	 * hlen: Hardware address length (e.g. 6 for Ethernet).
	 */
	uint8 hlen;
	/**
	 * hops: Client sets to zero, optionally used by relay agents when
	 * booting via a relay agent.
	 */
	uint8 hops;
	/**
	 * xid: Transaction ID, a random number chosen by the client, used by
	 * the client and server to associate messages and responses between a
	 * client and a server.
	 */
	MisAlignedLong xid;
	/**
	 * secs: Filled in by client, seconds elapsed since client began address
	 * aquisition or renewal process.
	 */
	MisAlignedShort secs;
	/**
	 * flags: 0th bit is the BROADCAST flag rest all should be zero.
	 */
	MisAlignedShort flags;
	/**
	 * ciaddr: Client IP address; only filled in if client is in BOUND,
	 * RENEW or REBINDING state and can respond to ARP requests.
	 */
	ip_addr ciaddr;
	/**
	 * yiaddr: 'your' (client) IP address.
	 */
	ip_addr yiaddr;
	/**
	 * IP address of next server to use in bootstrap; returned in
	 * DHCPOFFER, DHCPACK by server.
	 */
	ip_addr siaddr;
	/**
	 * giaddr: Relay agent IP address, used in booting via a relay agent.
	 */
	ip_addr giaddr;
	/**
	 * chaddr: Client hardware address.
	 */
	uint8 chaddr[DHCP_CHADDR_LEN];
	/**
	 * sname: Optional server host name, null terminated string.
	 */
	uint8 sname[DHCP_SNAME_LEN];
	/**
	 * file: Boot file name, null terminated string; "generic" name or null
	 * in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER.
	 */
	uint8 file[DHCP_FILE_LEN];
	MisAlignedLong cookie;
	/**
	 *  Optional parameters field.
	 */
	uint8 options[DHCP_OPTIONS_LEN];
	int GetType();
	void Print();
};

/* DHCP option and value (cf. RFC1533) */
enum DhcpOption
{
	padOption = 0, // token padding value (make be skipped)
	subnetMask = 1, // subnet mask client should use (4 byte mask)
	timerOffset = 2,
	routersOnSubnet = 3, // routers client should use (IP addr list)
	timeServer = 4, // time servers client should use (IP addr list)
	nameServer = 5, // name servers client should use (IP addr list)
	dns = 6,// DNS servers client should use (IP addr list)
	logServer = 7,
	cookieServer = 8,
	lprServer = 9,
	impressServer = 10,
	resourceLocationServer = 11,
	hostName = 12, // host name client should use (string)
	bootFileSize = 13,
	meritDumpFile = 14,
	domainName = 15, // domain name client should use (string)
	swapServer = 16,
	rootPath = 17,
	extentionsPath = 18,
	IPforwarding = 19,
	nonLocalSourceRouting = 20,
	policyFilter = 21,
	maxDgramReasmSize = 22,
	defaultIPTTL = 23,
	pathMTUagingTimeout = 24,
	pathMTUplateauTable = 25,
	ifMTU = 26,
	allSubnetsLocal = 27,
	broadcastAddr = 28,
	performMaskDiscovery = 29,
	maskSupplier = 30,
	performRouterDiscovery = 31,
	routerSolicitationAddr = 32,
	staticRoute = 33,
	trailerEncapsulation = 34,
	arpCacheTimeout = 35,
	ethernetEncapsulation = 36,
	tcpDefaultTTL = 37,
	tcpKeepaliveInterval = 38,
	tcpKeepaliveGarbage = 39,
	nisDomainName = 40,
	nisServers = 41,
	ntpServers = 42,
	vendorSpecificInfo = 43,
	netBIOSnameServer = 44,
	netBIOSdgramDistServer = 45,
	netBIOSnodeType = 46,
	netBIOSscope = 47,
	xFontServer = 48,
	xDisplayManager = 49,
	dhcpRequestedIPaddr = 50, // IP address requested by client (IP address)
	dhcpIPaddrLeaseTime = 51, // DHCP Lease Time (uint32 seconds)
	dhcpOptionOverload = 52,
	dhcpMessageType = 53, // DHCP message type (1 byte)
	dhcpServerIdentifier = 54, // Server Identifier (IP address)
	dhcpParamRequest = 55, // Paramerter Request List (n OPT codes)
	dhcpMsg = 56,
	dhcpMaxMsgSize = 57,
	dhcpT1value = 58, // DHCP Lease Renewal Time (uint32 seconds)
	dhcpT2value = 59, // DHCP Lease Rebinding Time (uint32 seconds)
	dhcpClassIdentifier = 60,
	dhcpClientIdentifier = 61,
	endOption = 255
// token end value (marks end of options list)
};

#endif
