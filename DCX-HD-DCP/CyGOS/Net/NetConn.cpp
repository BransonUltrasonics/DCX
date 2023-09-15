// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/NetConn.cpp_v   1.2   16 Aug 2011 08:51:48   pdwivedi  $
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

#include "NetConn.h"
#include "Net.h"
#include "TCP.h"
#include "Event.h"
#include "NetBuf.h"
#include "PktBuf.h"
#include "Raw.h"
#include "UDP.h"
#include "IGMP.h"
#include "MailBox.h"
#include "stdio.h"
#include "assert.h"
#include "DebugNew.h"

/* Default UDP Receive Buffer Size. So many bytes of data
 will be queued before any recv_from calls are made. Newer
 packets arriving after buffer is full will be trashed */
#define RECV_BUFSIZE_DEFAULT            65535

static inline void TCPIP_APIMSG(api_msg * msg)
{
	tcpip_apimsg(msg);
}

inline static void TCPIP_APIMSG_ACK(api_msg_msg * msg)
{
	msg->conn->op_completed->SetEvent();
}

void API_EVENT(NetConn * c, netconn_evt e, int l)
{
	if (c->callback)
		c->callback(c, e, l);
}

/**
 * Create a new netconn (of a specific type) that has a callback function.
 * The corresponding pcb is NOT created!
 *
 * @param t the type of 'connection' to create (@see enum netconn_type)
 * @param proto the IP protocol for RAW IP pcbs
 * @param callback a function to call on status changes (RX available, TX'ed)
 * @return a newly allocated struct netconn or
 *         NULL on memory error
 */
NetConn * netconn_alloc(netconn_type t, netconn_callback callback)
{
	NetConn *conn;

	conn = new NetConn();
	if (conn == NULL)
	{
		return NULL;
	}

	conn->err = ERR_OK;
	conn->type = t;
	conn->pcb.tcp = NULL;

	if ((conn->recvmbox = new MailBox(1000)) == 0)
	{
		delete conn;
		return NULL;
	}

	conn->acceptmbox = 0;
	conn->state = NETCONN_NONE;
	/* initialize socket to -1 since 0 is a valid socket */
	conn->socket = -1;
	conn->callback = callback;
	conn->recv_avail = 0;
	conn->write_msg = NULL;
	conn->write_offset = 0;
#if LWIP_TCPIP_CORE_LOCKING
	conn->write_delayed = 0;
#endif /* LWIP_TCPIP_CORE_LOCKING */
	conn->recv_timeout = 0;
	conn->recv_bufsize = RECV_BUFSIZE_DEFAULT;
	return conn;
}

/**
 * Close a netconn 'connection' and free its resources.
 * UDP and RAW connection are completely closed, TCP pcbs might still be in a waitstate
 * after this returns.
 *
 * @param conn the netconn to delete
 * @return ERR_OK if the connection was deleted
 */
int netconn_delete(NetConn *conn)
{
	api_msg msg;

	/* No ASSERT here because possible to get a (conn == NULL) if we got an accept error */
	if (conn == NULL)
	{
		return ERR_OK;
	}

	msg.function = do_delconn;
	msg.msg.conn = conn;
	tcpip_apimsg(&msg);

	conn->pcb.tcp = NULL;
	netconn_free(conn);

	return ERR_OK;
}

/**
 * Create a new netconn (of a specific type) that has a callback function.
 * The corresponding pcb is also created.
 *
 * @param t the type of 'connection' to create (@see enum netconn_type)
 * @param proto the IP protocol for RAW IP pcbs
 * @param callback a function to call on status changes (RX available, TX'ed)
 * @return a newly allocated struct netconn or
 *         NULL on memory error
 */
NetConn * netconn_new_with_proto_and_callback(netconn_type t, int proto,
		netconn_callback callback)
{
	NetConn *conn;
	api_msg msg;

	conn = netconn_alloc(t, callback);
	if (conn != NULL)
	{
		msg.function = do_newconn;
		msg.msg.msg.n.proto = proto;
		msg.msg.conn = conn;
		tcpip_apimsg(&msg);

		if (conn->err != ERR_OK)
		{
			ASSERT(conn->pcb.tcp == NULL);
			ASSERT(conn->op_completed != 0);
			ASSERT(conn->recvmbox != 0);
			ASSERT(conn->acceptmbox == 0);
			delete conn->recvmbox;
			delete conn;
			return NULL;
		}
	}
	return conn;
}

/**
 * Delete a netconn and all its resources.
 * The pcb is NOT freed (since we might not be in the right thread context do this).
 *
 * @param conn the netconn to free
 */
