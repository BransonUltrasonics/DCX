// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/Socket.cpp_v   1.1   24 Jul 2015 08:51:06   sesharma  $
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

#include "Socket.h"
#include "NetConn.h"
#include "Event.h"
#include "NetBuf.h"
#include "PktBuf.h"
#include "Mutex.h"
#include "TcpIp.h"
#include "TCP.h"
#include "UDP.h"
#include "Raw.h"
#include "IGMP.h"
#include "DebugNew.h"
#include "errno.h"

#define printf pprintf

#define O_NONBLOCK    04000U

#define set_errno(err)

#define ENSROK    0 /* DNS server returned answer with no data */
#define ENSRNODATA  160 /* DNS server returned answer with no data */
#define ENSRFORMERR 161 /* DNS server claims query was misformatted */
#define ENSRSERVFAIL 162  /* DNS server returned general failure */
#define ENSRNOTFOUND 163  /* Domain name not found */
#define ENSRNOTIMP  164 /* DNS server does not implement requested operation */
#define ENSRREFUSED 165 /* DNS server refused query */
#define ENSRBADQUERY 166  /* Misformatted DNS query */
#define ENSRBADNAME 167 /* Misformatted domain name */
#define ENSRBADFAMILY 168 /* Unsupported address family */
#define ENSRBADRESP 169 /* Misformatted DNS reply */
#define ENSRCONNREFUSED 170 /* Could not contact DNS servers */
#define ENSRTIMEOUT 171 /* Timeout while contacting DNS servers */
#define ENSROF    172 /* End of file */
#define ENSRFILE  173 /* Error reading file */
#define ENSRNOMEM 174 /* Out of memory */
#define ENSRDESTRUCTION 175 /* Application terminated lookup */
#define ENSRQUERYDOMAINTOOLONG  176 /* Domain name is too long */
#define ENSRCNAMELOOP 177 /* Domain name is too long */

static const int err_to_errno_table[] =
{ 0, /* ERR_OK          0      No error, everything OK. */
ENOMEM, /* ERR_MEM        -1      Out of memory error.     */
ENOBUFS, /* ERR_BUF        -2      Buffer error.            */
ETIMEDOUT, /* ERR_TIMEOUT    -3      Timeout                  */
EHOSTUNREACH, /* ERR_RTE        -4      Routing problem.         */
ECONNABORTED, /* ERR_ABRT       -5      Connection aborted.      */
ECONNRESET, /* ERR_RST        -6      Connection reset.        */
ESHUTDOWN, /* ERR_CLSD       -7      Connection closed.       */
ENOTCONN, /* ERR_CONN       -8      Not connected.           */
EINVAL, /* ERR_VAL        -9      Illegal value.           */
EIO, /* ERR_ARG        -10     Illegal argument.        */
EADDRINUSE, /* ERR_USE        -11     Address in use.          */
-1, /* ERR_IF         -12     Low-level netif error    */
-1, /* ERR_ISCONN     -13     Already connected.       */
EINPROGRESS /* ERR_INPROGRESS -14     Operation in progress    */
};

#define ERR_TO_ERRNO_TABLE_SIZE \
  (sizeof(err_to_errno_table)/sizeof(err_to_errno_table[0]))

#define sock_set_errno(sk, e) do { \
  sk->err = (e); \
  set_errno(sk->err); \
} while (0)

#define err_to_errno(err) \
  ((unsigned)(-(err)) < ERR_TO_ERRNO_TABLE_SIZE ? \
    err_to_errno_table[-(err)] : EIO)

#define NUM_SOCKETS 256

/** Description for a task waiting in select */
struct lwip_select_cb
{
	/** Pointer to the next waiting task */
	lwip_select_cb *next;
	/** readset passed to select */
	fd_set *readset;
	/** writeset passed to select */
	fd_set *writeset;
	/** unimplemented: exceptset passed to select */
	fd_set *exceptset;
	/** don't signal the same semaphore twice: set to 1 when signalled */
	int sem_signalled;
	/** semaphore to wake up a task waiting for select */
	Event sem;
	lwip_select_cb() :
		sem("Select")
	{
	   next = 0;
	   readset = 0;
	   writeset = 0;
	   exceptset = 0;
	   sem_signalled = 0;
	}
};

struct BsdSocket
{
	/** sockets currently are built on netconns, each socket has one netconn */
	NetConn *conn;
	/** data that was left from the previous read */
	NetBuf *lastdata;
	/** offset in the data that was left from the previous read */
	uint16 lastoffset;
	/** number of times data was received, set by event_callback(),
	 tested by the receive and select functions */
	int16 rcvevent;
	/** number of times data was received, set by event_callback(),
	 tested by select */
	bool sendevent;
	/** socket flags (currently, only used for O_NONBLOCK) */
	int flags;
	/** last error that occurred on this socket */
	int err;
};

static BsdSocket sockets[NUM_SOCKETS];
static Mutex socksem("Socket");
static Mutex selectsem("Select");
static lwip_select_cb *select_cb_list;

/** This struct is used to pass data to the set/getsockopt_internal
 * functions running in tcpip_thread context (only a void* is allowed) */
struct lwip_setgetsockopt_data
{
	/** socket struct for which to change options */
	BsdSocket *sock;
	/** socket index for which to change options */
	int s;
	/** level of the option to process */
	int level;
	/** name of the option to process */
	int optname;
	/** set: value to set the option to
	 * get: value of the option is stored here */
	void *optval;
	/** size of *optval */
	int *optlen;
	/** if an error occures, it is temporarily stored here */
	int err;
	/** Event signalling the completion of the message processing. */
	Event * event;
};

BsdSocket * get_socket(int s);
void event_callback(NetConn *conn, netconn_evt evt, int len);
int alloc_socket(NetConn *newconn);

void lwip_socket_init()
{
}

int lwip_socket(int domain, int type, int protocol)
{
	NetConn *conn;
	int i;

	switch (type)
	{
	case SOCK_RAW:
		conn = netconn_new_with_proto_and_callback(NETCONN_RAW, protocol,
				event_callback);
		break;
	case SOCK_DGRAM:
		conn = netconn_new_with_callback(NETCONN_UDP, event_callback);
		break;
	case SOCK_STREAM:
		conn = netconn_new_with_callback(NETCONN_TCP, event_callback);
		break;
	default:
		printf("lwip_socket(%d, %d/UNKNOWN, %d) = -1\n", domain, type, protocol);
		set_errno(EINVAL);
		return -1;
	}

	if (!conn)
	{
		printf("-1 / ENOBUFS (could not create netconn)\n");
		set_errno(ENOBUFS);
		return -1;
	}
	i = alloc_socket(conn);
	if (i == -1)
	{
		netconn_delete(conn);
		set_errno(ENFILE);
		return -1;
	}
	conn->socket = i;
	set_errno(0);
	return i;
}

