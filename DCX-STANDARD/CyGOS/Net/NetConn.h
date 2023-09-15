// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/NetConn.h_v   1.0   29 Oct 2010 10:39:34   ygupta  $
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

#ifndef NETCONN_H_
#define NETCONN_H_

#include "ArchDefs.h"

/* Flags for netconn_write */
#define NETCONN_NOFLAG 0x00
#define NETCONN_NOCOPY 0x00 /* Only for source code compatibility */
#define NETCONN_COPY   0x01
#define NETCONN_MORE   0x02

enum netconn_type
{
	NETCONN_INVALID = 0,
	/* NETCONN_TCP Group */
	NETCONN_TCP = 0x10,
	/* NETCONN_UDP Group */
	NETCONN_UDP = 0x20, NETCONN_UDPNOCHKSUM = 0x22,
	/* NETCONN_RAW Group */
	NETCONN_RAW = 0x40
};

#define NETCONNTYPE_GROUP(t)    (t&0xF0)
#define NETCONNTYPE_DATAGRAM(t) (t&0xE0)

enum netconn_state
{
	NETCONN_NONE, NETCONN_WRITE, NETCONN_LISTEN, NETCONN_CONNECT, NETCONN_CLOSE
};

enum netconn_evt
{
	NETCONN_EVT_RCVPLUS,
	NETCONN_EVT_RCVMINUS,
	NETCONN_EVT_SENDPLUS,
	NETCONN_EVT_SENDMINUS
};

enum netconn_igmp
{
	NETCONN_JOIN, NETCONN_LEAVE
};

class Socket;
class TcpSocket;
class UdpSocket;
class RawSocket;
class Event;
class MailBox;

struct NetConn;
struct NetBuf;
struct ip_addr;

struct api_msg_msg
{
	/** The netconn which to process - always needed: it includes the semaphore
	 which is used to block the application thread until the function finished. */
	NetConn *conn;
	/** Depending on the executed function, one of these union members is used */
	union
	{
		/** used for do_send */
		NetBuf *b;
		/** used for do_newconn */
		struct
		{
			int proto;
		} n;
		/** used for do_bind and do_connect */
		struct
		{
			ip_addr *ipaddr;
			int port;
		} bc;
		/** used for do_getaddr */
		struct
		{
			ip_addr *ipaddr;
			int *port;
			int local;
		} ad;
		/** used for do_write */
		struct
		{
			const void *dataptr;
			size_t len;
			int apiflags;
		} w;
		/** used ofr do_recv */
		struct
		{
			int len;
		} r;
		/** used for do_join_leave_group */
		struct
		{
			ip_addr *multiaddr;
			ip_addr *interfac;
			netconn_igmp join_or_leave;
		} jl;
		struct
		{
			int backlog;
		} lb;
	} msg;
};

struct api_msg
{
	/** function to execute in tcpip_thread context */
	void (* function)(api_msg_msg *msg);
	/** arguments for this function */
	api_msg_msg msg;
};

typedef void (* netconn_callback)(NetConn *, enum netconn_evt, int len);

struct NetConn
{
	/** type of the netconn (TCP, UDP or RAW) */
	enum netconn_type type;
	/** current state of the netconn */
	enum netconn_state state;
	/** the lwIP internal protocol control block */
	union PCB
	{
		Socket *ip;
		TcpSocket *tcp;
		UdpSocket *udp;
		RawSocket *raw;
	};
	PCB pcb;
	/** the last error this netconn had */
	int err;
	/** sem that is used to synchroneously execute functions in the core context */
	Event * op_completed;
	/** mbox where received packets are stored until they are fetched
	 by the netconn application thread (can grow quite big) */
	MailBox * recvmbox;
	/** mbox where new connections are stored until processed
	 by the application thread */
	MailBox * acceptmbox;
	/** only used for socket layer */
	int socket;
	/** timeout to wait for new data to be received (or connections to arrive
	 * for listening netconns). 0 is infinite wait, >0 is in milliseconds */
	int recv_timeout;
	/** maximum amount of bytes queued in recvmbox */
	int recv_bufsize;
	int16 recv_avail;
	/** TCP: when data passed to netconn_write doesn't fit into the send buffer,
	 this temporarily stores the message. */
	api_msg_msg *write_msg;
	/** TCP: when data passed to netconn_write doesn't fit into the send buffer,
	 this temporarily stores how much is already sent. */
	size_t write_offset;
#if LWIP_TCPIP_CORE_LOCKING
	/** TCP: when data passed to netconn_write doesn't fit into the send buffer,
	 this temporarily stores whether to wake up the original application task
	 if data couldn't be sent in the first try. */
	uint8 write_delayed;
#endif /* LWIP_TCPIP_CORE_LOCKING */
	/** A callback function that is informed about events for this netconn */
	netconn_callback callback;
};

#define netconn_new(t)                  netconn_new_with_proto_and_callback(t, 0, NULL)
#define netconn_new_with_callback(t, c) netconn_new_with_proto_and_callback(t, 0, c)
#define netconn_type(conn) (conn->type)

void API_EVENT(NetConn * c, netconn_evt e, int l);

NetConn * netconn_accept(NetConn *conn);
NetConn * netconn_new_with_proto_and_callback(netconn_type t, int proto,
		netconn_callback callback);
int netconn_delete(NetConn *conn);
void netconn_free(NetConn *conn);
NetConn * netconn_alloc(netconn_type t, netconn_callback callback);
int netconn_bind(NetConn *conn, ip_addr *addr, int port);
int netconn_listen_with_backlog(NetConn *conn, int backlog);
NetBuf * netconn_recv(NetConn *conn);
int netconn_getaddr(NetConn *conn, ip_addr *addr, int *port, int local);
int netconn_send(NetConn *conn, NetBuf *buf);
int
netconn_write(NetConn *conn, const void *dataptr, size_t size, int apiflags);
int netconn_connect(NetConn *conn, ip_addr *addr, int port);
int netconn_disconnect(NetConn *conn);

int netconn_getaddr(NetConn *conn, ip_addr *addr, int *port, bool local);
#define netconn_peer(c,i,p) netconn_getaddr(c,i,p,0)
#define netconn_addr(c,i,p) netconn_getaddr(c,i,p,1)

void do_delconn(api_msg_msg *msg);
void do_newconn(api_msg_msg *msg);
void do_close_internal(NetConn *conn);
int do_writemore(NetConn *conn);
void do_bind(api_msg_msg *msg);
void do_listen(api_msg_msg *msg);
void do_getaddr(api_msg_msg *msg);
void do_recv(api_msg_msg *msg);
void do_write(api_msg_msg *msg);
void do_send(api_msg_msg *msg);
void do_connect(api_msg_msg *msg);
void do_disconnect(api_msg_msg *msg);
int do_connected(void *arg, TcpSocket *pcb, int err);

int pcb_new(api_msg_msg *msg);

int tcpip_apimsg(api_msg *apimsg);

int accept_function(void *arg, TcpSocket *newpcb, int err);

#endif
