// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/PktBuf.cpp_v   1.0   21 Mar 2012 12:42:04   PDwivedi  $
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

#include "PktBuf.h"
#include "Net.h"
#include "TCP.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "DebugNew.h"

#define PBUF_POOL_BUFSIZE (sizeof(PktBuf) + 1536)
#define PBUF_POOL_BUFSIZE_ALIGNED LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE)

/**
 * Allocates a pbuf of the given type (possibly a chain for PBUF_POOL type).
 *
 * The actual memory allocated for the pbuf is determined by the
 * layer at which the pbuf is allocated and the requested size
 * (from the size parameter).
 *
 * @param layer flag to define header size
 * @param length size of the pbuf's payload
 * @param type this parameter decides how and where the pbuf
 * should be allocated as follows:
 *
 * - PBUF_RAM: buffer memory for pbuf is allocated as one large
 *             chunk. This includes protocol headers as well.
 * - PBUF_ROM: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. Additional headers must be prepended
 *             by allocating another pbuf and chain in to the front of
 *             the ROM pbuf. It is assumed that the memory used is really
 *             similar to ROM in that it is immutable and will not be
 *             changed. Memory which is dynamic should generally not
 *             be attached to PBUF_ROM pbufs. Use PBUF_REF instead.
 * - PBUF_REF: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. It is assumed that the pbuf is only
 *             being used in a single thread. If the pbuf gets queued,
 *             then pbuf_take should be called to copy the buffer.
 * - PBUF_POOL: the pbuf is allocated as a pbuf chain, with pbufs from
 *              the pbuf pool that is allocated during pbuf_init().
 *
 * @return the allocated pbuf. If multiple pbufs where allocated, this
 * is the first pbuf of a pbuf chain.
 */
PktBuf * pbuf_alloc(pbuf_layer layer, int length, pbuf_type type)
{
	PktBuf *p, *q, *r;
	uint16 offset;
	int32 rem_len; /* remaining length */

	/* determine header offset */
	offset = 0;
	switch (layer)
	{
	case PBUF_TRANSPORT:
		/* add room for transport (often TCP) layer header */
		offset += PBUF_TRANSPORT_HLEN;
		/* FALLTHROUGH */
	case PBUF_IP:
		/* add room for IP layer header */
		offset += PBUF_IP_HLEN;
		/* FALLTHROUGH */
	case PBUF_LINK:
		/* add room for link layer header */
		offset += PBUF_LINK_HLEN;
		break;
	case PBUF_RAW:
		break;
	default:
		printf("pbuf_alloc: bad pbuf layer %d\n", layer);
		return NULL;
	}

	switch (type)
	{
	case PBUF_POOL:
		/* allocate head of pbuf chain into p */
		p = alloc_pool_pbuf();
		if (p == NULL)
		{
			return NULL;
		}
		p->type = type;
		p->next = NULL;

		/* make the payload pointer point 'offset' bytes into pbuf data memory */
		p->payload = (uint8 *) LWIP_MEM_ALIGN(((uint8 *) p
						+ (SIZEOF_STRUCT_PBUF + offset)));
		/* the total length of the pbuf chain is the requested size */
		p->tot_len = length;
		/* set the length of the first pbuf in the chain */
		p->len = min(length, (int)PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(
						offset));
		/* set reference count (needed here in case we fail) */
		p->ref = 1;

		/* now allocate the tail of the pbuf chain */

		/* remember first pbuf for linkage in next iteration */
		r = p;
		/* remaining length to be allocated */
		rem_len = length - p->len;
		/* any remaining pbufs to be allocated? */
		while (rem_len > 0)
		{
			q = alloc_pool_pbuf();
			if (q == NULL)
			{
				/* free chain so far allocated */
				pbuf_free(p);
				/* bail out unsuccesfully */
				return NULL;
			}
			q->type = type;
			q->flags = 0;
			q->next = NULL;
			/* make previous pbuf point to this pbuf */
			r->next = q;
			/* set total length of this pbuf and next in chain */
			q->tot_len = (uint16) rem_len;
			/* this pbuf length is pool size, unless smaller sized tail */
			q->len = min((uint16) rem_len, PBUF_POOL_BUFSIZE_ALIGNED);
			q->payload = ((uint8 *) q + SIZEOF_STRUCT_PBUF);
			q->ref = 1;
			/* calculate remaining length to be allocated */
			rem_len -= q->len;
			/* remember this pbuf for linkage in next iteration */
			r = q;
		}
		/* end of chain */
		/*r->next = NULL;*/

		break;
	case PBUF_RAM:
		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		p = (PktBuf *) new uint8[LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF
				+ offset) + LWIP_MEM_ALIGN_SIZE(length)];
		if (p == NULL)
		{
			return NULL;
		}
		/* Set up internal structure of the pbuf. */
		p->payload = (uint8 *) LWIP_MEM_ALIGN(((uint8 *) p + SIZEOF_STRUCT_PBUF
						+ offset));
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		break;
		/* pbuf references existing (non-volatile static constant) ROM payload? */
	case PBUF_ROM:
		/* pbuf references existing (externally allocated) RAM payload? */
	case PBUF_REF:
		/* only allocate memory for the pbuf structure */
		p = new PktBuf();
		if (p == NULL)
		{
			return NULL;
		}
		/* caller must set this field properly, afterwards */
		p->payload = NULL;
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		break;
	default:
		printf("pbuf_alloc: erroneous type %d\n", type);
		return NULL;
	}
	/* set reference count */
	p->ref = 1;
	/* set flags */
	p->flags = 0;
	return p;
}

