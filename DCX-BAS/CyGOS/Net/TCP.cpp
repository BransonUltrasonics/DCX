// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/TCP.cpp_v   1.4   03 Sep 2013 18:59:12   ygupta  $
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

#include "TCP.h"
#include "PktBuf.h"
#include "Ethernet.h"
#include "NetConn.h"
#include "Event.h"
#include "MailBox.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "DebugNew.h"

//disable debug
#define printf
//#define pprintf

static inline int TCP_EVENT_ACCEPT(TcpSocket * pcb, int err)
{
	if (pcb->accept)
		return pcb->accept(pcb->callback_arg, pcb, err);
	return ERR_OK;
}

static inline int TCP_EVENT_SENT(TcpSocket *pcb, int space)
{
	if (pcb->sent)
		return pcb->sent(pcb->callback_arg, pcb, space);
	return ERR_OK;
}

static inline int TCP_EVENT_RECV(TcpSocket *pcb, PktBuf *p, int err)
{
	if (pcb->recv)
		return pcb->recv(pcb->callback_arg, pcb, p, err);
	if (p)
		pbuf_free(p);
	return ERR_OK;
}

static inline int TCP_EVENT_CONNECTED(TcpSocket * pcb, int err)
{
	if (pcb->connected)
		return pcb->connected(pcb->callback_arg, pcb, err);
	return ERR_OK;
}

static inline int TCP_EVENT_POLL(TcpSocket * pcb)
{
	if (pcb->poll)
		return pcb->poll(pcb->callback_arg, pcb);
	return ERR_OK;
}

static inline void TCP_EVENT_ERR(void(* errf)(void *arg, int err), void *arg,
		int err)
{
	if (errf)
		errf(arg, err);
}

static inline void TCP_REG(TcpSocket ** pcbs, TcpSocket * npcb)
{
	npcb->next = *pcbs;
	*pcbs = npcb;
}

static inline void TCP_REG(TcpSocketListen ** pcbs, TcpSocketListen * npcb)
{
	npcb->next = *pcbs;
	*pcbs = npcb;
}

static void TCP_RMV(TcpSocket ** pcbs, TcpSocket * npcb)
{
	if (*pcbs == npcb)
	{
		*pcbs = (*pcbs)->next;
	}
	else
	{
		TcpSocket * tcp_tmp_pcb;
		for (tcp_tmp_pcb = *pcbs; tcp_tmp_pcb != NULL; tcp_tmp_pcb
				= tcp_tmp_pcb->next)
		{
			if (tcp_tmp_pcb->next != NULL && tcp_tmp_pcb->next == npcb)
			{
				tcp_tmp_pcb->next = npcb->next;
				break;
			}
		}
	}
	npcb->next = 0;
}

void tcp_ack(TcpSocket *pcb)
{
	if (pcb->flags & TF_ACK_DELAY)
	{
		pcb->flags &= ~TF_ACK_DELAY;
		pcb->flags |= TF_ACK_NOW;
		tcp_output(pcb);
	}
	else
	{
		pcb->flags |= TF_ACK_DELAY;
	}
}

void tcp_ack_now(TcpSocket *pcb)
{
	pcb->flags |= TF_ACK_NOW;
	tcp_output(pcb);
}

#define tcp_do_output_nagle(tpcb) ((((tpcb)->unacked == NULL) || \
                            ((tpcb)->flags & TF_NODELAY) || \
                            (((tpcb)->unsent != NULL) && (((tpcb)->unsent->next != NULL) || \
                              ((tpcb)->unsent->len >= (tpcb)->mss))) \
                            ) ? 1 : 0)

#define tcp_output_nagle(tpcb) (tcp_do_output_nagle(tpcb) ? tcp_output(tpcb) : ERR_OK)

#define TCP_TCPLEN(seg) ((seg)->len + (((seg)->tcphdr->FLAGS() & TCP_FIN || \
		(seg)->tcphdr->FLAGS() & TCP_SYN)? 1: 0))

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4
/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

#define TCP_FIN_WAIT_TIMEOUT 20000 /* milliseconds */
#define TCP_SYN_RCVD_TIMEOUT 20000 /* milliseconds */
#define TCP_OOSEQ_TIMEOUT        6U /* x RTO */

#define TCP_TMR_INTERVAL       100  /* The TCP timer interval in milliseconds. */
#define TCP_FAST_INTERVAL      TCP_TMR_INTERVAL /* the fine grained timeout in milliseconds */
#define TCP_SLOW_INTERVAL      (5*TCP_TMR_INTERVAL)  /* the coarse grained timeout in milliseconds */

#define  TCP_KEEPIDLE_DEFAULT     7200000UL /* Default KEEPALIVE timer in milliseconds */
#define  TCP_KEEPINTVL_DEFAULT    75000UL   /* Default Time between KEEPALIVE probes in milliseconds */
#define  TCP_KEEPCNT_DEFAULT      9U        /* Default Counter for KEEPALIVE probes */

#define  TCP_MAXIDLE              TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT  /* Maximum KEEPALIVE probe time */

#define TCP_MSL 60000UL /* The maximum segment lifetime in milliseconds */

#define TCP_SEQ_LT(a,b)     ((int32)((a)-(b)) < 0)
#define TCP_SEQ_LEQ(a,b)    ((int32)((a)-(b)) <= 0)
#define TCP_SEQ_GT(a,b)     ((int32)((a)-(b)) > 0)
#define TCP_SEQ_GEQ(a,b)    ((int32)((a)-(b)) >= 0)
/* is b<=a<=c? */
#define TCP_SEQ_BETWEEN(a,b,c) (TCP_SEQ_GEQ(a,b) && TCP_SEQ_LEQ(a,c))

#define LWIP_TCP_OPT_LENGTH(flags)              \
  (flags & TF_SEG_OPTS_MSS ? 4  : 0) +          \
  (flags & TF_SEG_OPTS_TS  ? 12 : 0)

#define TCP_BUILD_MSS_OPTION(x) (x) = htonl(((uint32)2 << 24) |          \
                                            ((uint32)4 << 16) |          \
                                            (((uint32)TCP_MSS / 256) << 8) | \
                                            (TCP_MSS & 255))

#define TCP_TTL                 255
#define TCP_MSS                 1460
#define TCP_WND                 8096
#define TCP_WND_UPDATE_THRESHOLD   (TCP_WND / 4)
/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             2048
/* TCP sender buffer space (pbufs). This must be at least = 2 *
 TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN       (4 * TCP_SND_BUF/TCP_MSS)

/* Flags for "apiflags" parameter in tcp_write and tcp_enqueue */
#define TCP_WRITE_FLAG_COPY 0x01
#define TCP_WRITE_FLAG_MORE 0x02

const uint8 tcp_backoff[13] =
{ 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7 };
/* Times per slowtmr hits */
const uint8 tcp_persist_backoff[7] =
{ 3, 6, 12, 24, 48, 96, 120 };

TcpSeg static_inseg;
TcpHeader *static_tcphdr;
IPv4Header *static_iphdr;
uint32 static_seqno, static_ackno;
uint8 static_flags;
uint16 static_tcplen;
uint8 static_recv_flags;
PktBuf *static_recv_data;

/* Incremented every coarse grained timer shot (typically every 500 ms). */
uint32 static_tcp_ticks;

union tcp_listen_pcbs_t
{
	/* List of all TCP PCBs in LISTEN state. */
	TcpSocketListen *listen_pcbs;
	TcpSocket *pcbs;
};

/** List of all TCP PCBs in LISTEN state */
tcp_listen_pcbs_t tcp_listen_pcbs;

/** List of all TCP PCBs bound but not yet (connected || listening) */
TcpSocket * tcp_bound_pcbs;

/** List of all TCP PCBs that are in a state in which
 * they accept or send data. */
TcpSocket * tcp_active_pcbs;

/** List of all TCP PCBs in TIME-WAIT state */
TcpSocket * tcp_tw_pcbs;

TcpSocket * tcp_input_pcb;

void tcp_init()
{
}

/**
 * The initial input processing of TCP. It verifies the TCP header, demultiplexes
 * the segment between the PCBs and passes it on to tcp_process(), which implements
 * the TCP finite state machine. This function is called by the IP layer (in
 * ip_input()).
 *
 * @param p received TCP segment to process (p->payload pointing to the IP header)
 * @param inp network interface on which this segment was received
 */
//void UpdateTPVal(int);
//void UpdateTPVal2(int PointIndex, unsigned int val);
void tcp_input(PktBuf *p, Ethernet *inp)
{
	TcpSocket *pcb, *prev;
	TcpSocketListen *lpcb;
	int err;

	static_iphdr = (IPv4Header *) p->payload;
	static_tcphdr = (TcpHeader *) ((uint8 *) p->payload + static_iphdr->HL()
			* 4);

	/* remove header from payload */
	if (pbuf_header(p, -((int16) (static_iphdr->HL() * 4))) || (p->tot_len
			< sizeof(TcpHeader)))
	{
		/* drop short packets */
		pbuf_free(p);
		return;
	}

	/* Don't even process incoming broadcasts/multicasts. */
	if (ip_addr_isbroadcast(&(static_iphdr->dest), inp) || ip_addr_ismulticast(
			&(static_iphdr->dest)))
	{
		pbuf_free(p);
		return;
	}

	/* Verify TCP checksum. */
	if (inet_chksum_pseudo(p, (ip_addr *) &(static_iphdr->src),
			(ip_addr *) &(static_iphdr->dest), IP_PROTO_TCP, p->tot_len) != 0)
	{
		pbuf_free(p);
		return;
	}

	/* Move the payload pointer in the pbuf so that it points to the
	 TCP data instead of the TCP header. */
	if (pbuf_header(p, -(static_tcphdr->HDRLEN() * 4)))
	{
		/* drop short packets */
		pbuf_free(p);
		return;
	}

	/* Convert fields in TCP header to host byte order. */
	static_tcphdr->src = ntohs(static_tcphdr->src);
	static_tcphdr->dest = ntohs(static_tcphdr->dest);
	static_tcphdr->seqno = ntohl(static_tcphdr->seqno);
	static_seqno = static_tcphdr->seqno;//static_tcphdr->seqno = ntohl(static_tcphdr->seqno);
	static_tcphdr->ackno = ntohl(static_tcphdr->ackno);
	static_ackno = static_tcphdr->ackno;//static_tcphdr->ackno = ntohl(static_tcphdr->ackno);
	static_tcphdr->wnd = ntohs(static_tcphdr->wnd);

	static_flags = static_tcphdr->FLAGS();
	static_tcplen = p->tot_len + ((static_flags & (TCP_FIN | TCP_SYN)) ? 1 : 0);

	/* Demultiplex an incoming segment. First, we check if it is destined
	 for an active connection. */
	prev = NULL;
	int TestIndx = 1;
	int TcpDestPort = static_tcphdr->dest;
	if(TcpDestPort == 44818){
		//UpdateTPVal(156);
		pbuf_free(p);
		goto TcpInputReturn;
	}

	for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
	{
		ASSERT(pcb->state != CLOSED);
		ASSERT(pcb->state != TIME_WAIT);
		ASSERT(pcb->state != LISTEN);
		if (pcb->remote_port == static_tcphdr->src && pcb->local_port
				== static_tcphdr->dest && ip_addr_cmp(&(pcb->remote_ip),
				&(static_iphdr->src)) && ip_addr_cmp(&(pcb->local_ip),
				&(static_iphdr->dest)))
		{
			/* Move this PCB to the front of the list so that subsequent
			 lookups will be faster (we exploit locality in TCP segment
			 arrivals). */
			ASSERT(pcb->next != pcb);
			if (prev != NULL)
			{
				prev->next = pcb->next;
				pcb->next = tcp_active_pcbs;
				tcp_active_pcbs = pcb;
			}
			ASSERT(pcb->next != pcb);
			break;
		}
		prev = pcb;
	}

	if (pcb == NULL)
	{
		/* If it did not go to an active connection, we check the connections
		 in the TIME-WAIT state. */
		for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
		{
			ASSERT(pcb->state == TIME_WAIT);
			if (pcb->remote_port == static_tcphdr->src && pcb->local_port
					== static_tcphdr->dest && ip_addr_cmp(&(pcb->remote_ip),
					&(static_iphdr->src)) && ip_addr_cmp(&(pcb->local_ip),
					&(static_iphdr->dest)))
			{
				/* We don't really care enough to move this PCB to the front
				 of the list since we are not very likely to receive that
				 many segments for connections in TIME-WAIT. */
				tcp_timewait_input(pcb);
				pbuf_free(p);
				TestIndx = 2;
				goto TcpInputReturn;//return;
			}
		}

		/* Finally, if we still did not get a match, we check all PCBs that
		 are LISTENing for incoming connections. */
		prev = NULL;
		for (lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb
				= lpcb->next)
		{
			if ((ip_addr_isany(&(lpcb->local_ip)) || ip_addr_cmp(
					&(lpcb->local_ip), &(static_iphdr->dest)))
					&& lpcb->local_port == static_tcphdr->dest)
			{
				/* Move this PCB to the front of the list so that subsequent
				 lookups will be faster (we exploit locality in TCP segment
				 arrivals). */
				if (prev != NULL)
				{
					((TcpSocketListen *) prev)->next = lpcb->next;
					/* our successor is the remainder of the listening list */
					lpcb->next = tcp_listen_pcbs.listen_pcbs;
					/* put this listening pcb at the head of the listening list */
					tcp_listen_pcbs.listen_pcbs = lpcb;
				}
				tcp_listen_input(lpcb);
				pbuf_free(p);
				TestIndx = 3;
				goto TcpInputReturn;//return;
			}
			prev = (TcpSocket *) lpcb;
		}
	}

	if (pcb != NULL)
	{
		/* The incoming segment belongs to a connection. */
		/* Set up a tcp_seg structure. */
		static_inseg.next = NULL;
		static_inseg.len = p->tot_len;
		static_inseg.dataptr = p->payload;
		static_inseg.p = p;
		static_inseg.tcphdr = static_tcphdr;

		static_recv_data = NULL;
		static_recv_flags = 0;

		/* If there is data which was previously "refused" by upper layer */
		if (pcb->refused_data != NULL)
		{
			/* Notify again application with data previously received. */
			err = TCP_EVENT_RECV(pcb, pcb->refused_data, ERR_OK);
			if (err == ERR_OK)
			{
				pcb->refused_data = NULL;
			}
			else
			{
				/* drop incoming packets, because pcb is "full" */
				//printf("tcp_input: drop incoming packets, pcb is 'full'\n");
				pbuf_free(p);
				TestIndx = 4;
				goto TcpInputReturn;//return;
				return;
			}
		}

		tcp_input_pcb = pcb;
		err = tcp_process(pcb);
		tcp_input_pcb = NULL;
		/* A return value of ERR_ABRT means that tcp_abort() was called
		 and that the pcb has been freed. If so, we don't do anything. */
		if (err != ERR_ABRT)
		{
			if (static_recv_flags & TF_RESET)
			{
				/* TF_RESET means that the connection was reset by the other
				 end. We then call the error callback to inform the
				 application that the connection is dead before we
				 deallocate the PCB. */
				TCP_EVENT_ERR(pcb->errf, pcb->callback_arg, ERR_RST);
				tcp_pcb_remove(&tcp_active_pcbs, pcb);
				delete pcb;
			}
			else if (static_recv_flags & TF_CLOSED)
			{
				/* The connection has been closed and we will deallocate the
				 PCB. */
				tcp_pcb_remove(&tcp_active_pcbs, pcb);
				delete pcb;
			}
			else
			{
				err = ERR_OK;
				/* If the application has registered a "sent" function to be
				 called when new send buffer space is available, we call it
				 now. */
				if (pcb->acked > 0)
				{
					err = TCP_EVENT_SENT(pcb, pcb->acked);
				}

				if (static_recv_data != NULL)
				{
					if (static_flags & TCP_PSH)
					{
						static_recv_data->flags |= PBUF_FLAG_PUSH;
					}

					/* Notify application that data has been received. */
					err = TCP_EVENT_RECV(pcb, static_recv_data, ERR_OK);

					/* If the upper layer can't receive this data, store it */
					if (err != ERR_OK)
					{
						pcb->refused_data = static_recv_data;
//						printf("tcp_input: keep incoming packet, because"
//							"pcb is 'full'\n");
					}
				}

				/* If a FIN segment was received, we call the callback
				 function with a NULL buffer to indicate EOF. */
				if (static_recv_flags & TF_GOT_FIN)
				{
					err = TCP_EVENT_RECV(pcb, NULL, ERR_OK);
				}

				/* If there were no errors, we try to send something out. */
				if (err == ERR_OK)
				{
					tcp_output(pcb);
				}
			}
		}

		/* give up our reference to inseg.p */
		if (static_inseg.p != NULL)
		{
			pbuf_free(static_inseg.p);
			static_inseg.p = NULL;
		}
	}
	else
	{
		/* If no matching PCB was found, send a TCP RST (reset) to the
		 sender. */
		//printf("tcp_input: no PCB match found, resetting.\n");
		if (!(static_tcphdr->FLAGS() & TCP_RST))
		{
			tcp_rst(static_ackno, static_seqno + static_tcplen,
					&(static_iphdr->dest), &(static_iphdr->src),
					static_tcphdr->dest, static_tcphdr->src);
		}
		pbuf_free(p);
	}

	TcpInputReturn:
	if(TcpDestPort == 44818){
	//	UpdateTPVal(157);
		//UpdateTPVal2(158 , TestIndx);
	}
}

