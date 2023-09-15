// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/HttpConnection.h_v   1.5.1.1   19 Sep 2014 07:39:36   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines HTTP connection object.

#ifndef HTTPCONNECTION_H_
#define HTTPCONNECTION_H_

#include "TcpConnection.h"
#include "HttpServer.h"

#define MAX_SINCEMODIFIED_TIMESTAMP_LENGTH 50
#define MAX_LENGTH_BOUNDARY 300
#define MAX_LENGTH_BOUNDARYEND 304
#define MAX_LENGTH_CONTENTTYPE 200
#define MAX_LENGTH_FILENAME 200 //javascript code will take care of max file length

#define ERR_STR_RESOURCE "URL Not found. "
#define ERR_STR_FILELEN  "File is too big to upload. "
#define ERR_STR_FILEEXTENSION "File Of this type not supported. "
#define ERR_STR_FILEUPLOADED "File has been uploaded. Wait for System to restart. "
#define ERR_STR_FIRMWAREUPLOAD "Programming Error. Try again. "
#define ERR_FILE_START "File Start not found. "
#define ERR_NOT_IMPLEMNETED "Unsupported Request Type"
#define ERR_STR_UNKNOWN "Unknown Error"
enum ReqRetCode
{
	retOK,
	retNotOK,
};
class HttpConnection: public TcpConnection
{
	enum RequestMethod
	{
		Http_None,
		Http_OPTIONS,
		Http_GET,
		Http_HEAD,
		Http_POST,
		Http_PUT,
		Http_DELETE,
		Http_TRACE,
		Http_CONNECT,
		Http_TotalReq,
	};
	enum HttpError
	{
		httpOK = 200,
		httpCreated = 201,
		httpAccepted = 202,
		httpNoContent = 204,
		httpMovedPermanently = 301,
		httpMovedTemporarily = 302,
		httpNotModified = 304,
		httpBadRequest = 400,
		httpUnauthorized = 401,
		httpForbidden = 403,
		httpNotFound = 404,
		httpInternalServerError = 500,
		httpNotImplemented = 501,
		httpBadGateway = 502,
		httpServiceUnavailable = 503,
		httpVersionNotSupported = 504,
	};
public:
	HttpConnection(int afd, char * hostName, char * rootPath,
			HttpServer * svrPtr);
	HttpServer * httpServer;
	virtual ~HttpConnection();
	virtual void ProcessInput();
	HTTPReplyHeader header;
protected:
	virtual bool GetMoreData();
	bool ProcessInputLine();
	bool CheckForFileEnd();
	ReqRetCode ProcessRequestLine();
	void HandleRequestGet();
	void HandleRequestPost();
	void WriteStringToOutput(char * string);	
	void WriteDataToOutput();
	void WritefileToOutput();
	void SendError(char * err, bool backButton = true, HttpError errorCode = httpOK);
	void SendErrorNotFound(char * err);
	void SendBackUploadDataAsAttachMent(char * data, int dataLen, char * action);
	char * uri;
	int content_length;
	int http;
	const char * root;
	const char * host;
	RequestMethod request;
	void * file;
	char * UploadSendBackData;
	int UploadSendBackDataLen;
	static const char * requestMethods[Http_TotalReq];
	char modifiedSinceStr[MAX_SINCEMODIFIED_TIMESTAMP_LENGTH];
	char content_type[MAX_LENGTH_CONTENTTYPE];
	char boundaryStart[MAX_LENGTH_BOUNDARY];
	char boundaryEnd[MAX_LENGTH_BOUNDARYEND];
	char fileName[MAX_LENGTH_FILENAME];
	//bool fileUpload;
	void ProcessFileUpload(char * data, int dataLen, char * action);
	bool VerifyFileExtension(char * ext);
	bool VerifyFileLength(char * ext, int len);
	bool fileUpload;
	char * FileUploadData;
};

#endif
