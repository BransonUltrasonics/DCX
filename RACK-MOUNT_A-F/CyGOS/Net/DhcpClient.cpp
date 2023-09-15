// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/DhcpClient.cpp_v   1.1   24 Jul 2015 08:51:04   sesharma  $
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

#include "DhcpClient.h"
#include "Ethernet.h"
#include "UDP.h"
#include "PktBuf.h"
#include "DnsClient.h"
#include "ARP.h"
#include "IGMP.h"
#include "AutoIP.h"
#include "assert.h"
#include "DebugNew.h"

#define DHCP_MAX_MSG_LEN(netif)        (netif->mtu)

void netif_set_up(Ethernet * netif)
{
	if (!netif->up)
	{
		netif->up = true;

		printf("\nnetif_set_up\n");
		printf("IP: %s\n", netif->ipaddr.ToString());
		printf("NM: %s\n", netif->netmask.ToString());
		printf("GW: %s\n", netif->gw.ToString());
		printf("\n");

		if (netif->LinkCallback)
			netif->LinkCallback(netif);
		etharp_gratuitous(netif);
		// resend IGMP memberships
		igmp_report_groups(netif);
	}
}

void netif_set_down(Ethernet *netif)
{
	pprintf("netif->up%d",netif->up);
	if (netif->up)
	{
		netif->up = false;
		printf("\nnetif_set_down\n\n");
		if (netif->LinkCallback)
			netif->LinkCallback(netif);
		if(netif->useDhcp)//don't reset in DHCP server and static IP case
		{
			netif->ipaddr.addr = 0;
//			netif->netmask.addr = 0;
			netif->gw.addr = 0;
		}
	}
	pprintf("netif->up%d",netif->up);
}

void dhcp_init()
{
}

void dhcp_start(Ethernet * netif)
{
	// check MTU of the netif
	if (netif->mtu < DHCP_MAX_MSG_LEN_MIN_REQUIRED)
	{
		printf("dhcp_start(): Cannot use this netif with DHCP:"
			" MTU is too small\n");
		return;
	}

	DHCP * dhcp = netif->dhcp;
	// no DHCP client attached yet?
	if (!dhcp)
	{
		dhcp = new DHCP();
		netif->dhcp = dhcp;
	}
	else
	{
		if (dhcp->pcb)
			udp_remove(dhcp->pcb);
		ASSERT(!dhcp->p_out);
		ASSERT(!dhcp->msg_in);
		ASSERT(!dhcp->options_in);
		ASSERT(!dhcp->options_in_len);
	}

	// clear data structure
	memset(dhcp, 0, sizeof(DHCP));
	// allocate UDP PCB
	dhcp->pcb = udp_new();
#if IP_SOF_BROADCAST
	dhcp->pcb->so_options|=SOF_BROADCAST;
#endif
	// set up local and remote port for the pcb
	udp_bind(dhcp->pcb, IP_ADDR_ANY, DHCP_CLIENT_PORT);
	udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
	// set up the recv callback and argument
	udp_recv(dhcp->pcb, dhcp_recv, netif);
	// (re)start the DHCP negotiation
	dhcp_discover(netif);
}

void dhcp_discover(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	ip_addr_set(&dhcp->offered_ip_addr, IP_ADDR_ANY);
	dhcp_set_state(dhcp, DHCP_SELECTING);
	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_DISCOVER);
	dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
	dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));
#if LWIP_NETIF_HOSTNAME
	if (strlen(netif->HostName) > 0)
	{
		dhcp_option(dhcp, hostName, strlen(netif->HostName));
		dhcp_option_string(dhcp, netif->HostName, strlen(netif->HostName));
	}
	//dhcp_option(dhcp, domainName, strlen(testDomainname));
	//dhcp_option_string(dhcp, testDomainname, strlen(testDomainname));
#endif
	dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, 4);
	dhcp_option_byte(dhcp, DHCP_OPTION_SUBNET_MASK);
	dhcp_option_byte(dhcp, DHCP_OPTION_ROUTER);
	dhcp_option_byte(dhcp, DHCP_OPTION_BROADCAST);
	dhcp_option_byte(dhcp, DHCP_OPTION_DNS_SERVER);
	dhcp_option_trailer(dhcp);

	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);
	udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT,
			netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	if (dhcp->tries >= LWIP_DHCP_AUTOIP_COOP_TRIES && dhcp->autoip_coop_state
			== DHCP_AUTOIP_COOP_STATE_OFF)
	{
		dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_ON;
		autoip_start(netif);
	}
	int msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
}

void dhcp_set_state(DHCP * dhcp, DhcpState new_state)
{
	if (new_state != dhcp->state)
	{
		dhcp->state = new_state;
		dhcp->tries = 0;
	}
}