/**
 * Called every 500 ms and implements the retransmission timer and the timer that
 * removes PCBs that have been in TIME-WAIT for enough time. It also increments
 * various timers such as the inactivity timer in each PCB.
 *
 * Automatically called from tcp_tmr().
 */
void tcp_slowtmr()
{
	TcpSocket *pcb, *pcb2, *prev;
	uint16 eff_wnd;
	uint8 pcb_remove; /* flag if a PCB should be removed */
	int err;
	err = ERR_OK;

	++static_tcp_ticks;

	/* Steps through all of the active PCBs. */
	prev = NULL;
	pcb = tcp_active_pcbs;
	//pprintf("At %x",tcp_active_pcbs);
	//pprintf("TP:");
	while (pcb != NULL)
	{
		//pprintf(" %d",pcb->state);
		ASSERT(pcb->state != CLOSED);
		ASSERT(pcb->state != LISTEN);
		ASSERT(pcb->state != TIME_WAIT);

		pcb_remove = 0;

		if (pcb->state == SYN_SENT && pcb->nrtx == TCP_SYNMAXRTX)
		{
			++pcb_remove;
		}
		else if (pcb->nrtx == TCP_MAXRTX)
		{
			++pcb_remove;
		}
		else
		{
			if (pcb->persist_backoff > 0)
			{
				/* If snd_wnd is zero, use persist timer to send 1 byte probes
				 * instead of using the standard retransmission mechanism. */
				pcb->persist_cnt++;
				if (pcb->persist_cnt
						>= tcp_persist_backoff[pcb->persist_backoff - 1])
				{
					pcb->persist_cnt = 0;
					if (pcb->persist_backoff < ((int)sizeof(tcp_persist_backoff)))
					{
						pcb->persist_backoff++;
					}
					tcp_zero_window_probe(pcb);
				}
			}
			else
			{
				/* Increase the retransmission timer if it is running */
				if (pcb->rtime >= 0)
					++pcb->rtime;

				if (pcb->unacked != NULL && pcb->rtime >= pcb->rto)
				{
					/* Time for a retransmission. */
					/* Double retransmission time-out unless we are trying to
					 * connect to somebody (i.e., we are in SYN_SENT). */
					if (pcb->state != SYN_SENT)
					{
						pcb->rto = ((pcb->sa >> 3) + pcb->sv)
								<< tcp_backoff[pcb->nrtx];
					}

					/* Reset the retransmission timer. */
					pcb->rtime = 0;

					/* Reduce congestion window and ssthresh. */
					eff_wnd = min(pcb->cwnd, pcb->snd_wnd);
					pcb->ssthresh = eff_wnd >> 1;
					if (pcb->ssthresh < pcb->mss)
					{
						pcb->ssthresh = pcb->mss * 2;
					}
					pcb->cwnd = pcb->mss;

					/* The following needs to be called AFTER cwnd is set to one
					 mss - STJ */
					tcp_rexmit_rto(pcb);
				}
			}
		}
		/* Check if this PCB has stayed too long in FIN-WAIT-2 */
		if (pcb->state == FIN_WAIT_2)
		{
			if ((uint32) (static_tcp_ticks - pcb->tmr) > TCP_FIN_WAIT_TIMEOUT
					/ TCP_SLOW_INTERVAL)
			{
				++pcb_remove;
			}
		}

		/* Check if KEEPALIVE should be sent */
		if ((pcb->so_options & SOF_KEEPALIVE) && ((pcb->state == ESTABLISHED)
				|| (pcb->state == CLOSE_WAIT)))
		{
#if LWIP_TCP_KEEPALIVE
			if((uint32)(tcp_ticks - pcb->tmr)>
					(pcb->keep_idle + (pcb->keep_cnt*pcb->keep_intvl))
					/ TCP_SLOW_INTERVAL)
#else
			if ((uint32) (static_tcp_ticks - pcb->tmr) > (pcb->keep_idle
					+ TCP_MAXIDLE) / TCP_SLOW_INTERVAL)
#endif /* LWIP_TCP_KEEPALIVE */
			{
				pprintf("\n Aborting idle PCB");
				pcb2 = pcb->next;
				tcp_abort(pcb);
				pcb = pcb2;
				//Added Yugal
				if (prev != NULL)
				{
					ASSERT(pcb != tcp_active_pcbs);
					prev->next = pcb->next;
				}
				else
				{
				/* This PCB was the first. */
					ASSERT(tcp_active_pcbs == pcb);
					tcp_active_pcbs = pcb->next;
				}
				while(prev && (prev->next != pcb))
				{
					prev = prev->next;
				}
				continue;
			}
#if LWIP_TCP_KEEPALIVE
			else if((uint32)(static_tcp_ticks - pcb->tmr)>
					(pcb->keep_idle + pcb->keep_cnt_sent * pcb->keep_intvl)
					/ TCP_SLOW_INTERVAL)
#else
			else if ((uint32) (static_tcp_ticks - pcb->tmr) > (pcb->keep_idle
					+ pcb->keep_cnt_sent * TCP_KEEPINTVL_DEFAULT)
					/ TCP_SLOW_INTERVAL)
#endif /* LWIP_TCP_KEEPALIVE */
			{
				tcp_keepalive(pcb);
				pcb->keep_cnt_sent++;
			}
		}

		/* If this PCB has queued out of sequence data, but has been
		 inactive for too long, will drop the data (it will eventually
		 be retransmitted). */
		if (pcb->ooseq != NULL && (uint32) static_tcp_ticks - pcb->tmr
				>= pcb->rto * TCP_OOSEQ_TIMEOUT)
		{
			tcp_segs_free(pcb->ooseq);
			pcb->ooseq = NULL;
		}
		/* Check if this PCB has stayed too long in SYN-RCVD */
		if (pcb->state == SYN_RCVD)
		{
			if ((uint32) (static_tcp_ticks - pcb->tmr) > TCP_SYN_RCVD_TIMEOUT
					/ TCP_SLOW_INTERVAL)
			{
				++pcb_remove;
			}
		}

		/* Check if this PCB has stayed too long in LAST-ACK */
		if (pcb->state == LAST_ACK)
		{
			if ((uint32) (static_tcp_ticks - pcb->tmr) > 2 * TCP_MSL
					/ TCP_SLOW_INTERVAL)
			{
				++pcb_remove;
			}
		}

		/* If the PCB should be removed, do it. */
		if (pcb_remove)
		{
			tcp_pcb_purge(pcb);
			/* Remove PCB from tcp_active_pcbs list. */
			if (prev != NULL)
			{
				ASSERT(pcb != tcp_active_pcbs);
				prev->next = pcb->next;
			}
			else
			{
				/* This PCB was the first. */
				ASSERT(tcp_active_pcbs == pcb);
				tcp_active_pcbs = pcb->next;
			}

			TCP_EVENT_ERR(pcb->errf, pcb->callback_arg, ERR_ABRT);

			pcb2 = pcb->next;
			delete pcb;
			pcb = pcb2;
			//Added Yugal
			prev = tcp_active_pcbs;
			while(prev && (prev->next != pcb))
			{
			   prev = prev->next;
			}

		}
		else
		{
			/* We check if we should poll the connection. */
			++pcb->polltmr;
			if (pcb->polltmr >= pcb->pollinterval)
			{
				pcb->polltmr = 0;
				err = TCP_EVENT_POLL(pcb);
				if (err == ERR_OK)
					tcp_output(pcb);
			}
			prev = pcb;
			pcb = pcb->next;
		}
	}

	/* Steps through all of the TIME-WAIT PCBs. */
	prev = NULL;
	pcb = tcp_tw_pcbs;
	while (pcb != NULL)
	{
		ASSERT(pcb->state == TIME_WAIT);
		pcb_remove = 0;

		/* Check if this PCB has stayed long enough in TIME-WAIT */
		if ((uint32) (static_tcp_ticks - pcb->tmr) > 2 * TCP_MSL
				/ TCP_SLOW_INTERVAL)
		{
			++pcb_remove;
		}
		/* If the PCB should be removed, do it. */
		if (pcb_remove)
		{
			tcp_pcb_purge(pcb);
			/* Remove PCB from tcp_tw_pcbs list. */
			if (prev != NULL)
			{
				ASSERT(pcb != tcp_tw_pcbs);
				prev->next = pcb->next;
			}
			else
			{
				/* This PCB was the first. */
				ASSERT(tcp_tw_pcbs == pcb);
				tcp_tw_pcbs = pcb->next;
			}
			pcb2 = pcb->next;
			delete pcb;
			pcb = pcb2;
		}
		else
		{
			prev = pcb;
			pcb = pcb->next;
		}
	}
}

/**
 * Is called every TCP_FAST_INTERVAL (250 ms) and process data previously
 * "refused" by upper layer (application) and sends delayed ACKs.
 *
 * Automatically called from tcp_tmr().
 */
void tcp_fasttmr()
{
	TcpSocket *pcb;

	for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
	{
		/* If there is data which was previously "refused" by upper layer */
		if (pcb->refused_data != NULL)
		{
			/* Notify again application with data previously received. */
			if (TCP_EVENT_RECV(pcb, pcb->refused_data, ERR_OK) == ERR_OK)
				pcb->refused_data = NULL;
		}

		/* send delayed ACKs */
		if (pcb->flags & TF_ACK_DELAY)
		{
			tcp_ack_now(pcb);
			pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
		}
	}
}

/**
 * Called periodically to dispatch TCP timers.
 */
void tcp_tmr()
{
	static int tcp_timer = 0;
	++tcp_timer;

	/* Call tcp_fasttmr() every 250 ms */
	if ((tcp_timer % 2) == 0){	   
		tcp_fasttmr();		
	}

	if ((tcp_timer % 5) == 0)
	{
		/* Call tcp_tmr() every 500 ms, i.e., every other timer
		 tcp_tmr() is called. */	   
		tcp_slowtmr();

	}
}

/**
 * Closes the connection held by the PCB.
 *
 * Listening pcbs are freed and may not be referenced any more.
 * Connection pcbs are freed if not yet connected and may not be referenced
 * any more. If a connection is established (at least SYN received or in
 * a closing state), the connection is closed, and put in a closing state.
 * The pcb is then automatically freed in tcp_slowtmr(). It is therefore
 * unsafe to reference it.
 *
 * @param pcb the tcp_pcb to close
 * @return ERR_OK if connection has been closed
 *         another int if closing failed and pcb is not freed
 */
int tcp_close(TcpSocket *pcb)
{

	if((pcb->state == ESTABLISHED) &&(!HasLocalIP(pcb->local_ip)))
	{
		
		tcp_abandon(pcb, false);
		pcb = NULL;
		return ERR_OK;
	}
	
	int err;

	switch (pcb->state)
	{
	case CLOSED:
		/* Closing a pcb in the CLOSED state might seem erroneous,
		 * however, it is in this state once allocated and as yet unused
		 * and the user needs some way to free it should the need arise.
		 * Calling tcp_close() with a pcb that has already been closed, (i.e. twice)
		 * or for a pcb that has been used and then entered the CLOSED state
		 * is erroneous, but this should never happen as the pcb has in those cases
		 * been freed, and so any remaining handles are bogus. */
		err = ERR_OK;
		TCP_RMV(&tcp_bound_pcbs, pcb);
		delete pcb;
		pcb = NULL;
		break;
	case LISTEN:
		err = ERR_OK;
		tcp_pcb_remove((TcpSocket **) &tcp_listen_pcbs, pcb);
		delete pcb;
		pcb = NULL;
		break;
	case SYN_SENT:
		err = ERR_OK;
		tcp_pcb_remove(&tcp_active_pcbs, pcb);
		delete pcb;
		pcb = NULL;
		//		snmp_inc_tcpattemptfails();
		break;
	case SYN_RCVD:
		err = tcp_send_ctrl(pcb, TCP_FIN);
		if (err == ERR_OK)
		{
			//			snmp_inc_tcpattemptfails();
			pcb->state = FIN_WAIT_1;
		}
		break;
	case ESTABLISHED:
		err = tcp_send_ctrl(pcb, TCP_FIN);
		if (err == ERR_OK)
		{
			//			snmp_inc_tcpestabresets();
			pcb->state = FIN_WAIT_1;
		}
		break;
	case CLOSE_WAIT:
		err = tcp_send_ctrl(pcb, TCP_FIN);
		if (err == ERR_OK)
		{
			//			snmp_inc_tcpestabresets();
			pcb->state = LAST_ACK;
		}
		break;
	default:
		/* Has already been closed, do nothing. */
		err = ERR_OK;
		pcb = NULL;
		break;
	}

	if (pcb != NULL && err == ERR_OK)
	{
		/* To ensure all data has been sent when tcp_close returns, we have
		 to make sure tcp_output doesn't fail.
		 Since we don't really have to ensure all data has been sent when tcp_close
		 returns (unsent data is sent from tcp timer functions, also), we don't care
		 for the return value of tcp_output for now. */
		/* @todo: When implementing SO_LINGER, this must be changed somehow:
		 If SOF_LINGER is set, the data should be sent when tcp_close returns. */
		tcp_output(pcb);
	}
	return err;
}

/**
 * Abandons a connection and optionally sends a RST to the remote
 * host.  Deletes the local protocol control block. This is done when
 * a connection is killed because of shortage of memory.
 *
 * @param pcb the tcp_pcb to abort
 * @param reset boolean to indicate whether a reset should be sent
 */
