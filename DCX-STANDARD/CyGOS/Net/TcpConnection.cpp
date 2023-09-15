// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/TcpConnection.cpp_v   1.7   28 Aug 2011 11:41:36   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines TCP connection object.

#include "TcpConnection.h"
#include "Net.h"
#include "Socket.h"
#include "stdio.h"
#include "DebugNew.h"

//disable debug
//#define printf pprintf
//#define pprintf

TcpConnection::TcpConnection(int afd)
{
	fd = afd;
	rxLen = 700 * 1024;
	txLen = 90 * 1024;

	rxPtr = 0;
	txPtr = 0;
	rxBuf = new char[rxLen];
	txBuf = new char[txLen];
}

TcpConnection::~TcpConnection()
{
	CloseConnection();
	delete rxBuf;
	delete txBuf;
	rxPtr = 0;
	txPtr = 0;
}

int TcpConnection::WriteToOutput(const void * data, int length)
{
	// This function does not need to be synchronized because all access to
	// connection transmit buffers is executed at different times in same server
	// task.
	int len = length;
	if ((len + txPtr) > txLen)
		len = txLen - txPtr;
	memcpy(txBuf + txPtr, data, len);
	txPtr += len;
	return len;
}

void TcpConnection::ProcessInput()
{
	char buf[100];
	for (;;)
	{
		int length = lwip_recv(fd, buf, sizeof(buf) - 1, 0);
		if (length > 0)
		{
			buf[length] = 0;
			printf("Received (%d): %s %s\n", fd, buf, "TcpConnection::ProcessInput()");
		}
		else
			break;
	}
}

void TcpConnection::ProcessOutput()
{
	while (txPtr > 0)
	{
		//pprintf("\n%s", __FUNCTION__);
		int length = lwip_send(fd, txBuf, txPtr, 0);
		if (length < 0)
		{
			pprintf("ProcessOutput send() error\n");
			break;
		}
		else if (length > 0)
		{
			// Managed to send something, let's fix the output buffer. Again, this
			// is not synchronized (see WriteToOutput() comments).
			memcpy(txBuf, txBuf + length, txPtr - length);
			txPtr -= length;
			if (!GetMoreData())
				break;
		}
	}
}

void TcpConnection::CloseConnection()
{
	if (fd != INVALID_SOCKET)
	{
		pprintf("\n %s %d", __FUNCTION__, fd);
		lwip_close(fd);
		fd = INVALID_SOCKET;
	}
}

bool TcpConnection::HasOutput()
{
	return txPtr != 0;
}
