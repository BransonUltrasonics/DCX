// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/IGMP.h_v   1.0   21 Mar 2012 12:41:08   PDwivedi  $
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

#ifndef IGMP_H_
#define IGMP_H_

#include "ArchDefs.h"
#include "IPv4.h"

class Ethernet;

struct PktBuf;
struct ip_addr;

#define IP_PROTO_IGMP                  2
#define IGMP_TTL                       1
#define IGMP_MINLEN                    8
#define ROUTER_ALERT                   0x9404
#define ROUTER_ALERTLEN                4

/*
 * IGMP message types, including version number.
 */
#define IGMP_MEMB_QUERY                0x11 /* Membership query         */
#define IGMP_V1_MEMB_REPORT            0x12 /* Ver. 1 membership report */
#define IGMP_V2_MEMB_REPORT            0x16 /* Ver. 2 membership report */
#define IGMP_LEAVE_GROUP               0x17 /* Leave-group message      */

/* IGMP timer */
#define IGMP_TMR_INTERVAL              100 /* Milliseconds */
#define IGMP_V1_DELAYING_MEMBER_TMR   (1000/IGMP_TMR_INTERVAL)
#define IGMP_JOIN_DELAYING_MEMBER_TMR (500 /IGMP_TMR_INTERVAL)

/* MAC Filter Actions */
#define IGMP_DEL_MAC_FILTER            0
#define IGMP_ADD_MAC_FILTER            1

/* Group  membership states */
#define IGMP_GROUP_NON_MEMBER          0
#define IGMP_GROUP_DELAYING_MEMBER     1
#define IGMP_GROUP_IDLE_MEMBER         2

struct igmp_msg
{
	uint8 igmp_msgtype;
	uint8 igmp_maxresp;
	uint16 igmp_checksum;
	ip_addr igmp_group_address;
};

struct IgmpGroup
{
	IgmpGroup *next;
	Ethernet *interfac;
	ip_addr group_address;
	int last_reporter_flag; /* signifies we were the last person to report */
	int group_state;
	int timer;
	int use; /* counter of simultaneous uses */
};

void igmp_init();
void igmp_input(PktBuf *p, Ethernet *inp, ip_addr *dest);
void igmp_tmr();
void igmp_timeout(IgmpGroup *group);
int igmp_joingroup(ip_addr *ifaddr, ip_addr *groupaddr);
int igmp_leavegroup(ip_addr *ifaddr, ip_addr *groupaddr);
IgmpGroup * igmp_lookfor_group(Ethernet *ifp, ip_addr *addr);
void igmp_delaying_member(IgmpGroup *group, int maxresp);
void igmp_start_timer(IgmpGroup *group, int max_time);
void igmp_stop_timer(IgmpGroup *group);
IgmpGroup * igmp_lookup_group(Ethernet *ifp, ip_addr *addr);
void igmp_send(IgmpGroup *group, int type);
int igmp_ip_output_if(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl,
		int proto, Ethernet *netif);
int igmp_remove_group(IgmpGroup *group);
void igmp_report_groups(Ethernet *netif);

#endif