void tcp_abandon(TcpSocket *pcb, bool reset)
{
	pprintf("%s",__FUNCTION__);
	uint32 seqno, ackno;
	uint16 remote_port, local_port;
	ip_addr remote_ip, local_ip;
	void (* errf)(void *arg, int err);
	void *errf_arg;

	/* Figure out on which TCP PCB list we are, and remove us. If we
	 are in an active state, call the receive function associated with
	 the PCB with a NULL argument, and send an RST to the remote end. */
	if (pcb->state == TIME_WAIT)
	{
		tcp_pcb_remove(&tcp_tw_pcbs, pcb);
		delete pcb;
	}
	else
	{
		seqno = pcb->snd_nxt;
		ackno = pcb->rcv_nxt;
		ip_addr_set(&local_ip, &(pcb->local_ip));
		ip_addr_set(&remote_ip, &(pcb->remote_ip));
		local_port = pcb->local_port;
		remote_port = pcb->remote_port;
		errf = pcb->errf;
		errf_arg = pcb->callback_arg;
		tcp_pcb_remove(&tcp_active_pcbs, pcb);
		if (pcb->unacked != NULL)
		{
			tcp_segs_free(pcb->unacked);
		}
		if (pcb->unsent != NULL)
		{
			tcp_segs_free(pcb->unsent);
		}
		if (pcb->ooseq != NULL)
		{
			tcp_segs_free(pcb->ooseq);
		}
		delete pcb;
		TCP_EVENT_ERR(errf, errf_arg, ERR_ABRT);
		if (reset)
		{
			tcp_rst(seqno, ackno, &local_ip, &remote_ip, local_port,
					remote_port);
		}
	}
}

/**
 * Binds the connection to a local portnumber and IP address. If the
 * IP address is not given (i.e., ipaddr == NULL), the IP address of
 * the outgoing network interface is used instead.
 *
 * @param pcb the tcp_pcb to bind (no check is done whether this pcb is
 *        already bound!)
 * @param ipaddr the local ip address to bind to (use IP_ADDR_ANY to bind
 *        to any local address
 * @param port the local port to bind to
 * @return ERR_USE if the port is already in use
 *         ERR_OK if bound
 */
int tcp_bind(TcpSocket *pcb, ip_addr *ipaddr, int port)
{
	TcpSocket *cpcb;

	if (!(pcb->state == CLOSED))
	{
		//printf("tcp_bind: can only bind in state CLOSED");
		return ERR_ISCONN;
	}

	if (port == 0)
	{
		port = tcp_new_port();
	}
	/* Check if the address already is in use. */
	/* Check the listen pcbs. */
	for (cpcb = tcp_listen_pcbs.pcbs; cpcb != NULL; cpcb = cpcb->next)
	{
		if (cpcb->local_port == port)
		{
			if (ip_addr_isany(&(cpcb->local_ip)) || ip_addr_isany(ipaddr)
					|| ip_addr_cmp(&(cpcb->local_ip), ipaddr))
			{
				return ERR_USE;
			}
		}
	}
	/* Check the connected pcbs. */
	for (cpcb = tcp_active_pcbs; cpcb != NULL; cpcb = cpcb->next)
	{
		if (cpcb->local_port == port)
		{
			if (ip_addr_isany(&(cpcb->local_ip)) || ip_addr_isany(ipaddr)
					|| ip_addr_cmp(&(cpcb->local_ip), ipaddr))
			{
				return ERR_USE;
			}
		}
	}
	/* Check the bound, not yet connected pcbs. */
	for (cpcb = tcp_bound_pcbs; cpcb != NULL; cpcb = cpcb->next)
	{
		if (cpcb->local_port == port)
		{
			if (ip_addr_isany(&(cpcb->local_ip)) || ip_addr_isany(ipaddr)
					|| ip_addr_cmp(&(cpcb->local_ip), ipaddr))
			{
				return ERR_USE;
			}
		}
	}
	/* @todo: until SO_REUSEADDR is implemented (see task #6995 on savannah),
	 * we have to check the pcbs in TIME-WAIT state, also: */
	for (cpcb = tcp_tw_pcbs; cpcb != NULL; cpcb = cpcb->next)
	{
		if (cpcb->local_port == port)
		{
			if (ip_addr_cmp(&(cpcb->local_ip), ipaddr))
			{
				return ERR_USE;
			}
		}
	}

	if (!ip_addr_isany(ipaddr))
	{
		pcb->local_ip.addr = ipaddr->addr;
	}
	pcb->local_port = port;
	TCP_REG(&tcp_bound_pcbs, pcb);
	return ERR_OK;
}

/**
 * Default accept callback if no accept callback is specified by the user.
 * Default action - connection refused.
 */
int tcp_accept_null(void *arg, TcpSocket *pcb, int err)
{
	return ERR_ABRT;
	ASSERT(arg);
	ASSERT(pcb);
	ASSERT(err);
}

/**
 * Set the state of the connection to be LISTEN, which means that it
 * is able to accept incoming connections. The protocol control block
 * is reallocated in order to consume less memory. Setting the
 * connection to LISTEN is an irreversible process.
 *
 * @param pcb the original tcp_pcb
 * @param backlog the incoming connections queue limit
 * @return tcp_pcb used for listening, consumes less memory.
 *
 * @note The original tcp_pcb is freed. This function therefore has to be
 *       called like this:
 *             tpcb = tcp_listen(tpcb);
 */
TcpSocket *tcp_listen_with_backlog(TcpSocket *pcb, int backlog)
{
	TcpSocketListen *lpcb;

	if (!(pcb->state == CLOSED))
	{
		//printf("tcp_listen: pcb already connected\n");
		return NULL;
	}

	/* already listening? */
	if (pcb->state == LISTEN)
		return pcb;
	lpcb = new TcpSocketListen();
	if (lpcb == NULL)
	{
		//pprintf("lpcb == NULL");
		return NULL;
	}
	lpcb->callback_arg = pcb->callback_arg;
	lpcb->local_port = pcb->local_port;
	lpcb->state = LISTEN;
	lpcb->so_options = pcb->so_options;
	lpcb->so_options |= SOF_ACCEPTCONN;
	lpcb->ttl = pcb->ttl;
	lpcb->tos = pcb->tos;
	ip_addr_set(&lpcb->local_ip, &pcb->local_ip);
	TCP_RMV(&tcp_bound_pcbs, pcb);
	delete pcb;
	lpcb->accept = tcp_accept_null;
	lpcb->accepts_pending = 0;
	lpcb->backlog = (backlog ? backlog : 1);
	TCP_REG(&tcp_listen_pcbs.listen_pcbs, lpcb);
	return (TcpSocket *) lpcb;
}

/**
 * Creates a new TCP protocol control block but doesn't place it on
 * any of the TCP PCB lists.
 * The pcb is not put on any list until binding using tcp_bind().
 *
 * @internal: Maybe there should be a idle TCP PCB list where these
 * PCBs are put on. Port reservation using tcp_bind() is implemented but
 * allocated pcbs that are not bound can't be killed automatically if wanting
 * to allocate a pcb with higher prio (@see tcp_kill_prio())
 *
 * @return a new tcp_pcb that initially is in state CLOSED
 */
TcpSocket * tcp_new()
{
	return tcp_alloc(TCP_PRIO_NORMAL);
}

/**
 * Used to specify the argument that should be passed callback
 * functions.
 *
 * @param pcb tcp_pcb to set the callback argument
 * @param arg void pointer argument to pass to callback functions
 */
void tcp_arg(TcpSocket *pcb, void *arg)
{
	pcb->callback_arg = arg;
}

/**
 * Used to specify the function that should be called when a TCP
 * connection receives data.
 *
 * @param pcb tcp_pcb to set the recv callback
 * @param recv callback function to call for this pcb when data is received
 */
void tcp_recv(TcpSocket *pcb, int(* recv)(void *arg, TcpSocket *tpcb,
		PktBuf *p, int err))
{
	pcb->recv = recv;
}

/**
 * Used to specify the function that should be called when TCP data
 * has been successfully delivered to the remote host.
 *
 * @param pcb tcp_pcb to set the sent callback
 * @param sent callback function to call for this pcb when data is successfully sent
 */
void tcp_sent(TcpSocket *pcb, int(* sent)(void *arg, TcpSocket *tpcb, int len))
{
	pcb->sent = sent;
}

/**
 * Used to specify the function that should be called when a fatal error
 * has occured on the connection.
 *
 * @param pcb tcp_pcb to set the err callback
 * @param errf callback function to call for this pcb when a fatal error
 *        has occured on the connection
 */
void tcp_err(TcpSocket *pcb, void(* errf)(void *arg, int err))
{
	pcb->errf = errf;
}

/**
 * Used for specifying the function that should be called when a
 * LISTENing connection has been connected to another host.
 *
 * @param pcb tcp_pcb to set the accept callback
 * @param accept callback function to call for this pcb when LISTENing
 *        connection has been connected to another host
 */
void tcp_accept(TcpSocket *pcb, int(* accept)(void *arg, TcpSocket *newpcb,
		int err))
{
	pcb->accept = accept;
}

/**
 * Used to specify the function that should be called periodically
 * from TCP. The interval is specified in terms of the TCP coarse
 * timer interval, which is called twice a second.
 */
void tcp_poll(TcpSocket *pcb, int(* poll)(void *arg, TcpSocket *tpcb),
		int interval)
{
	pcb->poll = poll;
	pcb->pollinterval = interval;
}

/**
 * Purges a TCP PCB. Removes any buffered data and frees the buffer memory
 * (pcb->ooseq, pcb->unsent and pcb->unacked are freed).
 *
 * @param pcb tcp_pcb to purge. The pcb itself is not deallocated!
 */
void tcp_pcb_purge(TcpSocket *pcb)
{
	if (pcb->state != CLOSED && pcb->state != TIME_WAIT && pcb->state != LISTEN)
	{
		if (pcb->state == SYN_RCVD)
		{
			/* Need to find the corresponding listen_pcb and decrease its accepts_pending */
			TcpSocketListen *lpcb;
			ASSERT(tcp_listen_pcbs.listen_pcbs != NULL);
			for (lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb
					= lpcb->next)
			{
				if ((lpcb->local_port == pcb->local_port) && (ip_addr_isany(
						&lpcb->local_ip) || ip_addr_cmp(&pcb->local_ip,
						&lpcb->local_ip)))
				{
					/* port and address of the listen pcb match the timed-out pcb */
					ASSERT(lpcb->accepts_pending > 0);
					lpcb->accepts_pending--;
					break;
				}
			}
		}

		if (pcb->refused_data != NULL)
		{
			//printf("tcp_pcb_purge: data left on ->refused_data\n");
			pbuf_free(pcb->refused_data);
			pcb->refused_data = NULL;
		}
		if (pcb->unsent != NULL)
		{
//			printf("tcp_pcb_purge: not all data sent\n");
		}
		if (pcb->unacked != NULL)
		{
//			printf("tcp_pcb_purge: data left on ->unacked\n");
		}
		if (pcb->ooseq != NULL)
		{
//			printf("tcp_pcb_purge: data left on ->ooseq\n");
		}
		/* Stop the retransmission timer as it will expect data on unacked
		 queue if it fires */
		pcb->rtime = -1;
		tcp_segs_free(pcb->ooseq);
		pcb->ooseq = NULL;
		tcp_segs_free(pcb->unsent);
		tcp_segs_free(pcb->unacked);
		pcb->unacked = pcb->unsent = NULL;
	}
}

/**
 * Purges the PCB and removes it from a PCB list. Any delayed ACKs are sent first.
 *
 * @param pcblist PCB list to purge.
 * @param pcb tcp_pcb to purge. The pcb itself is also deallocated!
 */
void tcp_pcb_remove(TcpSocket **pcblist, TcpSocket *pcb)
{
	TCP_RMV(pcblist, pcb);
	tcp_pcb_purge(pcb);
	/* if there is an outstanding delayed ACKs, send it */
	if (pcb->state != TIME_WAIT && pcb->state != LISTEN && pcb->flags
			& TF_ACK_DELAY)
	{
		pcb->flags |= TF_ACK_NOW;
		tcp_output(pcb);
	}
	if (pcb->state != LISTEN)
	{
		ASSERT(pcb->unsent == NULL);
		ASSERT(pcb->unacked == NULL);
		ASSERT(pcb->ooseq == NULL);
	}
	pcb->state = CLOSED;
}

/**
 * Calculates a new initial sequence number for new connections.
 *
 * @return u32_t pseudo random sequence number
 */
uint32 tcp_next_iss()
{
	static uint32 iss = 6510;

	iss += static_tcp_ticks;
	return iss;
}

