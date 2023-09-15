// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/IGMP.cpp_v   1.0   09 Jun 2015 09:13:56   tayars  $
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

#include "IGMP.h"
#include "Net.h"
#include "IPv4.h"
#include "PktBuf.h"
#include "Ethernet.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "DebugNew.h"

static IgmpGroup * igmp_group_list;

static ip_addr allsystems;
static ip_addr allrouters;

/**
 * Initialize the IGMP module
 */
void igmp_init()
{
	IP4_ADDR(&allsystems, 224, 0, 0, 1);
	IP4_ADDR(&allrouters, 224, 0, 0, 2);
}

/**
 * Start IGMP processing on interface
 *
 * @param netif network interface on which start IGMP processing
 */
int igmp_start(Ethernet *netif)
{
	IgmpGroup* group;

	printf("igmp_start: starting IGMP processing on if %p\n", netif);

	group = igmp_lookup_group(netif, &allsystems);

	if (group != NULL)
	{
		group->group_state = IGMP_GROUP_IDLE_MEMBER;
		group->use++;

		/* Allow the igmp messages at the MAC level */
		if (netif->igmp_mac_filter)
		{
			printf("igmp_start: igmp_mac_filter(ADD ");
			ip_addr_debug_print(&allsystems);
			printf(") on if %p\n", netif);
			netif->igmp_mac_filter(netif, &allsystems, IGMP_ADD_MAC_FILTER);
		}
		return ERR_OK;
	}
	return ERR_MEM;
}

/**
 * Stop IGMP processing on interface
 *
 * @param netif network interface on which stop IGMP processing
 */
int igmp_stop(Ethernet *netif)
{
	IgmpGroup *group = igmp_group_list;
	IgmpGroup *prev = NULL;
	IgmpGroup *next;

	/* look for groups joined on this interface further down the list */
	while (group != NULL)
	{
		next = group->next;
		/* is it a group joined on this interface? */
		if (group->interfac == netif)
		{
			/* is it the first group of the list? */
			if (group == igmp_group_list)
			{
				igmp_group_list = next;
			}
			/* is there a "previous" group defined? */
			if (prev != NULL)
			{
				prev->next = next;
			}
			/* disable the group at the MAC level */
			if (netif->igmp_mac_filter)
			{
				printf("igmp_stop: igmp_mac_filter(DEL ");
				ip_addr_debug_print(&group->group_address);
				printf(") on if %p\n", netif);
				netif->igmp_mac_filter(netif, &(group->group_address),
						IGMP_DEL_MAC_FILTER);
			}
			/* free group */
			delete group;
		}
		else
		{
			/* change the "previous" */
			prev = group;
		}
		/* move to "next" */
		group = next;
	}
	return ERR_OK;
}

/**
 * Report IGMP memberships for this interface
 *
 * @param netif network interface on which report IGMP memberships
 */
void igmp_report_groups(Ethernet *netif)
{
	IgmpGroup *group = igmp_group_list;
	while (group != NULL)
	{
		if (group->interfac == netif)
		{
			igmp_delaying_member(group, IGMP_JOIN_DELAYING_MEMBER_TMR);
		}
		group = group->next;
	}
}

/**
 * The igmp timer function
 * Should be called every IGMP_TMR_INTERVAL milliseconds (100 ms is default).
 */
void igmp_tmr()
{
	IgmpGroup *group = igmp_group_list;

	while (group)
	{
		if (group->timer != 0)
		{
			group->timer -= 1;
			if (group->timer == 0)
			{
				igmp_timeout(group);
			}
		}
		group = group->next;
	}
}

/**
 * Called if a timeout for one group is reached.
 * Sends a report for this group.
 *
 * @param group an igmp_group for which a timeout is reached
 */
void igmp_timeout(IgmpGroup *group)
{
	/* If the state is IGMP_GROUP_DELAYING_MEMBER then we send a report for this group */
	if (group->group_state == IGMP_GROUP_DELAYING_MEMBER)
	{
		printf("igmp_timeout: report membership for group with address ");
		ip_addr_debug_print(&group->group_address);
		printf(" on if %p\n", group->interfac);
		igmp_send(group, IGMP_V2_MEMB_REPORT);
	}
}