/**
 * Allocate a new socket for a given netconn.
 *
 * @param newconn the netconn for which to allocate a socket
 * @return the index of the new socket; -1 on error
 */
int alloc_socket(NetConn *newconn)
{
	int i;

	/* Protect socket array */
	socksem.Lock();

	/* allocate a new socket identifier */
	for (i = 0; i < NUM_SOCKETS; ++i)
	{
		if (!sockets[i].conn)
		{
			sockets[i].conn = newconn;
			sockets[i].lastdata = NULL;
			sockets[i].lastoffset = 0;
			sockets[i].rcvevent = 0;
			sockets[i].sendevent = true; /* TCP send buf is empty */
			sockets[i].flags = 0;
			sockets[i].err = 0;
			socksem.Unlock();
			return i;
		}
	}
	socksem.Unlock();
	return -1;
}

/**
 * Callback registered in the netconn layer for each socket-netconn.
 * Processes recvevent (data available) and wakes up tasks waiting for select.
 */
void event_callback(NetConn *conn, netconn_evt evt, int len)
{
	int s;
	BsdSocket *sock;
	lwip_select_cb *scb;

	/* Get socket */
	if (conn)
	{
		s = conn->socket;
		if (s < 0)
		{
			/* Data comes in right away after an accept, even though
			 * the server task might not have created a new socket yet.
			 * Just count down (or up) if that's the case and we
			 * will use the data later. Note that only receive events
			 * can happen before the new socket is set up. */
			socksem.Lock();
			if (conn->socket < 0)
			{
				if (evt == NETCONN_EVT_RCVPLUS)
				{
					conn->socket--;
				}
				socksem.Unlock();
				return;
			}
			socksem.Unlock();
		}

		sock = get_socket(s);
		if (!sock)
			return;
	}
	else
	{
		return;
	}
	selectsem.Lock();
	/* Set event as required */
	switch (evt)
	{
	case NETCONN_EVT_RCVPLUS:
		sock->rcvevent++;
		break;
	case NETCONN_EVT_RCVMINUS:
		sock->rcvevent--;
		break;
	case NETCONN_EVT_SENDPLUS:
		sock->sendevent = true;
		break;
	case NETCONN_EVT_SENDMINUS:
		sock->sendevent = false;
		break;
	default:
		printf("Unknown event in event_callback, len %d\n",len);
		ASSERT(0);
		break;
	}
	selectsem.Unlock();

	/* Now decide if anyone is waiting for this socket */
	/* NOTE: This code is written this way to protect the select link list
	 but to avoid a deadlock situation by releasing socksem before
	 signalling for the select. This means we need to go through the list
	 multiple times ONLY IF a select was actually waiting. We go through
	 the list the number of waiting select calls + 1. This list is
	 expected to be small. */
	while (1)
	{
		selectsem.Lock();
		for (scb = select_cb_list; scb; scb = scb->next)
		{
			if (scb->sem_signalled == 0)
			{
				/* Test this select call for our socket */
				if (scb->readset && FD_ISSET(s, scb->readset))
					if (sock->rcvevent > 0)
						break;
				if (scb->writeset && FD_ISSET(s, scb->writeset))
					if (sock->sendevent)
						break;
			}
		}
		if (scb)
		{
			scb->sem_signalled = 1;
			scb->sem.SetEvent();
			selectsem.Unlock();
		}
		else
		{
			selectsem.Unlock();
			break;
		}
	}
}

/**
 * Map a externally used socket index to the internal socket representation.
 *
 * @param s externally used socket index
 * @return struct lwip_socket for the socket or NULL if not found
 */
BsdSocket * get_socket(int s)
{
	BsdSocket *sock;

	if ((s < 0) || (s >= NUM_SOCKETS))
	{
		printf("get_socket(%d): invalid\n", s);
		set_errno(EBADF);
		return NULL;
	}
	sock = &sockets[s];
	if (!sock->conn)
	{
		printf("get_socket(%d): not active\n", s);
		set_errno(EBADF);
		return NULL;
	}
	return sock;
}

/**
 * Call the lower part of a netconn_* function
 * This function is then running in the thread context
 * of tcpip_thread and has exclusive access to lwIP core code.
 *
 * @param apimsg a struct containing the function to call and its parameters
 * @return ERR_OK if the function was called, another int if not
 */
int tcpip_apimsg(api_msg *apimsg)
{
	Event event("ApiMsg");
	apimsg->msg.conn->op_completed = &event;
	tcpip->SendApiMessage(apimsg);
	event.Wait();
	return ERR_OK;
}

/**
 * Create a new pcb of a specific type.
 * Called from do_newconn().
 *
 * @param msg the api_msg_msg describing the connection type
 * @return msg->conn->err, but the return value is currently ignored
 */
int pcb_new(api_msg_msg *msg)
{
	msg->conn->err = ERR_OK;

	ASSERT(msg->conn->pcb.tcp == NULL);

	/* Allocate a PCB for this connection */
	switch (NETCONNTYPE_GROUP(msg->conn->type))
	{
	case NETCONN_RAW:
		msg->conn->pcb.raw = raw_new(msg->msg.n.proto);
		if (msg->conn->pcb.raw == NULL)
		{
			msg->conn->err = ERR_MEM;
			break;
		}
		raw_recv(msg->conn->pcb.raw, recv_raw, msg->conn);
		break;
	case NETCONN_UDP:
		msg->conn->pcb.udp = udp_new();
		if (msg->conn->pcb.udp == NULL)
		{
			msg->conn->err = ERR_MEM;
			break;
		}
		if (msg->conn->type == NETCONN_UDPNOCHKSUM)
		{
			udp_setflags(msg->conn->pcb.udp, UDP_FLAGS_NOCHKSUM);
		}
		udp_recv(msg->conn->pcb.udp, recv_udp, msg->conn);
		break;
	case NETCONN_TCP:
		msg->conn->pcb.tcp = tcp_new();
		if (msg->conn->pcb.tcp == NULL)
		{
			msg->conn->err = ERR_MEM;
			break;
		}
		setup_tcp(msg->conn);
		break;
	default:
		/* Unsupported netconn type, e.g. protocol disabled */
		msg->conn->err = ERR_VAL;
		break;
	}

	return msg->conn->err;
}