int tcp_enqueue(TcpSocket *pcb, void *arg, int len, int flags, int apiflags,
		int optflags)
{
	PktBuf *p;
	TcpSeg *seg, *useg, *queue;
	uint32 seqno;
	uint16 left, seglen;
	void *ptr;
	uint16 queuelen;
	uint8 optlen;

	if (!((len != 0) || (optflags != 0) || ((flags & (TCP_SYN | TCP_FIN)) != 0)))
	{
//		printf(
//				"tcp_enqueue: packet needs payload, options, or SYN/FIN " "(programmer violates API)\n");
		return ERR_ARG;
	}
	if (!((len != 0) || (arg == NULL)))
	{
//		printf("tcp_enqueue: len != 0 || arg == NULL "
//			"(programmer violates API)\n");
		return ERR_ARG;
	}
	/* fail on too much data */
	if (len > pcb->snd_buf)
	{
//		printf("tcp_enqueue: too much data (len=%d > snd_buf=%d)\n", len,
//				pcb->snd_buf);
		pcb->flags |= TF_NAGLEMEMERR;
		return ERR_MEM;
	}
	left = len;
	ptr = arg;

	optlen = LWIP_TCP_OPT_LENGTH(optflags);

	/* seqno will be the sequence number of the first segment enqueued
	 * by the call to this function. */
	seqno = pcb->snd_lbb;

	/* If total number of pbufs on the unsent/unacked queues exceeds the
	 * configured maximum, return an error */
	queuelen = pcb->snd_queuelen;
	/* check for configured max queuelen and possible overflow */
	if ((queuelen >= TCP_SND_QUEUELEN) || (queuelen > TCP_SNDQUEUELEN_OVERFLOW))
	{
		pprintf("tcp_enqueue: too long queue %d (max %d)\n", queuelen,
				TCP_SND_QUEUELEN);
		pcb->flags |= TF_NAGLEMEMERR;
		return ERR_MEM;
	}
	if (queuelen != 0)
	{
		ASSERT(pcb->unacked != NULL || pcb->unsent != NULL);
	}
	else
	{
		ASSERT(pcb->unacked == NULL && pcb->unsent == NULL);
	}

	/* First, break up the data into segments and tuck them together in
	 * the local "queue" variable. */
	useg = queue = seg = NULL;
	seglen = 0;
	while (queue == NULL || left > 0)
	{
		/* The segment length (including options) should be at most the MSS */
		seglen = left > (pcb->mss - optlen) ? (pcb->mss - optlen) : left;

		/* Allocate memory for tcp_seg, and fill in fields. */
		seg = new TcpSeg();
		if (seg == NULL)
		{
			goto memerr;
		}
		seg->next = NULL;
		seg->p = NULL;

		/* first segment of to-be-queued data? */
		if (queue == NULL)
		{
			queue = seg;
		}
		/* subsequent segments of to-be-queued data */
		else
		{
			/* Attach the segment to the end of the queued segments */
			ASSERT(useg != NULL);
			useg->next = seg;
		}
		/* remember last segment of to-be-queued data for next iteration */
		useg = seg;

		/* If copy is set, memory should be allocated
		 * and data copied into pbuf, otherwise data comes from
		 * ROM or other static memory, and need not be copied.  */
		if (apiflags & TCP_WRITE_FLAG_COPY)
		{
			if ((seg->p = pbuf_alloc(PBUF_TRANSPORT, seglen + optlen, PBUF_RAM))
					== NULL)
			{
				goto memerr;
			}
			ASSERT(seg->p->len >= seglen + optlen);
			queuelen += pbuf_clen(seg->p);
			if (arg != NULL)
			{
				memcpy((char *) seg->p->payload + optlen, ptr, seglen);
			}
			seg->dataptr = seg->p->payload;
		}
		/* do not copy data */
		else
		{
			/* First, allocate a pbuf for the headers. */
			if ((seg->p = pbuf_alloc(PBUF_TRANSPORT, optlen, PBUF_RAM)) == NULL)
			{
				goto memerr;
			}
			queuelen += pbuf_clen(seg->p);
			/* Second, allocate a pbuf for holding the data.
			 * since the referenced data is available at least until it is sent out on the
			 * link (as it has to be ACKed by the remote party) we can safely use PBUF_ROM
			 * instead of PBUF_REF here.
			 */
			if (left > 0)
			{
				if ((p = pbuf_alloc(PBUF_RAW, seglen, PBUF_ROM)) == NULL)
				{
					/* If allocation fails, we have to deallocate the header pbuf as well. */
					pbuf_free(seg->p);
					seg->p = NULL;
					goto memerr;
				}
				++queuelen;
				/* reference the non-volatile payload data */
				p->payload = (uint8 *) ptr;
				seg->dataptr = ptr;

				/* Concatenate the headers and data pbufs together. */
				pbuf_cat(seg->p/*header*/, p/*data*/);
				p = NULL;
			}
		}

		/* Now that there are more segments queued, we check again if the
		 length of the queue exceeds the configured maximum or overflows. */
		if ((queuelen > TCP_SND_QUEUELEN) || (queuelen
				> TCP_SNDQUEUELEN_OVERFLOW))
		{
//			printf("tcp_enqueue: queue too long %d (%d)\n", queuelen,
//					TCP_SND_QUEUELEN);
			goto memerr;
		}
		seg->len = seglen;
		/* build TCP header */
		if (pbuf_header(seg->p, sizeof(TcpHeader)))
		{
//			printf("tcp_enqueue: no room for TCP header in pbuf.\n");
			goto memerr;
		}
		seg->tcphdr = (TcpHeader *) seg->p->payload;
		seg->tcphdr->src = htons(pcb->local_port);
		seg->tcphdr->dest = htons(pcb->remote_port);
		seg->tcphdr->seqno = htonl(seqno);
		seg->tcphdr->urgp = 0;
		seg->tcphdr->FLAGS_SET(flags);
		/* don't fill in tcphdr->ackno and tcphdr->wnd until later */

		seg->flags = optflags;

		/* Set the length of the header */
		seg->tcphdr->HDRLEN_SET((sizeof(TcpHeader) + optlen) / 4);
		left -= seglen;
		seqno += seglen;
		ptr = (void *) ((uint8 *) ptr + seglen);
	}

	/* Now that the data to be enqueued has been broken up into TCP
	 segments in the queue variable, we add them to the end of the
	 pcb->unsent queue. */
	if (pcb->unsent == NULL)
	{
		useg = NULL;
	}
	else
	{
		for (useg = pcb->unsent; useg->next != NULL; useg = useg->next)
			;
	}
	/* { useg is last segment on the unsent queue, NULL if list is empty } */

	/* If there is room in the last pbuf on the unsent queue,
	 chain the first pbuf on the queue together with that. */
	if (useg != NULL && TCP_TCPLEN(useg) != 0 && !(useg->tcphdr->FLAGS()
			& (TCP_SYN | TCP_FIN)) && !(flags & (TCP_SYN | TCP_FIN)) &&
	/* fit within max seg size */
	(useg->len + queue->len <= pcb->mss) &&
	/* only concatenate segments with the same options */
	(useg->flags == queue->flags))
	{
		/* Remove TCP header from first segment of our to-be-queued list */
		if (pbuf_header(queue->p, -(int) (sizeof(TcpHeader) + optlen)))
		{
			/* Can we cope with this failing?  Just assert for now */
			ASSERT(0);
			goto memerr;
		}
		if (queue->p->len == 0)
		{
			/* free the first (header-only) pbuf if it is now empty (contained only headers) */
			PktBuf *old_q = queue->p;
			queue->p = queue->p->next;
			old_q->next = NULL;
			queuelen--;
			pbuf_free(old_q);
		}
		ASSERT((queue->p != NULL) && (queue->p->len > 0));
		pbuf_cat(useg->p, queue->p);
		useg->len += queue->len;
		useg->next = queue->next;

		if (seg == queue)
		{
			seg = useg;
			seglen = useg->len;
		}
		delete queue;
	}
	else
	{
		/* empty list */
		if (useg == NULL)
		{
			/* initialize list with this segment */
			pcb->unsent = queue;
		}
		/* enqueue segment */
		else
		{
			useg->next = queue;
		}
	}
	if ((flags & TCP_SYN) || (flags & TCP_FIN))
	{
		++len;
	}
	if (flags & TCP_FIN)
	{
		pcb->flags |= TF_FIN;
	}
	pcb->snd_lbb += len;

	pcb->snd_buf -= len;

	/* update number of segments on the queues */
	pcb->snd_queuelen = queuelen;
	if (pcb->snd_queuelen != 0)
	{
		ASSERT(pcb->unacked != NULL || pcb->unsent != NULL);
	}
	/* Set the PSH flag in the last segment that we enqueued, but only
	 if the segment has data (indicated by seglen > 0). */
	if (seg != NULL && seglen > 0 && seg->tcphdr != NULL && ((apiflags
			& TCP_WRITE_FLAG_MORE) == 0))
	{
		seg->tcphdr->SET_FLAG(TCP_PSH);
	}

	return ERR_OK;
	memerr: pcb->flags |= TF_NAGLEMEMERR;

	if (queue)
		tcp_segs_free(queue);
	if (pcb->snd_queuelen != 0)
	{
		ASSERT(pcb->unacked || pcb->unsent);
	}
	return ERR_MEM;
}

int tcp_output(TcpSocket *pcb)
{
	PktBuf *p;
	TcpHeader *tcphdr;
	TcpSeg *seg, *useg;
	uint32 wnd, snd_nxt;
	uint8 optlen = 0;

	/* First, check if we are invoked by the TCP input processing
	 code. If so, we do not output anything. Instead, we rely on the
	 input processing code to call us when input processing is done
	 with. */
	if (tcp_input_pcb == pcb)
	{
		return ERR_OK;
	}

	wnd = min(pcb->snd_wnd, pcb->cwnd);

	seg = pcb->unsent;

	/* useg should point to last segment on unacked queue */
	useg = pcb->unacked;
	if (useg != NULL)
	{
		for (; useg->next != NULL; useg = useg->next)
			;
	}

	/* If the TF_ACK_NOW flag is set and no data will be sent (either
	 * because the ->unsent queue is empty or because the window does
	 * not allow it), con an empty ACK segment and send it.
	 *
	 * If data is to be sent, we will just piggyback the ACK (see below).
	 */
	if (pcb->flags & TF_ACK_NOW && (seg == NULL || ntohl(seg->tcphdr->seqno)
			- pcb->lastack + seg->len > wnd))
	{
#if LWIP_TCP_TIMESTAMPS
		if (pcb->flags & TF_TIMESTAMP)
		optlen = LWIP_TCP_OPT_LENGTH(TF_SEG_OPTS_TS);
#endif
		p = pbuf_alloc(PBUF_IP, sizeof(TcpHeader) + optlen, PBUF_RAM);
		if (p == NULL)
			return ERR_BUF;
		/* remove ACK flags from the PCB, as we send an empty ACK now */
		pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);

		//printf("1 - slow one - ");
		tcphdr = tcp_output_set_header(pcb, p, optlen, htonl(pcb->snd_nxt));

		/* NB. MSS option is only sent on SYNs, so ignore it here */
#if LWIP_TCP_TIMESTAMPS
		pcb->ts_lastacksent = pcb->rcv_nxt;

		if (pcb->flags & TF_TIMESTAMP)
		tcp_build_timestamp_option(pcb, (uint32 *)(tcphdr + 1));
#endif

		tcphdr->chksum = inet_chksum_pseudo(p, &(pcb->local_ip),
				&(pcb->remote_ip), IP_PROTO_TCP, p->tot_len);
		ip_output(p, &(pcb->local_ip), &(pcb->remote_ip), pcb->ttl, pcb->tos,
				IP_PROTO_TCP);
		pbuf_free(p);

		return ERR_OK;
	}

	/* data available and window allows it to be sent? */
	while (seg != NULL && ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len
			<= wnd)
	{
		ASSERT((seg->tcphdr->FLAGS() & TCP_RST) == 0);
		/* Stop sending if the nagle algorithm would prevent it
		 * Don't stop:
		 * - if tcp_enqueue had a memory error before (prevent delayed ACK timeout) or
		 * - if FIN was already enqueued for this PCB (SYN is always alone in a segment -
		 *   either seg->next != NULL or pcb->unacked == NULL;
		 *   RST is no sent using tcp_enqueue/tcp_output.
		 */
		if ((tcp_do_output_nagle(pcb) == 0) && ((pcb->flags & (TF_NAGLEMEMERR
				| TF_FIN)) == 0))
		{
			break;
		}
		pcb->unsent = seg->next;
		if (pcb->state != SYN_SENT)
		{
			seg->tcphdr->SET_FLAG(TCP_ACK);
			pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
		}

		tcp_output_segment(seg, pcb);
		snd_nxt = ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg);
		if (TCP_SEQ_LT(pcb->snd_nxt, snd_nxt))
		{
			pcb->snd_nxt = snd_nxt;
		}
		/* put segment on unacknowledged list if length > 0 */
		if (TCP_TCPLEN(seg) > 0)
		{
			seg->next = NULL;
			/* unacked list is empty? */
			if (pcb->unacked == NULL)
			{
				pcb->unacked = seg;
				useg = seg;
				/* unacked list is not empty? */
			}
			else
			{
				/* In the case of fast retransmit, the packet should not go to the tail
				 * of the unacked queue, but rather somewhere before it. We need to check for
				 * this case. -STJ Jul 27, 2004 */
				if (TCP_SEQ_LT(ntohl(seg->tcphdr->seqno), ntohl(useg->tcphdr->seqno)))
				{
					/* add segment to before tail of unacked list, keeping the list sorted */
					TcpSeg **cur_seg = &(pcb->unacked);
					while (*cur_seg
							&& TCP_SEQ_LT(ntohl((*cur_seg)->tcphdr->seqno), ntohl(seg->tcphdr->seqno)))
					{
						cur_seg = &((*cur_seg)->next);
					}
					seg->next = (*cur_seg);
					(*cur_seg) = seg;
				}
				else
				{
					/* add segment to tail of unacked list */
					useg->next = seg;
					useg = useg->next;
				}
			}
			/* do not queue empty segments on the unacked list */
		}
		else
		{
			tcp_seg_free(seg);
		}
		seg = pcb->unsent;
	}

	if (seg != NULL && pcb->persist_backoff == 0 && ntohl(seg->tcphdr->seqno)
			- pcb->lastack + seg->len > pcb->snd_wnd)
	{
		/* prepare for persist timer */
		pcb->persist_cnt = 0;
		pcb->persist_backoff = 1;
	}

	pcb->flags &= ~TF_NAGLEMEMERR;
	return ERR_OK;
}

void tcp_output_segment(TcpSeg *seg, TcpSocket *pcb)
{
	uint16 len;
	Ethernet * netif;
	uint32 *opts;

	/** @bug Exclude retransmitted segments from this count. */
	//	snmp_inc_tcpoutsegs();

	/* The TCP header has already been constructed, but the ackno and
	 wnd fields remain. */
	seg->tcphdr->ackno = htonl(pcb->rcv_nxt);

	/* advertise our receive window size in this TCP segment */
	seg->tcphdr->wnd = htons(pcb->rcv_ann_wnd);

	pcb->rcv_ann_right_edge = pcb->rcv_nxt + pcb->rcv_ann_wnd;

	/* Add any requested options.  NB MSS option is only set on SYN
	 packets, so ignore it here */
	opts = (uint32 *) (seg->tcphdr + 1);
	if (seg->flags & TF_SEG_OPTS_MSS)
	{
		TCP_BUILD_MSS_OPTION(*opts);
		opts += 1;
	}
#if LWIP_TCP_TIMESTAMPS
	pcb->ts_lastacksent = pcb->rcv_nxt;

	if (seg->flags & TF_SEG_OPTS_TS)
	{
		tcp_build_timestamp_option(pcb, opts);
		opts += 3;
	}
#endif

	/* If we don't have a local IP address, we get one by
	 calling ip_route(). */
	if (ip_addr_isany(&pcb->local_ip))
	{
		netif = ip_route(&pcb->remote_ip, &pcb->local_ip);
		if (netif == NULL)
		{
			return;
		}
		ip_addr_set(&pcb->local_ip, &netif->ipaddr);
	}

	/* Set retransmission timer running if it is not currently enabled */
	if (pcb->rtime == -1)
		pcb->rtime = 0;

	if (pcb->rttest == 0)
	{
		pcb->rttest = static_tcp_ticks;
		pcb->rtseq = ntohl(seg->tcphdr->seqno);
	}
	len = (uint16) ((uint8 *) seg->tcphdr - (uint8 *) seg->p->payload);

	seg->p->len -= len;
	seg->p->tot_len -= len;

	seg->p->payload = (uint8 *) seg->tcphdr;

	seg->tcphdr->chksum = 0;
	seg->tcphdr->chksum = inet_chksum_pseudo(seg->p, &pcb->local_ip,
			&pcb->remote_ip, IP_PROTO_TCP, seg->p->tot_len);
	ip_output(seg->p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, pcb->tos,
			IP_PROTO_TCP);
}

void tcp_rst(uint32 seqno, uint32 ackno, ip_addr *local_ip, ip_addr *remote_ip,
		int local_port, int remote_port)
{
	PktBuf *p;
	TcpHeader *tcphdr;
	p = pbuf_alloc(PBUF_IP, sizeof(TcpHeader), PBUF_RAM);
	if (p == NULL)
		return;
	ASSERT(p->len >= sizeof(TcpHeader));

	tcphdr = (TcpHeader *) p->payload;
	tcphdr->src = htons(local_port);
	tcphdr->dest = htons(remote_port);
	tcphdr->seqno = htonl(seqno);
	tcphdr->ackno = htonl(ackno);
	tcphdr->FLAGS_SET(TCP_RST | TCP_ACK);
	tcphdr->wnd = htons(TCP_WND);
	tcphdr->urgp = 0;
	tcphdr->HDRLEN_SET(sizeof(TcpHeader) / 4);

	tcphdr->chksum = 0;
	tcphdr->chksum = inet_chksum_pseudo(p, local_ip, remote_ip, IP_PROTO_TCP,
			p->tot_len);
	//	snmp_inc_tcpoutrsts();
	/* Send output with hardcoded TTL since we have no access to the pcb */
	ip_output(p, local_ip, remote_ip, TCP_TTL, 0, IP_PROTO_TCP);
	pbuf_free(p);
}

