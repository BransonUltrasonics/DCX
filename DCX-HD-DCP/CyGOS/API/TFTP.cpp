// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/TFTP.cpp_v   1.1   14 Dec 2010 11:34:50   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements CyGOS TCP/IP API.

#include "CyGOS.h"
#include "TftpClient.h"
#include "Board.h"
#include "RTOS.h"
#include "DhcpClient.h"
#include "assert.h"

void CyGOS::CreateTftpClientTask(int intf, int & priority)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	RTOS::Create(new TftpClient(Board::eth[intf]), priority++);
}

int CyGOS::TftpReceive(int intf, char * name, int * length, void * data)
{
	ASSERT(Board::eth[intf]);
	POSITION pos = TftpClient::clients.GetHeadPosition();
	while (pos)
	{
		TftpClient * tftp = TftpClient::clients.GetNext(pos);
		if (Board::eth[intf] == tftp->netif)
		{
			return tftp->ReceiveFile(name, length, data,
					tftp->netif->dhcp->server_ip_addr);
		}
	}
	return ERR_RTE;
}


int CyGOS::TftpReceive(int intf, char * name, int * length, void * data, ip_addr server_ip_addr)
{
	ASSERT(Board::eth[intf]);
	POSITION pos = TftpClient::clients.GetHeadPosition();
	while (pos)
	{
		TftpClient * tftp = TftpClient::clients.GetNext(pos);
		if (Board::eth[intf] == tftp->netif)
		{
			return tftp->ReceiveFile(name, length, data,
					server_ip_addr);
		}
	}
	return ERR_RTE;
}