#define ip_addr_debug_print(ipaddr) \
  printf("%d.%d.%d.%d",              \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 24) & 0xff : 0,  \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 16) & 0xff : 0,  \
                      ipaddr != NULL ?                                  \
                      (uint16)(ntohl((ipaddr)->addr) >> 8) & 0xff : 0,   \
                      ipaddr != NULL ?                                  \
                      (uint16)ntohl((ipaddr)->addr) & 0xff : 0)

int lwip_bind(int s, const sockaddr *name, uint32 namelen)
{
	BsdSocket * sock = get_socket(s);
	if (!sock)
		return -1;

	if (!((namelen == sizeof(sockaddr_in))
			&& ((((sockaddr_in *) name)->sin_family) == AF_INET)))
	{
		printf("lwip_bind: invalid address\n");
		sock_set_errno(sock, err_to_errno(ERR_ARG));
		return -1;
	}
	ip_addr local_addr;
	local_addr.addr = ((sockaddr_in *) name)->sin_addr.s_addr;
	uint16 local_port = ((sockaddr_in *) name)->sin_port;
	int err = netconn_bind(sock->conn, &local_addr, ntohs(local_port));
	if (err != ERR_OK)
	{
		printf("lwip_bind(%d) failed, err=%d\n", s, err);
		sock_set_errno(sock, err_to_errno(err));
		return -1;
	}
	sock_set_errno(sock, 0);
	return 0;
}

/**
 * Set a socket into listen mode.
 * The socket may not have been used for another connection previously.
 *
 * @param s the socket to set to listening mode
 * @param backlog
 * @return 0 on success, non-zero on failure
 */
int lwip_listen(int s, int backlog)
{
	BsdSocket *sock;
	int err;

	sock = get_socket(s);
	if (!sock)
		return -1;

	/* limit the "backlog" parameter to fit in an u8_t */
	if (backlog < 0)
	{
		backlog = 0;
	}
	if (backlog > 0xff)
	{
		backlog = 0xff;
	}

	err = netconn_listen_with_backlog(sock->conn, backlog);
	if (err != ERR_OK)
	{
		printf("lwip_listen(%d) failed, err=%d\n", s, err);
		sock_set_errno(sock, err_to_errno(err));
		return -1;
	}
	sock_set_errno(sock, 0);
	return 0;
}

void printset(char * type, fd_set * set)
{
	printf("%s:\t", type);
	for (int i = 0; i < 30; i++)
		if (FD_ISSET(i, set))
			printf("%d ", i);
	printf("\n");
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset,
		fd_set *exceptset, struct timeval *timeout)
{
	int i;
	int nready;
	fd_set lreadset, lwriteset, lexceptset;
	uint32 msectimeout;
	// No need for dynamic allocation since this function is called from the
	// task that called select(), and this variable will be on stack of the
	// caller task all the time while this function executes. Concurrent calls
	// to select() from different tasks are registered in the lwip_select_cb as
	// separate entities.
	lwip_select_cb select_cb;
	lwip_select_cb *p_selcb;

	select_cb.next = 0;
	select_cb.readset = readset;
	select_cb.writeset = writeset;
	select_cb.exceptset = exceptset;
	select_cb.sem_signalled = 0;

	/* Protect ourselves searching through the list */
	selectsem.Lock();
	if (readset)
		lreadset = *readset;
	else
		FD_ZERO(&lreadset);
	if (writeset)
		lwriteset = *writeset;
	else
		FD_ZERO(&lwriteset);
	if (exceptset)
		lexceptset = *exceptset;
	else
		FD_ZERO(&lexceptset);

	//	printf("before:\n");
	//	printset("read", &lreadset);
	//	printset("write", &lwriteset);
	//	printset("except", &lexceptset);
	/* Go through each socket in each list to count number of sockets which
	 currently match */
	nready = lwip_selscan(maxfdp1, &lreadset, &lwriteset, &lexceptset);
	/* If we don't have any current events, then suspend if we are supposed to */
	if (!nready)
	{
		if (timeout && timeout->tv_sec == 0 && timeout->tv_usec == 0)
		{
			selectsem.Unlock();
			if (readset)
				FD_ZERO(readset);
			if (writeset)
				FD_ZERO(writeset);
			if (exceptset)
				FD_ZERO(exceptset);
			set_errno(0);

			return 0;
		}

		/* add our semaphore to list */
		/* We don't actually need any dynamic memory. Our entry on the
		 * list is only valid while we are in this function, so it's ok
		 * to use local variables */

		/* Note that we are still protected */
		/* Put this select_cb on top of list */
		select_cb.next = select_cb_list;
		select_cb_list = &select_cb;
		/* Now we can safely unprotect */
		selectsem.Unlock();
		/* Now just wait to be woken */
		if (timeout == 0)
			/* Wait forever */
			msectimeout = -1;
		else
		{
			msectimeout = ((timeout->tv_sec * 1000) + ((timeout->tv_usec + 500)
					/ 1000));
		}
		i = select_cb.sem.Wait(msectimeout);
		/* Take us off the list */
		selectsem.Lock();
		if (select_cb_list == &select_cb)
			select_cb_list = select_cb.next;
		else
		{
			for (p_selcb = select_cb_list; p_selcb; p_selcb = p_selcb->next)
			{
				if (p_selcb->next == &select_cb)
				{
					p_selcb->next = select_cb.next;
					break;
				}
			}
		}
		selectsem.Unlock();

		if (i == 0)
		{
			/* Timeout */
			if (readset)
				FD_ZERO(readset);
			if (writeset)
				FD_ZERO(writeset);
			if (exceptset)
				FD_ZERO(exceptset);
			set_errno(0);
			return 0;
		}
		if (readset)
			lreadset = *readset;
		else
			FD_ZERO(&lreadset);
		if (writeset)
			lwriteset = *writeset;
		else
			FD_ZERO(&lwriteset);
		if (exceptset)
			lexceptset = *exceptset;
		else
			FD_ZERO(&lexceptset);

		/* See what's set */
		nready = lwip_selscan(maxfdp1, &lreadset, &lwriteset, &lexceptset);
	}
	else
	{
		selectsem.Unlock();
	}
	if (readset)
		*readset = lreadset;
	if (writeset)
		*writeset = lwriteset;
	if (exceptset)
		*exceptset = lexceptset;
	set_errno(0);
	//	printf("after:\n");
	//	printset("read", &lreadset);
	//	printset("write", &lwriteset);
	//	printset("except", &lexceptset);
	return nready;
}