void tcp_rexmit_rto(TcpSocket *pcb)
{
	TcpSeg *seg;

	if (pcb->unacked == NULL)
	{
		return;
	}

	/* Move all unacked segments to the head of the unsent queue */
	for (seg = pcb->unacked; seg->next != NULL; seg = seg->next)
		;
	/* concatenate unsent queue after unacked queue */
	seg->next = pcb->unsent;
	/* unsent queue is the concatenated queue (of unacked, unsent) */
	pcb->unsent = pcb->unacked;
	/* unacked queue is now empty */
	pcb->unacked = NULL;

	/* increment number of retransmissions */
	++pcb->nrtx;

	/* Don't take any RTT measurements after retransmitting. */
	pcb->rttest = 0;

	/* Do the actual retransmission */
	tcp_output(pcb);
}

void tcp_rexmit(TcpSocket *pcb)
{
	TcpSeg *seg;
	TcpSeg **cur_seg;

	if (pcb->unacked == NULL)
	{
		return;
	}

	/* Move the first unacked segment to the unsent queue */
	/* Keep the unsent queue sorted. */
	seg = pcb->unacked;
	pcb->unacked = seg->next;

	cur_seg = &(pcb->unsent);
	while (*cur_seg
			&& TCP_SEQ_LT(ntohl((*cur_seg)->tcphdr->seqno), ntohl(seg->tcphdr->seqno)))
	{
		cur_seg = &((*cur_seg)->next);
	}
	seg->next = *cur_seg;
	*cur_seg = seg;

	++pcb->nrtx;

	/* Don't take any rtt measurements after retransmitting. */
	pcb->rttest = 0;

	/* Do the actual retransmission. */
	//	snmp_inc_tcpretranssegs();
	tcp_output(pcb);
}

void tcp_keepalive(TcpSocket *pcb)
{
	PktBuf *p;
	TcpHeader *tcphdr;

//	printf("tcp_keepalive: sending KEEPALIVE probe to %d.%d.%d.%d\n",
//			ip4_addr1(&pcb->remote_ip), ip4_addr2(&pcb->remote_ip),
//			ip4_addr3(&pcb->remote_ip), ip4_addr4(&pcb->remote_ip));

//	printf("tcp_keepalive: tcp_ticks %d   pcb->tmr %d pcb->keep_cnt_sent %d\n",
//			static_tcp_ticks, pcb->tmr, pcb->keep_cnt_sent);
	p = pbuf_alloc(PBUF_IP, sizeof(TcpHeader), PBUF_RAM);
	if (p == NULL)
		return;
	ASSERT(p->len >= sizeof(TcpHeader));
//	printf("2 ");
	tcphdr = tcp_output_set_header(pcb, p, 0, htonl(pcb->snd_nxt - 1));
	tcphdr->chksum = inet_chksum_pseudo(p, &pcb->local_ip, &pcb->remote_ip,
			IP_PROTO_TCP, p->tot_len);
	/* Send output to IP */
	ip_output(p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, 0, IP_PROTO_TCP);

	pbuf_free(p);
}

void tcp_zero_window_probe(TcpSocket *pcb)
{
	PktBuf *p;
	TcpHeader *tcphdr;
	TcpSeg *seg;

//	printf("tcp_zero_window_probe: sending ZERO WINDOW probe to %d.%d.%d.%d\n",
//			ip4_addr1(&pcb->remote_ip), ip4_addr2(&pcb->remote_ip),
//			ip4_addr3(&pcb->remote_ip), ip4_addr4(&pcb->remote_ip));

	seg = pcb->unacked;
	if (seg == NULL)
		seg = pcb->unsent;
	if (seg == NULL)
		return;
	p = pbuf_alloc(PBUF_IP, sizeof(TcpHeader) + 1, PBUF_RAM);
	if (p == NULL)
		return;
	ASSERT(p->len >= sizeof(TcpHeader));
//	printf("3 ");
	tcphdr = tcp_output_set_header(pcb, p, 0, seg->tcphdr->seqno);
	/* Copy in one byte from the head of the unacked queue */
	*((char *) p->payload + sizeof(TcpHeader)) = *(char *) seg->dataptr;
	tcphdr->chksum = inet_chksum_pseudo(p, &pcb->local_ip, &pcb->remote_ip,
			IP_PROTO_TCP, p->tot_len);
	/* Send output to IP */
	ip_output(p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, 0, IP_PROTO_TCP);
	pbuf_free(p);
}

TcpHeader *tcp_output_set_header(TcpSocket *pcb, PktBuf *p, int optlen,
		uint32 seqno_be)
{
	TcpHeader *tcphdr = (TcpHeader *) p->payload;
	tcphdr->src = htons(pcb->local_port);
	tcphdr->dest = htons(pcb->remote_port);
	tcphdr->seqno = seqno_be;
	tcphdr->ackno = htonl(pcb->rcv_nxt);
	tcphdr->FLAGS_SET(TCP_ACK);
	tcphdr->wnd = htons(pcb->rcv_ann_wnd);
	tcphdr->urgp = 0;
	tcphdr->HDRLEN_SET((sizeof(TcpHeader) + optlen) / 4);
	tcphdr->chksum = 0;

	/* If we're sending a packet, update the announced right window edge */
	pcb->rcv_ann_right_edge = pcb->rcv_nxt + pcb->rcv_ann_wnd;

	return tcphdr;
}

int tcp_send_ctrl(TcpSocket *pcb, int flags)
{
	/* no data, no length, flags, copy=1, no optdata */
	return tcp_enqueue(pcb, NULL, 0, flags, TCP_WRITE_FLAG_COPY, 0);
}

int tcp_write(TcpSocket *pcb, const void *data, int len, int apiflags)
{
	/* connection is in valid state for data transmission? */
	if (pcb->state == ESTABLISHED || pcb->state == CLOSE_WAIT || pcb->state
			== SYN_SENT || pcb->state == SYN_RCVD)
	{
		if (len > 0)
		{
#if LWIP_TCP_TIMESTAMPS
			return tcp_enqueue(pcb, (void *)data, len, 0, apiflags,
					pcb->flags & TF_TIMESTAMP ? TF_SEG_OPTS_TS : 0);
#else
			return tcp_enqueue(pcb, (void *) data, len, 0, apiflags, 0);
#endif
		}
		return ERR_OK;
	}
	else
	{
//		printf("tcp_write() called in invalid state %d\n", pcb->state);
		return ERR_CONN;
	}
}

/**
 * Allocate a new tcp_pcb structure.
 *
 * @param prio priority for the new pcb
 * @return a new tcp_pcb that initially is in state CLOSED
 */
TcpSocket * tcp_alloc(int prio)
{
	uint32 iss;

	TcpSocket * pcb = new TcpSocket();
	if (!pcb)
	{
		/* Try killing oldest connection in TIME-WAIT. */
//		printf("tcp_alloc: killing off oldest TIME-WAIT connection\n");
		tcp_kill_timewait();
		/* Try to allocate a tcp_pcb again. */
		pcb = new TcpSocket();
		if (!pcb)
		{
			/* Try killing active connections with lower priority than the new one. */
			tcp_kill_prio(prio);
			/* Try to allocate a tcp_pcb again. */
			pcb = new TcpSocket();
		}
	}
	if (pcb)
	{
		memset(pcb, 0, sizeof(TcpSocket));
		pcb->prio = TCP_PRIO_NORMAL;
		pcb->snd_buf = TCP_SND_BUF;
		pcb->snd_queuelen = 0;
		pcb->rcv_wnd = TCP_WND;
		pcb->rcv_ann_wnd = TCP_WND;
		pcb->tos = 0;
		pcb->ttl = TCP_TTL;
		/* As initial send MSS, we use TCP_MSS but limit it to 536.
		 The send MSS is updated when an MSS option is received. */
		pcb->mss = (TCP_MSS > 536) ? 536 : TCP_MSS;
		pcb->rto = 3000 / TCP_SLOW_INTERVAL;
		pcb->sa = 0;
		pcb->sv = 3000 / TCP_SLOW_INTERVAL;
		pcb->rtime = -1;
		pcb->cwnd = 1;
		iss = tcp_next_iss();
		pcb->snd_wl2 = iss;
		pcb->snd_nxt = iss;
		pcb->lastack = iss;
		pcb->snd_lbb = iss;
		pcb->tmr = static_tcp_ticks;

		pcb->polltmr = 0;

#if LWIP_CALLBACK_API
		pcb->recv = tcp_recv_null;
#endif /* LWIP_CALLBACK_API */

		/* Init KEEPALIVE timer */
		pcb->keep_idle = TCP_KEEPIDLE_DEFAULT;

#if LWIP_TCP_KEEPALIVE
		pcb->keep_intvl = TCP_KEEPINTVL_DEFAULT;
		pcb->keep_cnt = TCP_KEEPCNT_DEFAULT;
#endif /* LWIP_TCP_KEEPALIVE */

		pcb->keep_cnt_sent = 0;
	}
	return pcb;
}

/**
 * Kills the oldest active connection that has lower priority than prio.
 *
 * @param prio minimum priority
 */
void tcp_kill_prio(int prio)
{
	int mprio = TCP_PRIO_MAX;
	/* We kill the oldest active connection that has lower priority than prio. */
	uint32 inactivity = 0;
	TcpSocket *inactive = NULL;
	for (TcpSocket *pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
	{
		if (pcb->prio <= prio && pcb->prio <= mprio
				&& (uint32) (static_tcp_ticks - pcb->tmr) >= inactivity)
		{
			inactivity = static_tcp_ticks - pcb->tmr;
			inactive = pcb;
			mprio = pcb->prio;
		}
	}
	if (inactive != NULL)
	{
//		printf("tcp_kill_prio: killing oldest PCB %p (%d)\n", inactive,
//				inactivity);
		tcp_abort(inactive);
	}
}

/**
 * Kills the oldest connection that is in TIME_WAIT state.
 * Called from tcp_alloc() if no more connections are available.
 */
void tcp_kill_timewait()
{
	TcpSocket *pcb, *inactive;
	uint32 inactivity;

	inactivity = 0;
	inactive = NULL;
	/* Go through the list of TIME_WAIT pcbs and get the oldest pcb. */
	for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
	{
		if ((uint32) (static_tcp_ticks - pcb->tmr) >= inactivity)
		{
			inactivity = static_tcp_ticks - pcb->tmr;
			inactive = pcb;
		}
	}
	if (inactive != NULL)
	{
//		printf("tcp_kill_timewait: killing oldest TIME-WAIT PCB %p (%d)\n",
//				inactive, inactivity);
		tcp_abort(inactive);
	}
}

/**
 * Sets the priority of a connection.
 *
 * @param pcb the tcp_pcb to manipulate
 * @param prio new priority
 */
void tcp_setprio(TcpSocket *pcb, int prio)
{
	pcb->prio = prio;
}

/**
 * Implements the TCP state machine. Called by tcp_input. In some
 * states tcp_receive() is called to receive data. The tcp_seg
 * argument will be freed by the caller (tcp_input()) unless the
 * recv_data pointer in the pcb is set.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
int tcp_process(TcpSocket *pcb)
{
	TcpSeg *rseg;
	uint8 acceptable = 0;
	int err;

	err = ERR_OK;

	/* Process incoming RST segments. */
	if (static_flags & TCP_RST)
	{
		/* First, determine if the reset is acceptable. */
		if (pcb->state == SYN_SENT)
		{
			if (static_ackno == pcb->snd_nxt)
			{
				acceptable = 1;
			}
		}
		else
		{
			if (TCP_SEQ_BETWEEN(static_seqno, pcb->rcv_nxt, pcb->rcv_nxt+pcb->rcv_wnd))
			{
				acceptable = 1;
			}
		}

		if (acceptable)
		{
			//			printf("tcp_process: Connection RESET\n");
			ASSERT(pcb->state != CLOSED);
			static_recv_flags |= TF_RESET;
			pcb->flags &= ~TF_ACK_DELAY;
			return ERR_RST;
		}
		else
		{
			//			printf("tcp_process: unacceptable reset seqno %d rcv_nxt %d\n",
			//					static_seqno, pcb->rcv_nxt);
			return ERR_OK;
		}
	}

	if ((static_flags & TCP_SYN) && (pcb->state != SYN_SENT && pcb->state
			!= SYN_RCVD))
	{
		/* Cope with new connection attempt after remote end crashed */
		tcp_ack_now(pcb);
		return ERR_OK;
	}

	/* Update the PCB (in)activity timer. */
	if(pcb->state != FIN_WAIT_1 && pcb->state != FIN_WAIT_2 && pcb->state !=  LAST_ACK)
		pcb->tmr = static_tcp_ticks;
	pcb->keep_cnt_sent = 0;

	tcp_parseopt(pcb);

	/* Do different things depending on the TCP state. */
	switch (pcb->state)
	{
	case SYN_SENT:
		/* received SYN ACK with expected sequence number? */
		if ((static_flags & TCP_ACK) && (static_flags & TCP_SYN)
				&& static_ackno == ntohl(pcb->unacked->tcphdr->seqno) + 1)
		{
			pcb->snd_buf++;
			pcb->rcv_nxt = static_seqno + 1;
			pcb->rcv_ann_right_edge = pcb->rcv_nxt;
			pcb->lastack = static_ackno;
			pcb->snd_wnd = static_tcphdr->wnd;
			pcb->snd_wl1 = static_seqno - 1; /* initialise to seqno - 1 to force window update */
			pcb->state = ESTABLISHED;

			pcb->mss = tcp_eff_send_mss(pcb->mss, &(pcb->remote_ip));

			/* Set ssthresh again after changing pcb->mss (already set in tcp_connect
			 * but for the default value of pcb->mss) */
			pcb->ssthresh = pcb->mss * 10;

			pcb->cwnd = ((pcb->cwnd == 1) ? (pcb->mss * 2) : pcb->mss);
			ASSERT(pcb->snd_queuelen > 0);
			--pcb->snd_queuelen;
			rseg = pcb->unacked;
			pcb->unacked = rseg->next;

			/* If there's nothing left to acknowledge, stop the retransmit
			 timer, otherwise reset it to start again */
			if (pcb->unacked == NULL)
				pcb->rtime = -1;
			else
			{
				pcb->rtime = 0;
				pcb->nrtx = 0;
			}
			tcp_seg_free(rseg);
			/* Call the user specified function to call when sucessfully
			 * connected. */
			err = TCP_EVENT_CONNECTED(pcb, ERR_OK);
			tcp_ack_now(pcb);
		}
		/* received ACK? possibly a half-open connection */
		else if (static_flags & TCP_ACK)
		{
			/* send a RST to bring the other side in a non-synchronized state. */
			tcp_rst(static_ackno, static_seqno + static_tcplen,
					&(static_iphdr->dest), &(static_iphdr->src),
					static_tcphdr->dest, static_tcphdr->src);
		}
		break;
	case SYN_RCVD:
		if (static_flags & TCP_ACK)
		{
			/* expected ACK number? */
			if (TCP_SEQ_BETWEEN(static_ackno, pcb->lastack+1, pcb->snd_nxt))
			{
				uint16 old_cwnd;
				pcb->state = ESTABLISHED;
				//				printf("TCP connection established %d -> %d.\n",
				//						static_inseg.tcphdr->src, static_inseg.tcphdr->dest);
				ASSERT(pcb->accept != NULL);
				/* Call the accept function. */
				err = TCP_EVENT_ACCEPT(pcb, ERR_OK);
				if (err != ERR_OK)
				{
					/* If the accept function returns with an error, we abort
					 * the connection. */
					tcp_abort(pcb);
					return ERR_ABRT;
				}
				old_cwnd = pcb->cwnd;
				/* If there was any data contained within this ACK,
				 * we'd better pass it on to the application as well. */
				tcp_receive(pcb);

				pcb->cwnd = ((old_cwnd == 1) ? (pcb->mss * 2) : pcb->mss);

				if (static_recv_flags & TF_GOT_FIN)
				{
					tcp_ack_now(pcb);
					pcb->state = CLOSE_WAIT;
				}
			}
			/* incorrect ACK number */
			else
			{
				/* send RST */
				tcp_rst(static_ackno, static_seqno + static_tcplen,
						&(static_iphdr->dest), &(static_iphdr->src),
						static_tcphdr->dest, static_tcphdr->src);
			}
		}
		else if ((static_flags & TCP_SYN) && (static_seqno == pcb->rcv_nxt - 1))
		{
			/* Looks like another copy of the SYN - retransmit our SYN-ACK */
			tcp_rexmit(pcb);
		}
		break;
	case CLOSE_WAIT:
		/* FALLTHROUGH */
	case ESTABLISHED:
		tcp_receive(pcb);
		if (static_recv_flags & TF_GOT_FIN)
		{ /* passive close */
			tcp_ack_now(pcb);
			pcb->state = CLOSE_WAIT;
		}
		break;
	case FIN_WAIT_1:
		tcp_receive(pcb);
		if (static_recv_flags & TF_GOT_FIN)
		{
			if ((static_flags & TCP_ACK) && (static_ackno == pcb->snd_nxt))
			{
				//				printf("TCP connection closed %d -> %d.\n",
				//						static_inseg.tcphdr->src, static_inseg.tcphdr->dest);
				tcp_ack_now(pcb);
				tcp_pcb_purge(pcb);
				TCP_RMV(&tcp_active_pcbs, pcb);
				pcb->state = TIME_WAIT;
				TCP_REG(&tcp_tw_pcbs, pcb);
			}
			else
			{
				tcp_ack_now(pcb);
				pcb->state = CLOSING;
			}
		}
		else if ((static_flags & TCP_ACK) && (static_ackno == pcb->snd_nxt))
		{
			pcb->state = FIN_WAIT_2;
		}
		break;
	case FIN_WAIT_2:
		tcp_receive(pcb);
		if (static_recv_flags & TF_GOT_FIN)
		{
			//			printf("TCP connection closed %d -> %d.\n",
			//					static_inseg.tcphdr->src, static_inseg.tcphdr->dest);
			tcp_ack_now(pcb);
			tcp_pcb_purge(pcb);
			TCP_RMV(&tcp_active_pcbs, pcb);
			pcb->state = TIME_WAIT;
			TCP_REG(&tcp_tw_pcbs, pcb);
		}
		break;
	case CLOSING:
		tcp_receive(pcb);
		if (static_flags & TCP_ACK && static_ackno == pcb->snd_nxt)
		{
			//			printf("TCP connection closed %d -> %d.\n",
			//					static_inseg.tcphdr->src, static_inseg.tcphdr->dest);
			tcp_pcb_purge(pcb);
			TCP_RMV(&tcp_active_pcbs, pcb);
			pcb->state = TIME_WAIT;
			TCP_REG(&tcp_tw_pcbs, pcb);
		}
		break;
	case LAST_ACK:
		tcp_receive(pcb);
		if (static_flags & TCP_ACK && static_ackno == pcb->snd_nxt)
		{
			//			printf("TCP connection closed %d -> %d.\n",
			//					static_inseg.tcphdr->src, static_inseg.tcphdr->dest);
			/* bugfix #21699: don't set pcb->state to CLOSED here or we risk leaking segments */
			static_recv_flags |= TF_CLOSED;
		}
		break;
	default:
		break;
	}
	return ERR_OK;
}