/**
 * Dereference a pbuf chain or queue and deallocate any no-longer-used
 * pbufs at the head of this chain or queue.
 *
 * Decrements the pbuf reference count. If it reaches zero, the pbuf is
 * deallocated.
 *
 * For a pbuf chain, this is repeated for each pbuf in the chain,
 * up to the first pbuf which has a non-zero reference count after
 * decrementing. So, when all reference counts are one, the whole
 * chain is free'd.
 *
 * @param p The pbuf (chain) to be dereferenced.
 *
 * @return the number of pbufs that were de-allocated
 * from the head of the chain.
 *
 * @note MUST NOT be called on a packet queue (Not verified to work yet).
 * @note the reference counter of a pbuf equals the number of pointers
 * that refer to the pbuf (or into the pbuf).
 *
 * @internal examples:
 *
 * Assuming existing chains a->b->c with the following reference
 * counts, calling pbuf_free(a) results in:
 *
 * 1->2->3 becomes ...1->3
 * 3->3->3 becomes 2->3->3
 * 1->1->2 becomes ......1
 * 2->1->1 becomes 1->1->1
 * 1->1->1 becomes .......
 *
 */
int pbuf_free(PktBuf *p)
{
	/* de-allocate all consecutive pbufs from the head of the chain that
	 * obtain a zero reference count after decrementing*/
	int count = 0;
	while (p)
	{
		/* all pbufs in a chain are referenced at least once */
		ASSERT(p->ref > 0);
		/* decrease reference count (number of pointers to pbuf) */
		int ref = --(p->ref);
		if (ref)
			break;
		/* remember next pbuf in chain for next iteration */
		PktBuf * q = p->next;
		delete p;
		p = q;
		count++;
		/* p->ref > 0, this pbuf is still referenced to */
		/* (and so the remaining pbufs in chain as well) */
	}
	return count;
}