void dhcp_create_request(Ethernet * netif)
{
#ifndef DHCP_GLOBAL_XID
	// default global transaction identifier starting value (easy to match
	//  with a packet analyser). We simply increment for each new request.
	//  Predefine DHCP_GLOBAL_XID to a better value or a function call to generate one
	//  at runtime, any supporting function prototypes can be defined in DHCP_GLOBAL_XID_HEADER
	static uint32 xid = 0xABCD0123;
#else
	static uint32 xid;
	static u8_t xid_initialised = 0;
	if (!xid_initialised)
	{
		xid = DHCP_GLOBAL_XID;
		xid_initialised = !xid_initialised;
	}
#endif
	DHCP * dhcp = netif->dhcp;
	ASSERT(dhcp);
	ASSERT(!dhcp->p_out);
	ASSERT(!dhcp->msg_out);
	dhcp->p_out = pbuf_alloc(PBUF_TRANSPORT, sizeof(DhcpMsg), PBUF_RAM);
	ASSERT(dhcp->p_out->len >= sizeof(DhcpMsg));

	// reuse transaction identifier in retransmissions
	if (dhcp->tries == 0)
		xid++;
	dhcp->xid = xid;

	dhcp->msg_out = (DhcpMsg *) dhcp->p_out->payload;
	DhcpMsg * out = dhcp->msg_out;
	out->op = DHCP_BOOTREQUEST;
	// TODO: make link layer independent
	out->htype = DHCP_HTYPE_ETH;
	// TODO: make link layer independent
	out->hlen = DHCP_HLEN_ETH;
	out->hops = 0;
	out->xid = htonl(dhcp->xid);
	out->secs = 0;
	out->flags = 0;
	out->ciaddr.addr = 0;
	if (dhcp->state == DHCP_BOUND || dhcp->state == DHCP_RENEWING
			|| dhcp->state == DHCP_REBINDING)
	{
		out->ciaddr.addr = netif->ipaddr.addr;
	}
	out->yiaddr.addr = 0;
	out->siaddr.addr = 0;
	out->giaddr.addr = 0;
	memset(out->chaddr, 0, DHCP_CHADDR_LEN);
	memcpy(out->chaddr, netif->GetHardwareAddress(), EthAddrLen);
	memset(out->sname, 0, DHCP_SNAME_LEN);
	memset(out->file, 0, DHCP_FILE_LEN);
	out->cookie = htonl(0x63825363UL);
	dhcp->options_out_len = 0;
	memset(out->options, 0, DHCP_OPTIONS_LEN);
}

void dhcp_option(DHCP * dhcp, int option_type, int option_len)
{
	ASSERT(dhcp->options_out_len + 2U + option_len <= DHCP_OPTIONS_LEN);
	dhcp->msg_out->options[dhcp->options_out_len++] = option_type;
	dhcp->msg_out->options[dhcp->options_out_len++] = option_len;
}

void dhcp_option_byte(DHCP * dhcp, int value)
{
	ASSERT(dhcp->options_out_len < DHCP_OPTIONS_LEN);
	dhcp->msg_out->options[dhcp->options_out_len++] = value;
}

void dhcp_option_short(DHCP * dhcp, int value)
{
	ASSERT(dhcp->options_out_len + 2U <= DHCP_OPTIONS_LEN);
	dhcp->msg_out->options[dhcp->options_out_len++] = (value & 0xFF00u) >> 8;
	dhcp->msg_out->options[dhcp->options_out_len++] = (value & 0x00FFu);
}

void dhcp_option_long(DHCP * dhcp, long value)
{
	ASSERT(dhcp->options_out_len + 4U <= DHCP_OPTIONS_LEN);
	dhcp->msg_out->options[dhcp->options_out_len++] = ((value & 0xff000000UL)
			>> 24);
	dhcp->msg_out->options[dhcp->options_out_len++] = ((value & 0x00ff0000UL)
			>> 16);
	dhcp->msg_out->options[dhcp->options_out_len++] = ((value & 0x0000ff00UL)
			>> 8);
	dhcp->msg_out->options[dhcp->options_out_len++] = ((value & 0x000000ffUL));
}
void dhcp_option_string(DHCP * dhcp, char *string, int len)
{
	ASSERT(dhcp->options_out_len + len <= DHCP_OPTIONS_LEN);
	for (int i = 0; i < len; i++)
		dhcp->msg_out->options[dhcp->options_out_len++] = *string++;
}