void netconn_free(NetConn *conn)
{
	void *mem;
	ASSERT(conn->pcb.tcp == NULL);

	/* Drain the recvmbox. */
	if (conn->recvmbox)
	{
		while (conn->recvmbox->TryFetch(&mem))
		{
			if (conn->type == NETCONN_TCP)
			{
				if (mem != NULL)
				{
					pbuf_free((PktBuf *) mem);
				}
			}
			else
			{
				netbuf_delete((NetBuf *) mem);
			}
		}
		delete conn->recvmbox;
		conn->recvmbox = 0;
	}

	/* Drain the acceptmbox. */
	if (conn->acceptmbox)
	{
		while (conn->acceptmbox->TryFetch(&mem))
		{
			netconn_delete((NetConn *) mem);
		}
		delete conn->acceptmbox;
		conn->acceptmbox = 0;
	}
	conn->op_completed = 0;
	delete conn;
}

/**
 * Delete the pcb inside a netconn.
 * Called from netconn_delete.
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_delconn(api_msg_msg *msg)
{
	if (msg->conn->pcb.tcp != NULL)
	{
		switch (NETCONNTYPE_GROUP(msg->conn->type))
		{
		case NETCONN_RAW:
			raw_remove(msg->conn->pcb.raw);
			break;
		case NETCONN_UDP:
			msg->conn->pcb.udp->recv_arg = NULL;
			udp_remove(msg->conn->pcb.udp);
			break;
		case NETCONN_TCP:
			msg->conn->state = NETCONN_CLOSE;
			do_close_internal(msg->conn);
			/* API_EVENT is called inside do_close_internal, before releasing
			 the application thread, so we can return at this point! */
			return;
		default:
			break;
		}
	}
	/* tcp netconns don't come here! */

	/* Trigger select() in socket layer. This send should something else so the
	 errorfd is set, not the read and write fd! */
	API_EVENT(msg->conn, NETCONN_EVT_RCVPLUS, 0);
	API_EVENT(msg->conn, NETCONN_EVT_SENDPLUS, 0);

	if (msg->conn->op_completed)
		msg->conn->op_completed->SetEvent();
}

/**
 * Create a new pcb of a specific type inside a netconn.
 * Called from netconn_new_with_proto_and_callback.
 *
 * @param msg the api_msg_msg describing the connection type
 */