/**
 * Adjusts the payload pointer to hide or reveal headers in the payload.
 *
 * Adjusts the ->payload pointer so that space for a header
 * (dis)appears in the pbuf payload.
 *
 * The ->payload, ->tot_len and ->len fields are adjusted.
 *
 * @param p pbuf to change the header size.
 * @param header_size_increment Number of bytes to increment header size which
 * increases the size of the pbuf. New space is on the front.
 * (Using a negative value decreases the header size.)
 * If hdr_size_inc is 0, this function does nothing and returns succesful.
 *
 * PBUF_ROM and PBUF_REF type buffers cannot have their sizes increased, so
 * the call will fail. A check is made that the increase in header size does
 * not move the payload pointer in front of the start of the buffer.
 * @return non-zero on failure, zero on success.
 *
 */
int pbuf_header(PktBuf *p, int header_size_increment)
{
	uint16 type;
	uint8 *payload;
	uint16 increment_magnitude;

	ASSERT(p);
	if ((header_size_increment == 0) || (p == NULL))
		return 0;

	if (header_size_increment < 0)
	{
		increment_magnitude = -header_size_increment;
		/* Check that we aren't going to move off the end of the pbuf */
		if (increment_magnitude > p->len)
		{
			printf("increment_magnitude > p->len");
			return 1;
		}
	}
	else
	{
		increment_magnitude = header_size_increment;
	}

	type = p->type;
	/* remember current payload pointer */
	payload = p->payload;

	/* pbuf types containing payloads? */
	if (type == PBUF_RAM || type == PBUF_POOL)
	{
		/* set new payload pointer */
		p->payload = p->payload - header_size_increment;
		/* boundary check fails? */
		if (p->payload < (uint8 *) p + SIZEOF_STRUCT_PBUF)
		{
			printf(
					"pbuf_header: failed as %p < %p (not enough space for new header size)\n",
					p->payload, p + 1);
			/* restore old payload pointer */
			p->payload = payload;
			/* bail out unsuccesfully */
			return 1;
		}
		/* pbuf types refering to external payloads? */
	}
	else if (type == PBUF_REF || type == PBUF_ROM)
	{
		/* hide a header in the payload? */
		if ((header_size_increment < 0) && (increment_magnitude <= p->len))
		{
			/* increase payload pointer */
			p->payload = p->payload - header_size_increment;
		}
		else
		{
			/* cannot expand payload to front (yet!)
			 * bail out unsuccesfully */
			return 1;
		}
	}
	else
	{
		/* Unknown type */
		ASSERT(0);
		return 1;
	}
	/* modify pbuf length fields */
	p->len += header_size_increment;
	p->tot_len += header_size_increment;
	return 0;
}

static void *mem_realloc(void *mem, int size)
{
	return mem;
	ASSERT(size);
}

/**
 * Attempt to reclaim some memory from queued out-of-sequence TCP segments
 * if we run out of pool pbufs. It's better to give priority to new packets
 * if we're running out.
 *
 * @return the allocated pbuf.
 */
PktBuf * alloc_pool_pbuf()
{
	TcpSocket *pcb;
	PktBuf *p;

	retry: p = (PktBuf *) new uint8[PBUF_POOL_BUFSIZE];
	if (NULL == p)
	{
		for (pcb = tcp_active_pcbs; NULL != pcb; pcb = pcb->next)
		{
			if (NULL != pcb->ooseq)
			{
				tcp_segs_free(pcb->ooseq);
				pcb->ooseq = NULL;
				goto retry;
			}
		}
	}
	return p;
}

/**
 * Shrink a pbuf chain to a desired length.
 *
 * @param p pbuf to shrink.
 * @param new_len desired new length of pbuf chain
 *
 * Depending on the desired length, the first few pbufs in a chain might
 * be skipped and left unchanged. The new last pbuf in the chain will be
 * resized, and any remaining pbufs will be freed.
 *
 * @note If the pbuf is ROM/REF, only the ->tot_len and ->len fields are adjusted.
 * @note May not be called on a packet queue.
 *
 * @note Despite its name, pbuf_realloc cannot grow the size of a pbuf (chain).
 */