void dhcp_option_trailer(DHCP * dhcp)
{
	ASSERT(dhcp->msg_out);
	ASSERT( dhcp->options_out_len < DHCP_OPTIONS_LEN);
	dhcp->msg_out->options[dhcp->options_out_len++] = DHCP_OPTION_END;
	// packet is too small, or not 4 byte aligned?
	while ((dhcp->options_out_len < DHCP_MIN_OPTIONS_LEN)
			|| (dhcp->options_out_len & 3))
	{
		ASSERT(dhcp->options_out_len < DHCP_OPTIONS_LEN);
		// add a fill/padding byte
		dhcp->msg_out->options[dhcp->options_out_len++] = 0;
	}
}

void dhcp_delete_request(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	ASSERT(dhcp);
	ASSERT(dhcp->p_out);
	ASSERT(dhcp->msg_out);
	pbuf_free(dhcp->p_out);
	dhcp->p_out = 0;
	// Don't delete msg_out since it's a pointer into the PktBuf
	dhcp->msg_out = 0;
}

void dhcp_recv(void *arg, UdpSocket * pcb, PktBuf *p, ip_addr * addr, int port)
{
	Ethernet * netif = (Ethernet *) arg;
	DHCP * dhcp = netif->dhcp;
	DhcpMsg *reply_msg = (DhcpMsg *) p->payload;
	int msg_type;
	uint8 * options_ptr;

	ASSERT(dhcp->msg_in == NULL && dhcp->options_in == NULL && dhcp->options_in_len == 0);

	if (p->len < DHCP_MIN_REPLY_LEN)
	{
		printf("DHCP reply message too short\n");
		goto free_pbuf_and_return;
	}

	if (reply_msg->op != DHCP_BOOTREPLY)
	{
		printf("not a DHCP reply message, but type %d\n", reply_msg->op);
		goto free_pbuf_and_return;
	}
	// iterate through hardware address and match against DHCP message
	if (memcmp(netif->GetHardwareAddress(), reply_msg->chaddr, EthAddrLen) != 0)
	{
		goto free_pbuf_and_return;
	}
	// match transaction ID against what we expected
	if (ntohl(reply_msg->xid) != dhcp->xid)
	{
		printf("transaction id mismatch reply_msg->xid(%08X) != "
			"dhcp->xid(%08X)\n", (unsigned int)ntohl(reply_msg->xid), (unsigned int)dhcp->xid);
		goto free_pbuf_and_return;
	}
	// option fields could be unfold?
	dhcp_unfold_reply(dhcp, p);

	// obtain pointer to DHCP message type
	options_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_MESSAGE_TYPE);
	if (options_ptr == NULL)
	{
		printf("DHCP_OPTION_MESSAGE_TYPE option not found\n");
		goto free_pbuf_and_return;
	}
	// read DHCP message type
	msg_type = dhcp_get_option_byte(options_ptr + 2);
	// message type is DHCP ACK?
	if (msg_type == DHCP_ACK)
	{
		//		printf("DHCP_ACK received\n");
		// in requesting state?
		if (dhcp->state == DHCP_REQUESTING)
		{
			dhcp_handle_ack(netif);
			dhcp->request_timeout = 0;
			// TODO ARP check is not working...
#if !DHCP_DOES_ARP_CHECK
			// check if the acknowledged lease address is already in use
			dhcp_check(netif);
#else
			// bind interface to the acknowledged lease address
			dhcp_bind(netif);
#endif
		}
		// already bound to the given lease address?
		else if ((dhcp->state == DHCP_REBOOTING) || (dhcp->state
				== DHCP_REBINDING) || (dhcp->state == DHCP_RENEWING))
		{
			dhcp->request_timeout = 0;
			dhcp_bind(netif);
		}
	}
	// received a DHCP_NAK in appropriate state?
	else if ((msg_type == DHCP_NAK) && ((dhcp->state == DHCP_REBOOTING)
			|| (dhcp->state == DHCP_REQUESTING) || (dhcp->state
			== DHCP_REBINDING) || (dhcp->state == DHCP_RENEWING)))
	{
		printf("DHCP_NAK received\n");
		dhcp->request_timeout = 0;
		dhcp_handle_nak(netif);
	}
	// received a DHCP_OFFER in DHCP_SELECTING state?
	else if ((msg_type == DHCP_OFFER) && (dhcp->state == DHCP_SELECTING))
	{
		//		printf("DHCP_OFFER received in DHCP_SELECTING state\n");
		dhcp->request_timeout = 0;
		// remember offered lease
		dhcp_handle_offer(netif);
	}
	free_pbuf_and_return: dhcp_free_reply(dhcp);
	pbuf_free(p);
	return;
	addr = 0;
	pcb = 0;
	port = 0;
}

