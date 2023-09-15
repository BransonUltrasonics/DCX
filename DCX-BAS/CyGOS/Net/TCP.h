// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/TCP.h_v   1.0   21 Mar 2012 12:42:44   PDwivedi  $
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

#ifndef TCP_H_
#define TCP_H_

#include "ArchDefs.h"
#include "Net.h"

class Ethernet;
class NetConn;

struct PktBuf;
struct ip_addr;

enum tcp_state
{
	CLOSED = 0,
	LISTEN = 1,
	SYN_SENT = 2,
	SYN_RCVD = 3,
	ESTABLISHED = 4,
	FIN_WAIT_1 = 5,
	FIN_WAIT_2 = 6,
	CLOSE_WAIT = 7,
	CLOSING = 8,
	LAST_ACK = 9,
	TIME_WAIT = 10
};

#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

#define TCP_FLAGS 0x3fU

#define TF_RESET     (uint8)0x08U   /* Connection was reset. */
#define TF_CLOSED    (uint8)0x10U   /* Connection was sucessfully closed. */
#define TF_GOT_FIN   (uint8)0x20U   /* Connection was closed by the remote end. */

/*
 * Option flags per-socket. These are the same like SO_XXX.
 */
#define SOF_DEBUG       (uint16)0x0001U    /* turn on debugging info recording */
#define SOF_ACCEPTCONN  (uint16)0x0002U    /* socket has had listen() */
#define SOF_REUSEADDR   (uint16)0x0004U    /* allow local address reuse */
#define SOF_KEEPALIVE   (uint16)0x0008U    /* keep connections alive */
#define SOF_DONTROUTE   (uint16)0x0010U    /* just use interface addresses */
#define SOF_BROADCAST   (uint16)0x0020U    /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define SOF_USELOOPBACK (uint16)0x0040U    /* bypass hardware when possible */
#define SOF_LINGER      (uint16)0x0080U    /* linger on close if data present */
#define SOF_OOBINLINE   (uint16)0x0100U    /* leave received OOB data in line */
#define SOF_REUSEPORT   (uint16)0x0200U    /* allow local address & port reuse */

#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127

#define TCP_SND_BUF             2048

/* TCP sender buffer space (pbufs). This must be at least = 2 *
 TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN       (4 * TCP_SND_BUF/TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
 to TCP_SND_BUF. It is the amount of space which must be
 available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT           (TCP_SND_BUF/2)

struct TcpHeader
{
	MisAlignedShort src;
	MisAlignedShort dest;
	MisAlignedLong seqno;
	MisAlignedLong ackno;
	MisAlignedShort _hdrlen_rsvd_flags;
	MisAlignedShort wnd;
	MisAlignedShort chksum;
	MisAlignedShort urgp;
	int OFFSET()
	{
		return ntohs(_hdrlen_rsvd_flags) >> 8;
	}
	int HDRLEN()
	{
		return ntohs(_hdrlen_rsvd_flags) >> 12;
	}
	int FLAGS()
	{
		return ntohs(_hdrlen_rsvd_flags) & TCP_FLAGS;
	}
	void OFFSET_SET(int offset)
	{
		_hdrlen_rsvd_flags = htons((offset << 8) | FLAGS());
	}
	void HDRLEN_SET(int len)
	{
		_hdrlen_rsvd_flags = htons((len << 12) | FLAGS());
	}
	void FLAGS_SET(int flags)
	{
		_hdrlen_rsvd_flags = htons((ntohs(_hdrlen_rsvd_flags) & ~TCP_FLAGS)
				| flags);
	}
	void SET_FLAG(int flags)
	{
		_hdrlen_rsvd_flags = htons(ntohs(_hdrlen_rsvd_flags) | flags);
	}
	void UNSET_FLAG(int flags)
	{
		_hdrlen_rsvd_flags = htons(ntohs(_hdrlen_rsvd_flags) | (FLAGS()
				& ~flags));
	}
};

struct TcpSeg
{
	TcpSeg *next; /* used when putting segements on a queue */
	PktBuf *p; /* buffer containing data + TCP header */
	void *dataptr; /* pointer to the TCP data in the pbuf */
	int len; /* the TCP length of this segment */
	int flags;