/**
 * Go through the readset and writeset lists and see which socket of the sockets
 * set in the sets has events. On return, readset, writeset and exceptset have
 * the sockets enabled that had events.
 *
 * exceptset is not used for now!!!
 *
 * @param maxfdp1 the highest socket index in the sets
 * @param readset in: set of sockets to check for read events;
 *                out: set of sockets that had read events
 * @param writeset in: set of sockets to check for write events;
 *                 out: set of sockets that had write events
 * @param exceptset in: set of sockets to check for exception events;
 *                  out: set of sockets that had exception events
 * @return number of sockets that had events (read+write+except)
 */
int lwip_selscan(int maxfdp1, fd_set *readset, fd_set *writeset,
		fd_set *exceptset)
{
	int i, nready = 0;
	fd_set lreadset, lwriteset, lexceptset;
	BsdSocket *p_sock;

	FD_ZERO(&lreadset);
	FD_ZERO(&lwriteset);
	FD_ZERO(&lexceptset);

	/* Go through each socket in each list to count number of sockets which
	 currently match */
	for (i = 0; i < maxfdp1; i++)
	{
		if (FD_ISSET(i, readset))
		{
			/* See if netconn of this socket is ready for read */
			p_sock = get_socket(i);
			if (p_sock && (p_sock->lastdata || (p_sock->rcvevent > 0)))
			{
				FD_SET(i, &lreadset);
				nready++;
			}
		}
		if (FD_ISSET(i, writeset))
		{
			/* See if netconn of this socket is ready for write */
			p_sock = get_socket(i);
			if (p_sock && p_sock->sendevent)
			{
				FD_SET(i, &lwriteset);
				nready++;
			}
		}
		if (FD_ISSET(i, exceptset))
		{
			p_sock = get_socket(i);
			// a fix - socket is in CLOSE_WAIT when remote host sent FIN/ACK
			// and it was ACKed from this socket, but this socket did not yet
			// send FIN/ACK to the remote host, waiting for user application
			// to close() the socket.
			bool exc = false;
			if (p_sock)
			{
				if (p_sock->conn->pcb.tcp == 0)
					exc = true;
				else if (p_sock->conn->pcb.tcp->state == CLOSE_WAIT)
					exc = true;
			}
			if (exc)
			{
				FD_SET((unsigned int) i, &lexceptset);
				nready++;
			}
		}
	}
	*readset = lreadset;
	*writeset = lwriteset;
	*exceptset = lexceptset;
	return nready;
}

int lwip_close(int s)
{
	BsdSocket * sock = get_socket(s);
	if (!sock)
		return -1;

	netconn_delete(sock->conn);
	socksem.Lock();
	if (sock->lastdata)
		netbuf_delete(sock->lastdata);
	sock->lastdata = NULL;
	sock->lastoffset = 0;
	sock->conn = NULL;
	sock_set_errno(sock, 0);
	socksem.Unlock();
	return 0;
}

int lwip_recv(int s, void *mem, size_t len, int flags)
{
	return lwip_recvfrom(s, mem, len, flags, NULL, NULL);
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags, sockaddr *from,
		int *fromlen)
{
	BsdSocket *sock;
	NetBuf *buf;
	size_t buflen, copylen, off = 0;
	ip_addr *addr;
	int port;
	bool done = false;

	sock = get_socket(s);
	if (!sock)
		return -1;
	do
	{
		/* Check if there is data left from the last recv operation. */
		if (sock->lastdata)
		{
			buf = sock->lastdata;
		}
		else
		{
			/* If this is non-blocking call, then check first */
			if (((flags & MSG_DONTWAIT) || (sock->flags & O_NONBLOCK))
					&& (sock->rcvevent <= 0))
			{
				if (off > 0)
				{
					/* already received data, return that */
					sock_set_errno(sock, 0);
					return off;
				}
				sock_set_errno(sock, EWOULDBLOCK);
				return -1;
			}

			/* No data was left from the previous operation, so we try to get
			 some from the network. */
			sock->lastdata = buf = netconn_recv(sock->conn);
			if (!buf)
			{
				if (off > 0)
				{
					/* already received data, return that */
					sock_set_errno(sock, 0);
					return off;
				}
				/* We should really do some error checking here. */
				if (sock && sock->conn)
				{
					sock_set_errno(sock, (((sock->conn->pcb.ip != NULL) && (sock->conn->err == ERR_OK))
									? ETIMEDOUT : err_to_errno(sock->conn->err)));
					return 0;
				}
				else
					return -1;
			}
		}
		buflen = netbuf_len(buf);
		buflen -= sock->lastoffset;
		if (len > buflen)
		{
			copylen = buflen;
		}
		else
		{
			copylen = (uint16) len;
		}

		/* copy the contents of the received buffer into
		 the supplied memory pointer mem */
		netbuf_copy_partial(buf, (uint8*) mem + off, copylen, sock->lastoffset);
		off += copylen;
		if (netconn_type(sock->conn) == NETCONN_TCP)
		{
			ASSERT(len >= copylen);
			len -= copylen;
			if ((len <= 0) || (buf->p->flags & PBUF_FLAG_PUSH)
					|| (sock->rcvevent <= 0) || ((flags & MSG_PEEK) != 0))
			{
				done = true;
			}
		}
		else
		{
			done = true;
		}

		/* Check to see from where the data was.*/
		if (done)
		{
			if (from && fromlen)
			{
				sockaddr_in sin;

				if (netconn_type(sock->conn) == NETCONN_TCP)
				{
					addr = (ip_addr *) &(sin.sin_addr.s_addr);
					netconn_getaddr(sock->conn, addr, &port, 0);
				}
				else
				{
					addr = netbuf_fromaddr(buf);
					port = netbuf_fromport(buf);
				}
				memset(&sin, 0, sizeof(sin));
				sin.sin_family = AF_INET;
				sin.sin_port = htons(port);
				sin.sin_addr.s_addr = addr->addr;
				if (*fromlen > ((int)sizeof(sin)))
					*fromlen = sizeof(sin);
				memcpy(from, &sin, *fromlen);
			}
		}
		/* If we don't peek the incoming message... */
		if ((flags & MSG_PEEK) == 0)
		{
			/* If this is a TCP socket, check if there is data left in the
			 buffer. If so, it should be saved in the sock structure for next
			 time around. */
			if ((netconn_type(sock->conn) == NETCONN_TCP) && (buflen - copylen
					> 0))
			{
				sock->lastdata = buf;
				sock->lastoffset += copylen;
			}
			else
			{
				sock->lastdata = NULL;
				sock->lastoffset = 0;
				netbuf_delete(buf);
			}
		}
	} while (!done);

	sock_set_errno(sock, 0);
	return off;
}

