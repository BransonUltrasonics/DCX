// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/AutoIP.cpp_v   1.1   27 Jul 2015 08:49:32   sesharma  $
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

#include "AutoIP.h"
#include "Ethernet.h"
#include "ARP.h"
#include "string.h"
#include "DebugNew.h"
// 169.254.0.0
#define AUTOIP_NET         0xA9FE0000
// 169.254.1.0
#define AUTOIP_RANGE_START (AUTOIP_NET | 0x0100)
// 169.254.254.255
#define AUTOIP_RANGE_END   (AUTOIP_NET | 0xFEFF)

// Pseudo random macro based on netif information.
uint32 LWIP_AUTOIP_RAND(Ethernet * netif)
{
	uint8 * mac = netif->GetHardwareAddress();
	uint32 random = mac[5] << 24;
	random |= mac[3] << 16;
	random |= mac[2] << 8;
	random |= mac[4];
	if (netif->autoip)
		random += netif->autoip->tried_llipaddr;
	return random;
}

uint32 LWIP_AUTOIP_CREATE_SEED_ADDR(Ethernet * netif)
{
	uint32 start = AUTOIP_RANGE_START;
	uint8 * mac = netif->GetHardwareAddress();
	start += mac[4];
	start += mac[5] << 8;
	return htonl(start);
}

/**
 * Initialize this module
 */
void autoip_init(void)
{
}

void autoip_start(Ethernet * netif)
{
	AutoIP * autoip = netif->autoip;

	if (netif->up)
		netif_set_down(netif);

	// Set IP-Address, Netmask and Gateway to 0 to make sure that
	// ARP Packets are formed correctly

	netif->ipaddr.addr = 0;
	netif->netmask.addr = 0;
	netif->gw.addr = 0;

	if (!autoip)
	{
		// no AutoIP client attached yet?
		autoip = new AutoIP;
		memset(autoip, 0, sizeof(AutoIP));
		// store this AutoIP client in the netif
		netif->autoip = autoip;
	}
	else
	{
		autoip->state = AUTOIP_STATE_OFF;
		autoip->ttw = 0;
		autoip->sent_num = 0;
		memset(&autoip->llipaddr, 0, sizeof(ip_addr));
		autoip->lastconflict = 0;
	}
	autoip_create_addr(netif, &(autoip->llipaddr));
	autoip->tried_llipaddr++;
	autoip_start_probing(netif);
}

void autoip_stop(Ethernet * netif)
{
	netif->autoip->state = AUTOIP_STATE_OFF;
	netif_set_down(netif);
}

void autoip_start_probing(Ethernet * netif)
{
	AutoIP * autoip = netif->autoip;

	autoip->state = AUTOIP_STATE_PROBING;
	autoip->sent_num = 0;

	// time to wait to first probe, this is randomly
	// choosen out of 0 to PROBE_WAIT seconds.
	// compliant to RFC 3927 Section 2.2.1

	autoip->ttw = (LWIP_AUTOIP_RAND(netif) % (PROBE_WAIT
			* AUTOIP_TICKS_PER_SECOND));
	// if we tried more then MAX_CONFLICTS we must limit our rate for
	// accquiring and probing address
	// compliant to RFC 3927 Section 2.2.1
	if (autoip->tried_llipaddr > MAX_CONFLICTS)
	{
		autoip->ttw = RATE_LIMIT_INTERVAL * AUTOIP_TICKS_PER_SECOND;
	}
}

void autoip_create_addr(Ethernet * netif, ip_addr * ipaddr)
{
	// Here we create an IP-Address out of range 169.254.1.0 to 169.254.254.255
	// compliant to RFC 3927 Section 2.1
	// We have 254 * 256 possibilities

	uint32 addr = ntohl(LWIP_AUTOIP_CREATE_SEED_ADDR(netif));
	addr += netif->autoip->tried_llipaddr;
	addr = AUTOIP_NET | (addr & 0xffff);
	// Now, 169.254.0.0 <= addr <= 169.254.255.255

	if (addr < AUTOIP_RANGE_START)
	{
		addr += AUTOIP_RANGE_END - AUTOIP_RANGE_START + 1;
	}
	if (addr > AUTOIP_RANGE_END)
	{
		addr -= AUTOIP_RANGE_END - AUTOIP_RANGE_START + 1;
	}
	ASSERT((addr >= AUTOIP_RANGE_START) && (addr <= AUTOIP_RANGE_END));
	ipaddr->addr = htonl(addr);

	printf("autoip_create_addr(): tried_llipaddr=%d, %s\n",
			netif->autoip->tried_llipaddr, ipaddr->ToString());
}

