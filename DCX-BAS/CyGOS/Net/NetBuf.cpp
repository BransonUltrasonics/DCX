// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/NetBuf.cpp_v   1.0   21 Mar 2012 12:41:42   PDwivedi  $
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

#include "NetBuf.h"
#include "PktBuf.h"
#include "Net.h"
#include "stdio.h"
#include "assert.h"
#include "DebugNew.h"

/**
 * Deallocate a netbuf allocated by netbuf_new().
 *
 * @param buf pointer to a netbuf allocated by netbuf_new()
 */
void netbuf_delete(NetBuf *buf)
{
	if (buf)
	{
		if (buf->p)
			pbuf_free(buf->p);
		delete buf;
	}
}

/**
 * Let a netbuf reference existing (non-volatile) data.
 *
 * @param buf netbuf which should reference the data
 * @param dataptr pointer to the data to reference
 * @param size size of the data
 * @return ERR_OK if data is referenced
 *         ERR_MEM if data couldn't be referenced due to lack of memory
 */
int netbuf_ref(NetBuf *buf, const void *dataptr, int size)
{
	if (!buf)
	{
		printf("netbuf_ref: invalid buf\n");
		return ERR_ARG;
	}
	if (buf->p != NULL)
	{
		pbuf_free(buf->p);
	}
	buf->p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
	if (buf->p == NULL)
	{
		buf->ptr = NULL;
		return ERR_MEM;
	}
	buf->p->payload = (uint8 *) dataptr;
	buf->p->len = buf->p->tot_len = size;
	buf->ptr = buf->p;
	return ERR_OK;
}

/**
 * Free the packet buffer included in a netbuf
 *
 * @param buf pointer to the netbuf which contains the packet buffer to free
 */
void netbuf_free(NetBuf *buf)
{
	if (!buf)
	{
		printf("netbuf_free: invalid buf\n");
		return;
	}
	if (buf->p != NULL)
	{
		pbuf_free(buf->p);
	}
	buf->p = buf->ptr = NULL;
}

/**
 * Allocate memory for a packet buffer for a given netbuf.
 *
 * @param buf the netbuf for which to allocate a packet buffer
 * @param size the size of the packet buffer to allocate
 * @return pointer to the allocated memory
 *         NULL if no memory could be allocated
 */
void * netbuf_alloc(NetBuf *buf, uint16 size)
{
	if (!buf)
	{
		printf("netbuf_alloc: invalid buf\n");
		return NULL;
	}
	/* Deallocate any previously allocated memory. */
	if (buf->p)
		pbuf_free(buf->p);
	buf->p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
	if (!buf->p)
		return NULL;
	ASSERT(buf->p->len >= size);
	buf->ptr = buf->p;
	return buf->p->payload;
}

/**
 * Chain one netbuf to another (@see pbuf_chain)
 *
 * @param head the first netbuf
 * @param tail netbuf to chain after head, freed by this function, may not be reference after returning
 */
void netbuf_chain(NetBuf *head, NetBuf *tail)
{
	if (head && tail)
	{
		pbuf_cat(head->p, tail->p);
		head->ptr = head->p;
		delete tail;
	}
}

/**
 * Get the data pointer and length of the data inside a netbuf.
 *
 * @param buf netbuf to get the data from
 * @param dataptr pointer to a void pointer where to store the data pointer
 * @param len pointer to an u16_t where the length of the data is stored
 * @return ERR_OK if the information was retreived,
 *         ERR_BUF on error.
 */
int netbuf_data(NetBuf *buf, void **dataptr, int *len)
{
	if (!buf || !dataptr || !len)
		return ERR_ARG;

	if (buf->ptr == NULL)
	{
		return ERR_BUF;
	}
	*dataptr = buf->ptr->payload;
	*len = buf->ptr->len;
	return ERR_OK;
}

/**
 * Move the current data pointer of a packet buffer contained in a netbuf
 * to the next part.
 * The packet buffer itself is not modified.
 *
 * @param buf the netbuf to modify
 * @return -1 if there is no next part
 *         1  if moved to the next part but now there is no next part
 *         0  if moved to the next part and there are still more parts
 */
int netbuf_next(NetBuf *buf)
{
	if (!buf)
		return -1;
	if (buf->ptr->next == NULL)
		return -1;
	buf->ptr = buf->ptr->next;
	if (buf->ptr->next == NULL)
		return 1;
	return 0;
}

/**
 * Move the current data pointer of a packet buffer contained in a netbuf
 * to the beginning of the packet.
 * The packet buffer itself is not modified.
 *
 * @param buf the netbuf to modify
 */
void netbuf_first(NetBuf *buf)
{
	if (buf)
		buf->ptr = buf->p;
}