void pbuf_realloc(PktBuf *p, int new_len)
{
	PktBuf *q;
	uint16 rem_len; /* remaining length */
	int32 grow;

	/* desired length larger than current length? */
	if (new_len >= p->tot_len)
	{
		/* enlarging not yet supported */
		return;
	}

	/* the pbuf chain grows by (new_len - p->tot_len) bytes
	 * (which may be negative in case of shrinking) */
	grow = new_len - p->tot_len;

	/* first, step over any pbufs that should remain in the chain */
	rem_len = new_len;
	q = p;
	/* should this pbuf be kept? */
	while (rem_len > q->len)
	{
		/* decrease remaining length by pbuf length */
		rem_len -= q->len;
		/* decrease total length indicator */
		q->tot_len += (uint16) grow;
		/* proceed to next pbuf in chain */
		q = q->next;
	}
	/* we have now reached the new last pbuf (in q) */
	/* rem_len == desired length for pbuf q */

	/* shrink allocated memory for PBUF_RAM */
	/* (other types merely adjust their length fields */
	if ((q->type == PBUF_RAM) && (rem_len != q->len))
	{
		/* reallocate and adjust the length of the pbuf that will be split */
		q = (PktBuf *) mem_realloc(q, (uint8 *) q->payload - (uint8 *) q
				+ rem_len);
	}
	/* adjust length fields for new last pbuf */
	q->len = rem_len;
	q->tot_len = q->len;

	/* any remaining pbufs in chain? */
	if (q->next != NULL)
	{
		/* free remaining pbufs in chain */
		pbuf_free(q->next);
	}
	/* q is last packet in chain */
	q->next = NULL;
}

/**
 * Concatenate two pbufs (each may be a pbuf chain) and take over
 * the caller's reference of the tail pbuf.
 *
 * @note The caller MAY NOT reference the tail pbuf afterwards.
 * Use pbuf_chain() for that purpose.
 *
 * @see pbuf_chain()
 */
void pbuf_cat(PktBuf * h, PktBuf * t)
{
	PktBuf * p;

	if (!h || !t)
	{
		printf("h or t is NULL\n");
		return;
	}

	/* proceed to last pbuf of chain */
	for (p = h; p->next != NULL; p = p->next)
	{
		/* add total length of second chain to all totals of first chain */
		p->tot_len += t->tot_len;
	}
	/* { p is last pbuf of first h chain, p->next == NULL } */
	ASSERT(p->tot_len == p->len);
	ASSERT(p->next == NULL);
	/* add total length of second chain to last pbuf total of first chain */
	p->tot_len += t->tot_len;
	/* chain last pbuf of head (p) with first of tail (t) */
	p->next = t;
	/* p->next now references t, but the caller will drop its reference to t,
	 * so netto there is no change to the reference count of t.
	 */
}

/**
 * Chain two pbufs (or pbuf chains) together.
 *
 * The caller MUST call pbuf_free(t) once it has stopped
 * using it. Use pbuf_cat() instead if you no longer use t.
 *
 * @param h head pbuf (chain)
 * @param t tail pbuf (chain)
 * @note The pbufs MUST belong to the same packet.
 * @note MAY NOT be called on a packet queue.
 *
 * The ->tot_len fields of all pbufs of the head chain are adjusted.
 * The ->next field of the last pbuf of the head chain is adjusted.
 * The ->ref field of the first pbuf of the tail chain is adjusted.
 *
 */
void pbuf_chain(PktBuf *h, PktBuf *t)
{
	pbuf_cat(h, t);
	/* t is now referenced by h */
	pbuf_ref(t);
}

/**
 * Count number of pbufs in a chain
 *
 * @param p first pbuf of chain
 * @return the number of pbufs in a chain
 */
int pbuf_clen(PktBuf *p)
{
	int len = 0;
	while (p)
	{
		++len;
		p = p->next;
	}
	return len;
}