#define TF_SEG_OPTS_MSS   (uint8)0x01U   /* Include MSS option. */
#define TF_SEG_OPTS_TS    (uint8)0x02U   /* Include timestamp option. */
	TcpHeader *tcphdr; /* the TCP header */
};

#define TCP_PCB_COMMON(type) \
  type *next; /* for the linked list */ \
  enum tcp_state state; /* TCP state */ \
  /* ports are in host byte order */ \
  int local_port; \
  int prio; \
  void *callback_arg; \
  /* the accept callback for listen- and normal pcbs, if LWIP_CALLBACK_API */ \
  int (* accept)(void *arg, TcpSocket *newpcb, int err)

class TcpSocket;

class TcpSocketListen: public Socket
{
public:
	/* Protocol specific PCB members */
	TCP_PCB_COMMON(TcpSocketListen);
	int backlog;
	int accepts_pending;
};

class TcpSocket: public Socket
{
public:
	TCP_PCB_COMMON(TcpSocket);
	/* ports are in host byte order */
	int remote_port;
	int flags;
#define TF_ACK_DELAY   ((uint8)0x01U)   /* Delayed ACK. */
#define TF_ACK_NOW     ((uint8)0x02U)   /* Immediate ACK. */
#define TF_INFR        ((uint8)0x04U)   /* In fast recovery. */
#define TF_TIMESTAMP   ((uint8)0x08U)   /* Timestamp option enabled */
#define TF_FIN         ((uint8)0x20U)   /* Connection was closed locally (FIN segment enqueued). */
#define TF_NODELAY     ((uint8)0x40U)   /* Disable Nagle algorithm */
#define TF_NAGLEMEMERR ((uint8)0x80U)   /* nagle enabled, memerr, try to output to prevent delayed ACK to happen */

	/* the rest of the fields are in host byte order
	 as we have to do some math with them */
	/* receiver variables */
	uint32 rcv_nxt; /* next seqno expected */
	uint16 rcv_wnd; /* receiver window available */
	uint16 rcv_ann_wnd; /* receiver window to announce */
	uint32 rcv_ann_right_edge; /* announced right edge of window */

	/* Timers */
	uint32 tmr;
	int polltmr, pollinterval;

	/* Retransmission timer. */
	int16 rtime;

	uint16 mss; /* maximum segment size */

	/* RTT (round trip time) estimation variables */
	uint32 rttest; /* RTT estimate in 500ms ticks */
	uint32 rtseq; /* sequence number being timed */
	int16 sa, sv; /* @todo document this */

	int rto; /* retransmission time-out */
	int nrtx; /* number of retransmissions */
	int dupacks;
	/* fast retransmit/recovery */
	uint32 lastack; /* Highest acknowledged seqno. */

	/* congestion avoidance/control variables */
	uint16 cwnd;
	uint16 ssthresh;

	/* sender variables */
	uint32 snd_nxt; /* next new seqno to be sent */
	uint32 snd_wl1, snd_wl2; /* Sequence and acknowledgement numbers of last
	 window update. */
	uint32 snd_lbb; /* Sequence number of next byte to be buffered. */
	uint16 snd_wnd; /* sender window */
	uint16 acked;

	uint16 snd_buf; /* Available buffer space for sending (in bytes). */
#define TCP_SNDQUEUELEN_OVERFLOW (0xffff-3)
	uint16 snd_queuelen; /* Available buffer space for sending (in tcp_segs). */

	/* These are ordered by sequence number: */
	TcpSeg *unsent; /* Unsent (queued) segments. */
	TcpSeg *unacked; /* Sent but unacknowledged segments. */
	TcpSeg *ooseq; /* Received out of sequence segments. */

	PktBuf *refused_data; /* Data previously received but not yet taken by upper layer */