void dhcp_unfold_reply(DHCP * dhcp, PktBuf * p)
{
	ASSERT(dhcp);
	// free any left-overs from previous unfolds
	dhcp_free_reply(dhcp);
	// options present?
	if (p->tot_len > (sizeof(DhcpMsg) - DHCP_OPTIONS_LEN))
	{
		dhcp->options_in_len = p->tot_len
				- (sizeof(DhcpMsg) - DHCP_OPTIONS_LEN);
		dhcp->options_in = new char[dhcp->options_in_len];
	}
	dhcp->msg_in = (DhcpMsg *) new char[sizeof(DhcpMsg) - DHCP_OPTIONS_LEN];
	// copy the DHCP message without options
	int ret = pbuf_copy_partial(p, dhcp->msg_in, sizeof(DhcpMsg)
			- DHCP_OPTIONS_LEN, 0);
	ASSERT(ret == sizeof(DhcpMsg) - DHCP_OPTIONS_LEN);
	if (dhcp->options_in)
	{
		// copy the DHCP options
		ret = pbuf_copy_partial(p, dhcp->options_in, dhcp->options_in_len,
				sizeof(DhcpMsg) - DHCP_OPTIONS_LEN);
		ASSERT(ret == dhcp->options_in_len);
	}
}

void dhcp_free_reply(DHCP * dhcp)
{
	if (dhcp->msg_in)
	{
		delete dhcp->msg_in;
		dhcp->msg_in = NULL;
	}
	if (dhcp->options_in)
	{
		delete dhcp->options_in;
		dhcp->options_in = NULL;
		dhcp->options_in_len = 0;
	}
}

uint8 * dhcp_get_option_ptr(DHCP * dhcp, int option_type)
{
	int overload = DHCP_OVERLOAD_NONE;

	// options available?
	if ((dhcp->options_in != NULL) && (dhcp->options_in_len > 0))
	{
		// start with options field
		uint8 *options = (uint8 *) dhcp->options_in;
		int offset = 0;
		// at least 1 byte to read and no end marker, then at least 3 bytes to read?
		while ((offset < dhcp->options_in_len) && (options[offset]
				!= DHCP_OPTION_END))
		{
			// LWIP_DEBUGF(DHCP_DEBUG, ("msg_offset=%"U16_F", q->len=%"U16_F, msg_offset, q->len));
			// are the sname and/or file field overloaded with options?
			if (options[offset] == DHCP_OPTION_OVERLOAD)
			{
				//				printf("overloaded message detected\n");
				// skip option type and length
				offset += 2;
				overload = options[offset++];
			}
			// requested option found
			else if (options[offset] == option_type)
			{
				//				printf("option found at offset %d in options\n", offset);
				return &options[offset];
				// skip option
			}
			else
			{
				//				printf("skipping option %d in options\n", options[offset]);
				// skip option type
				offset++;
				// skip option length, and then length bytes
				offset += 1 + options[offset];
			}
		}
		// is this an overloaded message?
		if (overload != DHCP_OVERLOAD_NONE)
		{
			int field_len;
			if (overload == DHCP_OVERLOAD_FILE)
			{
				//				printf("overloaded file field\n");
				options = (uint8 *) &dhcp->msg_in->file;
				field_len = DHCP_FILE_LEN;
			}
			else if (overload == DHCP_OVERLOAD_SNAME)
			{
				//				printf("overloaded sname field\n");
				options = (uint8 *) &dhcp->msg_in->sname;
				field_len = DHCP_SNAME_LEN;
				// TODO: check if else if () is necessary
			}
			else
			{
				//				printf("overloaded sname and file field\n");
				options = (uint8 *) &dhcp->msg_in->sname;
				field_len = DHCP_FILE_LEN + DHCP_SNAME_LEN;
			}
			offset = 0;

			// at least 1 byte to read and no end marker
			while ((offset < field_len) && (options[offset] != DHCP_OPTION_END))
			{
				if (options[offset] == option_type)
				{
					//					printf("option found at offset=%d\n", offset);
					return &options[offset];
					// skip option
				}
				else
				{
					//					printf("skipping option %d\n", options[offset]);
					// skip option type
					offset++;
					if(offset < field_len)
						offset += 1 + options[offset];
					else
						return NULL;
				}
			}
		}
	}
	return NULL;
}

int dhcp_get_option_byte(uint8 *ptr)
{
	//	printf("option byte value=%d\n", *ptr);
	return *ptr;
}

void dhcp_handle_nak(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	printf("dhcp_handle_nak(netif=%p) %s\n", netif, netif->GetName());
	// Set the interface down since the address must no longer be used, as per RFC2131
	netif_set_down(netif);
	// remove IP address from interface
	ip_addr_set(&netif->ipaddr, IP_ADDR_ANY);
	ip_addr_set(&netif->gw, IP_ADDR_ANY);
	ip_addr_set(&netif->netmask, IP_ADDR_ANY);
	// Change to a defined state
	dhcp_set_state(dhcp, DHCP_BACKING_OFF);
	// We can immediately restart discovery
	dhcp_discover(netif);
}

