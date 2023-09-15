// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/TcpServer.cpp_v   1.7   25 Oct 2013 11:56:36   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines TCP server object.

#include "TcpServer.h"
#include "Ethernet.h"
#include "Socket.h"
#include "errno.h"
#include "stdio.h"
#include "trace.h"
#include "DebugNew.h"

//disable debug
//#define printf
//#define pprintf

void Ethernet::AddTcpServer(TcpServer * server)
{
	POSITION pos = servers.GetHeadPosition();
	while (pos)
	{
		TcpServer * s = servers.GetNext(pos);
		if (s == server)
		{
//			printf("Server %s is already registered on interface %s\n",
//					s->GetName(), name);
			CRASH("Duplicate server");
		}
	}
	servers.AddTail(server);
	LinkCallback = TcpServerLinkCallback;
}

void Ethernet::TcpServerLinkCallback(Ethernet * netif)
{
	POSITION pos = netif->servers.GetHeadPosition();
	while (pos)
	{
		TcpServer * server = netif->servers.GetNext(pos);
		server->SetUp(netif->up);
	}
}

// FIXME TcpServer binds on interface level, multi-IP not supported.
TcpServer::TcpServer(char * name, int stack, Ethernet * eth, int tcpPort) :
	Task(name, stack), run(10, name)
{
	netif = eth;
	port = tcpPort;
}

TcpServer::~TcpServer()
{
	while (clients.GetCount())
		delete clients.RemoveHead();
}

ip_addr TcpServer::GetIpAddress()
{
	return netif->ipaddr;
}

bool TcpServer::IsLinkUp()
{
	return netif->up;
}

void TcpServer::SetUp(bool up)
{
	pprintf("TcpServer(%s) - set %s\n", name, up ? "Up" : "Down");
	run.Write(0, up, 0);
}
//void UpdateTPVal(int);
void TcpServer::Run()
{
	bool oldRunning = false;
	bool running = false;
	int timeout = 0;
	for (;;)
	{
		if (run.Read(this, running, timeout))
		{			
			if (running == oldRunning)
			{
					pprintf("Received redundant %s command\n", running ? "Run"
					: "Stop");
			}

			if ((running) && (!oldRunning))
			{
				// Just started running, allocate new socket and do everything
				// necessary to get going...
				pprintf("Starting TCP server '%s'...\n", name);
				fd = lwip_socket(AF_INET, SOCK_STREAM, PF_INET);
				TRACE_D(fd);
				unsigned long val = 1;
				lwip_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val,
								sizeof(val));
				ReuseAddress();
				Linger();
				Bind(port);
				NonBlock();
				Listen();
			}
			if ((!running) && (oldRunning))
			{
				// Got stopped, clean up connections and prepare to restart...
				POSITION pos = clients.GetHeadPosition();
				while (pos)
				{
					POSITION last = pos;
					TcpConnection * client = clients.GetNext(pos);
					clients.RemoveAt(last);
					delete client;
				}
				lwip_close(fd);
			}
			oldRunning = running;
		}
		if (oldRunning)
		{	
			// Running, socket is already created, process connections
			SleepIfNoClients();
			PrepareFDSets();
			if (!Poll())
			{
				pprintf("Select poll nonblock error in %s.\n", name);
				Suspend();
			}
			AcceptNewConnections();
			ProcessExceptions();
			ProcessInput();
			ProcessOutput();
			// and in the end, set the timeout to 0 so we don't get stuck
			// while waiting for new run/stop command
			timeout = 0;
		}
		else
		{
			// Not running, set the queue read delay to infinite instead of
			// just looping and delaying...
			timeout = -1;
		}	
	}
}

int TcpServer::ReuseAddress()
{
	// FIXME SO_REUSEADDR is not implemented, so server will fail to restart until the socket times out and is closed completely.
	//			bool opt = TRUE;
	//			if (!SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt)))
	//				throw new CString("CServerSocket::ReuseAddress\n");
	return ERROR_SUCCESS;
}

int TcpServer::Linger()
{
	// FIXME Do we need linger()? Is it working?
	//	LINGER ld;
	//	ld.l_onoff = 1;
	//	ld.l_linger = 1000;
	//	if (!SetSockOpt(SO_LINGER, &ld, sizeof(ld)))
	//		throw new CString("CServerSocket::Linger\n");
	return ERROR_SUCCESS;
}

