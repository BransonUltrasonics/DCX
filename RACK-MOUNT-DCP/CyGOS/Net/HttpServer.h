// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/HttpServer.h_v   1.0   18 Aug 2014 09:16:32   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines HTTP server object.

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "TcpServer.h"

// Uncommented Ratnesh
#define MAX_DOWNLOAD_FILE_LENGTH 100
//struct PostHandler
//{
//	char action[MAX_ACTION_URI];
//	postHandler handler;
//};
//
//struct WebPages
//{
//	char PageName[MAX_ACTION_URI];
//	//char *WebPage_content;
//	//int content_length;
//	int Content_Type;
//	WebPageHandler WebHandler;
//};
// uncommented Ratnesh

//In case some image or css file gets modified we need to change this time stamp
//This way browser will send command to get the latest files otherwise the same will
//use the cached version of files
#define MODIFIED_SINCE_STR "Mon, 30 Jan 2013 13:24:51 GMT"
enum 
{
	texthtml,
	textcss,
	imagegif,
	imagejpg,
	imagepng,
	textjs,
	typeJSON,
};

enum HttpRequestType
{
	reqFileDownload = 1,
	reqData,
	reqFile,
	reqFileUpload,
	reqBinFileDownload
};

enum HttpStatusCode
{
	httpOk = 200,
	httpNotModified = 304,
};

struct HTTPReplyHeader
{
	int contentType;
	int contentLength;
	char downloadFileName[MAX_DOWNLOAD_FILE_LENGTH];
	HttpRequestType reqType;
	int statusCode;
	void * file;
	char * data;
	char * cacheReq;
};

typedef bool (*postHandler)(char * postData, char * uri, HTTPReplyHeader & header);
typedef bool (*getHandler)(char * uri, HTTPReplyHeader & header);
typedef bool (*uploadHandler)(char * data, int dataLen, char *ext, char* Action);
typedef bool (*jsonHandler) (char * uri, HTTPReplyHeader & header, char * JsonData, ip_addr Peer);

typedef void (*TickCallback)(int usecs);
class TcpConnection;
class HttpServer: public TcpServer
{
public:
	HttpServer(Ethernet * netif, int port);
	virtual ~HttpServer();
	//Ratnesh
	void AddHttpGetHandler(getHandler handler);
	void AddHttpPostHandler(postHandler handler);
	void AddTickCallback(TickCallback handler);
	void AddFileUploadHandler(uploadHandler handler);
	void AddJSONHandler(jsonHandler handler);
	bool HandlePostEvent(char * postData, char * uri,  HTTPReplyHeader & header, ip_addr PeerAddr);
	bool HandleGetEvent(char * uri, HTTPReplyHeader & header, ip_addr PeerAddr);
	bool HandleFileUpload(char * data, int dataLen, char *fileName, char* Action);
	//	void AddHttpHandle(postHandler handler, char * action);
	//	void AddHttpWebPages(char * PageName, WebPageHandler Handler,
	//			int Content_Type);
	//	PtrList<PostHandler> HttpHandlers;
	//	PtrList<WebPages> WebPagesList;
	//	char * HandlePostEvent(char * postData, int content_length, char * action);
protected:
	postHandler httpPostHnadler;
	getHandler httpGetHandler;
	TickCallback tickCallBack;
	uploadHandler httpFileUploadHandler;
	jsonHandler httpJSONHandler;
	virtual TcpConnection * CreateNewConnection(int socket);
	char * root;
	char * host;
	void Tick();
};

#endif