/**
 * Called by tcp_process. Checks if the given segment is an ACK for outstanding
 * data, and if so frees the memory of the buffered data. Next, is places the
 * segment on any of the receive queues (pcb->recved or pcb->ooseq). If the segment
 * is buffered, the pbuf is referenced by pbuf_ref so that it will not be freed until
 * i it has been removed from the buffer.
 *
 * If the incoming segment constitutes an ACK for a segment that was used for RTT
 * estimation, the RTT is estimated here as well.
 *
 * Called from tcp_process().
 *
 * @return 1 if the incoming segment is the next in sequence, 0 if not
 */
int tcp_receive(TcpSocket *pcb)
{
	TcpSeg *next;
	TcpSeg *prev, *cseg;
	PktBuf *p;
	int32 off;
	int16 m;
	uint32 right_wnd_edge;
	uint16 new_tot_len;
	uint8 accepted_inseq = 0;

	if (static_flags & TCP_ACK)
	{
		right_wnd_edge = pcb->snd_wnd + pcb->snd_wl2;

		/* Update window. */
		if (TCP_SEQ_LT(pcb->snd_wl1, static_seqno) || (pcb->snd_wl1
				== static_seqno && TCP_SEQ_LT(pcb->snd_wl2, static_ackno))
				|| (pcb->snd_wl2 == static_ackno && static_tcphdr->wnd
						> pcb->snd_wnd))
		{
			pcb->snd_wnd = static_tcphdr->wnd;
			pcb->snd_wl1 = static_seqno;
			pcb->snd_wl2 = static_ackno;
			if (pcb->snd_wnd > 0 && pcb->persist_backoff > 0)
			{
				pcb->persist_backoff = 0;
			}
			//			printf("tcp_receive: window update %d\n", pcb->snd_wnd);
		}
		if (pcb->lastack == static_ackno)
		{
			pcb->acked = 0;

			if (pcb->snd_wl2 + pcb->snd_wnd == right_wnd_edge)
			{
				++pcb->dupacks;
				if (pcb->dupacks >= 3 && pcb->unacked != NULL)
				{
					if (!(pcb->flags & TF_INFR))
					{
						/* This is fast retransmit. Retransmit the first unacked segment. */
						//						printf("tcp_receive: dupacks %d (%d), fast "
						//							"retransmit %d\n", pcb->dupacks, pcb->lastack,
						//								ntohl(pcb->unacked->tcphdr->seqno));
						tcp_rexmit(pcb);
						/* Set ssthresh to max (FlightSize / 2, 2*SMSS) */
						/*pcb->ssthresh = LWIP_MAX((pcb->snd_max -
						 pcb->lastack) / 2,
						 2 * pcb->mss);*/
						/* Set ssthresh to half of the minimum of the current cwnd and the advertised window */
						if (pcb->cwnd > pcb->snd_wnd)
							pcb->ssthresh = pcb->snd_wnd / 2;
						else
							pcb->ssthresh = pcb->cwnd / 2;

						/* The minimum value for ssthresh should be 2 MSS */
						if (pcb->ssthresh < 2 * pcb->mss)
						{
							pcb->ssthresh = 2 * pcb->mss;
						}

						pcb->cwnd = pcb->ssthresh + 3 * pcb->mss;
						pcb->flags |= TF_INFR;
					}
					else
					{
						/* Inflate the congestion window, but not if it means that
						 the value overflows. */
						if ((uint16) (pcb->cwnd + pcb->mss) > pcb->cwnd)
						{
							pcb->cwnd += pcb->mss;
						}
					}
				}
				if (pcb->unacked == NULL && pcb->unsent == NULL)
				    pcb->dupacks = 0;
			}
		}
		else if (TCP_SEQ_BETWEEN(static_ackno, pcb->lastack+1, pcb->snd_nxt))
		{
			/* We come here when the ACK acknowledges new data. */

			/* Reset the "IN Fast Retransmit" flag, since we are no longer
			 in fast retransmit. Also reset the congestion window to the
			 slow start threshold. */
			if (pcb->flags & TF_INFR)
			{
				pcb->flags &= ~TF_INFR;
				pcb->cwnd = pcb->ssthresh;
			}

			/* Reset the number of retransmissions. */
			pcb->nrtx = 0;

			/* Reset the retransmission time-out. */
			pcb->rto = (pcb->sa >> 3) + pcb->sv;

			/* Update the send buffer space. Diff between the two can never exceed 64K? */
			pcb->acked = (uint16) (static_ackno - pcb->lastack);

			pcb->snd_buf += pcb->acked;

			/* Reset the fast retransmit variables. */
			pcb->dupacks = 0;
			pcb->lastack = static_ackno;

			/* Update the congestion control variables (cwnd and
			 ssthresh). */
			if (pcb->state >= ESTABLISHED)
			{
				if (pcb->cwnd < pcb->ssthresh)
				{
					if ((uint16) (pcb->cwnd + pcb->mss) > pcb->cwnd)
					{
						pcb->cwnd += pcb->mss;
					}
				}
				else
				{
					uint16 new_cwnd = (pcb->cwnd + pcb->mss * pcb->mss
							/ pcb->cwnd);
					if (new_cwnd > pcb->cwnd)
					{
						pcb->cwnd = new_cwnd;
					}
				}
			}
			/* Remove segment from the unacknowledged list if the incoming
			 ACK acknowlegdes them. */
			while (pcb->unacked != NULL
					&& TCP_SEQ_LEQ(ntohl(pcb->unacked->tcphdr->seqno) +
							TCP_TCPLEN(pcb->unacked), static_ackno))
			{
				next = pcb->unacked;
				pcb->unacked = pcb->unacked->next;

				ASSERT(pcb->snd_queuelen >= pbuf_clen(next->p));
				pcb->snd_queuelen -= pbuf_clen(next->p);
				tcp_seg_free(next);
				if (pcb->snd_queuelen != 0)
				{
					ASSERT(pcb->unacked != NULL || pcb->unsent != NULL);
				}
			}
			/* If there's nothing left to acknowledge, stop the retransmit
			 timer, otherwise reset it to start again */
			if (pcb->unacked == NULL)
				pcb->rtime = -1;
			else
				pcb->rtime = 0;

			pcb->polltmr = 0;
		}
		else
		{
			/* Fix bug bug #21582: out of sequence ACK, didn't really ack anything */
			pcb->acked = 0;
		}

		/* We go through the ->unsent list to see if any of the segments
		 on the list are acknowledged by the ACK. This may seem
		 strange since an "unsent" segment shouldn't be acked. The
		 rationale is that lwIP puts all outstanding segments on the
		 ->unsent list after a retransmission, so these segments may
		 in fact have been sent once. */
		while (pcb->unsent != NULL
				&& TCP_SEQ_BETWEEN(static_ackno, ntohl(pcb->unsent->tcphdr->seqno) +
						TCP_TCPLEN(pcb->unsent), pcb->snd_nxt))
		{
			next = pcb->unsent;
			pcb->unsent = pcb->unsent->next;
			ASSERT(pcb->snd_queuelen >= pbuf_clen(next->p));
			pcb->snd_queuelen -= pbuf_clen(next->p);
			tcp_seg_free(next);
			if (pcb->snd_queuelen != 0)
			{
				ASSERT(pcb->unacked != NULL || pcb->unsent != NULL);
			}
		}
		/* End of ACK for new data processing. */
		/* RTT estimation calculations. This is done by checking if the
		 incoming segment acknowledges the segment we use to take a
		 round-trip time measurement. */
		if (pcb->rttest && TCP_SEQ_LT(pcb->rtseq, static_ackno))
		{
			/* diff between this shouldn't exceed 32K since this are tcp timer ticks
			 and a round-trip shouldn't be that long... */
			m = (int16) (static_tcp_ticks - pcb->rttest);
			/* This is taken directly from VJs original code in his paper */
			m = m - (pcb->sa >> 3);
			pcb->sa += m;
			if (m < 0)
			{
				m = -m;
			}
			m = m - (pcb->sv >> 2);
			pcb->sv += m;
			pcb->rto = (pcb->sa >> 3) + pcb->sv;
			pcb->rttest = 0;
		}
	}
	/* If the incoming segment contains data, we must process it
	 further. */
	if (static_tcplen > 0)
	{
		/* This code basically does three things:

		 +) If the incoming segment contains data that is the next
		 in-sequence data, this data is passed to the application. This
		 might involve trimming the first edge of the data. The rcv_nxt
		 variable and the advertised window are adjusted.

		 +) If the incoming segment has data that is above the next
		 sequence number expected (->rcv_nxt), the segment is placed on
		 the ->ooseq queue. This is done by finding the appropriate
		 place in the ->ooseq queue (which is ordered by sequence
		 number) and trim the segment in both ends if needed. An
		 immediate ACK is sent to indicate that we received an
		 out-of-sequence segment.

		 +) Finally, we check if the first segment on the ->ooseq queue
		 now is in sequence (i.e., if rcv_nxt >= ooseq->seqno). If
		 rcv_nxt > ooseq->seqno, we must trim the first edge of the
		 segment on ->ooseq before we adjust rcv_nxt. The data in the
		 segments that are now on sequence are chained onto the
		 incoming segment so that we only need to call the application
		 once.
		 */

		/* First, we check if we must trim the first edge. We have to do
		 this if the sequence number of the incoming segment is less
		 than rcv_nxt, and the sequence number plus the length of the
		 segment is larger than rcv_nxt. */
		/*    if (TCP_SEQ_LT(seqno, pcb->rcv_nxt)){
		 if (TCP_SEQ_LT(pcb->rcv_nxt, seqno + tcplen)) {*/
		if (TCP_SEQ_BETWEEN(pcb->rcv_nxt, static_seqno + 1, static_seqno + static_tcplen - 1))
		{
			/* Trimming the first edge is done by pushing the payload
			 pointer in the pbuf downwards. This is somewhat tricky since
			 we do not want to discard the full contents of the pbuf up to
			 the new starting point of the data since we have to keep the
			 TCP header which is present in the first pbuf in the chain.

			 What is done is really quite a nasty hack: the first pbuf in
			 the pbuf chain is pointed to by inseg.p. Since we need to be
			 able to deallocate the whole pbuf, we cannot change this
			 inseg.p pointer to point to any of the later pbufs in the
			 chain. Instead, we point the ->payload pointer in the first
			 pbuf to data in one of the later pbufs. We also set the
			 inseg.data pointer to point to the right place. This way, the
			 ->p pointer will still point to the first pbuf, but the
			 ->p->payload pointer will point to data in another pbuf.

			 After we are done with adjusting the pbuf pointers we must
			 adjust the ->data pointer in the seg and the segment
			 length.*/

			off = pcb->rcv_nxt - static_seqno;
			p = static_inseg.p;
			ASSERT(static_inseg.p);
			ASSERT(off < 0x7fff);
			if (static_inseg.p->len < off)
			{
				ASSERT(((int32) static_inseg.p->tot_len) >= off);
				new_tot_len = (uint16) (static_inseg.p->tot_len - off);
				while (p->len < off)
				{
					off -= p->len;
					/* KJM following line changed (with addition of new_tot_len var)
					 to fix bug #9076
					 inseg.p->tot_len -= p->len; */
					p->tot_len = new_tot_len;
					p->len = 0;
					p = p->next;
				}
				if (pbuf_header(p, (int16) -off))
				{
					/* Do we need to cope with this failing?  Assert for now */
					ASSERT(0);
				}
			}
			else
			{
				if (pbuf_header(static_inseg.p, (int16) -off))
				{
					/* Do we need to cope with this failing?  Assert for now */
					ASSERT(0);
				}
			}
			/* KJM following line changed to use p->payload rather than inseg->p->payload
			 to fix bug #9076 */
			static_inseg.dataptr = p->payload;
			static_inseg.len -= (uint16) (pcb->rcv_nxt - static_seqno);
			static_inseg.tcphdr->seqno = static_seqno = pcb->rcv_nxt;
		}
		else
		{
			if (TCP_SEQ_LT(static_seqno, pcb->rcv_nxt))
			{
				/* the whole segment is < rcv_nxt */
				/* must be a duplicate of a packet that has already been correctly handled */
				tcp_ack_now(pcb);
			}
		}

		/* The sequence number must be within the window (above rcv_nxt
		 and below rcv_nxt + rcv_wnd) in order to be further
		 processed. */
		if (TCP_SEQ_BETWEEN(static_seqno, pcb->rcv_nxt,
				pcb->rcv_nxt + pcb->rcv_wnd - 1))
		{
			if (pcb->rcv_nxt == static_seqno)
			{
				accepted_inseq = 1;
				/* The incoming segment is the next in sequence. We check if
				 we have to trim the end of the segment and update rcv_nxt
				 and pass the data to the application. */
				static_tcplen = TCP_TCPLEN(&static_inseg);

				if (static_tcplen > pcb->rcv_wnd)
				{
					if (static_inseg.tcphdr->FLAGS() & TCP_FIN)
					{
						/* Must remove the FIN from the header as we're trimming
						 * that byte of sequence-space from the packet */
						static_inseg.tcphdr->FLAGS_SET(
								static_inseg.tcphdr->FLAGS() & ~TCP_FIN);
					}
					/* Adjust length of segment to fit in the window. */
					static_inseg.len = pcb->rcv_wnd;
					if (static_inseg.tcphdr->FLAGS() & TCP_SYN)
					{
						static_inseg.len -= 1;
					}
					pbuf_realloc(static_inseg.p, static_inseg.len);
					static_tcplen = TCP_TCPLEN(&static_inseg);
					ASSERT((static_seqno + static_tcplen) == (pcb->rcv_nxt
									+ pcb->rcv_wnd));
				}
				if (pcb->ooseq != NULL)
				{
					if (static_inseg.tcphdr->FLAGS() & TCP_FIN)
					{
						/* Received in-order FIN means anything that was received
						 * out of order must now have been received in-order, so
						 * bin the ooseq queue */
						while (pcb->ooseq != NULL)
						{
							TcpSeg *old_ooseq = pcb->ooseq;
							pcb->ooseq = pcb->ooseq->next;
							delete old_ooseq;
						}
					}
					else if (TCP_SEQ_LEQ(pcb->ooseq->tcphdr->seqno, static_seqno + static_tcplen))
					{
						if (pcb->ooseq->len > 0)
						{
							/* We have to trim the second edge of the incoming segment. */
							ASSERT(
									TCP_SEQ_GT(pcb->ooseq->tcphdr->seqno, static_seqno));
							/* FIN in inseg already handled by dropping whole ooseq queue */
							static_inseg.len
									= (uint16) (pcb->ooseq->tcphdr->seqno
											- static_seqno);
							if (static_inseg.tcphdr->FLAGS() & TCP_SYN)
							{
								static_inseg.len -= 1;
							}
							pbuf_realloc(static_inseg.p, static_inseg.len);
							static_tcplen = TCP_TCPLEN(&static_inseg);
							ASSERT((static_seqno + static_tcplen)
									== pcb->ooseq->tcphdr->seqno);
						}
						else
						{
							/* does the ooseq segment contain only flags that are in inseg also? */
							if ((static_inseg.tcphdr->FLAGS() & (TCP_FIN
									| TCP_SYN)) == (pcb->ooseq->tcphdr->FLAGS()
									& (TCP_FIN | TCP_SYN)))
							{
								TcpSeg *old_ooseq = pcb->ooseq;
								pcb->ooseq = pcb->ooseq->next;
								delete old_ooseq;
							}
						}
					}
				}
				pcb->rcv_nxt = static_seqno + static_tcplen;

				/* Update the receiver's (our) window. */
				ASSERT(pcb->rcv_wnd >= static_tcplen);
				pcb->rcv_wnd -= static_tcplen;

				tcp_update_rcv_ann_wnd(pcb);

				/* If there is data in the segment, we make preparations to
				 pass this up to the application. The ->recv_data variable
				 is used for holding the pbuf that goes to the
				 application. The code for reassembling out-of-sequence data
				 chains its data on this pbuf as well.

				 If the segment was a FIN, we set the TF_GOT_FIN flag that will
				 be used to indicate to the application that the remote side has
				 closed its end of the connection. */
				if (static_inseg.p->tot_len > 0)
				{
					static_recv_data = static_inseg.p;
					/* Since this pbuf now is the responsibility of the
					 application, we delete our reference to it so that we won't
					 (mistakingly) deallocate it. */
					static_inseg.p = NULL;
				}
				if (static_inseg.tcphdr->FLAGS() & TCP_FIN)
				{
					static_recv_flags |= TF_GOT_FIN;
				}

				/* We now check if we have segments on the ->ooseq queue that
				 is now in sequence. */
				while (pcb->ooseq != NULL && pcb->ooseq->tcphdr->seqno
						== pcb->rcv_nxt)
				{

					cseg = pcb->ooseq;
					static_seqno = pcb->ooseq->tcphdr->seqno;

					pcb->rcv_nxt += TCP_TCPLEN(cseg);
					ASSERT(pcb->rcv_wnd >= TCP_TCPLEN(cseg));
					pcb->rcv_wnd -= TCP_TCPLEN(cseg);

					tcp_update_rcv_ann_wnd(pcb);

					if (cseg->p->tot_len > 0)
					{
						/* Chain this pbuf onto the pbuf that we will pass to
						 the application. */
						if (static_recv_data)
						{
							pbuf_cat(static_recv_data, cseg->p);
						}
						else
						{
							static_recv_data = cseg->p;
						}
						cseg->p = NULL;
					}
					if (cseg->tcphdr->FLAGS() & TCP_FIN)
					{
//						printf("tcp_receive: dequeued FIN.\n");
						static_recv_flags |= TF_GOT_FIN;
						if (pcb->state == ESTABLISHED)
						{ /* force passive close or we can move to active close */
							pcb->state = CLOSE_WAIT;
						}
					}
					pcb->ooseq = cseg->next;
					tcp_seg_free(cseg);
				}
				/* Acknowledge the segment(s). */
				tcp_ack(pcb);
			}
			else
			{
				/* We get here if the incoming segment is out-of-sequence. */
				tcp_ack_now(pcb);
				/* We queue the segment on the ->ooseq queue. */
				if (pcb->ooseq == NULL)
				{
					pcb->ooseq = tcp_seg_copy(&static_inseg);
				}
				else
				{
					/* If the queue is not empty, we walk through the queue and
					 try to find a place where the sequence number of the
					 incoming segment is between the sequence numbers of the
					 previous and the next segment on the ->ooseq queue. That is
					 the place where we put the incoming segment. If needed, we
					 trim the second edges of the previous and the incoming
					 segment so that it will fit into the sequence.

					 If the incoming segment has the same sequence number as a
					 segment on the ->ooseq queue, we discard the segment that
					 contains less data. */

					prev = NULL;
					for (next = pcb->ooseq; next != NULL; next = next->next)
					{
						if (static_seqno == next->tcphdr->seqno)
						{
							/* The sequence number of the incoming segment is the
							 same as the sequence number of the segment on
							 ->ooseq. We check the lengths to see which one to
							 discard. */
							if (static_inseg.len > next->len)
							{
								/* The incoming segment is larger than the old
								 segment. We replace the old segment with the new
								 one. */
								cseg = tcp_seg_copy(&static_inseg);
								if (cseg != NULL)
								{
									cseg->next = next->next;
									if (prev != NULL)
									{
										prev->next = cseg;
									}
									else
									{
										pcb->ooseq = cseg;
									}
									tcp_seg_free(next);
									if (cseg->next != NULL)
									{
										next = cseg->next;
										if (TCP_SEQ_GT(static_seqno + cseg->len, next->tcphdr->seqno))
										{
											/* We need to trim the incoming segment. */
											cseg->len
													= (uint16) (next->tcphdr->seqno
															- static_seqno);
											pbuf_realloc(cseg->p, cseg->len);
										}
									}
								}
								break;
							}
							else
							{
								/* Either the lenghts are the same or the incoming
								 segment was smaller than the old one; in either
								 case, we ditch the incoming segment. */
								break;
							}
						}
						else
						{
							if (prev == NULL)
							{
								if (TCP_SEQ_LT(static_seqno, next->tcphdr->seqno))
								{
									/* The sequence number of the incoming segment is lower
									 than the sequence number of the first segment on the
									 queue. We put the incoming segment first on the
									 queue. */

									if (TCP_SEQ_GT(static_seqno + static_inseg.len, next->tcphdr->seqno))
									{
										/* We need to trim the incoming segment. */
										static_inseg.len
												= (uint16) (next->tcphdr->seqno
														- static_seqno);
										pbuf_realloc(static_inseg.p,
												static_inseg.len);
									}
									cseg = tcp_seg_copy(&static_inseg);
									if (cseg != NULL)
									{
										cseg->next = next;
										pcb->ooseq = cseg;
									}
									break;
								}
							}
							else
							/*if (TCP_SEQ_LT(prev->tcphdr->seqno, seqno) &&
							 TCP_SEQ_LT(seqno, next->tcphdr->seqno)) {*/
							if (TCP_SEQ_BETWEEN(static_seqno, prev->tcphdr->seqno+1, next->tcphdr->seqno-1))
							{
								/* The sequence number of the incoming segment is in
								 between the sequence numbers of the previous and
								 the next segment on ->ooseq. We trim and insert the
								 incoming segment and trim the previous segment, if
								 needed. */
								if (TCP_SEQ_GT(static_seqno + static_inseg.len, next->tcphdr->seqno))
								{
									/* We need to trim the incoming segment. */
									static_inseg.len
											= (uint16) (next->tcphdr->seqno
													- static_seqno);
									pbuf_realloc(static_inseg.p,
											static_inseg.len);
								}

								cseg = tcp_seg_copy(&static_inseg);
								if (cseg != NULL)
								{
									cseg->next = next;
									prev->next = cseg;
									if (TCP_SEQ_GT(prev->tcphdr->seqno + prev->len, static_seqno))
									{
										/* We need to trim the prev segment. */
										prev->len = (uint16) (static_seqno
												- prev->tcphdr->seqno);
										pbuf_realloc(prev->p, prev->len);
									}
								}
								break;
							}
							/* If the "next" segment is the last segment on the
							 ooseq queue, we add the incoming segment to the end
							 of the list. */
							if (next->next == NULL
									&& TCP_SEQ_GT(static_seqno, next->tcphdr->seqno))
							{
								next->next = tcp_seg_copy(&static_inseg);
								if (next->next != NULL)
								{
									if (TCP_SEQ_GT(next->tcphdr->seqno + next->len, static_seqno))
									{
										/* We need to trim the last segment. */
										next->len = (uint16) (static_seqno
												- next->tcphdr->seqno);
										pbuf_realloc(next->p, next->len);
									}
								}
								break;
							}
						}
						prev = next;
					}
				}
			}
		}
		else
		{
			tcp_ack_now(pcb);
		}
	}
	else
	{
		/* Segments with length 0 is taken care of here. Segments that
		 fall out of the window are ACKed. */
		/*if (TCP_SEQ_GT(pcb->rcv_nxt, seqno) ||
		 TCP_SEQ_GEQ(seqno, pcb->rcv_nxt + pcb->rcv_wnd)) {*/
		if (!TCP_SEQ_BETWEEN(static_seqno, pcb->rcv_nxt, pcb->rcv_nxt + pcb->rcv_wnd-1))
			tcp_ack_now(pcb);
	}
	return accepted_inseq;
}