void dhcp_handle_ack(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	// clear options we might not get from the ACK
	dhcp->offered_sn_mask.addr = 0;
	dhcp->offered_gw_addr.addr = 0;
	dhcp->offered_bc_addr.addr = 0;

	// lease time given?
	uint8 * option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_LEASE_TIME);
	if (option_ptr != NULL)
	{
		// remember offered lease time
		dhcp->offered_t0_lease = dhcp_get_option_long(option_ptr + 2);
	}
	// renewal period given?
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_T1);
	if (option_ptr != NULL)
	{
		// remember given renewal period
		dhcp->offered_t1_renew = dhcp_get_option_long(option_ptr + 2);
	}
	else
	{
		// calculate safe periods for renewal
		dhcp->offered_t1_renew = dhcp->offered_t0_lease / 2;
	}
	// renewal period given?
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_T2);
	if (option_ptr != NULL)
	{
		// remember given rebind period
		dhcp->offered_t2_rebind = dhcp_get_option_long(option_ptr + 2);
	}
	else
	{
		// calculate safe periods for rebinding
		dhcp->offered_t2_rebind = dhcp->offered_t0_lease;
	}
	// (y)our internet address
	ip_addr_set(&dhcp->offered_ip_addr, &dhcp->msg_in->yiaddr);
	// subnet mask
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_SUBNET_MASK);
	// subnet mask given?
	if (option_ptr != NULL)
	{
		dhcp->offered_sn_mask.addr
				= htonl(dhcp_get_option_long(&option_ptr[2]));
	}
	// gateway router
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_ROUTER);
	if (option_ptr != NULL)
	{
		dhcp->offered_gw_addr.addr
				= htonl(dhcp_get_option_long(&option_ptr[2]));
	}
	// broadcast address
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_BROADCAST);
	if (option_ptr != NULL)
	{
		dhcp->offered_bc_addr.addr
				= htonl(dhcp_get_option_long(&option_ptr[2]));
	}
	// DNS servers
	option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_DNS_SERVER);
	if (option_ptr != NULL)
	{
		dhcp->dns_count = dhcp_get_option_byte(&option_ptr[1])
				/ (uint32) sizeof(ip_addr);
		// limit to at most DHCP_MAX_DNS DNS servers
		if (dhcp->dns_count > DHCP_MAX_DNS)
			dhcp->dns_count = DHCP_MAX_DNS;
		uint32 n;
		for (n = 0; n < dhcp->dns_count; n++)
		{
			dhcp->offered_dns_addr[n].addr = htonl(dhcp_get_option_long(
					&option_ptr[2 + n * 4]));
#if LWIP_DNS
			dns_setserver(n, (ip_addr *) (&(dhcp->offered_dns_addr[n].addr)));
#endif
		}
#if LWIP_DNS
		dns_setserver(n, (ip_addr *) (&ip_addr_any));
#endif
	}
}

uint32 dhcp_get_option_long(uint8 *ptr)
{
	uint32 value;
	value = (uint32) (*ptr++) << 24;
	value |= (uint32) (*ptr++) << 16;
	value |= (uint32) (*ptr++) << 8;
	value |= (uint32) (*ptr++);
	//	printf("option long value=%d\n", value);
	return value;
}

void dhcp_bind(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	ASSERT(dhcp);
	// temporary DHCP lease?
	if (dhcp->offered_t1_renew != 0xFFFFFFFFul)
	{
		// set renewal period timer
		uint32 timeout = (dhcp->offered_t1_renew + DHCP_COARSE_TIMER_SECS / 2)
				/ DHCP_COARSE_TIMER_SECS;
		if (timeout > 0xffff)
		{
			timeout = 0xffff;
		}
		dhcp->t1_timeout = timeout;
		if (dhcp->t1_timeout == 0)
		{
			dhcp->t1_timeout = 1;
		}
	}
	// set renewal period timer
	if (dhcp->offered_t2_rebind != 0xFFFFFFFFul)
	{
		uint32 timeout = (dhcp->offered_t2_rebind + DHCP_COARSE_TIMER_SECS / 2)
				/ DHCP_COARSE_TIMER_SECS;
		if (timeout > 0xffff)
		{
			timeout = 0xffff;
		}
		dhcp->t2_timeout = timeout;
		if (dhcp->t2_timeout == 0)
		{
			dhcp->t2_timeout = 1;
		}
	}
	ip_addr sn_mask, gw_addr;
	// copy offered network mask
	ip_addr_set(&sn_mask, &dhcp->offered_sn_mask);
	// subnet mask not given?
	// TODO: this is not a valid check. what if the network mask is 0?
	if (sn_mask.addr == 0)
	{
		// choose a safe subnet mask given the network class
		int first_octet = ip4_addr1(&sn_mask);
		if (first_octet <= 127)
		{
			sn_mask.addr = htonl(0xff000000);
		}
		else if (first_octet >= 192)
		{
			sn_mask.addr = htonl(0xffffff00);
		}
		else
		{
			sn_mask.addr = htonl(0xffff0000);
		}
	}
	ip_addr_set(&gw_addr, &dhcp->offered_gw_addr);
	// gateway address not given?
	if (gw_addr.addr == 0)
	{
		// copy network address
		// use first host address on network as gateway
		gw_addr.addr = (dhcp->offered_ip_addr.addr & sn_mask.addr) | htonl(
				0x00000001);
	}
	if (dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON)
	{
		autoip_stop(netif);
		dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
	}
	//	printf("dhcp_bind(): IP: %s\n", dhcp->offered_ip_addr.ToString());
	netif->ipaddr = dhcp->offered_ip_addr;
	//	printf("dhcp_bind(): SN: %s\n", sn_mask.ToString());
	netif->netmask = sn_mask;
	//	printf("dhcp_bind(): GW: %s\n", gw_addr.ToString());
	netif->gw = gw_addr;
	// bring the interface up
	netif_set_up(netif);
	// netif is now bound to DHCP leased address
	dhcp_set_state(dhcp, DHCP_BOUND);
}

