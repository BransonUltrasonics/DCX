// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/TcpServer.h_v   1.0   09 Jun 2015 09:13:52   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines TCP server object.

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "Task.h"
#include "IPv4.h"
#include "TcpConnection.h"
#include "LinkedList.h"
#include "Fifo.h"
#include "Socket.h"

class Ethernet;

class TcpServer: public Task
{
public:
	TcpServer(char * name, int stack, Ethernet * eth, int tcpPort);
	virtual ~TcpServer();
	virtual void SetUp(bool up);
	ip_addr GetIpAddress();
	bool IsLinkUp();
protected:
	virtual void Run();
	virtual TcpConnection * CreateNewConnection(int socket) = 0;
	int ReuseAddress();
	int Linger();
	int Bind(int port);
	int NonBlock();
	int Listen();
	void SleepIfNoClients();
	void PrepareFDSets();
	void AcceptNewConnections();
	bool Poll();
	void ProcessInput();
	void ProcessOutput();
	void ProcessExceptions();
	int fd;
	int port;
	Ethernet * netif;
	PtrList<TcpConnection> clients;
	fd_set input_set;
	fd_set output_set;
	fd_set exc_set;
	Fifo<bool> run;
};

#endif