int lwip_send(int s, const void *data, size_t size, int flags)
{
	BsdSocket * sock = get_socket(s);
	if (!sock)
		return -1;
	if (sock->conn->type != NETCONN_TCP)
	{
		return lwip_sendto(s, data, size, flags, NULL, 0);
	}
	int err = netconn_write(sock->conn, data, size, NETCONN_COPY | ((flags
			& MSG_MORE) ? NETCONN_MORE : 0));

	//pprintf("\n %s", __FUNCTION__);
	sock_set_errno(sock, err_to_errno(err));
	return (err == ERR_OK ? (int) size : -1);
}

int lwip_sendto(int s, const void *data, size_t size, int flags,
		const sockaddr *to, int tolen)
{
	BsdSocket *sock;
	ip_addr remote_addr;
	int err;
	uint16 short_size;
	NetBuf buf;
	uint16 remote_port;

	sock = get_socket(s);
	//pprintf("\n %s %d", __FUNCTION__, s);
	if (!sock)
		return -1;

	if (sock->conn->type == NETCONN_TCP)
	{
		return lwip_send(s, data, size, flags);
	}

	ASSERT(size <= 0xffff);
	short_size = (uint16) size;
	if (!((((to == NULL) && (tolen == 0)) || ((tolen == sizeof(sockaddr_in))
			&& (to && ((((const sockaddr_in *) to)->sin_family) == AF_INET))))))//TODO:Test it
	{
		printf("lwip_sendto: invalid address\n");
		sock_set_errno(sock, err_to_errno(ERR_ARG));
		return -1;
	}

	/* initialize a buffer */
	buf.p = buf.ptr = NULL;
	if (to)
	{
		remote_addr.addr = ((const sockaddr_in *) to)->sin_addr.s_addr;
		remote_port = ntohs(((const sockaddr_in *) to)->sin_port);
		buf.addr = &remote_addr;
		buf.port = remote_port;
	}
	else
	{
		remote_addr.addr = 0;
		remote_port = 0;
		buf.addr = NULL;
		buf.port = 0;
	}

	//	printf("lwip_sendto(%d, data=%p, short_size=%d, flags=0x%x ", s, data,
	//			short_size, flags);
	//	ip_addr_debug_print(&remote_addr);
	//	printf(" port=%d\n", remote_port);

	/* make the buffer point to the data that should be sent */
#if LWIP_NETIF_TX_SINGLE_PBUF
	/* Allocate a new netbuf and copy the data into it. */
	if (netbuf_alloc(&buf, short_size) == NULL)
	{
		err = ERR_MEM;
	}
	else
	{
		err = netbuf_take(&buf, data, short_size);
	}
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
	err = netbuf_ref(&buf, data, short_size);
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */
	if (err == ERR_OK)
	{
		/* send the data */
		err = netconn_send(sock->conn, &buf);
	}

	/* deallocated the buffer */
	netbuf_free(&buf);
	sock_set_errno(sock, err_to_errno(err));
	return (err == ERR_OK ? short_size : -1);
}

int lwip_accept(int s, sockaddr *addr, int *addrlen)
{
	BsdSocket *sock, *nsock;
	NetConn *newconn;
	ip_addr naddr;
	int port;
	int newsock;
	sockaddr_in sin;
	int err;

	sock = get_socket(s);
	if (!sock)
		return -1;

	if ((sock->flags & O_NONBLOCK) && (sock->rcvevent <= 0))
	{
		printf("lwip_accept(%d): returning EWOULDBLOCK\n", s);
		sock_set_errno(sock, EWOULDBLOCK);
		return -1;
	}

	newconn = netconn_accept(sock->conn);
	if (!newconn)
	{
		printf("lwip_accept(%d) failed, err=%d\n", s, sock->conn->err);
		sock_set_errno(sock, err_to_errno(sock->conn->err));
		return -1;
	}

	/* get the IP address and port of the remote host */
	err = netconn_peer(newconn, &naddr, &port);
	if (err != ERR_OK)
	{
		netconn_delete(newconn);
		sock_set_errno(sock, err_to_errno(err));
		return -1;
	}

	/* Note that POSIX only requires us to check addr is non-NULL. addrlen must
	 * not be NULL if addr is valid.
	 */
	if (NULL != addr)
	{
		ASSERT(addrlen != NULL);
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = naddr.addr;

		if (*addrlen > ((int)sizeof(sin)))
			*addrlen = sizeof(sin);

		memcpy(addr, &sin, *addrlen);
	}

	newsock = alloc_socket(newconn);
	if (newsock == -1)
	{
		netconn_delete(newconn);
		sock_set_errno(sock, ENFILE);
		return -1;
	}
	ASSERT((newsock >= 0) && (newsock < NUM_SOCKETS));
	newconn->callback = event_callback;
	nsock = &sockets[newsock];
	ASSERT(nsock != NULL);

	socksem.Lock();
	/* See event_callback: If data comes in right away after an accept, even
	 * though the server task might not have created a new socket yet.
	 * In that case, newconn->socket is counted down (newconn->socket--),
	 * so nsock->rcvevent is >= 1 here!
	 */
	nsock->rcvevent += -1 - newconn->socket;
	newconn->socket = newsock;
	socksem.Unlock();
	sock_set_errno(sock, 0);
	return newsock;
}

int lwip_errno(int s)
{
	BsdSocket * sock = get_socket(s);
	if (!sock)
		return -1;
	return sock->err;
}

int lwip_ioctl(int s, long cmd, void *argp)
{
	BsdSocket *sock = get_socket(s);
	uint16 buflen = 0;
	uint16 recv_avail;

	if (!sock)
		return -1;

	switch (cmd)
	{
	case FIONREAD:
		if (!argp)
		{
			sock_set_errno(sock, EINVAL);
			return -1;
		}

		recv_avail = sock->conn->recv_avail;
//		if (recv_avail < 0)
//			recv_avail = 0;
		*((uint16*) argp) = (uint16) recv_avail;

		/* Check if there is data left from the last recv operation. /maq 041215 */
		if (sock->lastdata)
		{
			buflen = netbuf_len(sock->lastdata);
			buflen -= sock->lastoffset;

			*((uint16*) argp) += buflen;
		}
		sock_set_errno(sock, 0);
		return 0;

	case FIONBIO:
		if (argp && *(uint32*) argp)
			sock->flags |= O_NONBLOCK;
		else
			sock->flags &= ~O_NONBLOCK;
		sock_set_errno(sock, 0);
		return 0;

	default:
		printf("lwip_ioctl(%d, UNIMPL: 0x%lx, %p)\n", s, cmd, argp);
		//    sock_set_errno(sock, ENOSYS); /* not yet implemented */
		return -1;
	}
}

