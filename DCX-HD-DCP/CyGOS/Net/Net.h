// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/Net.h_v   1.0   29 Oct 2010 10:39:34   ygupta  $
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

#ifndef ERR_H_
#define ERR_H_

#include "IPv4.h"
#include "string.h"

/* Definitions for error constants. */
#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */
#define ERR_IS_FATAL(e) ((e) < ERR_RTE)
#define ERR_ABRT       -5    /* Connection aborted.      */
#define ERR_RST        -6    /* Connection reset.        */
#define ERR_CLSD       -7    /* Connection closed.       */
#define ERR_CONN       -8    /* Not connected.           */
#define ERR_VAL        -9    /* Illegal value.           */
#define ERR_ARG        -10   /* Illegal argument.        */
#define ERR_USE        -11   /* Address in use.          */
#define ERR_IF         -12   /* Low-level netif error    */
#define ERR_ISCONN     -13   /* Already connected.       */
#define ERR_INPROGRESS -14   /* Operation in progress    */

typedef uint32 mem_ptr_t;
#define MEM_ALIGNMENT 4
#define LWIP_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) \
		& ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#define LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) \
		& ~(MEM_ALIGNMENT-1))

class Socket
{
public:
	/* ip addresses in network byte order */
	ip_addr local_ip;
	ip_addr remote_ip;
	/* Socket options */
	int so_options;
	/* Type Of Service */
	int tos;
	/* Time To Live */
	int ttl;
};

#endif
