// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/HTTP.cpp_v   1.2   02 Mar 2011 08:15:28   PDwivedi  $
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
#include "HttpServer.h"
#include "Board.h"
#include "RTOS.h"
#include "LinkedList.h"
#include "Ethernet.h"

struct HttpServerDesc
{
	int intf;
	int port;
	HttpServer * server;
};

static PtrList<HttpServerDesc> httpServers;

static HttpServerDesc * FindServer(int intf, int serverPort)
{
	HttpServerDesc * desc = 0;
	POSITION pos = httpServers.GetHeadPosition();
	while (pos)
	{
		desc = httpServers.GetNext(pos);
		if ((desc->intf == intf) && (desc->port == serverPort))
			break;
		desc = 0;
	}
	return desc;
}

void CyGOS::CreateHttpServerTask(int intf, int serverPort, int & priority)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, serverPort);
	if (desc)
	{
		printf("HTTP server on interface %d, port %d\n", intf, serverPort);
		CRASH("Duplicate HTTP server");
	}
	HttpServer * httpServer = new HttpServer(Board::eth[intf], serverPort);
	desc = new HttpServerDesc();
	desc->intf = intf;
	desc->port = serverPort;
	desc->server = httpServer;
	httpServers.AddTail(desc);
	Board::eth[intf]->AddTcpServer(httpServer);
	RTOS::Create(httpServer, priority++);
}

//Ratnesh
void CyGOS::AddHttpGetHandler(int intf, int srvrPort, getHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	HttpServer * svr = desc->server;
	svr->AddHttpGetHandler(handler);
}
void CyGOS::AddHttpPostHandler(int intf, int srvrPort, postHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	HttpServer * svr =  desc->server;
	svr->AddHttpPostHandler(handler);
}

void CyGOS::AddTickCallbackForWebSite(int intf, int srvrPort, TickCallback handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	HttpServer * svr =  desc->server;
	svr->AddTickCallback(handler);
}

void CyGOS::AddHttpFileUploadHandler(int intf, int srvrPort, uploadHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	HttpServer * svr =  desc->server;
	svr->AddFileUploadHandler(handler);
}