static int lwip_getaddrname(int s, sockaddr *name, int *namelen, uint8 local)
{
	BsdSocket *sock;
	sockaddr_in sin;
	ip_addr naddr;

	sock = get_socket(s);
	if (!sock)
		return -1;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	/* get the IP address and port */
	int sin_port;
	netconn_getaddr(sock->conn, &naddr, &sin_port, local);

	sin.sin_port = htons(sin_port);
	sin.sin_addr.s_addr = naddr.addr;

	if (*namelen > ((int)sizeof(sin)))
		*namelen = sizeof(sin);

	memcpy(name, &sin, *namelen);
	sock_set_errno(sock, 0);
	return 0;
}

int lwip_getpeername(int s, sockaddr *name, int *namelen)
{
	return lwip_getaddrname(s, name, namelen, 0);
}

int lwip_getsockname(int s, sockaddr *name, int *namelen)
{
	return lwip_getaddrname(s, name, namelen, 1);
}

static void lwip_getsockopt_internal(void *arg)
{
	BsdSocket *sock;
	int level, optname;
	void *optval;
	lwip_setgetsockopt_data * data;

	ASSERT(arg );

	data = (struct lwip_setgetsockopt_data*) arg;
	sock = data->sock;
	level = data->level;
	optname = data->optname;
	optval = data->optval;

	switch (level)
	{
	/* Level: SOL_SOCKET */
	case SOL_SOCKET:
		switch (optname)
		{
		/* The option flags */
		case SO_ACCEPTCONN:
		case SO_BROADCAST:
			/* UNIMPL case SO_DEBUG: */
			/* UNIMPL case SO_DONTROUTE: */
		case SO_KEEPALIVE:
			/* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
			case SO_REUSEADDR:
			case SO_REUSEPORT:
#endif /* SO_REUSE */
			/*case SO_USELOOPBACK: UNIMPL */
			*(int*) optval = sock->conn->pcb.ip->so_options & optname;
			break;

		case SO_TYPE:
			switch (NETCONNTYPE_GROUP(sock->conn->type))
			{
			case NETCONN_RAW:
				*(int*) optval = SOCK_RAW;
				break;
			case NETCONN_TCP:
				*(int*) optval = SOCK_STREAM;
				break;
			case NETCONN_UDP:
				*(int*) optval = SOCK_DGRAM;
				break;
			default: /* unrecognized socket type */
				*(int*) optval = sock->conn->type;
				printf("getsockopt(?, SOL_SOCKET, SO_TYPE): "
					"unrecognized socket type %d\n", *(int *) optval);
			}
			break;

		case SO_ERROR:
			if (sock->err == 0)
			{
				sock_set_errno(sock, err_to_errno(sock->conn->err));
			}
			*(int *) optval = sock->err;
			sock->err = 0;
			break;

		case SO_RCVTIMEO:
			*(int *) optval = sock->conn->recv_timeout;
			break;
		case SO_RCVBUF:
			*(int *) optval = sock->conn->recv_bufsize;
			break;
		case SO_NO_CHECK:
			*(int*) optval = (udp_flags(sock->conn->pcb.udp)
					& UDP_FLAGS_NOCHKSUM) ? 1 : 0;
			break;
		}
		break;

	case IPPROTO_IP:
		switch (optname)
		{
		case IP_TTL:
			*(int*) optval = sock->conn->pcb.ip->ttl;
			break;
		case IP_TOS:
			*(int*) optval = sock->conn->pcb.ip->tos;
			break;
		case IP_MULTICAST_TTL:
			*(uint8*) optval = sock->conn->pcb.ip->ttl;
			break;
		case IP_MULTICAST_IF:
			((in_addr*) optval)->s_addr
					= sock->conn->pcb.udp->multicast_ip.addr;
			break;
		}
		break;

	case IPPROTO_TCP:
		switch (optname)
		{
		case TCP_NODELAY:
			*(int*) optval = (sock->conn->pcb.tcp->flags & TF_NODELAY);
			break;
		case TCP_KEEPALIVE:
			*(int*) optval = (int) sock->conn->pcb.tcp->keep_idle;
			break;

#if LWIP_TCP_KEEPALIVE
			case TCP_KEEPIDLE:
			*(int*)optval = (int)(sock->conn->pcb.tcp->keep_idle/1000);
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPIDLE) = %d\n",
							s, *(int *)optval));
			break;
			case TCP_KEEPINTVL:
			*(int*)optval = (int)(sock->conn->pcb.tcp->keep_intvl/1000);
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPINTVL) = %d\n",
							s, *(int *)optval));
			break;
			case TCP_KEEPCNT:
			*(int*)optval = (int)sock->conn->pcb.tcp->keep_cnt;
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, TCP_KEEPCNT) = %d\n",
							s, *(int *)optval));
			break;
#endif /* LWIP_TCP_KEEPALIVE */
		}
		break;
	}
	data->event->SetEvent();
}