/**
 * Parses the options contained in the incoming segment.
 *
 * Called from tcp_listen_input() and tcp_process().
 * Currently, only the MSS option is supported!
 *
 * @param pcb the tcp_pcb for which a segment arrived
 */
void tcp_parseopt(TcpSocket *pcb)
{
	uint16 c, max_c;
	uint16 mss;
	uint8 *opts, opt;
#if LWIP_TCP_TIMESTAMPS
	uint32 tsval;
#endif

	opts = (uint8 *) static_tcphdr + sizeof(TcpHeader);

	/* Parse the TCP MSS option, if present. */
	if (static_tcphdr->HDRLEN() > ((int)(sizeof(TcpHeader) / 4)))
	{
		max_c = (static_tcphdr->HDRLEN() - sizeof(TcpHeader) / 4) << 2;
		for (c = 0; c < max_c;)
		{
			opt = opts[c];
			switch (opt)
			{
			case 0x00:
				/* End of options. */
				return;
			case 0x01:
				/* NOP option. */
				++c;
				break;
			case 0x02:
				if (opts[c + 1] != 0x04 || c + 0x04 > max_c)
				{
					/* Bad length */
//					printf("tcp_parseopt: bad length\n");
					return;
				}
				/* An MSS option with the right option length. */
				mss = (opts[c + 2] << 8) | opts[c + 3];
				/* Limit the mss to the configured TCP_MSS and prevent division by zero */
				pcb->mss = ((mss > TCP_MSS) || (mss == 0)) ? TCP_MSS : mss;
				/* Advance to next option */
				c += 0x04;
				break;
#if LWIP_TCP_TIMESTAMPS
				case 0x08:
				LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: TS\n"));
				if (opts[c + 1] != 0x0A || c + 0x0A> max_c)
				{
					/* Bad length */
					LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
					return;
				}
				/* TCP timestamp option with valid length */
				tsval = (opts[c+2]) | (opts[c+3] << 8) |
				(opts[c+4] << 16) | (opts[c+5] << 24);
				if (flags & TCP_SYN)
				{
					pcb->ts_recent = ntohl(tsval);
					pcb->flags |= TF_TIMESTAMP;
				}
				else if (TCP_SEQ_BETWEEN(pcb->ts_lastacksent, seqno, seqno+tcplen))
				{
					pcb->ts_recent = ntohl(tsval);
				}
				/* Advance to next option */
				c += 0x0A;
				break;
#endif
			default:
				if (opts[c + 1] == 0)
				{
//					printf("tcp_parseopt: bad length\n");
					/* If the length field is zero, the options are malformed
					 and we don't process them further. */
					return;
				}
				/* All other options have a length field, so that we easily
				 can skip past them. */
				c += opts[c + 1];
			}
		}
	}
}

