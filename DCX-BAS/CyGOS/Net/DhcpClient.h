// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/DhcpClient.h_v   1.0   21 Mar 2012 12:39:38   PDwivedi  $
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

#ifndef DHCPCLIENT_H_
#define DHCPCLIENT_H_

#include "IPv4.h"
#include "DHCP.h"

#define LWIP_DHCP 1

#define LWIP_DHCP_AUTOIP_COOP 1
#define DHCP_DOES_ARP_CHECK 1
#define LWIP_NETIF_HOSTNAME 0

#define LWIP_DHCP_AUTOIP_COOP_TRIES     9
/** AUTOIP cooperatation flags */
#define DHCP_AUTOIP_COOP_STATE_OFF 0
#define DHCP_AUTOIP_COOP_STATE_ON 1

/** period (in seconds) of the application calling dhcp_coarse_tmr() */
#define DHCP_COARSE_TIMER_SECS 60
/** period (in milliseconds) of the application calling dhcp_coarse_tmr() */
#define DHCP_COARSE_TIMER_MSECS (DHCP_COARSE_TIMER_SECS * 1000UL)
/** period (in milliseconds) of the application calling dhcp_fine_tmr() */
#define DHCP_FINE_TIMER_MSECS 500

#define DHCP_MAX_MSG_LEN_MIN_REQUIRED  576

#define DHCP_HTYPE_ETH 1

#define DHCP_HLEN_ETH 6

#define DHCP_BROADCAST_FLAG 15
#define DHCP_BROADCAST_MASK (1 << DHCP_FLAG_BROADCAST)

/** BootP options */
#define DHCP_OPTION_PAD 0
#define DHCP_OPTION_SUBNET_MASK 1 /* RFC 2132 3.3 */
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS_SERVER 6
#define DHCP_OPTION_HOSTNAME 12
#define DHCP_OPTION_IP_TTL 23
#define DHCP_OPTION_MTU 26
#define DHCP_OPTION_BROADCAST 28
#define DHCP_OPTION_TCP_TTL 37
#define DHCP_OPTION_END 255

/** DHCP options */
#define DHCP_OPTION_REQUESTED_IP 50 /* RFC 2132 9.1, requested IP address */
#define DHCP_OPTION_LEASE_TIME 51 /* RFC 2132 9.2, time in seconds, in 4 bytes */
#define DHCP_OPTION_OVERLOAD 52 /* RFC2132 9.3, use file and/or sname field for options */

#define DHCP_OPTION_MESSAGE_TYPE 53 /* RFC 2132 9.6, important for DHCP */
#define DHCP_OPTION_MESSAGE_TYPE_LEN 1

#define DHCP_OPTION_SERVER_ID 54 /* RFC 2132 9.7, server IP address */
#define DHCP_OPTION_PARAMETER_REQUEST_LIST 55 /* RFC 2132 9.8, requested option types */

#define DHCP_OPTION_MAX_MSG_SIZE 57 /* RFC 2132 9.10, message size accepted >= 576 */
#define DHCP_OPTION_MAX_MSG_SIZE_LEN 2

#define DHCP_OPTION_T1 58 /* T1 renewal time */
#define DHCP_OPTION_T2 59 /* T2 rebinding time */
#define DHCP_OPTION_US 60
#define DHCP_OPTION_CLIENT_ID 61
#define DHCP_OPTION_TFTP_SERVERNAME 66
#define DHCP_OPTION_BOOTFILE 67

/** possible combinations of overloading the file and sname fields with options */
#define DHCP_OVERLOAD_NONE 0
#define DHCP_OVERLOAD_FILE 1
#define DHCP_OVERLOAD_SNAME  2
#define DHCP_OVERLOAD_SNAME_FILE 3

#define REBOOT_TRIES 2

/** DHCP client states */
enum DhcpState
{
	DHCP_ZERO_UNUSED,
	DHCP_REQUESTING,
	DHCP_INIT,
	DHCP_REBOOTING,
	DHCP_REBINDING,
	DHCP_RENEWING,
	DHCP_SELECTING,
	DHCP_INFORMING,
	DHCP_CHECKING,
	DHCP_PERMANENT,
	DHCP_BOUND,
	DHCP_RELEASING,
	DHCP_BACKING_OFF,
	DHCP_OFF,
};