void dhcp_check(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;

	printf("dhcp_check(netif=%p) %s\n", netif, netif->GetName());
	dhcp_set_state(dhcp, DHCP_CHECKING);
	// create an ARP query for the offered IP address, expecting that no host
	// responds, as the IP address should not be in use.
	int result = etharp_query(netif, &dhcp->offered_ip_addr, NULL);
	if (result != ERR_OK)
	{
		printf("dhcp_check: could not perform ARP query\n");
	}
	dhcp->tries++;
	int msecs = 500;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
	printf("dhcp_check(): set request timeout %d msecs\n", msecs);
}

void dhcp_handle_offer(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	// obtain the server address
	uint8 *option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_SERVER_ID);
	if (option_ptr)
	{
		dhcp->server_ip_addr.addr = htonl(dhcp_get_option_long(&option_ptr[2]));
		//		printf("dhcp_handle_offer(): server %s\n",
		//				dhcp->server_ip_addr.ToString());
		// remember offered address
		ip_addr_set(&dhcp->offered_ip_addr, (ip_addr *)&dhcp->msg_in->yiaddr);
		//		printf("dhcp_handle_offer(): offer for %s\n",
		//				dhcp->offered_ip_addr.ToString());
		dhcp_select(netif);
	}
}

void dhcp_select(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;

	dhcp_set_state(dhcp, DHCP_REQUESTING);

	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_REQUEST);

	dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
	dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));
#if	LWIP_NETIF_HOSTNAME
	if (strlen(netif->HostName) > 0)
	{
		dhcp_option(dhcp, hostName, strlen(netif->HostName));
		dhcp_option_string(dhcp, netif->HostName, strlen(netif->HostName));
	}
	//dhcp_option(dhcp, domainName, strlen(testDomainname));
	//dhcp_option_string(dhcp, testDomainname, strlen(testDomainname));
#endif
	// MUST request the offered IP address
	dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
	dhcp_option_long(dhcp, ntohl((uint32) dhcp->offered_ip_addr.addr));

	dhcp_option(dhcp, DHCP_OPTION_SERVER_ID, 4);
	dhcp_option_long(dhcp, ntohl((uint32) dhcp->server_ip_addr.addr));

	dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, 4); // num options
	dhcp_option_byte(dhcp, DHCP_OPTION_SUBNET_MASK);
	dhcp_option_byte(dhcp, DHCP_OPTION_ROUTER);
	dhcp_option_byte(dhcp, DHCP_OPTION_BROADCAST);
	dhcp_option_byte(dhcp, DHCP_OPTION_DNS_SERVER);

#if LWIP_NETIF_HOSTNAME
//	const char * p = (const char*)netif->hostname;
//	if (p != NULL)
//	{
//		dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
//		while (*p)
//		{
//			dhcp_option_byte(dhcp, *p++);
//		}
//	}
#endif

	dhcp_option_trailer(dhcp);
	// shrink the pbuf to the actual content length
	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);
	// send broadcast to any DHCP server
	udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT,
			netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	int msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
}

void dhcp_fine_tmr()
{
	POSITION pos = netif_list.GetHeadPosition();
	while (pos)
	{
		Ethernet * netif = netif_list.GetNext(pos);
		// only act on DHCP configured interfaces
		if (netif->dhcp)
		{
			// timer is active (non zero), and is about to trigger now
			if (netif->dhcp->request_timeout > 1)
			{
				netif->dhcp->request_timeout--;
			}
			else if (netif->dhcp->request_timeout == 1)
			{
				netif->dhcp->request_timeout--;
				// this clients' request timeout triggered
				dhcp_timeout(netif);
			}
		}
	}
}