/**
 *
 * Create PBUF_RAM copies of pbufs.
 *
 * Used to queue packets on behalf of the lwIP stack, such as
 * ARP based queueing.
 *
 * @note You MUST explicitly use p = pbuf_take(p);
 *
 * @note Only one packet is copied, no packet queue!
 *
 * @param p_to pbuf destination of the copy
 * @param p_from pbuf source of the copy
 *
 * @return ERR_OK if pbuf was copied
 *         ERR_ARG if one of the pbufs is NULL or p_to is not big
 *                 enough to hold p_from
 */
int pbuf_copy(PktBuf *p_to, PktBuf *p_from)
{
	uint16 offset_to = 0, offset_from = 0, len;

	/* is the target big enough to hold the source? */
	if ((p_to == NULL) || (p_from == NULL) || (p_to->tot_len < p_from->tot_len))
	{
		printf("pbuf_copy: target not big enough to hold source\n");
		return ERR_ARG;
	}
	/* iterate through pbuf chain */
	do
	{
		ASSERT(p_to);
		/* copy one part of the original chain */
		if ((p_to->len - offset_to) >= (p_from->len - offset_from))
		{
			/* complete current p_from fits into current p_to */
			len = p_from->len - offset_from;
		}
		else
		{
			/* current p_from does not fit into current p_to */
			len = p_to->len - offset_to;
		}
		memcpy((uint8*) p_to->payload + offset_to, (uint8*) p_from->payload
				+ offset_from, len);
		offset_to += len;
		offset_from += len;
		ASSERT(offset_to <= p_to->len);
		if (offset_to == p_to->len)
		{
			/* on to next p_to (if any) */
			offset_to = 0;
			p_to = p_to->next;
		}
		ASSERT(offset_from <= p_from->len);
		if (offset_from >= p_from->len)
		{
			/* on to next p_from (if any) */
			offset_from = 0;
			p_from = p_from->next;
		}
		if ((p_from != NULL) && (p_from->len == p_from->tot_len))
		{
			/* don't copy more than one packet! */
			if (p_from->next != NULL)
			{
				printf("pbuf_copy() does not allow packet queues!\n");
				return ERR_VAL;
			}
		}
		if ((p_to != NULL) && (p_to->len == p_to->tot_len))
		{
			/* don't copy more than one packet! */
			if (p_to->next != NULL)
			{
				printf("pbuf_copy() does not allow packet queues!\n");
				return ERR_VAL;
			}
		}
	} while (p_from);
	return ERR_OK;
}

/**
 * Increment the reference count of the pbuf.
 *
 * @param p pbuf to increase reference counter of
 *
 */
void pbuf_ref(PktBuf *p)
{
	if (p)
		++p->ref;
}

/**
 * Copy (part of) the contents of a packet buffer
 * to an application supplied buffer.
 *
 * @param buf the pbuf from which to copy data
 * @param dataptr the application supplied buffer
 * @param len length of data to copy (dataptr must be big enough). No more
 * than buf->tot_len will be copied, irrespective of len
 * @param offset offset into the packet buffer from where to begin copying len bytes
 * @return the number of bytes copied, or 0 on failure
 */
int pbuf_copy_partial(PktBuf *buf, void *dataptr, int len, int offset)
{
	PktBuf *p;
	int left;
	int buf_copy_len;
	int copied_total = 0;

	left = 0;

	if ((buf == NULL) || (dataptr == NULL))
	{
		return 0;
	}

	/* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
	for (p = buf; len != 0 && p != NULL; p = p->next)
	{
		if ((offset != 0) && (offset >= p->len))
		{
			/* don't copy from this buffer -> on to the next */
			offset -= p->len;
		}
		else
		{
			/* copy from this buffer. maybe only partially. */
			buf_copy_len = p->len - offset;
			if (buf_copy_len > len)
				buf_copy_len = len;
			/* copy the necessary parts of the buffer */
			memcpy(&((char*) dataptr)[left], &((char*) p->payload)[offset],
					buf_copy_len);
			copied_total += buf_copy_len;
			left += buf_copy_len;
			len -= buf_copy_len;
			offset = 0;
		}
	}
	return copied_total;
}