/**
 * Called by tcp_input() when a segment arrives for a listening
 * connection (from tcp_input()).
 *
 * @param pcb the tcp_pcb_listen for which a segment arrived
 * @return ERR_OK if the segment was processed
 *         another int on error
 *
 * @note the return value is not (yet?) used in tcp_input()
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
int tcp_listen_input(TcpSocketListen *pcb)
{
	TcpSocket *npcb;
	int rc;

	/* In the LISTEN state, we check for incoming SYN segments,
	 creates a new PCB, and responds with a SYN|ACK. */
	if (static_flags & TCP_ACK)
	{
		/* For incoming segments with the ACK flag set, respond with a
		 RST. */
		tcp_rst(static_ackno + 1, static_seqno + static_tcplen,
				&(static_iphdr->dest), &(static_iphdr->src),
				static_tcphdr->dest, static_tcphdr->src);
		return ERR_OK;
	}
	else if (static_flags & TCP_SYN)
	{
		if (pcb->accepts_pending >= pcb->backlog)
		{
			//printf("tcp_listen_input: listen backlog exceeded for port %d\n",
			//	static_tcphdr->dest);
			return ERR_ABRT;
		}
		npcb = tcp_alloc(pcb->prio);
		/* If a new PCB could not be created (probably due to lack of memory),
		 we don't do anything, but rely on the sender will retransmit the
		 SYN at a time when we have more memory available. */
		if (!npcb)
			return ERR_MEM;
		pcb->accepts_pending++;
		/* Set up the new PCB. */
		ip_addr_set(&(npcb->local_ip), &(static_iphdr->dest));
		npcb->local_port = pcb->local_port;
		ip_addr_set(&(npcb->remote_ip), &(static_iphdr->src));
		npcb->remote_port = static_tcphdr->src;
		npcb->state = SYN_RCVD;
		npcb->rcv_nxt = static_seqno + 1;
		npcb->rcv_ann_right_edge = npcb->rcv_nxt;
		npcb->snd_wnd = static_tcphdr->wnd;
		npcb->ssthresh = npcb->snd_wnd;
		npcb->snd_wl1 = static_seqno - 1;/* initialise to seqno-1 to force window update */
		npcb->callback_arg = pcb->callback_arg;
		npcb->accept = pcb->accept;
		/* inherit socket options */
		npcb->so_options = pcb->so_options & (SOF_DEBUG | SOF_DONTROUTE
				|SOF_KEEPALIVE | SOF_OOBINLINE | SOF_LINGER);
		/* Register the new PCB so that we can begin receiving segments
		 for it. */
		TCP_REG(&tcp_active_pcbs, npcb);

		/* Parse any options in the SYN. */
		tcp_parseopt(npcb);
		npcb->mss = tcp_eff_send_mss(npcb->mss, &(npcb->remote_ip));

		//		snmp_inc_tcppassiveopens();

		/* Send a SYN|ACK together with the MSS option. */
		rc = tcp_enqueue(npcb, NULL, 0, TCP_SYN | TCP_ACK, 0, TF_SEG_OPTS_MSS
#if LWIP_TCP_TIMESTAMPS
				/* and maybe include the TIMESTAMP option */
				| (npcb->flags & TF_TIMESTAMP ? TF_SEG_OPTS_TS : 0)
#endif
				);
		if (rc != ERR_OK)
		{
			tcp_abandon(npcb, false);
			return rc;
		}
		return tcp_output(npcb);
	}
	return ERR_OK;
}

/**
 * Returns a copy of the given TCP segment.
 * The pbuf and data are not copied, only the pointers
 *
 * @param seg the old tcp_seg
 * @return a copy of seg
 */
TcpSeg *tcp_seg_copy(TcpSeg *seg)
{
	TcpSeg * cseg = new TcpSeg();
	if (cseg == NULL)
		return NULL;
	memcpy(cseg, seg, sizeof(TcpSeg));
	pbuf_ref(cseg->p);
	return cseg;
}

/**
 * Update the state that tracks the available window space to advertise.
 *
 * Returns how much extra window would be advertised if we sent an
 * update now.
 */
uint32 tcp_update_rcv_ann_wnd(TcpSocket *pcb)
{
	uint32 new_right_edge = pcb->rcv_nxt + pcb->rcv_wnd;

	if (TCP_SEQ_GEQ(new_right_edge, pcb->rcv_ann_right_edge + pcb->mss))
	{
		/* we can advertise more window */
		pcb->rcv_ann_wnd = pcb->rcv_wnd;
		return new_right_edge - pcb->rcv_ann_right_edge;
	}
	else
	{
		if (TCP_SEQ_GT(pcb->rcv_nxt, pcb->rcv_ann_right_edge))
		{
			/* Can happen due to other end sending out of advertised window,
			 * but within actual available (but not yet advertised) window */
			pcb->rcv_ann_wnd = 0;
		}
		else
		{
			/* keep the right edge of window constant */
			pcb->rcv_ann_wnd = pcb->rcv_ann_right_edge - pcb->rcv_nxt;
		}
		return 0;
	}
}

/**
 * This function should be called by the application when it has
 * processed the data. The purpose is to advertise a larger window
 * when the data has been processed.
 *
 * @param pcb the tcp_pcb for which data is read
 * @param len the amount of bytes that have been read by the application
 */
void tcp_recved(TcpSocket *pcb, int len)
{
	int wnd_inflation;

	ASSERT(len <= 0xffff - pcb->rcv_wnd);

	pcb->rcv_wnd += len;
	if (pcb->rcv_wnd > TCP_WND)
		pcb->rcv_wnd = TCP_WND;

	wnd_inflation = tcp_update_rcv_ann_wnd(pcb);

	/* If the change in the right edge of window is significant (default
	 * watermark is TCP_WND/2), then send an explicit update now.
	 * Otherwise wait for a packet to be sent in the normal course of
	 * events (or more window to be available later) */
	if (wnd_inflation >= TCP_WND_UPDATE_THRESHOLD)
		tcp_ack_now(pcb);
}

/**
 * Allocate a new TCP local port.
 *
 * @return a new (free) local TCP port number
 */
int tcp_new_port()
{
	TcpSocket *pcb;
#ifndef TCP_LOCAL_PORT_RANGE_START
#define TCP_LOCAL_PORT_RANGE_START 4096
#define TCP_LOCAL_PORT_RANGE_END   0x7fff
#endif
	static int port = TCP_LOCAL_PORT_RANGE_START;

	again: if (++port > TCP_LOCAL_PORT_RANGE_END)
	{
		port = TCP_LOCAL_PORT_RANGE_START;
	}

	for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
	{
		if (pcb->local_port == port)
		{
			goto again;
		}
	}
	for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
	{
		if (pcb->local_port == port)
		{
			goto again;
		}
	}
	for (pcb = tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next)
	{
		if (pcb->local_port == port)
		{
			goto again;
		}
	}
	return port;
}

/**
 * Called by tcp_input() when a segment arrives for a connection in
 * TIME_WAIT.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
int tcp_timewait_input(TcpSocket *pcb)
{
	if (TCP_SEQ_GT(static_seqno + static_tcplen, pcb->rcv_nxt))
		pcb->rcv_nxt = static_seqno + static_tcplen;
	if (static_tcplen > 0)
		tcp_ack_now(pcb);
	return tcp_output(pcb);
}

/**
 * Calcluates the effective send mss that can be used for a specific IP address
 * by using ip_route to determin the netif used to send to the address and
 * calculating the minimum of TCP_MSS and that netif's mtu (if set). This code
 * assumes that source of the interface is not needed because all interfaces on
 * same subnet should have same MSS.
 */
int tcp_eff_send_mss(int sendmss, ip_addr *addr)
{
	Ethernet * outif = ip_route(addr, 0);
	if ((outif) && (outif->mtu != 0))
	{
		int mss_s = outif->mtu - sizeof(IPv4Header) - sizeof(TcpHeader);
		/* RFC 1122, chap 4.2.2.6:
		 * Eff.snd.MSS = min(SendMSS+20, MMS_S) - TCPhdrsize - IPoptionsize
		 * We correct for TCP options in tcp_enqueue(), and don't support
		 * IP options
		 */
		sendmss = min(sendmss, mss_s);
	}
	return sendmss;
}

/**
 * Connects to another host. The function given as the "connected"
 * argument will be called when the connection has been established.
 *
 * @param pcb the tcp_pcb used to establish the connection
 * @param ipaddr the remote ip address to connect to
 * @param port the remote tcp port to connect to
 * @param connected callback function to call when connected (or on error)
 * @return ERR_VAL if invalid arguments are given
 *         ERR_OK if connect request has been sent
 *         other int values if connect request couldn't be sent
 */
int tcp_connect(TcpSocket *pcb, ip_addr *ipaddr, int port, int(* connected)(
		void *arg, TcpSocket *tpcb, int err))
{
	if (pcb->state != CLOSED)
	{
//		printf("tcp_connect: can only connect from state CLOSED\n");
		return ERR_ISCONN;
	}

//	printf("tcp_connect to port %d\n", port);
	if (!ipaddr)
		return ERR_VAL;
	pcb->remote_ip = *ipaddr;
	pcb->remote_port = port;
	if (pcb->local_port == 0)
		pcb->local_port = tcp_new_port();
	uint32 iss = tcp_next_iss();
	pcb->rcv_nxt = 0;
	pcb->snd_nxt = iss;
	pcb->lastack = iss - 1;
	pcb->snd_lbb = iss - 1;
	pcb->rcv_wnd = TCP_WND;
	pcb->rcv_ann_wnd = TCP_WND;
	pcb->rcv_ann_right_edge = pcb->rcv_nxt;
	pcb->snd_wnd = TCP_WND;
	/* As initial send MSS, we use TCP_MSS but limit it to 536.
	 The send MSS is updated when an MSS option is received. */
	pcb->mss = (TCP_MSS > 536) ? 536 : TCP_MSS;
	pcb->mss = tcp_eff_send_mss(pcb->mss, ipaddr);
	pcb->cwnd = 1;
	pcb->ssthresh = pcb->mss * 10;
	pcb->state = SYN_SENT;
	pcb->connected = connected;
	TCP_RMV(&tcp_bound_pcbs, pcb);
	TCP_REG(&tcp_active_pcbs, pcb);

	//	snmp_inc_tcpactiveopens();

	int ret = tcp_enqueue(pcb, NULL, 0, TCP_SYN, 0, TF_SEG_OPTS_MSS
#if LWIP_TCP_TIMESTAMPS
			| TF_SEG_OPTS_TS
#endif
			);
	if (ret == ERR_OK)
		tcp_output(pcb);
	return ret;
}

/**
 * Default receive callback that is called if the user didn't register
 * a recv callback for the pcb.
 */
int tcp_recv_null(void *arg, TcpSocket *pcb, PktBuf *p, int err)
{
	if (p)
		pbuf_free(p);
	else if (err == ERR_OK)
		return tcp_close(pcb);
	return ERR_OK;
	ASSERT(arg);
}

/**
 * Deallocates a list of TCP segments (tcp_seg structures).
 *
 * @param seg tcp_seg list of TCP segments to free
 */
void tcp_segs_free(TcpSeg *seg)
{
	while (seg)
	{
		TcpSeg * next = seg->next;
		tcp_seg_free(seg);
		seg = next;
	}
}

/**
 * Frees a TCP segment (tcp_seg structure).
 *
 * @param seg single tcp_seg to free
 */
void tcp_seg_free(TcpSeg *seg)
{
	if (seg)
	{
		if (seg->p)
			pbuf_free(seg->p);
		delete seg;
	}
}

/**
 * Setup a tcp_pcb with the correct callback function pointers
 * and their arguments.
 *
 * @param conn the TCP netconn to setup
 */
void setup_tcp(NetConn *conn)
{
	TcpSocket *pcb;

	pcb = conn->pcb.tcp;
	tcp_arg(pcb, conn);
	tcp_recv(pcb, recv_tcp);
	tcp_sent(pcb, sent_tcp);
	tcp_poll(pcb, poll_tcp, 4);
	tcp_err(pcb, err_tcp);
}

/**
 * Receive callback function for TCP netconns.
 * Posts the packet to conn->recvmbox, but doesn't delete it on errors.
 *
 * @see tcp.h (struct tcp_pcb.recv) for parameters and return value
 */
int recv_tcp(void *arg, TcpSocket *pcb, PktBuf *p, int err)
{
	ASSERT(pcb);
	ASSERT(arg);
	NetConn * conn = (NetConn *) arg;
	ASSERT(conn->pcb.tcp == pcb);

	if ((conn == NULL) || (conn->recvmbox == 0))
		return ERR_VAL;

	conn->err = err;
	uint16 len;
	if (p != NULL)
	{
		len = p->tot_len;
		conn->recv_avail += len;
	}
	else
	{
		len = 0;
	}
	if (!conn->recvmbox->TryPost(p))
		return ERR_MEM;
	/* Register event with callback */
	API_EVENT(conn, NETCONN_EVT_RCVPLUS, len);
	return ERR_OK;
}

/**
 * Error callback function for TCP netconns.
 * Signals conn->sem, posts to all conn mboxes and calls API_EVENT.
 * The application thread has then to decide what to do.
 *
 * @see tcp.h (struct tcp_pcb.err) for parameters
 */
void err_tcp(void *arg, int err)
{
	NetConn * conn = (NetConn *) arg;
	ASSERT(conn);
	conn->pcb.tcp = NULL;
	conn->err = err;
	if (conn->recvmbox)
	{
		/* Register event with callback */
		API_EVENT(conn, NETCONN_EVT_RCVPLUS, 0);
		conn->recvmbox->Post(NULL, -1);
	}
	if (conn->op_completed && conn->state == NETCONN_CONNECT)
	{
		conn->state = NETCONN_NONE;
		conn->op_completed->SetEvent();
	}
	if (conn->acceptmbox)
	{
		/* Register event with callback */
		API_EVENT(conn, NETCONN_EVT_RCVPLUS, 0);
		conn->acceptmbox->Post(NULL, -1);
	}
	if ((conn->state == NETCONN_WRITE) || (conn->state == NETCONN_CLOSE))
	{
		/* calling do_writemore/do_close_internal is not necessary
		 since the pcb has already been deleted! */
		conn->state = NETCONN_NONE;
		/* wake up the waiting task */
		if (conn->op_completed)
			conn->op_completed->SetEvent();
	}
}

void DebugPCBs()
{
	TcpSocket * ptr = 0;
	TcpSocketListen * ptr1 = 0;
	pprintf("\n bound pcbs");
	for (ptr = tcp_bound_pcbs; ptr != NULL; ptr = ptr->next)
	{
		pprintf("\n local %d %d", ptr->local_port,ptr->state);
	}

	pprintf("\n active pcbs");
	for (ptr = tcp_active_pcbs; ptr != NULL; ptr = ptr->next)
	{
		pprintf("\n local %d %d", ptr->local_port, ptr->state);
	}

	pprintf("\n TW pcbs");
	for (ptr = tcp_tw_pcbs; ptr != NULL; ptr = ptr->next)
	{
		pprintf("\n local %d %d", ptr->local_port, ptr->state);
	}

	pprintf("\n IP pcbs");
	for (ptr = tcp_input_pcb; ptr != NULL; ptr = ptr->next)
	{
		pprintf("\n local %d %d", ptr->local_port, ptr->state);
	}
	/*
	{

	TcpSocketListen *listen_pcbs;
	TcpSocket *pcbs;
	};
	 */
	pprintf("\n Listen pcbs");
	for (ptr1 = tcp_listen_pcbs.listen_pcbs; ptr1 != NULL; ptr1 = ptr1->next)
	{
		pprintf("\n local %d %d", ptr1->local_port, ptr1->state);
	}
}

