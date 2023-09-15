// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/API/HTTP.cpp_v   1.1   24 Jul 2015 09:01:48   sesharma  $
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
	if(desc){
		HttpServer * svr = desc->server;
		svr->AddHttpGetHandler(handler);
	}
}
void CyGOS::AddHttpPostHandler(int intf, int srvrPort, postHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddHttpPostHandler(handler);
	}
}

void CyGOS::AddTickCallbackForWebSite(int intf, int srvrPort, TickCallback handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddTickCallback(handler);
	}
}

void CyGOS::AddHttpFileUploadHandler(int intf, int srvrPort, uploadHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddFileUploadHandler(handler);
	}
}

void CyGOS::AddJSONHandler(int intf, int srvrPort, jsonHandler handler)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddJSONHandler(handler);
	}
}

void CyGOS::AddToUploadSupportList(int intf, int srvrPort, char * ext , int MaxUploadSize)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddToUploadSupportList(ext , MaxUploadSize);
	}
}

void CyGOS::AddToSendBackUploadDataList(int intf, int srvrPort, char * FormAction, char * SendBackFileName)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->AddToSendBackUploadDataList(FormAction, SendBackFileName);
	}
}

void CyGOS::SetResourceUnAvailableMsg(int intf, int srvrPort, char * Msg)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->SetResourceUnAvailableMsg(Msg);
	}
}

void CyGOS::SetFileLenInvalidMsg(int intf, int srvrPort, char * Msg)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->SetFileLenInvalidMsg(Msg);
	}
}

void CyGOS::SetFileExtensionInvalidMsg(int intf, int srvrPort, char * Msg)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->SetFileExtensionInvalidMsg(Msg);
	}
}

void CyGOS::SetFileUploadSuccessMsg(int intf, int srvrPort, char * Msg)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->SetFileUploadSuccessMsg(Msg);
	}
}

void CyGOS::SetFileUploadRejectMsg(int intf, int srvrPort, char * Msg)
{
	ASSERT((intf >= 0) && (intf < Board::EthCount()));
	ASSERT(Board::eth[intf]);
	HttpServerDesc * desc = FindServer(intf, srvrPort);
	if(desc){
		HttpServer * svr =  desc->server;
		svr->SetFileUploadRejectMsg(Msg);
	}
}