/**
 * Called from ip_input() if a new IGMP packet is received.
 *
 * @param p received igmp packet, p->payload pointing to the ip header
 * @param inp network interface on which the packet was received
 * @param dest destination ip address of the igmp packet
 */
void igmp_input(PktBuf *p, Ethernet *inp, ip_addr *dest)
{
	IPv4Header * iphdr;
	igmp_msg* igmp;
	IgmpGroup* group;
	IgmpGroup* groupref;

	/* Note that the length CAN be greater than 8 but only 8 are used - All are included in the checksum */
	iphdr = (IPv4Header *) p->payload;
	if (pbuf_header(p, -(int16) (iphdr->HL() * 4)) || (p->len < IGMP_MINLEN))
	{
		pbuf_free(p);
		printf("igmp_input: length error\n");
		return;
	}

	printf("igmp_input: message from ");
	ip_addr_debug_print(&(iphdr->src));
	printf(" to address ");
	ip_addr_debug_print(&(iphdr->dest));
	printf(" on if %p\n", inp);

	/* Now calculate and check the checksum */
	igmp = (igmp_msg *) p->payload;
	if (inet_chksum(igmp, p->len))
	{
		pbuf_free(p);
		printf("igmp_input: checksum error\n");
		return;
	}

	/* Packet is ok so find an existing group */
	group = igmp_lookfor_group(inp, dest); /* use the incoming IP address! */

	/* If group can be found or create... */
	if (!group)
	{
		pbuf_free(p);
		printf("igmp_input: IGMP frame not for us\n");
		return;
	}

	/* NOW ACT ON THE INCOMING MESSAGE TYPE... */
	switch (igmp->igmp_msgtype)
	{
	case IGMP_MEMB_QUERY:
		/* IGMP_MEMB_QUERY to the "all systems" address ? */
		if ((ip_addr_cmp(dest, &allsystems)) && (igmp->igmp_group_address.addr
				== 0))
		{
			/* THIS IS THE GENERAL QUERY */
			printf("igmp_input: General IGMP_MEMB_QUERY on \"ALL SYSTEMS\" "
				"address (224.0.0.1) [igmp_maxresp=%i]\n", igmp->igmp_maxresp);
			if (igmp->igmp_maxresp == 0)
			{
				printf("igmp_input: got an all hosts query with time== 0 "
					"- this is V1 and not implemented - treat as v2\n");
				igmp->igmp_maxresp = IGMP_V1_DELAYING_MEMBER_TMR;
			}
			groupref = igmp_group_list;
			while (groupref)
			{
				/* Do not send messages on the all systems group address! */
				if ((groupref->interfac == inp) && (!(ip_addr_cmp(
						&(groupref->group_address), &allsystems))))
				{
					igmp_delaying_member(groupref, igmp->igmp_maxresp);
				}
				groupref = groupref->next;
			}
		}
		else
		{
			/* IGMP_MEMB_QUERY to a specific group ? */
			if (group->group_address.addr != 0)
			{
				printf("igmp_input: IGMP_MEMB_QUERY to a specific group ");
				ip_addr_debug_print(&group->group_address);
				if (ip_addr_cmp(dest, &allsystems))
				{
					printf(" using \"ALL SYSTEMS\" address (224.0.0.1)"
						"[igmp_maxresp=%i]\n", igmp->igmp_maxresp);
					/* we first need to re-lookfor the group since we used dest last time */
					group = igmp_lookfor_group(inp, &igmp->igmp_group_address);
				}
				else
				{
					printf(" with the group address as destination"
						"[igmp_maxresp=%i]\n", igmp->igmp_maxresp);
				}
				if (group)
				{
					igmp_delaying_member(group, igmp->igmp_maxresp);
				}
			}
		}
		break;
	case IGMP_V2_MEMB_REPORT:
		printf("igmp_input: IGMP_V2_MEMB_REPORT\n");

		if (group->group_state == IGMP_GROUP_DELAYING_MEMBER)
		{
			/* This is on a specific group we have already looked up */
			group->timer = 0; /* stopped */
			group->group_state = IGMP_GROUP_IDLE_MEMBER;
			group->last_reporter_flag = 0;
		}
		break;
	default:
		printf("igmp_input: msg %d in state %d on group %p on if %p\n",
				igmp->igmp_msgtype, group->group_state, &group, group->interfac);
		break;
	}

	pbuf_free(p);
	return;
}

