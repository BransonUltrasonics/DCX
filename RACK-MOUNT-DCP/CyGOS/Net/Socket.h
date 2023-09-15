// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/Socket.h_v   1.0   18 Aug 2014 09:16:36   amaurya  $
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

#ifndef SOCKET_H_
#define SOCKET_H_

#include "ArchDefs.h"
#include "string.h"

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h!
 */
#define  SO_DEBUG       0x0001 /* Unimplemented: turn on debugging info recording */
#define  SO_ACCEPTCONN  0x0002 /* socket has had listen() */
#define  SO_REUSEADDR   0x0004 /* Unimplemented: allow local address reuse */
#define  SO_KEEPALIVE   0x0008 /* keep connections alive */
#define  SO_DONTROUTE   0x0010 /* Unimplemented: just use interface addresses */
#define  SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define  SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define  SO_LINGER      0x0080 /* linger on close if data present */
#define  SO_OOBINLINE   0x0100 /* Unimplemented: leave received OOB data in line */
#define  SO_REUSEPORT   0x0200 /* Unimplemented: allow local address & port reuse */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF    0x1001    /* Unimplemented: send buffer size */
#define SO_RCVBUF    0x1002    /* receive buffer size */
#define SO_SNDLOWAT  0x1003    /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT  0x1004    /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO  0x1005    /* Unimplemented: send timeout */
#define SO_RCVTIMEO  0x1006    /* receive timeout */
#define SO_ERROR     0x1007    /* get error status and clear */
#define SO_TYPE      0x1008    /* get socket type */
#define SO_CONTIMEO  0x1009    /* Unimplemented: connect timeout */
#define SO_NO_CHECK  0x100a    /* don't create UDP checksum */

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

/* Flags we can use with send and recv. */
#define MSG_WAITALL    0x02    /* Unimplemented: Requests that the function block until the full amount of data requested can be returned */
#define MSG_DONTWAIT   0x08    /* Nonblocking i/o for this operation only */
#define MSG_MORE       0x10    /* Sender will send more */

/*
 * Options for level IPPROTO_IP
 */
#define IP_TOS             1
#define IP_TTL             2

/*
 * Options for level IPPROTO_TCP
 */
#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */

/*
 * TCP options.
 */
#define TCP_NODELAY     0x0001

/*
 * Options and types for UDP multicast traffic handling
 */
#define IP_ADD_MEMBERSHIP  3
#define IP_DROP_MEMBERSHIP 4
#define IP_MULTICAST_TTL   5
#define IP_MULTICAST_IF    6
#define IP_MULTICAST_LOOP  7

#define MSG_PEEK        0x2             /* peek at incoming message */

/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 * lwip_ioctl only supports FIONREAD and FIONBIO, for now
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    0x7fU           /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
/* 0x20000000 distinguishes new &
 old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

#ifndef FIONREAD
#define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
#endif
#ifndef FIONBIO
#define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
#endif

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff          /* options for socket level */

#define INVALID_SOCKET -1

#define SOCKET_ERROR -1

#define IP_ADDR_ANY_VALUE 0x00000000UL
#define IP_ADDR_BROADCAST_VALUE 0xffffffffUL

/* FD_SET used for lwip_select. Surrounded by ifdef just in case if same source
 * is used elsewhere where FD_ macros are already defined */
#ifndef FD_SET
#undef  FD_SETSIZE
#define FD_SETSIZE    256

struct fd_set
{
	uint8 fd_bits[(FD_SETSIZE + 7) / 8];
};

inline void FD_SET(int n, fd_set * p)
{
	p->fd_bits[n / 8] |= (1 << (n & 7));
}

inline void FD_CLR(int n, fd_set * p)
{
	p->fd_bits[n / 8] &= ~(1 << (n & 7));
}

inline int FD_ISSET(int n, fd_set * p)
{
	return p->fd_bits[n / 8] & (1 << (n & 7));
}

inline void FD_ZERO(fd_set * p)
{
	memset(p, 0, sizeof(fd_set));
}

#endif

struct sockaddr
{
	uint16 sa_family; /* address family */
	char sa_data[14]; /* up to 14 bytes of direct address */
};

struct in_addr
{
	union
	{
		struct
		{
			uint8 s_b1, s_b2, s_b3, s_b4;
		} S_un_b;
		struct
		{
			uint16 s_w1, s_w2;
		} S_un_w;
		uint32 S_addr;
	} S_un;
#define s_addr  S_un.S_addr
	/* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
	/* host on imp */
#define s_net   S_un.S_un_b.s_b1
	/* network */
#define s_imp   S_un.S_un_w.s_w2
	/* imp */
#define s_impno S_un.S_un_b.s_b4
	/* imp # */
#define s_lh    S_un.S_un_b.s_b3
	/* logical host */
};

