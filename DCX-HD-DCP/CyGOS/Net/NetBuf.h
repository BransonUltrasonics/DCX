// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/NetBuf.h_v   1.0   29 Oct 2010 10:39:34   ygupta  $
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

#ifndef NETBUF_H_
#define NETBUF_H_

struct PktBuf;
struct ip_addr;

struct NetBuf
{
	PktBuf *p, *ptr;
	ip_addr *addr;
	int port;
};

void netbuf_delete(NetBuf *buf);
int netbuf_ref(NetBuf *buf, const void *dataptr, int size);
void netbuf_free(NetBuf *buf);
void * netbuf_alloc(NetBuf *buf, int size);
void netbuf_chain(NetBuf *head, NetBuf *tail);
int netbuf_data(NetBuf *buf, void **dataptr, int *len);
int netbuf_next(NetBuf *buf);
void netbuf_first(NetBuf *buf);

#define netbuf_copy_partial(buf, dataptr, len, offset) \
  pbuf_copy_partial((buf)->p, (dataptr), (len), (offset))
#define netbuf_copy(buf,dataptr,len) netbuf_copy_partial(buf, dataptr, len, 0)
#define netbuf_take(buf, dataptr, len) pbuf_take((buf)->p, dataptr, len)
#define netbuf_len(buf)              ((buf)->p->tot_len)
#define netbuf_fromaddr(buf)         ((buf)->addr)
#define netbuf_fromport(buf)         ((buf)->port)

#endif