/**
 * Delaying membership report for a group if necessary
 *
 * @param group the igmp_group for which "delaying" membership report
 * @param maxresp query delay
 */
void igmp_delaying_member(IgmpGroup *group, int maxresp)
{
	if ((group->group_state == IGMP_GROUP_IDLE_MEMBER) || ((group->group_state
			== IGMP_GROUP_DELAYING_MEMBER) && (maxresp > group->timer)))
	{
		igmp_start_timer(group, (maxresp) / 2);
		group->group_state = IGMP_GROUP_DELAYING_MEMBER;
	}
}

/**
 * Start a timer for an igmp group
 *
 * @param group the igmp_group for which to start a timer
 * @param max_time the time in multiples of IGMP_TMR_INTERVAL (decrease with
 *        every call to igmp_tmr())
 */
void igmp_start_timer(IgmpGroup *group, int max_time)
{
	/**
	 * @todo Important !! this should be random 0 -> max_time. Find out how to do this
	 */
	group->timer = max_time;
}

/**
 * Stop a timer for an igmp_group
 *
 * @param group the igmp_group for which to stop the timer
 */
void igmp_stop_timer(IgmpGroup *group)
{
	group->timer = 0;
}

/**
 * Join a group on one network interface.
 *
 * @param ifaddr ip address of the network interface which should join a new group
 * @param groupaddr the ip address of the group which to join
 * @return ERR_OK if group was joined on the netif(s), an int otherwise
 */
int igmp_joingroup(ip_addr *ifaddr, ip_addr *groupaddr)
{
	int err = ERR_VAL; /* no matching interface */
	IgmpGroup *group = 0;
	Ethernet *netif;

	if(groupaddr){
		/* make sure it is multicast address */
		if (!ip_addr_ismulticast(groupaddr))
		{
			printf("igmp_joingroup: attempt to join non-multicast address\n");
			return ERR_VAL;
		}
		if (ip_addr_cmp(groupaddr, &allsystems))
		{
			printf("igmp_joingroup: attempt to join allsystems address\n");
			return ERR_VAL;
		}
	}
	/* loop through netif's */
	POSITION pos = netif_list.GetHeadPosition();
	while (pos)
	{
		netif = netif_list.GetNext(pos);
		/* Should we join this interface ? */
		if (ip_addr_isany(ifaddr) || ip_addr_cmp(&(netif->ipaddr), ifaddr))
		{
			/* find group or create a new one if not found */
			if(groupaddr)
				group = igmp_lookup_group(netif, groupaddr);
			if (group != NULL)
			{
				/* This should create a new group, check the state to make sure */
				if (group->group_state != IGMP_GROUP_NON_MEMBER)
				{
					printf("igmp_joingroup: join to group not in state"
						"IGMP_GROUP_NON_MEMBER\n");
				}
				else
				{
					/* OK - it was new group */
					printf("igmp_joingroup: join to new group: ");
					ip_addr_debug_print(groupaddr);
					printf("\n");
					/* If first use of the group, allow the group at the MAC level */
					if ((group->use == 0) && (netif->igmp_mac_filter))
					{
						printf("igmp_joingroup: igmp_mac_filter(ADD ");
						ip_addr_debug_print(groupaddr);
						printf(") on if %p\n", netif);
						netif->igmp_mac_filter(netif, groupaddr,
								IGMP_ADD_MAC_FILTER);
					}
					igmp_send(group, IGMP_V2_MEMB_REPORT);

					igmp_start_timer(group, IGMP_JOIN_DELAYING_MEMBER_TMR);

					/* Need to work out where this timer comes from */
					group->group_state = IGMP_GROUP_DELAYING_MEMBER;
				}
				/* Increment group use */
				group->use++;
				/* Join on this interface */
				err = ERR_OK;
			}
			else
			{
				/* Return an error even if some network interfaces are joined */
				/** @todo undo any other netif already joined */
				printf("igmp_joingroup: Not enought memory to join to"
					"group\n");
				return ERR_MEM;
			}
		}
	}
	return err;
}