#define DHCP_MAX_DNS 2

#define DHCP_MIN_REPLY_LEN             44

class Ethernet;
class UdpSocket;
class PktBuf;

struct DHCP
{
	/** transaction identifier of last sent request */
	uint32 xid;
	/** our connection to the DHCP server */
	UdpSocket * pcb;
	/** incoming msg */
	DhcpMsg *msg_in;
	/** incoming msg options */
	char * options_in;
	/** ingoing msg options length */
	int options_in_len;
	/** current DHCP state machine state */
	DhcpState state;
	/** retries of current request */
	int tries;
	/** pbuf of outcoming msg */
	PktBuf *p_out;
	/** outgoing msg */
	DhcpMsg *msg_out;
	/** outgoing msg options length */
	int options_out_len;
	/** #ticks with period DHCP_FINE_TIMER_SECS for request timeout */
	uint32 request_timeout;
	/** #ticks with period DHCP_COARSE_TIMER_SECS for renewal time */
	uint32 t1_timeout;
	/** #ticks with period DHCP_COARSE_TIMER_SECS for rebind time */
	uint16 t2_timeout;
	/** dhcp server address that offered this lease */
	ip_addr server_ip_addr;
	ip_addr offered_ip_addr;
	ip_addr offered_sn_mask;
	ip_addr offered_gw_addr;
	ip_addr offered_bc_addr;
	/** actual number of DNS servers obtained */
	uint32 dns_count;
	/** DNS server addresses */
	ip_addr offered_dns_addr[DHCP_MAX_DNS];

	/** lease period (in seconds) */
	uint32 offered_t0_lease;
	/** recommended renew time (usually 50% of lease period) */
	uint32 offered_t1_renew;
	/** recommended rebind time (usually 66% of lease period)  */
	uint32 offered_t2_rebind;
#if LWIP_DHCP_AUTOIP_COOP
	int autoip_coop_state;
#endif
	//	/** Patch #1308
	//	 *  TODO: See dhcp.c "TODO"s
	//	 */
	//	#if 0
	//	  struct ip_addr offered_si_addr;
	//	  uint8 *boot_file_name;
	//	#endif
};

void dhcp_init();

void dhcp_start(Ethernet * netif);
void dhcp_discover(Ethernet * netif);
void dhcp_release(Ethernet * netif);
void dhcp_renew(Ethernet * netif);
void dhcp_reboot(Ethernet * netif);
void dhcp_rebind(Ethernet * netif);
void dhcp_create_request(Ethernet * netif);
void dhcp_delete_request(Ethernet * netif);
void dhcp_handle_ack(Ethernet * netif);
void dhcp_handle_nak(Ethernet * netif);
void dhcp_bind(Ethernet * netif);
void dhcp_check(Ethernet * netif);
void dhcp_handle_offer(Ethernet * netif);
void dhcp_select(Ethernet * netif);
void dhcp_arp_reply(Ethernet * netif, ip_addr *addr);
void dhcp_decline(Ethernet * netif);

void dhcp_recv(void *arg, UdpSocket * pcb, PktBuf *p, ip_addr * addr, int port);
void dhcp_set_state(DHCP * dhcp, DhcpState new_state);
void dhcp_option(DHCP * dhcp, int option_type, int option_len);
void dhcp_option_byte(DHCP * dhcp, int value);
void dhcp_option_short(DHCP * dhcp, int value);
void dhcp_option_long(DHCP * dhcp, long value);
void dhcp_option_string(DHCP * dhcp, char *string, int len);
void dhcp_option_trailer(DHCP * dhcp);
void dhcp_unfold_reply(DHCP * dhcp, PktBuf * p);
void dhcp_free_reply(DHCP * dhcp);
uint8 * dhcp_get_option_ptr(DHCP * dhcp, int option_type);

int dhcp_get_option_byte(uint8 *ptr);
uint32 dhcp_get_option_long(uint8 *ptr);

void dhcp_coarse_tmr();
void dhcp_fine_tmr();
void dhcp_timeout(Ethernet * netif);
void dhcp_t1_timeout(Ethernet * netif);
void dhcp_t2_timeout(Ethernet * netif);

#endif