void do_newconn(api_msg_msg *msg)
{
	if (msg->conn->pcb.tcp == NULL)
		pcb_new(msg);
	/* Else? This "new" connection already has a PCB allocated. */
	/* Is this an error condition? Should it be deleted? */
	/* We currently just are happy and return. */
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Internal helper function to close a TCP netconn: since this sometimes
 * doesn't work at the first attempt, this function is called from multiple
 * places.
 *
 * @param conn the TCP netconn to close
 */
void do_close_internal(struct NetConn *conn)
{
	int err;

	ASSERT(conn != NULL);
	ASSERT(conn->type == NETCONN_TCP);
	ASSERT(conn->state == NETCONN_CLOSE);
	ASSERT(conn->pcb.tcp != NULL);

	/* Set back some callback pointers */
	tcp_arg(conn->pcb.tcp, NULL);
	if (conn->pcb.tcp->state == LISTEN)
	{
		tcp_accept(conn->pcb.tcp, NULL);
	}
	else
	{
		tcp_recv(conn->pcb.tcp, NULL);
		tcp_accept(conn->pcb.tcp, NULL);
		/* some callbacks have to be reset if tcp_close is not successful */
		tcp_sent(conn->pcb.tcp, NULL);
		tcp_poll(conn->pcb.tcp, NULL, 4);
		tcp_err(conn->pcb.tcp, NULL);
	}
	/* Try to close the connection */
	err = tcp_close(conn->pcb.tcp);
	if (err == ERR_OK)
	{
		/* Closing succeeded */
		conn->state = NETCONN_NONE;
		/* Set back some callback pointers as conn is going away */
		conn->pcb.tcp = NULL;
		conn->err = ERR_OK;
		/* Trigger select() in socket layer. This send should something else so the
		 errorfd is set, not the read and write fd! */
		API_EVENT(conn, NETCONN_EVT_RCVPLUS, 0);
		API_EVENT(conn, NETCONN_EVT_SENDPLUS, 0);
		/* wake up the application task */
		conn->op_completed->SetEvent();
	}
	else
	{
		/* Closing failed, restore some of the callbacks */
		/* Closing of listen pcb will never fail! */
		ASSERT(conn->pcb.tcp->state != LISTEN);
		tcp_sent(conn->pcb.tcp, sent_tcp);
		tcp_poll(conn->pcb.tcp, poll_tcp, 4);
		tcp_err(conn->pcb.tcp, err_tcp);
		tcp_arg(conn->pcb.tcp, conn);
	}
	/* If closing didn't succeed, we get called again either
	 from poll_tcp or from sent_tcp */
}

/**
 * Poll callback function for TCP netconns.
 * Wakes up an application thread that waits for a connection to close
 * or data to be sent. The application thread then takes the
 * appropriate action to go on.
 *
 * Signals the conn->sem.
 * netconn_close waits for conn->sem if closing failed.
 *
 * @see tcp.h (struct tcp_pcb.poll) for parameters and return value
 */
int poll_tcp(void *arg, TcpSocket *pcb)
{
	NetConn *conn = (NetConn *) arg;
	ASSERT(conn);
	switch (conn->state)
	{
	case NETCONN_WRITE:
		do_writemore(conn);
		break;
	case NETCONN_CLOSE:
		do_close_internal(conn);
		break;
	default:
		break;
	}
	return ERR_OK;
	ASSERT(pcb);
}

/**
 * See if more data needs to be written from a previous call to netconn_write.
 * Called initially from do_write. If the first call can't send all data
 * (because of low memory or empty send-buffer), this function is called again
 * from sent_tcp() or poll_tcp() to send more data. If all data is sent, the
 * blocking application thread (waiting in netconn_write) is released.
 *
 * @param conn netconn (that is currently in state NETCONN_WRITE) to process
 * @return ERR_OK
 *         ERR_MEM if LWIP_TCPIP_CORE_LOCKING=1 and sending hasn't yet finished
 */
int do_writemore(NetConn *conn)
{
	int err;
	void *dataptr;
	uint16 len, available;
	bool write_finished = false;
	size_t diff;

	ASSERT(conn->state == NETCONN_WRITE);

	dataptr = (uint8*) conn->write_msg->msg.w.dataptr + conn->write_offset;
	diff = conn->write_msg->msg.w.len - conn->write_offset;
	if (diff > 0xffffUL)
	{ /* max_u16_t */
		len = 0xffff;
#if LWIP_TCPIP_CORE_LOCKING
		conn->write_delayed = 1;
#endif
	}
	else
	{
		len = (uint16) diff;
	}
	available = conn->pcb.tcp->snd_buf;
	if (available < len)
	{
		/* don't try to write more than sendbuf */
		len = available;
#if LWIP_TCPIP_CORE_LOCKING
		conn->write_delayed = 1;
#endif
	}

	err = tcp_write(conn->pcb.tcp, dataptr, len,
			conn->write_msg->msg.w.apiflags);
	ASSERT((conn->write_offset + len) <= conn->write_msg->msg.w.len);
	if (err == ERR_OK)
	{
		conn->write_offset += len;
		if (conn->write_offset == conn->write_msg->msg.w.len)
		{
			/* everything was written */
			write_finished = true;
			conn->write_msg = NULL;
			conn->write_offset = 0;
			/* API_EVENT might call tcp_tmr, so reset conn->state now */
			conn->state = NETCONN_NONE;
		}
		err = tcp_output_nagle(conn->pcb.tcp);
		conn->err = err;
		if ((err == ERR_OK) && (conn->pcb.tcp->snd_buf <= TCP_SNDLOWAT))
		{
			API_EVENT(conn, NETCONN_EVT_SENDMINUS, len);
		}
	}
	else if (err == ERR_MEM)
	{
		/* If ERR_MEM, we wait for sent_tcp or poll_tcp to be called
		 we do NOT return to the application thread, since ERR_MEM is
		 only a temporary error! */

		/* tcp_enqueue returned ERR_MEM, try tcp_output anyway */
		err = tcp_output(conn->pcb.tcp);

#if LWIP_TCPIP_CORE_LOCKING
		conn->write_delayed = 1;
#endif
	}
	else
	{
		/* On errors != ERR_MEM, we don't try writing any more but return
		 the error to the application thread. */
		conn->err = err;
		write_finished = true;
	}

	if (write_finished)
	{
		/* everything was written: set back connection state
		 and back to application task */
		conn->state = NETCONN_NONE;
#if LWIP_TCPIP_CORE_LOCKING
		if (conn->write_delayed != 0)
#endif
		{
			conn->op_completed->SetEvent();
		}
	}
#if LWIP_TCPIP_CORE_LOCKING
	else
	return ERR_MEM;
#endif
	return ERR_OK;
}

/**
 * Sent callback function for TCP netconns.
 * Signals the conn->sem and calls API_EVENT.
 * netconn_write waits for conn->sem if send buffer is low.
 *
 * @see tcp.h (struct tcp_pcb.sent) for parameters and return value
 */
int sent_tcp(void *arg, TcpSocket *pcb, int len)
{
	NetConn * conn = (NetConn *) arg;
	ASSERT(conn);
	switch (conn->state)
	{
	case NETCONN_WRITE:
		ASSERT(conn->pcb.tcp);
		do_writemore(conn);
		break;
	case NETCONN_CLOSE:
		do_close_internal(conn);
		break;
	default:
		break;
	}
	if (conn)
	{
		if ((conn->pcb.tcp != NULL) && (conn->pcb.tcp->snd_buf > TCP_SNDLOWAT))
		{
			API_EVENT(conn, NETCONN_EVT_SENDPLUS, len);
		}
	}
	return ERR_OK;
	ASSERT(pcb);
}

/**
 * Bind a netconn to a specific local IP address and port.
 * Binding one netconn twice might not always be checked correctly!
 *
 * @param conn the netconn to bind
 * @param addr the local IP address to bind the netconn to (use IP_ADDR_ANY
 *             to bind to all addresses)
 * @param port the local port to bind the netconn to (not used for RAW)
 * @return ERR_OK if bound, any other int on failure
 */
int netconn_bind(NetConn *conn, ip_addr *addr, int port)
{
	if (!conn)
	{
		printf("netconn_bind: invalid conn\n");
		return ERR_ARG;
	}
	struct api_msg msg;
	msg.function = do_bind;
	msg.msg.conn = conn;
	msg.msg.msg.bc.ipaddr = addr;
	msg.msg.msg.bc.port = port;
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Bind a pcb contained in a netconn
 * Called from netconn_bind.
 *
 * @param msg the api_msg_msg pointing to the connection and containing
 *            the IP address and port to bind to
 */
void do_bind(api_msg_msg *msg)
{
	if (!ERR_IS_FATAL(msg->conn->err))
	{
		if (msg->conn->pcb.tcp != NULL)
		{
			switch (NETCONNTYPE_GROUP(msg->conn->type))
			{
			case NETCONN_RAW:
				msg->conn->err = raw_bind(msg->conn->pcb.raw,
						msg->msg.bc.ipaddr);
				break;
			case NETCONN_UDP:
				msg->conn->err = udp_bind(msg->conn->pcb.udp,
						msg->msg.bc.ipaddr, msg->msg.bc.port);
				break;
			case NETCONN_TCP:
				msg->conn->err = tcp_bind(msg->conn->pcb.tcp,
						msg->msg.bc.ipaddr, msg->msg.bc.port);
				break;
			default:
				break;
			}
		}
		else
		{
			/* msg->conn->pcb is NULL */
			msg->conn->err = ERR_VAL;
		}
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Close a TCP pcb contained in a netconn
 * Called from netconn_close
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_close(api_msg_msg *msg)
{
	if ((msg->conn->pcb.tcp != NULL) && (msg->conn->type == NETCONN_TCP))
	{
		msg->conn->state = NETCONN_CLOSE;
		do_close_internal(msg->conn);
		/* for tcp netconns, do_close_internal ACKs the message */
	}
	else
	{
		msg->conn->err = ERR_VAL;
		TCPIP_APIMSG_ACK(msg);
	}
}

/**
 * Join multicast groups for UDP netconns.
 * Called from netconn_join_leave_group
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_join_leave_group(api_msg_msg *msg)
{
	if (!ERR_IS_FATAL(msg->conn->err))
	{
		if (msg->conn->pcb.tcp != NULL)
		{
			if (NETCONNTYPE_GROUP(msg->conn->type) == NETCONN_UDP)
			{
				if (msg->msg.jl.join_or_leave == NETCONN_JOIN)
				{
					msg->conn->err = igmp_joingroup(msg->msg.jl.interfac,
							msg->msg.jl.multiaddr);
				}
				else
				{
					msg->conn->err = igmp_leavegroup(msg->msg.jl.interfac,
							msg->msg.jl.multiaddr);
				}
			}
			else
			{
				msg->conn->err = ERR_VAL;
			}
		}
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Set a TCP netconn into listen mode
 *
 * @param conn the tcp netconn to set to listen mode
 * @param backlog the listen backlog
 * @return ERR_OK if the netconn was set to listen (UDP and RAW netconns
 *         don't return any error (yet?))
 */
int netconn_listen_with_backlog(NetConn *conn, int backlog)
{
	if (!conn)
	{
		printf("netconn_listen: invalid conn\n");
		return ERR_ARG;
	}
	api_msg msg;
	msg.function = do_listen;
	msg.msg.conn = conn;
	msg.msg.msg.lb.backlog = backlog;
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Set a TCP pcb contained in a netconn into listen mode
 * Called from netconn_listen.
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_listen(api_msg_msg *msg)
{
	if (ERR_IS_FATAL(msg->conn->err))
	{
		TCPIP_APIMSG_ACK(msg);
		return;
	}
	if (msg->conn->pcb.tcp == NULL)
	{
		TCPIP_APIMSG_ACK(msg);
		return;
	}
	if (msg->conn->type != NETCONN_TCP)
	{
		TCPIP_APIMSG_ACK(msg);
		return;
	}
	if (msg->conn->pcb.tcp->state == CLOSED)
	{
		TcpSocket * lpcb = tcp_listen_with_backlog(msg->conn->pcb.tcp,
				msg->msg.lb.backlog);
		if (lpcb == NULL)
		{
			msg->conn->err = ERR_MEM;
		}
		else
		{
			/* delete the recvmbox and allocate the acceptmbox */
			if (msg->conn->recvmbox)
			{
				/** @todo: should we drain the recvmbox here? */
				delete msg->conn->recvmbox;
				msg->conn->recvmbox = 0;
			}
			if (!msg->conn->acceptmbox)
			{
				if ((msg->conn->acceptmbox = new MailBox(20)) == 0)
				{
					msg->conn->err = ERR_MEM;
				}
			}
			if (msg->conn->err == ERR_OK)
			{
				msg->conn->state = NETCONN_LISTEN;
				msg->conn->pcb.tcp = lpcb;
				tcp_arg(msg->conn->pcb.tcp, msg->conn);
				tcp_accept(msg->conn->pcb.tcp, accept_function);
			}
		}
	}
	else
	{
		msg->conn->err = ERR_CONN;
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Accept callback function for TCP netconns.
 * Allocates a new netconn and posts that to conn->acceptmbox.
 *
 * @see tcp.h (struct tcp_pcb_listen.accept) for parameters and return value
 */
int accept_function(void *arg, TcpSocket *newpcb, int err)
{
	NetConn * conn = (NetConn *) arg;
	if (!conn->acceptmbox)
	{
		printf("accept_function: invalid conn->acceptmbox\n");
		return ERR_VAL;
	}
	/* We have to set the callback here even though
	 * the new socket is unknown. conn->socket is marked as -1. */
	NetConn * newconn = netconn_alloc(conn->type, conn->callback);
	if (!newconn)
		return ERR_MEM;
	newconn->pcb.tcp = newpcb;
	setup_tcp(newconn);
	newconn->err = err;
	if (!conn->acceptmbox->TryPost(newconn))
	{
		/* When returning != ERR_OK, the connection is aborted in tcp_process(),
		 so do nothing here! */
		newconn->pcb.tcp = NULL;
		netconn_free(newconn);
		return ERR_MEM;
	}
	else
	{
		/* Register event with callback */
		API_EVENT(conn, NETCONN_EVT_RCVPLUS, 0);
	}
	return ERR_OK;
}

/**
 * Receive data (in form of a netbuf containing a packet buffer) from a netconn
 *
 * @param conn the netconn from which to receive data
 * @return a new netbuf containing received data or NULL on memory error or timeout
 */
NetBuf * netconn_recv(NetConn *conn)
{
	api_msg msg;
	NetBuf *buf = NULL;
	PktBuf *p;
	uint16 len;

	ASSERT(conn);

	if (!conn->recvmbox)
	{
		/* @todo: should calling netconn_recv on a TCP listen conn be fatal (ERR_CONN)?? */
		/* TCP listen conns don't have a recvmbox! */
		conn->err = ERR_CONN;
		return NULL;
	}

	if (ERR_IS_FATAL(conn->err))
		return NULL;

	if (conn->type == NETCONN_TCP)
	{
		if (conn->state == NETCONN_LISTEN)
		{
			/* @todo: should calling netconn_recv on a TCP listen conn be fatal?? */
			conn->err = ERR_CONN;
			return NULL;
		}

		buf = new NetBuf();

		if (buf == NULL)
		{
			conn->err = ERR_MEM;
			return NULL;
		}

		if (!conn->recvmbox->Fetch((void **) &p, conn->recv_timeout))
		{
			conn->err = ERR_TIMEOUT;
			p = NULL;
		}

		if (p)
		{
			len = p->tot_len;
			conn->recv_avail -= len;
		}
		else
		{
			len = 0;
		}

		/* Register event with callback */
		API_EVENT(conn, NETCONN_EVT_RCVMINUS, len);

		/* If we are closed, we indicate that we no longer wish to use the socket */
		if (!p)
		{
			delete buf;
			/* Avoid to lose any previous error code */
			if (conn->err == ERR_OK)
				conn->err = ERR_CLSD;
			return NULL;
		}

		buf->p = p;
		buf->ptr = p;
		buf->port = 0;
		buf->addr = NULL;

		/* Let the stack know that we have taken the data. */
		msg.function = do_recv;
		msg.msg.conn = conn;
		if (buf != NULL)
			msg.msg.msg.r.len = buf->p->tot_len;
		else
			msg.msg.msg.r.len = 1;
		TCPIP_APIMSG(&msg);
	}
	else
	{
		if (!conn->recvmbox->Fetch((void **) &buf, conn->recv_timeout))
			buf = NULL;
		if (buf != NULL)
		{
			conn->recv_avail -= buf->p->tot_len;
			/* Register event with callback */
			API_EVENT(conn, NETCONN_EVT_RCVMINUS, buf->p->tot_len);
		}
	}
	return buf;
}

/**
 * Get the local or remote IP address and port of a netconn.
 * For RAW netconns, this returns the protocol instead of a port!
 *
 * @param conn the netconn to query
 * @param addr a pointer to which to save the IP address
 * @param port a pointer to which to save the port (or protocol for RAW)
 * @param local 1 to get the local IP address, 0 to get the remote one
 * @return ERR_CONN for invalid connections
 *         ERR_OK if the information was retrieved
 */
int netconn_getaddr(NetConn *conn, ip_addr *addr, int *port, int local)
{
	if (!conn)
	{
		printf("netconn_getaddr: invalid conn\n");
		return ERR_ARG;
	}
	if (!addr)
	{
		printf("netconn_getaddr: invalid addr\n");
		return ERR_ARG;
	}
	if (!port)
	{
		printf("netconn_getaddr: invalid port\n");
		return ERR_ARG;
	}
	api_msg msg;
	msg.function = do_getaddr;
	msg.msg.conn = conn;
	msg.msg.msg.ad.ipaddr = addr;
	msg.msg.msg.ad.port = port;
	msg.msg.msg.ad.local = local;
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Return a connection's local or remote address
 * Called from netconn_getaddr
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_getaddr(api_msg_msg *msg)
{
	if (msg->conn->pcb.ip != NULL)
	{
		*(msg->msg.ad.ipaddr)
				= (msg->msg.ad.local ? msg->conn->pcb.ip->local_ip
						: msg->conn->pcb.ip->remote_ip);

		switch (NETCONNTYPE_GROUP(msg->conn->type))
		{
		case NETCONN_RAW:
			if (msg->msg.ad.local)
				*(msg->msg.ad.port) = msg->conn->pcb.raw->protocol;
			else
				/* return an error as connecting is only a helper for upper layers */
				msg->conn->err = ERR_CONN;
			break;
		case NETCONN_UDP:
			if (msg->msg.ad.local)
			{
				*(msg->msg.ad.port) = msg->conn->pcb.udp->local_port;
			}
			else
			{
				if ((msg->conn->pcb.udp->flags & UDP_FLAGS_CONNECTED) == 0)
					msg->conn->err = ERR_CONN;
				else
					*(msg->msg.ad.port) = msg->conn->pcb.udp->remote_port;
			}
			break;
		case NETCONN_TCP:
			*(msg->msg.ad.port)
					= (msg->msg.ad.local ? msg->conn->pcb.tcp->local_port
							: msg->conn->pcb.tcp->remote_port);
			break;
		}
	}
	else
	{
		msg->conn->err = ERR_CONN;
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Indicate data has been received from a TCP pcb contained in a netconn
 * Called from netconn_recv
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_recv(api_msg_msg *msg)
{
	if (!ERR_IS_FATAL(msg->conn->err))
	{
		if (msg->conn->pcb.tcp != NULL)
		{
			if (msg->conn->type == NETCONN_TCP)
			{
				if (msg->conn->pcb.tcp->state == LISTEN)
					tcp_accepted(msg->conn->pcb.tcp);
				else
					tcp_recved(msg->conn->pcb.tcp, msg->msg.r.len);
			}
		}
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Send data over a TCP netconn.
 *
 * @param conn the TCP netconn over which to send data
 * @param dataptr pointer to the application buffer that contains the data to send
 * @param size size of the application data to send
 * @param apiflags combination of following flags :
 * - NETCONN_COPY (0x01) data will be copied into memory belonging to the stack
 * - NETCONN_MORE (0x02) for TCP connection, PSH flag will be set on last segment sent
 * @return ERR_OK if data was sent, any other int on error
 */
int netconn_write(NetConn *conn, const void *dataptr, size_t size, int apiflags)
{
	if (!conn)
	{
		printf("netconn_write: invalid conn\n");
		return ERR_ARG;
	}
	if (!(conn->type == NETCONN_TCP))
	{
		printf("netconn_write: invalid conn->type\n");
		return ERR_VAL;
	}
	api_msg msg;
	msg.function = do_write;
	msg.msg.conn = conn;
	msg.msg.msg.w.dataptr = dataptr;
	msg.msg.msg.w.apiflags = apiflags;
	msg.msg.msg.w.len = size;
	/* For locking the core: this _can_ be delayed on low memory/low send buffer,
	 but if it is, this is done inside api_msg.c:do_write(), so we can use the
	 non-blocking version here. */
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Send some data on a TCP pcb contained in a netconn
 * Called from netconn_write
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_write(api_msg_msg *msg)
{
	if (!ERR_IS_FATAL(msg->conn->err))
	{
		if ((msg->conn->pcb.tcp != NULL) && (msg->conn->type == NETCONN_TCP))
		{
			msg->conn->state = NETCONN_WRITE;
			/* set all the variables used by do_writemore */
			ASSERT(msg->conn->write_msg == NULL
					&& msg->conn->write_offset == 0);
			msg->conn->write_msg = msg;
			msg->conn->write_offset = 0;
			do_writemore(msg->conn);
			/* for both cases: if do_writemore was called, don't ACK the APIMSG! */
			return;
		}
		else
		{
			msg->conn->err = ERR_VAL;
		}
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Send data over a UDP or RAW netconn (that is already connected).
 *
 * @param conn the UDP or RAW netconn over which to send data
 * @param buf a netbuf containing the data to send
 * @return ERR_OK if data was sent, any other int on error
 */
int netconn_send(NetConn *conn, NetBuf *buf)
{
	if (!conn)
	{
		printf("netconn_send: invalid conn\n");
		return ERR_ARG;
	}
	api_msg msg;
	msg.function = do_send;
	msg.msg.conn = conn;
	msg.msg.msg.b = buf;
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Send some data on a RAW or UDP pcb contained in a netconn
 * Called from netconn_send
 *
 * @param msg the api_msg_msg pointing to the connection
 */
void do_send(api_msg_msg *msg)
{
	if (!ERR_IS_FATAL(msg->conn->err))
	{
		if (msg->conn->pcb.tcp != NULL)
		{
			switch (NETCONNTYPE_GROUP(msg->conn->type))
			{
			case NETCONN_RAW:
				if (msg->msg.b->addr == NULL)
				{
					msg->conn->err
							= raw_send(msg->conn->pcb.raw, msg->msg.b->p);
				}
				else
				{
					msg->conn->err = raw_sendto(msg->conn->pcb.raw,
							msg->msg.b->p, msg->msg.b->addr);
				}
				break;
			case NETCONN_UDP:
				if (msg->msg.b->addr == NULL)
				{
					msg->conn->err
							= udp_send(msg->conn->pcb.udp, msg->msg.b->p);
				}
				else
				{
					msg->conn->err = udp_sendto(msg->conn->pcb.udp,
							msg->msg.b->p, msg->msg.b->addr, msg->msg.b->port);
				}
				break;
			default:
				break;
			}
		}
	}
	TCPIP_APIMSG_ACK(msg);
}

/**
 * Accept a new connection on a TCP listening netconn.
 *
 * @param conn the TCP listen netconn
 * @return the newly accepted netconn or NULL on timeout
 */
NetConn * netconn_accept(NetConn *conn)
{
	if (!conn)
	{
		printf("netconn_accept: invalid conn\n");
		return NULL;
	}
	if (!conn->acceptmbox)
	{
		printf("netconn_accept: invalid acceptmbox\n");
		return NULL;
	}

	NetConn * newconn;
	if (!conn->acceptmbox->Fetch((void **) &newconn, conn->recv_timeout))
	{
		newconn = NULL;
	}
	else
	{
		/* Register event with callback */
		API_EVENT(conn, NETCONN_EVT_RCVMINUS, 0);
		if (newconn != NULL)
		{
			/* Let the stack know that we have accepted the connection. */
			api_msg msg;
			msg.function = do_recv;
			msg.msg.conn = conn;
			TCPIP_APIMSG(&msg);
		}
	}
	return newconn;
}

/**
 * Disconnect a netconn from its current peer (only valid for UDP netconns).
 *
 * @param conn the netconn to disconnect
 * @return TODO: return value is not set here...
 */
int netconn_disconnect(NetConn *conn)
{
	if (!conn)
	{
		printf("netconn_disconnect: invalid conn\n");
		return ERR_ARG;
	}
	api_msg msg;
	msg.function = do_disconnect;
	msg.msg.conn = conn;
	TCPIP_APIMSG(&msg);
	return conn->err;
}

/**
 * Connect a netconn to a specific remote IP address and port.
 *
 * @param conn the netconn to connect
 * @param addr the remote IP address to connect to
 * @param port the remote port to connect to (no used for RAW)
 * @return ERR_OK if connected, return value of tcp_/udp_/raw_connect otherwise
 */
int netconn_connect(NetConn *conn, ip_addr *addr, int port)
{
	if (!conn)
	{
		printf("netconn_connect: invalid conn\n");
		return ERR_ARG;
	}
	api_msg msg;
	msg.function = do_connect;
	msg.msg.conn = conn;
	msg.msg.msg.bc.ipaddr = addr;
	msg.msg.msg.bc.port = port;
	/* This is the only function which need to not block tcpip_thread */
	tcpip_apimsg(&msg);
	return conn->err;
}

/**
 * Connect a pcb contained inside a netconn
 * Called from netconn_connect.
 *
 * @param msg the api_msg_msg pointing to the connection and containing
 *            the IP address and port to connect to
 */
void do_connect(struct api_msg_msg *msg)
{
	if (msg->conn->pcb.tcp == NULL)
	{
		msg->conn->op_completed->SetEvent();
		return;
	}
	switch (NETCONNTYPE_GROUP(msg->conn->type))
	{
	case NETCONN_RAW:
		msg->conn->err = raw_connect(msg->conn->pcb.raw, msg->msg.bc.ipaddr);
		msg->conn->op_completed->SetEvent();
		break;
	case NETCONN_UDP:
		msg->conn->err = udp_connect(msg->conn->pcb.udp, msg->msg.bc.ipaddr,
				msg->msg.bc.port);
		msg->conn->op_completed->SetEvent();
		break;
	case NETCONN_TCP:
		msg->conn->state = NETCONN_CONNECT;
		setup_tcp(msg->conn);
		msg->conn->err = tcp_connect(msg->conn->pcb.tcp, msg->msg.bc.ipaddr,
				msg->msg.bc.port, do_connected);
		/* sys_sem_signal() is called from do_connected (or err_tcp()),
		 * when the connection is established! */
		break;
	default:
		break;
	}
}

/**
 * Connect a pcb contained inside a netconn
 * Only used for UDP netconns.
 * Called from netconn_disconnect.
 *
 * @param msg the api_msg_msg pointing to the connection to disconnect
 */
void do_disconnect(struct api_msg_msg *msg)
{
	if (NETCONNTYPE_GROUP(msg->conn->type) == NETCONN_UDP)
		udp_disconnect(msg->conn->pcb.udp);
	TCPIP_APIMSG_ACK(msg);
}

/**
 * TCP callback function if a connection (opened by tcp_connect/do_connect) has
 * been established (or reset by the remote host).
 *
 * @see tcp.h (struct tcp_pcb.connected) for parameters and return values
 */
int do_connected(void *arg, TcpSocket *pcb, int err)
{
	NetConn * conn = (NetConn *) arg;
	if (!conn)
		return ERR_VAL;
	conn->err = err;
	if ((conn->type == NETCONN_TCP) && (err == ERR_OK))
	{
		setup_tcp(conn);
	}
	conn->state = NETCONN_NONE;
	conn->op_completed->SetEvent();
	return ERR_OK;
	ASSERT(pcb);
}

/**
 * Send data (in form of a netbuf) to a specific remote IP address and port.
 * Only to be used for UDP and RAW netconns (not TCP).
 *
 * @param conn the netconn over which to send data
 * @param buf a netbuf containing the data to send
 * @param addr the remote IP address to which to send the data
 * @param port the remote port to which to send the data
 * @return ERR_OK if data was sent, any other int on error
 */
int netconn_sendto(NetConn *conn, NetBuf *buf, ip_addr *addr, int port)
{
	if (!buf)
		return ERR_VAL;
	buf->addr = addr;
	buf->port = port;
	return netconn_send(conn, buf);
}

/**
 * Close a TCP netconn (doesn't delete it).
 *
 * @param conn the TCP netconn to close
 * @return ERR_OK if the netconn was closed, any other int on error
 */
int netconn_close(NetConn *conn)
{
	if (!conn)
		return ERR_ARG;
	api_msg msg;
	msg.function = do_close;
	msg.msg.conn = conn;
	tcpip_apimsg(&msg);
	return conn->err;
}

/**
 * Join multicast groups for UDP netconns.
 *
 * @param conn the UDP netconn for which to change multicast addresses
 * @param multiaddr IP address of the multicast group to join or leave
 * @param interface the IP address of the network interface on which to send
 *                  the igmp message
 * @param join_or_leave flag whether to send a join- or leave-message
 * @return ERR_OK if the action was taken, any int on error
 */
int netconn_join_leave_group(NetConn *conn, ip_addr *multiaddr,
		ip_addr *interfac, netconn_igmp join_or_leave)
{
	if (!conn)
		return ERR_ARG;
	api_msg msg;
	msg.function = do_join_leave_group;
	msg.msg.conn = conn;
	msg.msg.msg.jl.multiaddr = multiaddr;
	msg.msg.msg.jl.interfac = interfac;
	msg.msg.msg.jl.join_or_leave = join_or_leave;
	TCPIP_APIMSG(&msg);
	return conn->err;
}