void dhcp_coarse_tmr()
{
	POSITION pos = netif_list.GetHeadPosition();
	while (pos)
	{
		Ethernet * netif = netif_list.GetNext(pos);
		// only act on DHCP configured interfaces
		if (netif->dhcp)
		{
			// timer is active (non zero), and triggers (zeroes) now?
			if (netif->dhcp->t2_timeout-- == 1)
			{
				// this clients' rebind timeout triggered
				dhcp_t2_timeout(netif);
				// timer is active (non zero), and triggers (zeroes) now
			}
			else if (netif->dhcp->t1_timeout-- == 1)
			{
				// this clients' renewal timeout triggered
				dhcp_t1_timeout(netif);
			}
		}
	}
}

void dhcp_timeout(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	//	printf("dhcp_timeout()\n");
	// back-off period has passed, or server selection timed out
	if ((dhcp->state == DHCP_BACKING_OFF) || (dhcp->state == DHCP_SELECTING))
	{
		dhcp_discover(netif);
		// receiving the requested lease timed out
	}
	else if (dhcp->state == DHCP_REQUESTING)
	{
		if (dhcp->tries <= 5)
		{
			dhcp_select(netif);
		}
		else
		{
			dhcp_release(netif);
			dhcp_discover(netif);
		}
		//#if DHCP_DOES_ARP_CHECK
		// received no ARP reply for the offered address (which is good)
	}
	else if (dhcp->state == DHCP_CHECKING)
	{
		if (dhcp->tries <= 1)
		{
			dhcp_check(netif);
			// no ARP replies on the offered address,
			// looks like the IP address is indeed free
		}
		else
		{
			// bind the interface to the offered address
			dhcp_bind(netif);
		}
		//#endif // DHCP_DOES_ARP_CHECK
	}
	// did not get response to renew request?
	else if (dhcp->state == DHCP_RENEWING)
	{
		// just retry renewal
		// note that the rebind timer will eventually time-out if renew does not work
		dhcp_renew(netif);
		// did not get response to rebind request?
	}
	else if (dhcp->state == DHCP_REBINDING)
	{
		if (dhcp->tries <= 8)
		{
			dhcp_rebind(netif);
		}
		else
		{
			dhcp_release(netif);
			dhcp_discover(netif);
		}
	}
	else if (dhcp->state == DHCP_REBOOTING)
	{
		if (dhcp->tries < REBOOT_TRIES)
		{
			dhcp_reboot(netif);
		}
		else
		{
			dhcp_discover(netif);
		}
	}
}

void dhcp_t1_timeout(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	if ((dhcp->state == DHCP_REQUESTING) || (dhcp->state == DHCP_BOUND)
			|| (dhcp->state == DHCP_RENEWING))
	{
		// just retry to renew - note that the rebind timer (t2) will
		// eventually time-out if renew tries fail.
		dhcp_renew(netif);
	}
}

void dhcp_t2_timeout(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	if ((dhcp->state == DHCP_REQUESTING) || (dhcp->state == DHCP_BOUND)
			|| (dhcp->state == DHCP_RENEWING))
	{
		// just retry to rebind
		dhcp_rebind(netif);
	}
}

void dhcp_release(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	// idle DHCP client
	dhcp_set_state(dhcp, DHCP_OFF);
	// clean old DHCP offer
	dhcp->server_ip_addr.addr = 0;
	dhcp->offered_ip_addr.addr = 0;
	dhcp->offered_sn_mask.addr = 0;
	dhcp->offered_gw_addr.addr = 0;
	dhcp->offered_bc_addr.addr = 0;
	dhcp->offered_t0_lease = 0;
	dhcp->offered_t1_renew = 0;
	dhcp->offered_t2_rebind = 0;
	dhcp->dns_count = 0;

	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_RELEASE);

	dhcp_option_trailer(dhcp);

	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);

	udp_sendto_if(dhcp->pcb, dhcp->p_out, &dhcp->server_ip_addr,
			DHCP_SERVER_PORT, netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	int msecs = dhcp->tries < 10 ? dhcp->tries * 1000 : 10 * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
	// bring the interface down
	netif_set_down(netif);
	// remove IP address from interface
	ip_addr_set(&netif->ipaddr, IP_ADDR_ANY);
	ip_addr_set(&netif->gw, IP_ADDR_ANY);
	ip_addr_set(&netif->netmask, IP_ADDR_ANY);
	// TODO: netif_down(netif);
}

void dhcp_renew(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	dhcp_set_state(dhcp, DHCP_RENEWING);

	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_REQUEST);

	dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
	dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));