int TcpServer::Bind(int port)
{
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.S_un.S_addr = netif->ipaddr.addr; // no ntohl
	sa.sin_port = htons(port);
	int error = lwip_bind(fd, (sockaddr *) &sa, sizeof(sa));
	if (error == SOCKET_ERROR)
	{
		pprintf("Bind error\n");
	}
	return error;
}

int TcpServer::NonBlock()
{
	unsigned long val = 1;
	return lwip_ioctl(fd, FIONBIO, &val);
}
//1.3.2   1.4
int TcpServer::Listen()
{
	int error = lwip_listen(fd, 1);
	if (error == SOCKET_ERROR)
	{
		pprintf("Listen error\n");
	}
	return error;
}

void TcpServer::SleepIfNoClients()
{
	//	if (clients.GetCount() == 0)
	//	{
	//		printf("No connections. Going to sleep.\n");
	//		FD_ZERO(&input_set);
	//		FD_SET(fd, &input_set);
	//		if (lwip_select(30, &input_set, 0, 0, 0) < 0)
	//			printf("\n\n\nLoop: Select error\n\n\n\n");
	//		printf("SleepIfNoClients - wakeup\n\n");
	//	}
}

void TcpServer::PrepareFDSets()
{
	FD_ZERO(&input_set);
	FD_ZERO(&output_set);
	FD_ZERO(&exc_set);

	FD_SET(fd, &input_set);

	POSITION pos = clients.GetHeadPosition();
	while (pos)
	{
		TcpConnection * client = clients.GetNext(pos);
		client->FdSet(input_set);
		if (client->HasOutput())
			client->FdSet(output_set);
		client->FdSet(exc_set);
	}
}

bool TcpServer::Poll()
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	if (lwip_select(30, &input_set, &output_set, &exc_set, &tv) < 0)
	{
		pprintf("select error %s\n", __FUNCTION__);
		return false;
	}
	return true;
}

void TcpServer::AcceptNewConnections()
{
	if (FD_ISSET(fd, &input_set))
	{
		sockaddr_in peer;
		int size = sizeof(peer);
		int afd = lwip_accept(fd, (sockaddr *) &peer, &size);
		if (afd == INVALID_SOCKET)
			return;
		unsigned long val = 1;
		lwip_ioctl(afd, FIONBIO, &val);
		ip_addr addr;
		addr.addr = peer.sin_addr.s_addr;
		val = 1;
		lwip_setsockopt(afd, IPPROTO_TCP, TCP_NODELAY, &val,
				sizeof(val));
//		pprintf("New connection from %s port %d sock %d\n", addr.ToString(), ntohs(
//				peer.sin_port), afd);
		TcpConnection * newClient = CreateNewConnection(afd);
		newClient->PeerAddr = peer.sin_addr.s_addr;
		clients.AddTail(newClient);
	}
}

void TcpServer::ProcessOutput()
{
	POSITION pos = clients.GetHeadPosition();
	while (pos)
	{
		TcpConnection * client = clients.GetNext(pos);
		if (client->IsSet(output_set))
			client->ProcessOutput();
	}
}

void TcpServer::ProcessExceptions()
{
	POSITION pos = clients.GetHeadPosition();
	while (pos)
	{
		POSITION last = pos;
		TcpConnection * client = clients.GetNext(pos);
		if (client->IsSet(exc_set))
		{
			//pprintf("\n remove %d", client->GetSockNum());
			client->FdClr(input_set);
			client->FdClr(output_set);
			client->FdClr(exc_set);
			clients.RemoveAt(last);
			delete client;
		}
	}
}

void TcpServer::ProcessInput()
{
	POSITION pos = clients.GetHeadPosition();
	while (pos)
	{
		TcpConnection * client = clients.GetNext(pos);
		if (client->IsSet(input_set))
		{
			//pprintf("\nips=%d\n", client->GetSockNum());
			client->ProcessInput();
			//pprintf("\nrips=%d\n", client->GetSockNum());
		}
	}
}