/**
 * Search for a specific igmp group and create a new one if not found-
 *
 * @param ifp the network interface for which to look
 * @param addr the group ip address to search
 * @return a struct igmp_group*,
 *         NULL on memory error.
 */
IgmpGroup * igmp_lookup_group(Ethernet *ifp, ip_addr *addr)
{
	IgmpGroup *group = igmp_group_list;

	/* Search if the group already exists */
	group = igmp_lookfor_group(ifp, addr);
	/* Group already exists. */
	if (group)
		return group;

	/* Group doesn't exist yet, create a new one */
	group = new IgmpGroup();
	if (group != NULL)
	{
		group->interfac = ifp;
		ip_addr_set(&(group->group_address), addr);
		group->timer = 0; /* Not running */
		group->group_state = IGMP_GROUP_NON_MEMBER;
		group->last_reporter_flag = 0;
		group->use = 0;
		group->next = igmp_group_list;

		igmp_group_list = group;
	}

	printf("igmp_lookup_group: %sallocated a new group with address ",
			(group ? "" : "impossible to "));
	ip_addr_debug_print(addr);
	printf(" on if %p\n", ifp);

	return group;
}

/**
 * Send an igmp packet to a specific group.
 *
 * @param group the group to which to send the packet
 * @param type the type of igmp packet to send
 */
void igmp_send(IgmpGroup *group, int type)
{
	/* IP header + "router alert" option + IGMP header */
	PktBuf* p = pbuf_alloc(PBUF_TRANSPORT, IGMP_MINLEN, PBUF_RAM);

	if (p)
	{
		igmp_msg * igmp = (igmp_msg *) p->payload;
		ASSERT(p->len >= sizeof(igmp_msg));
		ip_addr src;
		ip_addr_set(&src, &((group->interfac)->ipaddr));

		ip_addr * dest = NULL;
		if (type == IGMP_V2_MEMB_REPORT)
		{
			dest = &group->group_address;
			ip_addr_set(&(igmp->igmp_group_address), &group->group_address);
			group->last_reporter_flag = 1; /* Remember we were the last to report */
		}
		else
		{
			if (type == IGMP_LEAVE_GROUP)
			{
				dest = &allrouters;
				ip_addr_set(&(igmp->igmp_group_address), &(group->group_address));
			}
		}

		if ((type == IGMP_V2_MEMB_REPORT) || (type == IGMP_LEAVE_GROUP))
		{
			igmp->igmp_msgtype = type;
			igmp->igmp_maxresp = 0;
			igmp->igmp_checksum = 0;
			igmp->igmp_checksum = inet_chksum(igmp, IGMP_MINLEN);
			igmp_ip_output_if(p, &src, dest, IGMP_TTL, IP_PROTO_IGMP,
					group->interfac);
		}
		pbuf_free(p);
	}
	else
	{
		printf("igmp_send: not enough memory for igmp_send\n");
	}
}

/**
 * Sends an IP packet on a network interface. This function constructs the IP header
 * and calculates the IP header checksum. If the source IP address is NULL,
 * the IP address of the outgoing network interface is filled in as source address.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
 protocol header; if dest == IP_HDRINCL, p already includes an IP
 header and p->payload points to that IP header)
 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
 *         IP  address of the netif used to send is used as source address)
 * @param dest the destination IP address to send the packet to
 * @param ttl the TTL value to be set in the IP header
 * @param proto the PROTOCOL to be set in the IP header
 * @param netif the netif on which to send this packet
 * @return ERR_OK if the packet was sent OK
 *         ERR_BUF if p doesn't have enough space for IP/LINK headers
 *         returns errors returned by netif->output
 */
int igmp_ip_output_if(PktBuf *p, ip_addr *src, ip_addr *dest, int ttl,
		int proto, Ethernet *netif)
{
	/* This is the "router alert" option */
	uint16 ra[2];
	ra[0] = htons(ROUTER_ALERT);
	ra[1] = 0x0000; /* Router shall examine packet */
	return ip_output_if_opt(p, src, dest, ttl, 0, proto, netif, ra,
			ROUTER_ALERTLEN);
}

