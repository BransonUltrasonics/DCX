// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/TcpConnection.h_v   1.1.2.0   19 Sep 2014 07:39:38   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines TCP connection object.

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "Socket.h"
#include "IPv4.h"
class TcpConnection
{
public:
	bool HasOutput();
	TcpConnection(int afd);
	virtual ~TcpConnection();
	void FdSet(fd_set & set)
	{
		FD_SET(fd, &set);
	}
	void FdClr(fd_set & set)
	{
		FD_CLR(fd, &set);
	}
	bool IsSet(fd_set & set)
	{
		return FD_ISSET(fd, &set) != 0;
	}
	int WriteToOutput(const void * data, int length);
	virtual void ProcessInput();
	virtual void ProcessOutput();
	void CloseConnection();
	int GetSockNum()
	{
		return fd;
	}
	ip_addr PeerAddr;
protected:
	virtual bool GetMoreData() = 0;
	int fd;
	int rxLen;
	int txLen;
	int rxPtr;
	int txPtr;
	char * rxBuff;
	char * txBuf;
	char * rxBuf;
};

#endif