void autoip_tmr()
{
	POSITION pos = netif_list.GetHeadPosition();
	// loop through netif's

	while (pos)
	{
		Ethernet * netif = netif_list.GetNext(pos);
		// only act on AutoIP configured interfaces
		if (netif->autoip)
		{
			if (netif->autoip->lastconflict > 0)
			{
				netif->autoip->lastconflict--;
			}
			switch (netif->autoip->state)
			{
			case AUTOIP_STATE_PROBING:

				if (netif->autoip->ttw > 0)
				{
					netif->autoip->ttw--;
				}
				else
				{
					if (netif->autoip->sent_num >= PROBE_NUM)
					{
						netif->autoip->state = AUTOIP_STATE_ANNOUNCING;
						netif->autoip->sent_num = 0;
						netif->autoip->ttw = ANNOUNCE_WAIT
								* AUTOIP_TICKS_PER_SECOND;
					}
					else
					{
						autoip_arp_probe(netif);
						netif->autoip->sent_num++;
						// calculate time to wait to next probe
						netif->autoip->ttw = ((LWIP_AUTOIP_RAND(netif)
								% ((PROBE_MAX - PROBE_MIN)
										* AUTOIP_TICKS_PER_SECOND)) + PROBE_MIN
								* AUTOIP_TICKS_PER_SECOND);
					}
				}

				break;

			case AUTOIP_STATE_ANNOUNCING:

				if (netif->autoip->ttw > 0)
				{
					netif->autoip->ttw--;
				}
				else
				{
					if (netif->autoip->sent_num == 0)
					{
						// We are here the first time, so we waited
						// ANNOUNCE_WAIT seconds. Now we can bind to an IP
						// address and use it. autoip_bind calls netif_set_up.
						// This triggers a gratuitous ARP which counts as an
						// announcement.
						autoip_bind(netif);
					}
					else
					{
						autoip_arp_announce(netif);
					}
					netif->autoip->ttw = ANNOUNCE_INTERVAL
							* AUTOIP_TICKS_PER_SECOND;
					netif->autoip->sent_num++;

					if (netif->autoip->sent_num >= ANNOUNCE_NUM)
					{
						netif->autoip->state = AUTOIP_STATE_BOUND;
						netif->autoip->sent_num = 0;
						netif->autoip->ttw = 0;
					}
				}
				break;

			default:
				break;
			}
		}
	}

}

void autoip_arp_probe(Ethernet * netif)
{
	etharp_raw(netif, (EthAddr *) netif->GetHardwareAddress(), &ethbroadcast,
			(EthAddr *) netif->GetHardwareAddress(), IP_ADDR_ANY, &ethzero,
			&netif->autoip->llipaddr, ARP_REQUEST);
}

void autoip_bind(Ethernet * netif)
{
	AutoIP * autoip = netif->autoip;
	ip_addr sn_mask, gw_addr;

	printf("autoip_bind %s %s\n", netif->GetName(), autoip->llipaddr.ToString());

	IP4_ADDR(&sn_mask, 255, 255, 0, 0);
	IP4_ADDR(&gw_addr, 0, 0, 0, 0);

	netif->ipaddr = autoip->llipaddr;
	netif->netmask = sn_mask;
	netif->gw = gw_addr;
	// bring the interface up
	netif_set_up(netif);
}

void autoip_arp_announce(Ethernet * netif)
{
	etharp_raw(netif, (EthAddr *) netif->GetHardwareAddress(), &ethbroadcast,
			(EthAddr *) netif->GetHardwareAddress(), &netif->autoip->llipaddr,
			&ethzero, &netif->autoip->llipaddr, ARP_REQUEST);
}

void autoip_arp_reply(Ethernet * netif, struct etharp_hdr * hdr)
{
	if ((netif->autoip != NULL) && (netif->autoip->state != AUTOIP_STATE_OFF))
	{
		// when ip.src == llipaddr && hw.src != netif->hwaddr
		// when probing ip.dst == llipaddr && hw.src != netif->hwaddr
		// we have a conflict and must solve it
		EthAddr netifaddr;
		memcpy(&netifaddr, netif->GetHardwareAddress(), sizeof(EthAddr));

		// Copy struct ip_addr2 to aligned ip_addr, to support compilers without
		// structure packing (not using structure copy which breaks
		// strict-aliasing rules).
		ip_addr sipaddr = hdr->sipaddr;
		ip_addr dipaddr = hdr->dipaddr;

		if ((netif->autoip->state == AUTOIP_STATE_PROBING)
				|| ((netif->autoip->state == AUTOIP_STATE_ANNOUNCING)
						&& (netif->autoip->sent_num == 0)))
		{
			// RFC 3927 Section 2.2.1:
			// from beginning to after ANNOUNCE_WAIT  seconds we have a
			// conflict if ip.src == llipaddr OR
			// ip.dst == llipaddr && hw.src != own hwaddr
			if ((ip_addr_cmp(&sipaddr, &netif->autoip->llipaddr))
					|| (ip_addr_cmp(&dipaddr, &netif->autoip->llipaddr)
							&& !eth_addr_cmp(&netifaddr, &hdr->shwaddr)))
			{
				printf("autoip_arp_reply(): Probe Conflict detected\n");
				autoip_start(netif);
			}
		}
		else
		{
			// RFC 3927 Section 2.5:
			// in any state we have a conflict if
			// ip.src == llipaddr && hw.src != own hwaddr
			if (ip_addr_cmp(&sipaddr, &netif->autoip->llipaddr)
					&& !eth_addr_cmp(&netifaddr, &hdr->shwaddr))
			{
				printf("autoip_arp_reply(): Conflicting ARP-Packet detected\n");
				autoip_handle_arp_conflict(netif);
			}
		}
	}
}

void autoip_handle_arp_conflict(Ethernet * netif)
{
	// TODO Somehow detect if we are defending or retreating
	bool defend = true;

	if (defend)
	{
		if (netif->autoip->lastconflict > 0)
		{
			// retreat, there was a conflicting ARP in the last
			// DEFEND_INTERVAL seconds
			printf("autoip_handle_arp_conflict(): we are defending, but in"
				" DEFEND_INTERVAL, retreating\n");
			// TODO: close all TCP sessions
			autoip_start(netif);
		}
		else
		{
			printf("autoip_handle_arp_conflict(): we are defending,"
				" send ARP Announce\n");
			autoip_arp_announce(netif);
			netif->autoip->lastconflict = DEFEND_INTERVAL
					* AUTOIP_TICKS_PER_SECOND;
		}
	}
	else
	{
		printf("autoip_handle_arp_conflict(): we do not defend,"
			" retreating\n");
		// TODO: close all TCP sessions
		autoip_start(netif);
	}
}