/**
 * Leave a group on one network interface.
 *
 * @param ifaddr ip address of the network interface which should leave a group
 * @param groupaddr the ip address of the group which to leave
 * @return ERR_OK if group was left on the netif(s), an int otherwise
 */
int igmp_leavegroup(ip_addr *ifaddr, ip_addr *groupaddr)
{
	int err = ERR_VAL; /* no matching interface */
	IgmpGroup *group = 0;
	Ethernet *netif;
	if(groupaddr){
		/* make sure it is multicast address */
		if (!ip_addr_ismulticast(groupaddr))
		{
			printf("igmp_leavegroup: attempt to leave non-multicast address\n");
			return ERR_VAL;
		}
		if (ip_addr_cmp(groupaddr, &allsystems))
		{
			printf("igmp_leavegroup: attempt to leave allsystems address\n");
			return ERR_VAL;
		}
	}

	/* loop through netif's */
	POSITION pos = netif_list.GetHeadPosition();
	while (pos)
	{
		netif = netif_list.GetNext(pos);
		/* Should we leave this interface ? */
		if (ip_addr_isany(ifaddr) || ip_addr_cmp(&netif->ipaddr, ifaddr))
		{
			/* find group */
			if(groupaddr)
				group = igmp_lookfor_group(netif, groupaddr);
			if (group)
			{
				/* Only send a leave if the flag is set according to the state diagram */
				printf("igmp_leavegroup: Leaving group: ");
				ip_addr_debug_print(groupaddr);
				printf("\n");
				/* If there is no other use of the group */
				if (group->use <= 1)
				{
					/* If we are the last reporter for this group */
					if (group->last_reporter_flag)
					{
						printf("igmp_leavegroup: sending leaving group\n");
						igmp_send(group, IGMP_LEAVE_GROUP);
					}
					/* Disable the group at the MAC level */
					if (netif->igmp_mac_filter)
					{
						printf("igmp_leavegroup: igmp_mac_filter(DEL ");
						ip_addr_debug_print(groupaddr);
						printf(") on if %p\n", netif);
						netif->igmp_mac_filter(netif, groupaddr,
								IGMP_DEL_MAC_FILTER);
					}
					printf("igmp_leavegroup: remove group: ");
					ip_addr_debug_print(groupaddr);
					printf("\n");
					/* Free the group */
					igmp_remove_group(group);
				}
				else
				{
					/* Decrement group use */
					group->use--;
				}
				/* Leave on this interface */
				err = ERR_OK;
			}
			else
			{
				/* It's not a fatal error on "leavegroup" */
				printf("igmp_leavegroup: not member of group\n");
			}
		}
	}
	return err;
}

/**
 * Search for a group in the global igmp_group_list
 *
 * @param ifp the network interface for which to look
 * @param addr the group ip address to search for
 * @return a struct igmp_group* if the group has been found,
 *         NULL if the group wasn't found.
 */
IgmpGroup * igmp_lookfor_group(Ethernet *ifp, ip_addr *addr)
{
	IgmpGroup *group = igmp_group_list;
	while (group)
	{
		if ((group->interfac == ifp) && (ip_addr_cmp(&(group->group_address),
				addr)))
		{
			return group;
		}
		group = group->next;
	}
	return 0;
}

/**
 * Remove a group in the global igmp_group_list
 *
 * @param group the group to remove from the global igmp_group_list
 * @return ERR_OK if group was removed from the list, an int otherwise
 */
int igmp_remove_group(IgmpGroup *group)
{
	int err = ERR_OK;

	/* Is it the first group? */
	if (igmp_group_list == group)
	{
		igmp_group_list = group->next;
	}
	else
	{
		/* look for group further down the list */
		IgmpGroup * tmpGroup;
		for (tmpGroup = igmp_group_list; tmpGroup != NULL; tmpGroup
				= tmpGroup->next)
		{
			if (tmpGroup->next == group)
			{
				tmpGroup->next = group->next;
				break;
			}
		}
		/* Group not found in the global igmp_group_list */
		if (tmpGroup == NULL)
			err = ERR_ARG;
	}
	/* free group */
	delete group;

	return err;
}