	/* Function to be called when more send buffer space is available.
	 * @param arg user-supplied argument (tcp_pcb.callback_arg)
	 * @param pcb the tcp_pcb which has send buffer space available
	 * @param space the amount of bytes available
	 * @return ERR_OK: try to send some data by calling tcp_output
	 */
	int (* sent)(void *arg, TcpSocket *pcb, int space);

	/* Function to be called when (in-sequence) data has arrived.
	 * @param arg user-supplied argument (tcp_pcb.callback_arg)
	 * @param pcb the tcp_pcb for which data has arrived
	 * @param p the packet buffer which arrived
	 * @param err an error argument (TODO: that is current always ERR_OK?)
	 * @return ERR_OK: try to send some data by calling tcp_output
	 */
	int (* recv)(void *arg, TcpSocket *pcb, PktBuf *p, int err);

	/* Function to be called when a connection has been set up.
	 * @param arg user-supplied argument (tcp_pcb.callback_arg)
	 * @param pcb the tcp_pcb that now is connected
	 * @param err an error argument (TODO: that is current always ERR_OK?)
	 * @return value is currently ignored
	 */
	int (* connected)(void *arg, TcpSocket *pcb, int err);

	/* Function which is called periodically.
	 * The period can be adjusted in multiples of the TCP slow timer interval
	 * by changing tcp_pcb.polltmr.
	 * @param arg user-supplied argument (tcp_pcb.callback_arg)
	 * @param pcb the tcp_pcb to poll for
	 * @return ERR_OK: try to send some data by calling tcp_output
	 */
	int (* poll)(void *arg, TcpSocket *pcb);

	/* Function to be called whenever a fatal error occurs.
	 * There is no pcb parameter since most of the times, the pcb is
	 * already deallocated (or there is no pcb) when this function is called.
	 * @param arg user-supplied argument (tcp_pcb.callback_arg)
	 * @param err an indication why the error callback is called:
	 *            ERR_ABRT: aborted through tcp_abort or by a TCP timer
	 *            ERR_RST: the connection was reset by the remote host
	 */
	void (* errf)(void *arg, int err);

#if LWIP_TCP_TIMESTAMPS
	uint32 ts_lastacksent;
	uint32 ts_recent;
#endif /* LWIP_TCP_TIMESTAMPS */

	/* idle time before KEEPALIVE is sent */
	uint32 keep_idle;
#if LWIP_TCP_KEEPALIVE
	uint32 keep_intvl;
	uint32 keep_cnt;
#endif /* LWIP_TCP_KEEPALIVE */

	/* Persist timer counter */
	uint32 persist_cnt;
	/* Persist timer back-off */
	int persist_backoff;
	/* KEEPALIVE counter */
	int keep_cnt_sent;
};

void tcp_init();
void tcp_input(PktBuf *p, Ethernet *inp);
void tcp_pcb_remove(TcpSocket **pcblist, TcpSocket *pcb);
int tcp_listen_input(TcpSocketListen * pcb);
int tcp_timewait_input(TcpSocket *pcb);
int tcp_process(TcpSocket *pcb);
int tcp_output(TcpSocket *pcb);
void tcp_rst(uint32 seqno, uint32 ackno, ip_addr *local_ip, ip_addr *remote_ip,
		int local_port, int remote_port);
void tcp_parseopt(TcpSocket *pcb);
TcpSocket * tcp_alloc(int prio);
int tcp_enqueue(TcpSocket *pcb, void *arg, int len, int flags, int apiflags,
		int optflags);
void tcp_abandon(TcpSocket *pcb, bool reset);
int tcp_eff_send_mss(int sendmss, ip_addr *addr);
void tcp_tmr();
void tcp_zero_window_probe(TcpSocket *pcb);
void tcp_rexmit_rto(TcpSocket *pcb);
void tcp_keepalive(TcpSocket *pcb);
void tcp_pcb_purge(TcpSocket *pcb);
TcpHeader * tcp_output_set_header(TcpSocket *pcb, PktBuf *p, int optlen,
		uint32 seqno_be);
