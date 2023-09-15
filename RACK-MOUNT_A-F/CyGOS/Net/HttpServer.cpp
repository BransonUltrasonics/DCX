// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/HttpServer.cpp_v   1.1   24 Jul 2015 08:51:06   sesharma  $
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
	httpJSONHandler = 0;

	ResourceUnavailableMsg = ERR_STR_RESOURCE;
	FileLenInvalidMsg = ERR_STR_FILELEN;
	FileExtensionInvalidMsg = ERR_STR_FILEEXTENSION;
	FileUploadSuccessMsg = ERR_STR_FILEUPLOADED;
	FileUploadRejectMsg = ERR_STR_FILEUPLOADFAIL;
	FileUploadInvalidStartMsg = ERR_FILE_START;
	FileUploadInvalidFileNameMsg = ERR_FILENAME_LEN;
}

HttpServer::~HttpServer()
{
	if (root)
		delete root;
	if (host)
		delete host;

	POSITION pos = UploadSupportedList.GetHeadPosition();
	while (pos)
	{
		POSITION last = pos;
		UploadSupportedFile * Temp = UploadSupportedList.GetNext(pos);
		UploadSupportedList.RemoveAt(last);
		delete Temp->extension;
		delete Temp;
	}

	pos = SendBackUploadList.GetHeadPosition();
	while (pos)
	{
		POSITION last = pos;
		SendBackUploadData * Temp = SendBackUploadList.GetNext(pos);
		SendBackUploadList.RemoveAt(last);
		delete Temp->HtmlFormAction;
		delete Temp->SendBackFileName;
		delete Temp;
	}
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


void HttpServer::AddJSONHandler(jsonHandler handler)
{
	httpJSONHandler = handler;
}

bool HttpServer::HandlePostEvent(char * postData, char * uri,  HTTPReplyHeader & header, ip_addr PeerAddr, int content_length)
{
	bool retVal = FALSE;
	if(strstr(uri, "Services/")){
		if(httpJSONHandler){
			retVal = httpJSONHandler(uri, header, postData, PeerAddr, content_length);
		}
	}
	if((retVal == false) && httpPostHnadler)
	{
		//	return httpPostHnadler(postData, content_length, action, isFileReq);
		retVal =  httpPostHnadler(postData, uri, header);
	}
	return retVal;
}

bool HttpServer::HandleGetEvent(char * uri, HTTPReplyHeader & header, ip_addr PeerAddr)
{
	bool retVal = FALSE;
	if(strstr(uri, "Services/"))
	{
		if(httpJSONHandler)
		{
			retVal = httpJSONHandler(uri, header, 0, PeerAddr, 0);
		}
	}
	if((retVal == false) && httpGetHandler)
	{
		retVal = httpGetHandler(uri , header);
	}

	return retVal;
}

bool HttpServer::HandleFileUpload(char * data, int dataLen, char *fileName, char* Action)
{
	 bool retVal = FALSE;
	 if(httpFileUploadHandler)
		 retVal = httpFileUploadHandler(data, dataLen, fileName,Action);

	 return retVal;
}

void HttpServer::AddToUploadSupportList(char * ext , int MaxUploadSize)
{
	bool Add = true;
	POSITION pos = UploadSupportedList.GetHeadPosition();
	UploadSupportedFile  * Temp;
	while (pos)
	{
		Temp = UploadSupportedList.GetNext(pos);
		if(!stricmp(Temp->extension, ext))
		{
			Temp->maxSize = MaxUploadSize + 1024;//extra size for header and all
			Add = false;
			break;
		}
	}

	if(Add)
	{
		Temp = new UploadSupportedFile;
		Temp->extension = new char[strlen(ext) + 1];
		strcpy(Temp->extension, ext);
		Temp->maxSize = MaxUploadSize + 1024;
		UploadSupportedList.AddHead(Temp);
	}
}

int HttpServer::GetMaxUploadLength()
{
	int MaxUploadLen = 0;
	POSITION pos = UploadSupportedList.GetHeadPosition();
	UploadSupportedFile  * Temp;
	while (pos)
	{
		Temp = UploadSupportedList.GetNext(pos);
		if(Temp->maxSize > MaxUploadLen)
			MaxUploadLen = Temp->maxSize;
	}
	return MaxUploadLen;
}

void HttpServer::AddToSendBackUploadDataList(char * FormAction, char * SendBackFileName)
{
	bool Add = true;
	POSITION pos = SendBackUploadList.GetHeadPosition();
	SendBackUploadData  * Temp;
	while (pos)
	{
		Temp = SendBackUploadList.GetNext(pos);
		if(!strcmp(Temp->HtmlFormAction, FormAction))
		{
			delete Temp->SendBackFileName;
			Temp->SendBackFileName = new char[strlen(SendBackFileName) + 1];
			strcpy(Temp->SendBackFileName, SendBackFileName);
			Add = false;
			break;
		}
	}

	if(Add)
	{
		Temp = new SendBackUploadData;
		Temp->HtmlFormAction = new char[strlen(FormAction) + 1];
		Temp->SendBackFileName = new char[strlen(SendBackFileName) + 1];
		strcpy(Temp->HtmlFormAction, FormAction);
		strcpy(Temp->SendBackFileName, SendBackFileName);
		SendBackUploadList.AddHead(Temp);
	}
}

void HttpServer::SetResourceUnAvailableMsg(char * Msg)
{
	ResourceUnavailableMsg = Msg;
}

void HttpServer::SetFileLenInvalidMsg(char * Msg)
{
	FileLenInvalidMsg = Msg;
}

void HttpServer::SetFileExtensionInvalidMsg(char * Msg)
{
	FileExtensionInvalidMsg = Msg;
}

void HttpServer::SetFileUploadSuccessMsg(char * Msg)
{
	FileUploadSuccessMsg = Msg;
}

void HttpServer::SetFileUploadRejectMsg(char * Msg)
{
	FileUploadRejectMsg = Msg;
}

void HttpServer::Tick()
{
	if(tickCallBack)
		tickCallBack(usecPerTick);
}
