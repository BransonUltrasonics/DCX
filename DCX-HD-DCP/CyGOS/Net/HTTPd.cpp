// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/HTTPd.cpp_v   1.1   02 Aug 2011 07:31:54   pdwivedi  $
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

#include "HTTPd.h"
#include "Net.h"
#include "TCP.h"
#include "PktBuf.h"
#include "string.h"
#include "stdio.h"
#include "DebugNew.h"

struct http_state
{
	char *file;
	int left;
	int retries;
};

struct fs_file
{
	char *data;
	int len;
};

void close_conn(TcpSocket *pcb, http_state *hs)
{
	tcp_arg(pcb, NULL);
	tcp_sent(pcb, NULL);
	tcp_recv(pcb, NULL);
	delete hs;
	tcp_close(pcb);
}

void send_data(TcpSocket *pcb, http_state *hs)
{
	int err;

	/* We cannot send more data than space available in the send
	 buffer. */
	int len = min(pcb->snd_buf, hs->left);
	do
	{
		err = tcp_write(pcb, hs->file, len, 0);
		if (err == ERR_MEM)
		{
			len /= 2;
		}
	} while (err == ERR_MEM && len > 1);

	if (err == ERR_OK)
	{
		hs->file += len;
		hs->left -= len;
	}
}

int fs_open(char *name, fs_file *file)
{
	file->data
			= "<html><head><title>Error</title></head><body><h2>Error</h2></body></html>";
	file->len = strlen(file->data);
	return 1;
}

int http_sent(void *arg, TcpSocket *pcb, int len)
{
	http_state * hs = (http_state *) arg;

	hs->retries = 0;

	if (hs->left > 0)
		send_data(pcb, hs);
	else
		close_conn(pcb, hs);

	return ERR_OK;
}

int http_recv(void *arg, TcpSocket *pcb, PktBuf *p, int err)
{
	int i;
	char *data;
	fs_file file;

	http_state * hs = (http_state *) arg;
	if (err == ERR_OK && p != NULL)
	{
		/* Inform TCP that we have taken the data. */
		tcp_recved(pcb, p->tot_len);

		if (hs->file == NULL)
		{
			data = (char *) p->payload;

			if (strncmp(data, "GET ", 4) == 0)
			{
				for (i = 0; i < 40; i++)
				{
					if (((char *) data + 4)[i] == ' ' || ((char *) data + 4)[i]
							== '\r' || ((char *) data + 4)[i] == '\n')
					{
						((char *) data + 4)[i] = 0;
					}
				}

				if (*(char *) (data + 4) == '/' && *(char *) (data + 5) == 0)
				{
					fs_open("/index.html", &file);
				}
				else if (!fs_open((char *) data + 4, &file))
				{
					fs_open("/404.html", &file);
				}

				hs->file = file.data;
				hs->left = file.len;
				/* printf("data %p len %ld\n", hs->file, hs->left);*/

				pbuf_free(p);
				send_data(pcb, hs);

				/* Tell TCP that we wish be to informed of data that has been
				 successfully sent by a call to the http_sent() function. */
				tcp_sent(pcb, http_sent);
			}
			else
			{
				pbuf_free(p);
				close_conn(pcb, hs);
			}
		}
		else
		{
			pbuf_free(p);
		}
	}

	if (err == ERR_OK && p == NULL)
	{
		close_conn(pcb, hs);
	}
	return ERR_OK;
}

int http_poll(void *arg, TcpSocket *pcb)
{
	http_state * hs = (http_state *) arg;
	if (!hs)
	{
		tcp_abort(pcb);
		return ERR_ABRT;
	}
	++hs->retries;
	if (hs->retries == 4)
	{
		tcp_abort(pcb);
		return ERR_ABRT;
	}
	send_data(pcb, hs);

	return ERR_OK;
}

void conn_err(void *arg, int err)
{
	http_state * hs = (http_state *) arg;
	delete hs;
}

int http_accept(void *arg, TcpSocket *pcb, int err)
{
	tcp_setprio(pcb, TCP_PRIO_MIN);
	/* Allocate memory for the structure that holds the state of the
	 connection. */
	http_state * hs = new http_state();
	if (hs == NULL)
	{
		printf("http_accept: Out of memory\n");
		return ERR_MEM;
	}
	/* Initialize the structure. */
	hs->file = NULL;
	hs->left = 0;
	hs->retries = 0;

	/* Tell TCP that this is the structure we wish to be passed for our
	 callbacks. */
	tcp_arg(pcb, hs);
	/* Tell TCP that we wish to be informed of incoming data by a call
	 to the http_recv() function. */
	tcp_recv(pcb, http_recv);
	tcp_err(pcb, conn_err);
	tcp_poll(pcb, http_poll, 4);
	return ERR_OK;
}

void httpd_init(ip_addr * ipaddr, int port)
{
	TcpSocket * pcb;

	pcb = tcp_new();
	tcp_bind(pcb, ipaddr, port);
	pcb = tcp_listen_with_backlog(pcb, 10);
	tcp_accept(pcb, http_accept);
}
