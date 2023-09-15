// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/HttpServer.cpp_v   1.6   05 Aug 2011 10:27:44   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines HTTP server object.

// FIXME No way to specify root for the server file system.

#include "HttpServer.h"
#include "HttpConnection.h"
#include "DebugNew.h"

HttpServer::HttpServer(Ethernet * netif, int port) :
	TcpServer("HttpServer", 6144, netif, port)
{
	root = 0;
	host = 0;
	//	host = strdup(hostName);
	httpPostHnadler = 0;
	httpGetHandler = 0;
	tickCallBack = 0;
	httpFileUploadHandler = 0;
}

HttpServer::~HttpServer()
{
	if (root)
		delete root;
	if (host)
		delete host;
}

TcpConnection * HttpServer::CreateNewConnection(int socket)
{
	return new HttpConnection(socket, host, root, this);
}
//Added Ratnesh
void HttpServer::AddHttpGetHandler(getHandler handler)
{
	httpGetHandler = handler;
}
void HttpServer::AddHttpPostHandler(postHandler handler)
{
	httpPostHnadler = handler;
}

void HttpServer::AddTickCallback(TickCallback handler)
{
	tickCallBack = handler;
}

void HttpServer::AddFileUploadHandler(uploadHandler handler)
{
	httpFileUploadHandler = handler;
}

bool HttpServer::HandlePostEvent(char * postData, char * uri,  HTTPReplyHeader & header)
{
	bool retVal = FALSE;
	if(httpPostHnadler)
	{
		//	return httpPostHnadler(postData, content_length, action, isFileReq);
		retVal =  httpPostHnadler(postData, uri, header);
	}
	return retVal;
}

bool HttpServer::HandleGetEvent(char * uri, HTTPReplyHeader & header)
{
	bool retVal = FALSE;
	if(httpGetHandler)
		retVal = httpGetHandler(uri , header);

	return retVal;
}

bool HttpServer::HandleFileUpload(char * data, int dataLen, char *fileName)
{
	 bool retVal = FALSE;
	 if(httpFileUploadHandler)
		 retVal = httpFileUploadHandler(data, dataLen, fileName);

	 return retVal;
}


void HttpServer::Tick()
{
	if(tickCallBack)
		tickCallBack(usecPerTick);
}

//char * HttpServer::HandlePostEvent(char * postData, int & content_length,
//		char * action, int & isFileReq)
//{
//
//
//}
//void HttpServer::AddHttpHandle(postHandler handler, char * action)
//{
//	PostHandler * h = new PostHandler();
//	strcpy(h->action, action);
//	h->handler = handler;
//	HttpHandlers.AddTail(h);
//}
//void HttpServer::AddHttpWebPages(char * PageName, WebPageHandler Handler,
//		int Content_Type)
//{
//	WebPages * WebPage = new WebPages;
//	strcpy(WebPage->PageName, PageName);
//	//WebPage->WebPage_content = content;
//	//WebPage->content_length = contentlength;
//	WebPage->WebHandler = Handler;
//	WebPage->Content_Type = Content_Type;
//	WebPagesList.AddTail(WebPage);
//}
//
//char * HttpServer::HandlePostEvent(char * postData, int content_length,
//		char * action)
//{
//
//	POSITION pos = HttpHandlers.GetHeadPosition();
//	while (pos)
//	{
//		PostHandler * httphandler = HttpHandlers.GetNext(pos);
//		if (strcmp(httphandler->action, action) == 0)
//		{
//			return httphandler->handler(postData, content_length);
//		}
//	}
//	return errorHtml;
//}