#if	LWIP_NETIF_HOSTNAME
	if (strlen(netif->HostName) > 0)
	{
		dhcp_option(dhcp, hostName, strlen(netif->HostName));
		dhcp_option_string(dhcp, netif->HostName, strlen(netif->HostName));
	}
	//dhcp_option(dhcp, domainName, strlen(testDomainname));
	//dhcp_option_string(dhcp, testDomainname, strlen(testDomainname));
#endif
#if LWIP_NETIF_HOSTNAME
//	const char * p = (const char *) netif->hostname;
//	if (p)
//	{
//		dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
//		while (*p)
//		{
//			dhcp_option_byte(dhcp, *p++);
//		}
//	}
#endif // LWIP_NETIF_HOSTNAME
#if 0
	dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));
#endif
#if 0
	dhcp_option(dhcp, DHCP_OPTION_SERVER_ID, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->server_ip_addr.addr));
#endif
	// append DHCP message trailer
	dhcp_option_trailer(dhcp);

	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);

	udp_sendto_if(dhcp->pcb, dhcp->p_out, &dhcp->server_ip_addr,
			DHCP_SERVER_PORT, netif);
	dhcp_delete_request(netif);

	dhcp->tries++;
	// back-off on retries, but to a maximum of 20 seconds
	int msecs = dhcp->tries < 10 ? dhcp->tries * 2000 : 20 * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
}

void dhcp_reboot(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;

	dhcp_set_state(dhcp, DHCP_REBOOTING);
	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_REQUEST);

	dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
	dhcp_option_short(dhcp, 576);

	dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));

	dhcp_option_trailer(dhcp);

	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);

	// broadcast to server
	udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT,
			netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	int msecs = dhcp->tries < 10 ? dhcp->tries * 1000 : 10 * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
	printf("dhcp_reboot(): set request timeout %d msecs\n", msecs);
}

void dhcp_rebind(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
#if LWIP_NETIF_HOSTNAME
//	const char *p;
#endif // LWIP_NETIF_HOSTNAME
	dhcp_set_state(dhcp, DHCP_REBINDING);

	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_REQUEST);

	dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
	dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));
#if LWIP_NETIF_HOSTNAME
	if (strlen(netif->HostName) > 0)
	{
		dhcp_option(dhcp, hostName, strlen(netif->HostName));
		dhcp_option_string(dhcp, netif->HostName, strlen(netif->HostName));
	}
	//dhcp_option(dhcp, domainName, strlen(testDomainname));
	//dhcp_option_string(dhcp, testDomainname, strlen(testDomainname));
#endif
#if LWIP_NETIF_HOSTNAME
//	p = (const char*)netif->hostname;
//	if (p != NULL)
//	{
//		dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
//		while (*p)
//		{
//			dhcp_option_byte(dhcp, *p++);
//		}
//	}
#endif // LWIP_NETIF_HOSTNAME
#if 0
	dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));

	dhcp_option(dhcp, DHCP_OPTION_SERVER_ID, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->server_ip_addr.addr));
#endif

	dhcp_option_trailer(dhcp);

	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);
	// broadcast to server
	udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT,
			netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	int msecs = dhcp->tries < 10 ? dhcp->tries * 1000 : 10 * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
}

void dhcp_arp_reply(Ethernet * netif, ip_addr *addr)
{
	// is a DHCP client doing an ARP check?
	if ((netif->dhcp != NULL) && (netif->dhcp->state == DHCP_CHECKING))
	{
		printf("dhcp_arp_reply(): CHECKING, arp reply for %08X\n",
				(unsigned int) addr->addr);
		// did a host respond with the address we were offered by the DHCP server?
		if (ip_addr_cmp(addr, &netif->dhcp->offered_ip_addr))
		{
			// we will not accept the offered address
			printf("dhcp_arp_reply(): arp reply matched with offered address,"
				" declining\n");
			dhcp_decline(netif);
		}
	}
}

void dhcp_decline(Ethernet * netif)
{
	DHCP * dhcp = netif->dhcp;
	dhcp_set_state(dhcp, DHCP_BACKING_OFF);
	// create and initialize the DHCP message header
	dhcp_create_request(netif);
	dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
	dhcp_option_byte(dhcp, DHCP_DECLINE);

	dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
	dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));

	dhcp_option_trailer(dhcp);
	// resize pbuf to reflect true size of options
	pbuf_realloc(dhcp->p_out, sizeof(DhcpMsg) - DHCP_OPTIONS_LEN
			+ dhcp->options_out_len);

	// per section 4.4.4, broadcast DECLINE messages
	udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT,
			netif);
	dhcp_delete_request(netif);
	dhcp->tries++;
	int msecs = 10 * 1000;
	dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1)
			/ DHCP_FINE_TIMER_MSECS;
}