/*
 * Socket address, internet style.
 */
struct sockaddr_in
{
	short sin_family;
	uint16 sin_port;
	in_addr sin_addr;
	char sin_zero[8];
};

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct timeval
{
	long tv_sec; /* seconds */
	long tv_usec; /* and microseconds */
};

struct ip_mreq
{
	in_addr imr_multiaddr; /* IP multicast address of group */
	in_addr imr_interface; /* local IP address of interface */
};

void lwip_socket_init();
int lwip_socket(int domain, int type, int protocol);
int lwip_accept(int s, sockaddr *addr, int *addrlen);
int lwip_bind(int s, const sockaddr *name, uint32 namelen);
int lwip_ioctl(int s, long cmd, void *argp);
int lwip_listen(int s, int backlog);
int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset,
		fd_set *exceptset, timeval *timeout);
int lwip_selscan(int maxfdp1, fd_set *readset, fd_set *writeset,
		fd_set *exceptset);
int lwip_recv(int s, void *mem, size_t len, int flags);
int lwip_recvfrom(int s, void *mem, size_t len, int flags, sockaddr *from,
		int *fromlen);
int lwip_send(int s, const void *data, size_t size, int flags);
int lwip_sendto(int s, const void *data, size_t size, int flags,
		const sockaddr *to, int tolen);
int lwip_close(int s);
int lwip_getpeername(int s, sockaddr *name, int *namelen);
int lwip_getsockname(int s, sockaddr *name, int *namelen);
int lwip_getsockopt(int s, int level, int optname, void *optval, int *optlen);
int lwip_setsockopt(int s, int level, int optname, const void *optval,
		int optlen);
int lwip_connect(int s, const sockaddr *name, int namelen);
int lwip_read(int s, void *mem, size_t len);
int lwip_write(int s, const void *dataptr, size_t size);

int lwip_errno(int s);

inline int accept(int s, sockaddr *addr, int *addrlen)
{
	return lwip_accept(s, addr, addrlen);
}
inline int bind(int s, const sockaddr *name, uint32 namelen)
{
	return lwip_bind(s, name, namelen);
}
inline int closesocket(int s)
{
	return lwip_close(s);
}
inline int connect(int s, const sockaddr *name, int namelen)
{
	return lwip_connect(s, name, namelen);
}
inline int getpeername(int s, sockaddr *name, int *namelen)
{
	return lwip_getpeername(s, name, namelen);
}
inline int getsockname(int s, sockaddr *name, int *namelen)
{
	return lwip_getsockname(s, name, namelen);
}
inline int getsockopt(int s, int level, int optname, void *optval, int *optlen)
{
	return lwip_getsockopt(s, level, optname, optval, optlen);
}
inline int setsockopt(int s, int level, int optname, const void *optval,
		int optlen)
{
	return lwip_setsockopt(s, level, optname, optval, optlen);
}
inline int listen(int s, int backlog)
{
	return lwip_listen(s, backlog);
}
inline int recv(int s, void *mem, size_t len, int flags)
{
	return lwip_recv(s, mem, len, flags);
}
inline int recvfrom(int s, void *mem, size_t len, int flags, sockaddr *from,
		int *fromlen)
{
	return lwip_recvfrom(s, mem, len, flags, from, fromlen);
}
inline int send(int s, const void *data, size_t size, int flags)
{
	return lwip_send(s, data, size, flags);
}
inline int sendto(int s, const void *data, size_t size, int flags,
		const sockaddr *to, int tolen)
{
	return lwip_sendto(s, data, size, flags, to, tolen);
}
inline int socket(int domain, int type, int protocol)
{
	return lwip_socket(domain, type, protocol);
}
inline int select(int maxfdp1, fd_set *readset, fd_set *writeset,
		fd_set *exceptset, timeval *timeout)
{
	return lwip_select(maxfdp1, readset, writeset, exceptset, timeout);
}
inline int ioctlsocket(int s, long cmd, void *argp)
{
	return lwip_ioctl(s, cmd, argp);
}

inline uint32 MakeIP(int a, int b, int c, int d)
{
	return htonl(((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8)
			| ((d & 0xff) << 0));
}

inline uint32 ClassA()
{
	return MakeIP(255, 0, 0, 0);
}

inline uint32 ClassB()
{
	return MakeIP(255, 255, 0, 0);
}

inline uint32 ClassC()
{
	return MakeIP(255, 255, 255, 0);
}

#endif
