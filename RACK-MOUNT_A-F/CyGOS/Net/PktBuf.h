// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/PktBuf.h_v   1.0   09 Jun 2015 09:13:54   tayars  $
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

#ifndef PKTBUF_H_
#define PKTBUF_H_

#include "ArchDefs.h"

enum pbuf_layer
{
	PBUF_TRANSPORT, PBUF_IP, PBUF_LINK, PBUF_RAW,
};

enum pbuf_type
{
	/* pbuf data is stored in RAM */
	PBUF_RAM,
	/* pbuf data is stored in ROM */
	PBUF_ROM,
	/* pbuf comes from the pbuf pool */
	PBUF_REF,
	/* pbuf payload refers to RAM */
	PBUF_POOL,
};

#define PBUF_TRANSPORT_HLEN 20
#define PBUF_IP_HLEN        20
#define PBUF_LINK_HLEN      14

/** indicates this packet's data should be immediately passed to the application */
#define PBUF_FLAG_PUSH 0x01U

#define SIZEOF_STRUCT_PBUF        LWIP_MEM_ALIGN_SIZE(sizeof(PktBuf))

/**
 * Packet buffer structure.
 */
struct PktBuf
{
	/** next pbuf in singly linked pbuf chain */
	PktBuf *next;
	/** pointer to the actual data in the buffer */
	uint8 *payload;
	/**
	 * total length of this buffer and all next buffers in chain
	 * belonging to the same packet.
	 *
	 * For non-queue packet chains this is the invariant:
	 * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
	 */
	uint16 tot_len;
	/** length of this buffer */
	uint16 len;
	/** pbuf_type as u8_t instead of enum to save space */
	uint8 /*pbuf_type*/type;
	/** misc flags */
	uint8 flags;
	/**
	 * the reference count always equals the number of pointers
	 * that refer to this pbuf. This can be pointers from an application,
	 * the stack itself, or pbuf->next pointers from a chain.
	 */
	uint16 ref;
};

PktBuf * pbuf_alloc(pbuf_layer l, int size, pbuf_type type);
int pbuf_free(PktBuf *p);
int pbuf_header(PktBuf *p, int header_size_increment);
void pbuf_realloc(PktBuf *p, int new_len);
int pbuf_clen(PktBuf *p);
void pbuf_cat(PktBuf * h, PktBuf * t);
PktBuf * alloc_pool_pbuf();
void pbuf_chain(PktBuf *h, PktBuf *t);
int inet_chksum_pbuf(PktBuf *p);
int pbuf_copy(PktBuf *p_to, PktBuf *p_from);
void pbuf_ref(PktBuf *p);
int pbuf_copy_partial(PktBuf *buf, void *dataptr, int len, int offset);
PktBuf * pbuf_dechain(PktBuf *p);
PktBuf * pbuf_coalesce(PktBuf *p, pbuf_layer layer);
int pbuf_take(PktBuf *buf, const void *dataptr, int len);
PktBuf * pbuf_dechain(PktBuf *p);

#endif