void tcp_output_segment(TcpSeg *seg, TcpSocket *pcb);
int tcp_receive(TcpSocket *pcb);
void tcp_rexmit(TcpSocket *pcb);
uint32 tcp_update_rcv_ann_wnd(TcpSocket *pcb);
TcpSeg *tcp_seg_copy(TcpSeg *seg);
void tcp_kill_timewait();
void tcp_kill_prio(int prio);
uint32 tcp_next_iss();
int tcp_recv_null(void *arg, TcpSocket *pcb, PktBuf *p, int err);
int tcp_close(TcpSocket *pcb);
int tcp_send_ctrl(TcpSocket *pcb, int flags);

inline void tcp_abort(TcpSocket * pcb)
{
	tcp_abandon(pcb, true);
}

inline void tcp_accepted(TcpSocket * pcb)
{
	TcpSocketListen * lpcb = (TcpSocketListen *) pcb;
	lpcb->accepts_pending--;
}

int tcp_accept_null(void *arg, TcpSocket *pcb, int err);
int tcp_recv_null(void *arg, TcpSocket *pcb, PktBuf *p, int err);

TcpSocket * tcp_new();
int tcp_bind(TcpSocket *pcb, ip_addr *ipaddr, int port);
int tcp_new_port();
TcpSocket * tcp_listen_with_backlog(TcpSocket *pcb, int backlog);
void tcp_accept(TcpSocket *pcb, int(* accept)(void *arg, TcpSocket *newpcb,
		int err));
void tcp_setprio(TcpSocket *pcb, int prio);
void tcp_arg(TcpSocket *pcb, void *arg);
void tcp_recv(TcpSocket *pcb, int(* recv)(void *arg, TcpSocket *tpcb,
		PktBuf *p, int err));
void tcp_err(TcpSocket *pcb, void(* errf)(void *arg, int err));
void tcp_poll(TcpSocket *pcb, int(* poll)(void *arg, TcpSocket *tpcb),
		int interval);
int tcp_write(TcpSocket *pcb, const void *data, int len, int apiflags);
void tcp_sent(TcpSocket *pcb, int(* sent)(void *arg, TcpSocket *tpcb, int len));
void tcp_recved(TcpSocket *pcb, int len);
void tcp_accept(TcpSocket *pcb, int(* accept)(void *arg, TcpSocket *newpcb,
		int err));
int tcp_connect(TcpSocket *pcb, ip_addr *ipaddr, int port, int(* connected)(
		void *arg, TcpSocket *tpcb, int err));
void tcp_segs_free(TcpSeg *seg);
void tcp_seg_free(TcpSeg *seg);
void tcp_recv(TcpSocket *pcb, int(* recv)(void *arg, TcpSocket *tpcb,
		PktBuf *p, int err));
void setup_tcp(NetConn *conn);
int recv_tcp(void *arg, TcpSocket *pcb, PktBuf *p, int err);
int sent_tcp(void *arg, TcpSocket *pcb, int len);
int poll_tcp(void *arg, TcpSocket *pcb);
void err_tcp(void *arg, int err);

/**
 * This is the Nagle algorithm: try to combine user data to send as few TCP
 * segments as possible. Only send if
 * - no previously transmitted data on the connection remains unacknowledged or
 * - the TF_NODELAY flag is set (nagle algorithm turned off for this pcb) or
 * - the only unsent segment is at least pcb->mss bytes long (or there is more
 *   than one unsent segment - with lwIP, this can happen although unsent->len < mss)
 */
#define tcp_do_output_nagle(tpcb) ((((tpcb)->unacked == NULL) || \
                            ((tpcb)->flags & TF_NODELAY) || \
                            (((tpcb)->unsent != NULL) && (((tpcb)->unsent->next != NULL) || \
                              ((tpcb)->unsent->len >= (tpcb)->mss))) \
                            ) ? 1 : 0)
#define tcp_output_nagle(tpcb) (tcp_do_output_nagle(tpcb) ? tcp_output(tpcb) : ERR_OK)

extern TcpSocket *tcp_active_pcbs;
extern void DebugPCBs();
#endif