/**
 * Dechains the first pbuf from its succeeding pbufs in the chain.
 *
 * Makes p->tot_len field equal to p->len.
 * @param p pbuf to dechain
 * @return remainder of the pbuf chain, or NULL if it was de-allocated.
 * @note May not be called on a packet queue.
 */
PktBuf * pbuf_dechain(PktBuf *p)
{
	PktBuf *q;
	int tail_gone = 1;
	/* tail */
	q = p->next;
	/* pbuf has successor in chain? */
	if (q != NULL)
	{
		/* assert tot_len invariant: (p->tot_len == p->len + (p->next? p->next->tot_len: 0) */
		ASSERT(q->tot_len == p->tot_len - p->len);
		/* enforce invariant if assertion is disabled */
		q->tot_len = p->tot_len - p->len;
		/* decouple pbuf from remainder */
		p->next = NULL;
		/* total length of pbuf p is its own length only */
		p->tot_len = p->len;
		/* q is no longer referenced by p, free it */
		tail_gone = pbuf_free(q);
		/* return remaining tail or NULL if deallocated */
	}
	/* assert tot_len invariant: (p->tot_len == p->len + (p->next? p->next->tot_len: 0) */
	ASSERT(p->tot_len == p->len);
	return ((tail_gone > 0) ? NULL : q);
}

/**
 * Copy application supplied data into a pbuf.
 * This function can only be used to copy the equivalent of buf->tot_len data.
 *
 * @param buf pbuf to fill with data
 * @param dataptr application supplied data buffer
 * @param len length of the application supplied data buffer
 *
 * @return ERR_OK if successful, ERR_MEM if the pbuf is not big enough
 */
int pbuf_take(PktBuf *buf, const void *dataptr, int len)
{
	PktBuf *p;
	uint16 buf_copy_len;
	uint16 total_copy_len = len;
	uint16 copied_total = 0;

	if (!buf)
	{
		printf("pbuf_take: invalid buf\n");
		return 0;
	}
	if (!dataptr)
	{
		printf("pbuf_take: invalid dataptr\n");
		return 0;
	}

	if ((buf == NULL) || (dataptr == NULL) || (buf->tot_len < len))
	{
		return ERR_ARG;
	}

	/* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
	for (p = buf; total_copy_len != 0; p = p->next)
	{
		ASSERT(p);
		buf_copy_len = total_copy_len;
		if (buf_copy_len > p->len)
		{
			/* this pbuf cannot hold all remaining data */
			buf_copy_len = p->len;
		}
		/* copy the necessary parts of the buffer */
		memcpy(p->payload, &((char*) dataptr)[copied_total], buf_copy_len);
		total_copy_len -= buf_copy_len;
		copied_total += buf_copy_len;
	}
	ASSERT(total_copy_len == 0 && copied_total == len);
	return ERR_OK;
}

/**
 * Creates a single pbuf out of a queue of pbufs.
 *
 * @TODO wrong remark: The source pbuf 'p' is not freed by this function because that can be illegal in some places!
 *
 * @param p the source pbuf
 * @param layer pbuf_layer of the new pbuf
 *
 * @return a new, single pbuf (p->next is NULL)
 *         or the old pbuf if allocation fails
 */
PktBuf * pbuf_coalesce(PktBuf *p, pbuf_layer layer)
{

	if (!p->next)
		return p;
	PktBuf * q = pbuf_alloc(layer, p->tot_len, PBUF_RAM);
	if (!q)
		return p;
	int err = pbuf_copy(q, p);
	ASSERT(err == ERR_OK);
	pbuf_free(p);
	return q;
}