int lwip_getsockopt(int s, int level, int optname, void *optval, int *optlen)
{
	int err = ERR_OK;
	BsdSocket *sock = get_socket(s);
	lwip_setgetsockopt_data data;

	if (!sock)
		return -1;

	if ((!optval) || (!optlen))
	{
		sock_set_errno(sock, EFAULT);
		return -1;
	}

	/* Do length and type checks for the various options first, to keep it readable. */
	switch (level)
	{

	/* Level: SOL_SOCKET */
	case SOL_SOCKET:
		switch (optname)
		{
		case SO_ACCEPTCONN:
		case SO_BROADCAST:
			/* UNIMPL case SO_DEBUG: */
			/* UNIMPL case SO_DONTROUTE: */
		case SO_ERROR:
		case SO_KEEPALIVE:
			/* UNIMPL case SO_CONTIMEO: */
			/* UNIMPL case SO_SNDTIMEO: */
		case SO_RCVTIMEO:
		case SO_RCVBUF:
			/* UNIMPL case SO_OOBINLINE: */
			/* UNIMPL case SO_SNDBUF: */
			/* UNIMPL case SO_RCVLOWAT: */
			/* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
			case SO_REUSEADDR:
			case SO_REUSEPORT:
#endif /* SO_REUSE */
		case SO_TYPE:
			/* UNIMPL case SO_USELOOPBACK: */
			if (*optlen < ((int)sizeof(int)))
			{
				err = EINVAL;
			}
			break;
		case SO_NO_CHECK:
			if (*optlen < ((int)sizeof(int)))
			{
				err = EINVAL;
			}
			if (sock->conn->type != NETCONN_UDP)
			{
				/* this flag is only available for UDP, not for UDP lite */
				err = EAFNOSUPPORT;
			}
			break;

		default:
			printf(
					"lwip_getsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
					s, optname);
			err = ENOPROTOOPT;
		} /* switch (optname) */
		break;

		/* Level: IPPROTO_IP */
	case IPPROTO_IP:
		switch (optname)
		{
		/* UNIMPL case IP_HDRINCL: */
		/* UNIMPL case IP_RCVDSTADDR: */
		/* UNIMPL case IP_RCVIF: */
		case IP_TTL:
		case IP_TOS:
			if (*optlen < ((int)sizeof(int)))
			{
				err = EINVAL;
			}
			break;
		case IP_MULTICAST_TTL:
			if (*optlen < ((int)sizeof(uint8)))
			{
				err = EINVAL;
			}
			break;
		case IP_MULTICAST_IF:
			if (*optlen < ((int)sizeof(in_addr)))
			{
				err = EINVAL;
			}
			break;

		default:
			printf("getsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n", s,
					optname);
			err = ENOPROTOOPT;
		} /* switch (optname) */
		break;

		/* Level: IPPROTO_TCP */
	case IPPROTO_TCP:
		if (*optlen < ((int)sizeof(int)))
		{
			err = EINVAL;
			break;
		}

		/* If this is no TCP socket, ignore any options. */
		if (sock->conn->type != NETCONN_TCP)
			return 0;

		switch (optname)
		{
		case TCP_NODELAY:
		case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
			case TCP_KEEPIDLE:
			case TCP_KEEPINTVL:
			case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
			break;

		default:
			printf("getsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
					s, optname);
			err = ENOPROTOOPT;
		} /* switch (optname) */
		break;
		/* UNDEFINED LEVEL */
	default:
		printf("getsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n", s,
				level, optname);
		err = ENOPROTOOPT;
	} /* switch */

	if (err != ERR_OK)
	{
		sock_set_errno(sock, err);
		return -1;
	}
	/* Now do the actual option processing */
	data.sock = sock;
	data.level = level;
	data.optname = optname;
	data.optval = optval;
	data.optlen = optlen;
	data.err = err;
	Event event("GetSockOpt");
	data.event = &event;
	tcpip->SendCallbackMessage(lwip_getsockopt_internal, &data);
	event.Wait();
	err = data.err;
	sock_set_errno(sock, err);
	return err ? -1 : 0;
}

static void lwip_setsockopt_internal(void *arg)
{
	BsdSocket *sock;
	int level, optname;
	const void *optval;
	lwip_setgetsockopt_data * data;

	ASSERT(arg);
	data = (lwip_setgetsockopt_data*) arg;
	sock = data->sock;
	level = data->level;
	optname = data->optname;
	optval = data->optval;
	ip_mreq *imr = (ip_mreq *) optval;

	switch (level)
	{
	case SOL_SOCKET:
		switch (optname)
		{
		case SO_BROADCAST:
			/* UNIMPL case SO_DEBUG: */
			/* UNIMPL case SO_DONTROUTE: */
		case SO_KEEPALIVE:
			/* UNIMPL case SO_OOBINCLUDE: */
// FIXME SO_REUSEADDR is not implemented?
#if SO_REUSE
			case SO_REUSEADDR:
			case SO_REUSEPORT:
#endif /* SO_REUSE */
			/* UNIMPL case SO_USELOOPBACK: */
			if (*(int*) optval)
			{
				sock->conn->pcb.ip->so_options |= optname;
			}
			else
			{
				sock->conn->pcb.ip->so_options &= ~optname;
			}
			break;
		case SO_RCVTIMEO:
			sock->conn->recv_timeout = (*(int*) optval);
			break;
		case SO_RCVBUF:
			sock->conn->recv_bufsize = (*(int*) optval);
			break;
		case SO_NO_CHECK:
			if (*(int*) optval)
			{
				udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_NOCHKSUM);
			}
			else
			{
				udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_NOCHKSUM);
			}
			break;
		}
		break;
	case IPPROTO_IP:
		switch (optname)
		{
		case IP_TTL:
			sock->conn->pcb.ip->ttl = (uint8) (*(int*) optval);
			break;
		case IP_TOS:
			sock->conn->pcb.ip->tos = (uint8) (*(int*) optval);
			break;
		case IP_MULTICAST_TTL:
			sock->conn->pcb.udp->ttl = (uint8) (*(int*) optval);
			break;
		case IP_MULTICAST_IF:
			sock->conn->pcb.udp->multicast_ip.addr
					= ((in_addr*) optval)->s_addr;
			break;
		case IP_ADD_MEMBERSHIP:
		case IP_DROP_MEMBERSHIP:
			/* If this is a TCP or a RAW socket, ignore these options. */
			if (optname == IP_ADD_MEMBERSHIP)
			{
				data->err = igmp_joingroup(
						(ip_addr*) &(imr->imr_interface.s_addr),
						(ip_addr*) &(imr->imr_multiaddr.s_addr));
			}
			else
			{
				data->err = igmp_leavegroup(
						(ip_addr*) &(imr->imr_interface.s_addr),
						(ip_addr*) &(imr->imr_multiaddr.s_addr));
			}
			if (data->err != ERR_OK)
				data->err = EADDRNOTAVAIL;
			break;
		}
		break;
	case IPPROTO_TCP:
		switch (optname)
		{
		case TCP_NODELAY:
			if (*(int*) optval)
				sock->conn->pcb.tcp->flags |= TF_NODELAY;
			else
				sock->conn->pcb.tcp->flags &= ~TF_NODELAY;
			break;
		case TCP_KEEPALIVE:
			sock->conn->pcb.tcp->keep_idle = (uint32) (*(int*) optval);
			break;
#if LWIP_TCP_KEEPALIVE
			case TCP_KEEPIDLE:
			sock->conn->pcb.tcp->keep_idle = 1000*(uint32)(*(int*)optval);
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPIDLE) -> %"U32_F"\n",
							s, sock->conn->pcb.tcp->keep_idle));
			break;
			case TCP_KEEPINTVL:
			sock->conn->pcb.tcp->keep_intvl = 1000*(uint32)(*(int*)optval);
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPINTVL) -> %"U32_F"\n",
							s, sock->conn->pcb.tcp->keep_intvl));
			break;
			case TCP_KEEPCNT:
			sock->conn->pcb.tcp->keep_cnt = (uint32)(*(int*)optval);
			LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPCNT) -> %"U32_F"\n",
							s, sock->conn->pcb.tcp->keep_cnt));
			break;
