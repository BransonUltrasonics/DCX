// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/AutoIP.h_v   1.0   21 Mar 2012 12:39:22   PDwivedi  $
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

#ifndef AUTOIP_H_
#define AUTOIP_H_

#include "IPv4.h"

#define LWIP_AUTOIP 1

/** AutoIP Timing */
#define AUTOIP_TMR_INTERVAL      100
#define AUTOIP_TICKS_PER_SECOND (1000 / AUTOIP_TMR_INTERVAL)

/* RFC 3927 Constants */
#define PROBE_WAIT               1   /* second   (initial random delay)                 */
#define PROBE_MIN                1   /* second   (minimum delay till repeated probe)    */
#define PROBE_MAX                2   /* seconds  (maximum delay till repeated probe)    */
#define PROBE_NUM                3   /*          (number of probe packets)              */
#define ANNOUNCE_NUM             2   /*          (number of announcement packets)       */
#define ANNOUNCE_INTERVAL        2   /* seconds  (time between announcement packets)    */
#define ANNOUNCE_WAIT            2   /* seconds  (delay before announcing)              */
#define MAX_CONFLICTS            10  /*          (max conflicts before rate limiting)   */
#define RATE_LIMIT_INTERVAL      60  /* seconds  (delay between successive attempts)    */
#define DEFEND_INTERVAL          10  /* seconds  (min. wait between defensive ARPs)     */

/** AutoIP client states */
enum AutoIpState
{
	AUTOIP_STATE_OFF,
	AUTOIP_STATE_PROBING,
	AUTOIP_STATE_ANNOUNCING,
	AUTOIP_STATE_BOUND,
};

struct AutoIP
{
	/** the currently selected, probed, announced or used LL IP-Address */
	ip_addr llipaddr;
	/** current AutoIP state machine state */
	AutoIpState state;
	/** sent number of probes or announces, dependent on state */
	int sent_num;
	/** ticks to wait, tick is AUTOIP_TMR_INTERVAL long */
	int ttw;
	/** ticks until a conflict can be solved by defending */
	int lastconflict;
	/** total number of probed/used Link Local IP-Addresses */
	int tried_llipaddr;
};

void autoip_start(Ethernet * netif);
void autoip_stop(Ethernet * netif);
void autoip_start_probing(Ethernet * netif);
void autoip_create_addr(Ethernet * netif, ip_addr * ipaddr);
void autoip_arp_probe(Ethernet * netif);
void autoip_bind(Ethernet * netif);
void autoip_arp_announce(Ethernet * netif);
void autoip_arp_reply(Ethernet * netif, struct etharp_hdr * hdr);
void autoip_handle_arp_conflict(Ethernet * netif);

void autoip_init();
void autoip_tmr();

#endif