#endif /* LWIP_TCP_KEEPALIVE */
		}
		break;
	}
	data->event->SetEvent();
}

int lwip_setsockopt(int s, int level, int optname, const void *optval,
		int optlen)
{
	BsdSocket * sock = get_socket(s);
	int err = ERR_OK;

	if (!sock)
		return -1;
	if (!optval)
	{
		sock_set_errno(sock, EFAULT);
		return -1;
	}
	switch (level)
	{
	case SOL_SOCKET:
		switch (optname)
		{
		case SO_BROADCAST:
			/* UNIMPL case SO_DEBUG: */
			/* UNIMPL case SO_DONTROUTE: */
		case SO_KEEPALIVE:
			/* UNIMPL case case SO_CONTIMEO: */
			/* UNIMPL case case SO_SNDTIMEO: */
		case SO_RCVTIMEO:
		case SO_RCVBUF:
			/* UNIMPL case SO_OOBINLINE: */
			/* UNIMPL case SO_SNDBUF: */
			/* UNIMPL case SO_RCVLOWAT: */
			/* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
			case SO_REUSEADDR:
			case SO_REUSEPORT:
#endif /* SO_REUSE */
			/* UNIMPL case SO_USELOOPBACK: */
			if (optlen < ((int)sizeof(int)))
				err = EINVAL;
			break;
		case SO_NO_CHECK:
			if (optlen < ((int)sizeof(int)))
				err = EINVAL;
			/* this flag is only available for UDP, not for UDP lite */
			if (sock->conn->type != NETCONN_UDP)
				err = EAFNOSUPPORT;
			break;
		default:
			printf("setsockopt(?, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
					optname);
			err = ENOPROTOOPT;
		}
		break;
	case IPPROTO_IP:
		switch (optname)
		{
		/* UNIMPL case IP_HDRINCL: */
		/* UNIMPL case IP_RCVDSTADDR: */
		/* UNIMPL case IP_RCVIF: */
		case IP_TTL:
		case IP_TOS:
			if (optlen < ((int)sizeof(int)))
				err = EINVAL;
			break;
		case IP_MULTICAST_TTL:
			if (optlen < ((int)sizeof(uint8)))
				err = EINVAL;
			if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP)
				err = EAFNOSUPPORT;
			break;
		case IP_MULTICAST_IF:
			if (optlen < ((int)sizeof(in_addr)))
				err = EINVAL;
			if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP)
				err = EAFNOSUPPORT;
			break;
		case IP_ADD_MEMBERSHIP:
		case IP_DROP_MEMBERSHIP:
			if (optlen < ((int)sizeof(ip_mreq)))
				err = EINVAL;
			if (NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_UDP)
				err = EAFNOSUPPORT;
			break;
		default:
			printf("setsockopt(?, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
					optname);
			err = ENOPROTOOPT;
		}
		break;
	case IPPROTO_TCP:
		if (optlen < ((int)sizeof(int)))
		{
			err = EINVAL;
			break;
		}
		/* If this is no TCP socket, ignore any options. */
		if (sock->conn->type != NETCONN_TCP)
			return 0;

		switch (optname)
		{
		case TCP_NODELAY:
		case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
			case TCP_KEEPIDLE:
			case TCP_KEEPINTVL:
			case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
			break;

		default:
			printf("setsockopt(?, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
					optname);
			err = ENOPROTOOPT;
		}
		break;
	default:
		printf("setsockopt(?, level=0x%x, UNIMPL: optname=0x%x, ..)\n", level,
				optname);
		err = ENOPROTOOPT;
	}
	if (err != ERR_OK)
	{
		sock_set_errno(sock, err);
		return -1;
	}
	// Now do the actual option processing
	lwip_setgetsockopt_data data;
	data.sock = sock;
	data.level = level;
	data.optname = optname;
	data.optval = (void*) optval;
	data.optlen = &optlen;
	data.err = err;
	Event event("SetSockOpt");
	data.event = &event;
	tcpip->SendCallbackMessage(lwip_setsockopt_internal, &data);
	event.Wait();
	/* maybe lwip_setsockopt_internal has changed err */
	err = data.err;
	sock_set_errno(sock, err);
	return err ? -1 : 0;
}

int lwip_connect(int s, const sockaddr *name, int namelen)
{

	BsdSocket * sock = get_socket(s);
	if (!sock)
		return -1;

	if (!((namelen == sizeof(sockaddr_in))
			&& ((((const sockaddr_in *) name)->sin_family) == AF_INET)))
	{
		printf("lwip_connect: invalid address\n");
		sock_set_errno(sock, err_to_errno(ERR_ARG));
		return -1;
	}
	int err;
	if (((const sockaddr_in *) name)->sin_family == AF_UNSPEC)
	{
		printf("lwip_connect(%d, AF_UNSPEC)\n", s);
		err = netconn_disconnect(sock->conn);
	}
	else
	{
		ip_addr remote_addr;
		uint16 remote_port;

		remote_addr.addr = ((const sockaddr_in *) name)->sin_addr.s_addr;
		remote_port = ((const sockaddr_in *) name)->sin_port;

		printf("lwip_connect(%d, addr=", s);
		ip_addr_debug_print(&remote_addr);
		printf(" port=%d)\n", ntohs(remote_port));

		err = netconn_connect(sock->conn, &remote_addr, ntohs(remote_port));
	}

	if (err != ERR_OK)
	{
		printf("lwip_connect(%d) failed, err=%d\n", s, err);
		sock_set_errno(sock, err_to_errno(err));
		return -1;
	}

	printf("lwip_connect(%d) succeeded\n", s);
	sock_set_errno(sock, 0);
	return 0;
}

int lwip_read(int s, void *mem, size_t len)
{
	return lwip_recvfrom(s, mem, len, 0, NULL, NULL);
}

int lwip_write(int s, const void *data, size_t size)
{
	return lwip_send(s, data, size, 0);
}

