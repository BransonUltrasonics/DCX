// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/HttpConnection.cpp_v   1.1   17 Sep 2014 07:36:30   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines HTTP connection object.

#include "HttpConnection.h"
#include "Socket.h"
#include "stdio.h"
#include "assert.h"
#include "stdlib.h"
#include "errno.h"
#include "CyGOS.h"
//#include "..\..\Application\Log.h"
//disable debug
//#define printf
//#define pprintf

//alternatively TAR_SIZE and FLASH_SIZE variables from linker file can be used
#define MAX_UPLOAD_LENGTH_BIN 0x4A4000 + 1024//4096K + Extra HttpPacket plus checksum
#define MAX_UPLOAD_LENGTH_TAR 0x70000 + 1024//448K + Extra HttpPacket
#define MAX_UPLOAD_LENGTH_CFG 0x20000

struct UploadSupportedFile
{
	int maxSize;//max supported size for corresponding file type
	const char * extension;//type of file
};
const UploadSupportedFile uploadFileObj[] = {
	{MAX_UPLOAD_LENGTH_BIN ,"bin"},
	{MAX_UPLOAD_LENGTH_TAR  ,"cyg"},
	{MAX_UPLOAD_LENGTH_CFG,"cfg"}
};

const char * HttpConnection::requestMethods[Http_TotalReq] =
{ "<None>", "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE",
		"CONNECT" };
bool fileUploadInitiated = false;
//bool lenError = false;
HttpConnection::HttpConnection(int afd, char * hostName, char * rootPath,
		HttpServer * svrPtr) :
	TcpConnection(afd)
{
	root = rootPath;
	host = hostName;
	request = Http_None;
	uri = 0;
	file = 0;
	UploadSendBackData = 0;
	UploadSendBackDataLen = 0;
	FileUploadData = 0;
	//	search = 0;
	httpServer = svrPtr;
	memset(modifiedSinceStr , 0, sizeof(modifiedSinceStr));
	memset(boundaryEnd , 0, sizeof(boundaryEnd));
	content_type[0] = 0;
	//file upload related variables
	fileUpload = false;
	rxBuf = rxBuff;
}

HttpConnection::~HttpConnection()
{
	if (uri)
		delete uri;
	if(file)
	{
		CyGOS::FileClose(file);
		file = 0;
	}
	UploadSendBackData = 0;
	UploadSendBackDataLen = 0;
	if(FileUploadData)
	   delete FileUploadData;
}

bool HttpConnection::CheckForFileEnd()
{
	bool ret = false;
	char * endPtr = rxBuf + rxPtr - (strlen(boundaryEnd) + strlen("\r\n"));
	if((memcmp(endPtr, boundaryEnd, strlen(boundaryEnd))) == 0)
	{
		pprintf("\nFile receive completed ");
		ret = true;
	}
	return ret;
}
void HttpConnection::ProcessInput()
{
	bool ready = false;
	int length = 0;
	//This is a hypothetical limit which is imposed on Process Input loop
	//If somehow process loop doesn't break it will after looping maxLoopLimit count
	//TODO:if connection is slow then this count should be higher
	//unsigned int maxLoopLimitFileUpload = 2000000;
	//unsigned int maxLoopLimit = 2000000;
	//unsigned int XX = 0;
	//pprintf("^");
	for (;;)
	{
		if(!httpServer->IsLinkUp())
		{
			pprintf("\n break on link down");
			fileUploadInitiated = false;
			fileUpload = false;
			rxBuf = rxBuff;
			rxPtr = 0;
			request = Http_None;
			if(FileUploadData){
				delete FileUploadData;
				FileUploadData = 0;
			}
			break;//immediately break if link goes down so that TcpServer can close all connections.
		}
		if(fileUpload)
		   length = content_length - rxPtr - 1 + 1024;
		else
		   length = rxLen - rxPtr - 1;
		if(length < 0){
		//pprintf("\n negative length ");
		}
		length = lwip_recv(fd, rxBuf + rxPtr, length, 0);
		// Nothing new received - return. Also, if line buffer is full we will
		// return 0 bytes, so check if we have a full line in the buffer.
		if ((length <= 0) && (rxPtr <= 0))
			break;

		if(fileUpload)
		{
			//CyGOS::KickWatchDog();
		    printf(".");
			if((content_length > (MAX_UPLOAD_LENGTH_BIN)) || ((rxPtr + length) >= (content_length + 1024)))
			{
				rxPtr = 0;
				fileUpload = false;
				fileUploadInitiated = false;
				ready = false;
				pprintf("\n file len error");
				request = Http_None;
				rxBuf = rxBuff;
				SendError(ERR_STR_FILELEN);

				if(FileUploadData){
				   delete FileUploadData;
				   FileUploadData = 0;
				}
				break;
			}
			if (length < 0)
				length = 0;
			rxBuf[rxPtr + length] = 0;
			rxPtr += length;			
			if(CheckForFileEnd() == true)
			{
				ready = true;			
				request = Http_POST;
				break;
			}
			//XX++;
			/*if(--maxLoopLimitFileUpload <= 0)
			{
				pprintf("\n breaking from file upload %d %X", XX, this);
				CyGOS::WriteSystemEvent(UPLOAD_ABORT, (short)(content_length >> 16) , (short)content_length, rxPtr,true);
				fileUploadInitiated = false;
				fileUpload = false;
				rxBuf = rxBuff;
				rxPtr = 0;
				request = Http_None;
				ready = false;
            if(FileUploadData){
               delete FileUploadData;
               FileUploadData = 0;
            }
            SendError(ERR_STR_FIRMWAREUPLOAD);
				break;
			}*/
			//pprintf("m=%d ", maxLoopLimitFileUpload);
			break;
		}
		else
		{
         if((rxPtr + length) >= rxLen)
         {
            pprintf("\n breaking from Process Input %X", this);
            CyGOS::WriteSystemEvent(CRLF_INVALID, (short)rxPtr >> 16, (short)rxPtr,0,true);
            fileUploadInitiated = false;
            fileUpload = false;
            rxBuf = rxBuff;
            request = Http_None;
            rxPtr = 0;
            ready = false;
            if(FileUploadData){
               delete FileUploadData;
               FileUploadData = 0;
            }
            SendError(ERR_STR_UNKNOWN);
            break;
         }
         if ((length == 0) && (strstr(rxBuf, "\r\n") == 0))
         {
            // Discard beginning of a very long line.
            rxPtr = 0;
            request = Http_None;
            break;
         }

         if (length < 0)
            length = 0;
         //pprintf("\n rxPtr = %d ", rxPtr);
         rxBuf[rxPtr + length] = 0;
         rxPtr += length;
         char * crlf = strstr(rxBuf, "\r\n");
         if (crlf)
         {
            //maxLoopLimit = 2000000;
            // Got a full line terminated with CRLF.
            length = crlf - rxBuf + 2;
            ready = ProcessInputLine();
            //if(fileUpload)
            memcpy(rxBuf, rxBuf + length, rxPtr - length + 1);
            //else
               //strcpy(rxBuf, rxBuf + length);
            rxPtr -= length;
            ///////////////////////////////////////
             if(fileUpload && ready){
            	if(CheckForFileEnd() == true){
            		request = Http_POST;
            		ready = true;
            	}
            	else{
            		request = Http_None;
            		ready = false;
            	}
                break;
             }
             /////////////////////////
             else if (ready)// && !fileUpload)
               break;

         }
		}
	}
	if (ready)
	{
		//		if (http != 11)
		//		{
		//			SendError(httpVersionNotSupported);
		//		}
		//		else
		{
			//pprintf("\n process input returned 1");
			switch (request)
			{
			case Http_GET:
				HandleRequestGet();
				break;
			case Http_POST:
				if((strncmp(content_type, "multipart/form-data", strlen("multipart/form-data")) == 0))
				{
					if(fileUploadInitiated && fileUpload){
						ProcessFileUpload(rxBuf , rxPtr, uri + 1);
						if(FileUploadData && (UploadSendBackData == 0)){
							delete FileUploadData;
							FileUploadData = 0;
							pprintf("\n deleting FileUploadData %X", this);
						}
						else{
						   pprintf("\n not deleting FileUploadData %X", this);
						}
						rxBuf = rxBuff;
						
					}
					else
                  		pprintf("\n unexpected multipart form-data content %d %d %X", fileUploadInitiated, fileUpload, this);
                  //SendError(ERR_STR_RESOURCE, false, httpNoContent);
				}
				else
					HandleRequestPost();
				break;
            //       case Http_OPTIONS:
            //          HandleRequestOptions();
            //          break;
            //       case Http_HEAD:
            //          HandleRequestHead();
            //          break;
            //			case Http_PUT:
            //				HandleRequestPut();
            //				break;
            //			case Http_DELETE:
            //				HandleRequestDelete();
            //				break;
            //			case Http_TRACE:
            //				HandleRequestTrace();
            //				break;
            //			case Http_CONNECT:
            //				HandleRequestConnect();
            //				break;
			default:
				//SendError(ERR_NOT_IMPLEMNETED, false, httpNotImplemented);
				pprintf("Request type %d not handled\n", request);
				break;
			}
		}
		rxPtr = 0;
		rxBuf = rxBuff;
		fileUpload = false;
		ready = false;
		content_type[0] = 0;
	}
	//pprintf(" \n process ip return %d", GetSockNum());
}

bool HttpConnection::GetMoreData()
{
	bool retVal = false;
	if (file)
	{
		// This much space is available in transmit buffer.
		int length = txLen - txPtr;
		if (length > 0)
		{
			int error = CyGOS::FileRead(file, txBuf + txPtr, &length);
			if ((error == ERROR_SUCCESS) && (length > 0))
			{
				txPtr += length;
				retVal = true;
			}
			else
			{
				CyGOS::FileClose(file);
				file = 0;
				retVal = false;
			}
		}
	}

	if(UploadSendBackDataLen > 0)
	{
		int length = txLen - txPtr;
		if(UploadSendBackDataLen < length)
			length = UploadSendBackDataLen;
		memcpy(txBuf + txPtr, UploadSendBackData, length);
		UploadSendBackData += length;
		txPtr += length;
		UploadSendBackDataLen -= length;
		retVal = true;
	}
	if((retVal == false) && (FileUploadData)){
	   delete FileUploadData;
	   FileUploadData = 0;
	   UploadSendBackData = 0;
	   pprintf("\n deleted now FileUploadData %x", this);
	}
	return retVal;
}

void HttpConnection::WriteStringToOutput(char * string)
{
	//TODO:Though not expecting strings bigger than rxbuf size
	//but this also needs to be done through get more data
	int length = strlen(string);
	if (WriteToOutput(string, length) != length)
	{
		pprintf("writestr to op Buffer overflow\n");
	}
}

void HttpConnection::WriteDataToOutput()
{
	GetMoreData();
}

void HttpConnection::WritefileToOutput()
{
	//int length = strlen(string);
	//if (WriteToOutput(string, len) != len)
	//	printf("\n Buffer overflow %s\n", __FUNCTION__);
	GetMoreData();
}

bool HttpConnection::ProcessInputLine()
{
	if (request == Http_None)
	{
		if(ProcessRequestLine() == retNotOK)
			return true;
		return false;
	}
	if (strncmp(rxBuf, "\r\n", 2) == 0)
	{
		if(strncmp(content_type, "multipart/form-data", strlen("multipart/form-data")) == 0)
		{
			if(uri)
			{
//				pprintf("\n uri + 1=%s", uri + 1);
				if((!stricmp(uri + 1, "SendBackCSVGraphDataHS")) || (!stricmp(uri + 1, "SendBackCSVGraphDataPS")))
				{
					pprintf("\n file upload export data");
					fileUploadInitiated = true;
				}
			}
//			pprintf(" \n file upload command %s %08X",fileUploadInitiated?"JS Init":"Qualys check", this);
			if(fileUploadInitiated)
			{
				pprintf("\n %s %08X", __FUNCTION__, this);
				fileUpload = true;
			}

		   if((fileUpload) && (!FileUploadData)){
		         if(content_length < (MAX_UPLOAD_LENGTH_BIN)){
		            FileUploadData = new char[content_length + 1024];
		            memcpy(FileUploadData, rxBuf, rxPtr);
		            rxBuf = FileUploadData;
		            pprintf("\n file upload length %d buff %x", content_length, FileUploadData);
		         }
		         else
		            pprintf("\n file upload length error %d buff %x", content_length, FileUploadData);
		      }
		}
		return true;
	}

	char * cr = strchr(rxBuf, '\r');
	if(cr){
		 cr = strchr(rxBuf, ':');
		 if(cr)
		   *cr = 0;
		 else{
			 // UpdateTPVal(111);
			  request = Http_None;
			  return true;
		 }
	 }
	 else{
	    // UpdateTPVal(110);
	     request = Http_None;
	     return true;
	 }

	// TODO We ignore lots of features...
	//printf("\n\n Ignoring1: %s\n", rxBuf);
	if (strnicmp(rxBuf, "content-length", strlen("content-length")) == 0)
	{
		content_length = atoi(rxBuf + strlen("content-length: "));
		//pprintf(" content - length %d", content_length);
	}

	if(strnicmp(rxBuf, "content-type", strlen("content-type")) == 0)
	{
		char * temp1 = rxBuf + strlen("content-type: ");
		char * temp2 = strstr(temp1, ";");
		if(temp2 && ((temp2 - temp1) < MAX_LENGTH_CONTENTTYPE)){
			strncpy(content_type ,temp1 ,temp2 - temp1);//TOD):test
			if(strncmp(content_type, "multipart/form-data", strlen("multipart/form-data")) == 0)
			{
				//point to boundary start
				temp1 = strstr(temp2, "boundary=");
				if(temp1){
					temp1 += strlen("boundary=");
					temp2 = strstr(temp1, "\r\n");
				}
				//point to boundary end
				if(temp1 && temp2 && ((temp2 - temp1) < MAX_LENGTH_BOUNDARY))
				{
					strncpy(boundaryStart,temp1, temp2 -  temp1);
					sprintf(boundaryEnd,"--%s--", boundaryStart);
					pprintf("\n boundray strart=%s , boundary end=%s", boundaryStart ,boundaryEnd);
				}
				else
				{
					//Log::WriteEventLog(LEN_INVALID, (short)(2) , (short)0, 0, false);
					//Log::WriteEventLog(LEN_INVALID, (short)(2) , (short)0, 0, 0, TRUE);
					request = Http_None;
					pprintf("\n boundary start length too long");
					return true;
				}
			}
		}
		else
		{
			//Log::WriteEventLog(LEN_INVALID, (short)(1) , (short)0, 0, false);
			//Log::WriteEventLog(LEN_INVALID, (short)(1), (short)0, 0, 0, TRUE);
			//pprintf("\n Content type length too long");
		}
		//printf("\n typ %s ", content_type);
	}

	if(strcmp(rxBuf, "If-Modified-Since") == 0)
	{
		char * temp1 = rxBuf + strlen("If-Modified-Since: ");
		char * temp2 = strstr(temp1, "\r\n");
		if(temp2 && ((temp2 - temp1) < MAX_SINCEMODIFIED_TIMESTAMP_LENGTH))
			strncpy(modifiedSinceStr ,temp1 ,temp2 - temp1);
		else
		{
			pprintf("\n Modified since length too long");
			request = Http_None;
			return true;
			//Log::WriteEventLog(LEN_INVALID, (short)(3) , (short)0, 0, false);
			//Log::WriteEventLog(LEN_INVALID, (short)(3), (short)0, 0, 0, TRUE);
		}
		//printf("\n modified since %s ", modifiedSinceStr);
	}
	*cr = ':';
	//	cr = strchr(rxBuf, '\r');
	//	*cr = '\r';
	return false;
}

ReqRetCode HttpConnection::ProcessRequestLine()
{
	ReqRetCode retVal = retNotOK;
	char * space = strchr(rxBuf, ' ');
	// If space is not found, we have bad input line.
	if (!space)
		return retNotOK;
	*space = 0;
	for (int i = Http_None; i < Http_TotalReq; i++)
	{
		// Skip special placeholders.
		if (requestMethods[i][0] == '<')
			continue;
		if (strcmp(rxBuf, requestMethods[i]) == 0)
		{
			request = (RequestMethod) i;
			if((request == Http_GET) || (request == Http_POST))
				retVal = retOK;
			break;
		}
	}
	*space = ' '; // Restore the input line so that next time we can try again
	if (request == Http_None) // Couldn't recognize the request type
		return retNotOK;
	// We have identified the request, now we need URI... It starts past the
	// space we found earlier.
	while (*space == ' ')
		space++;
	if (space[0] == 0) // No URI?
		return retNotOK;
	if (strchr(space, ' ') == 0) // No space between URI and protocol?
		return retNotOK;
	if (uri)
	{
		//pprintf("\n delete uri %08X", uri);
		delete uri;
	}
	uri = strdup(space);
	//pprintf("\n uri pointer %08X sock %d", uri, GetSockNum());
	// Terminate the URI, we always have the space at the end so we can replace
	// it with 0.
	space = strchr(uri, ' ');
	*space++ = 0;
	// Now the protocol... We will only support HTTP/1.1
	if (strncmp(space, "HTTP/1.1", 8) != 0)
		return retNotOK;
	http = 11;
	return retVal;
}

void HttpConnection::HandleRequestGet()
{
	//printf("\n\n\n GET '%s %s'\n\n", root, uri);
	memset(&header, 0, sizeof(header));
	header.cacheReq = modifiedSinceStr;
	if(httpServer->HandleGetEvent(uri , header, PeerAddr))
	{
		memset(modifiedSinceStr, 0, 50);
		if(header.statusCode == httpNotModified)
		{
			WriteStringToOutput("HTTP/1.1 304 Not Modified\r\n");
			WriteStringToOutput("Connection: Keep-Alive\r\n");
			WriteStringToOutput("Cache-Control: max-age=2160000\r\n");
			WriteStringToOutput("\r\n");
		}
		else
		{
			file = header.file;
			WriteStringToOutput("HTTP/1.1 200 OK\r\n");

			char buf[60];
			sprintf(buf, "Content-Length: %d\r\n", header.contentLength);
			WriteStringToOutput(buf);
			WriteStringToOutput("Connection: Keep-Alive\r\n");

			if (header.contentType == imagegif)
				WriteStringToOutput("Content-Type: image/gif\r\n");
			else if (header.contentType == textcss)
				WriteStringToOutput("Content-Type: text/css\r\n");
			else if (header.contentType == textjs)
			    WriteStringToOutput("Content-Type: text/javascript\r\n");
			else if(header.contentType == typeJSON){
				//pprintf("\n Replying JSON GET");
				WriteStringToOutput("Content-Type: application/json;charset=UTF-8\r\n");
			}
			else
				WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");

			if((header.contentType != texthtml) && (header.contentType != typeJSON))
			{
				WriteStringToOutput("Cache-Control: max-age=2160000\r\n");
				sprintf(buf, "Last-Modified: %s\r\n", MODIFIED_SINCE_STR);
				WriteStringToOutput(buf);
			}
			WriteStringToOutput("\r\n");
			if(header.contentType == typeJSON)
			{
				//pprintf("\n replying JSON request");
				WriteStringToOutput(header.data);
			}
			else if(header.reqType == reqData)
			  WriteStringToOutput(header.data);
			else
			  WritefileToOutput();
		}
		request = Http_None;
	}
	else
		SendErrorNotFound(ERR_STR_RESOURCE);
}

void HttpConnection::HandleRequestPost()
{
	//printf("POST '%s%s'\n", root, uri);
	memset(&header, 0, sizeof(header));
	if(httpServer->HandlePostEvent(rxBuf, uri + 1, header, PeerAddr))
	{
		WriteStringToOutput("HTTP/1.1 200 OK\r\n");
		char buf[80];
		sprintf(buf, "Content-Length: %d\r\n", header.contentLength);
		WriteStringToOutput(buf);
		WriteStringToOutput("Connection: Keep-Alive\r\n");
		if(header.reqType == reqFileDownload)
		{
			sprintf(buf, "Content-Disposition: attachment; filename=%s\r\n",header.downloadFileName);
			WriteStringToOutput(buf);
			WriteStringToOutput("Content-Type: text/plain; charset=UTF-8\r\n");
		}
		else if(header.reqType == reqBinFileDownload)
		{
			sprintf(buf, "Content-Disposition: attachment; filename=%s\r\n",header.downloadFileName);
			WriteStringToOutput(buf);
			WriteStringToOutput("Content-Type: image/gif\r\n");
		}
		else if(header.contentType == typeJSON)
			WriteStringToOutput("Content-Type: application/json;charset=UTF-8\r\n");
		else
			WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");
		WriteStringToOutput("\r\n");

		if(header.reqType == reqFile)
		{
			file = header.file;
			WritefileToOutput();
		}
		else if(header.reqType == reqData)
			WriteStringToOutput(header.data);
		else if(header.reqType == reqFileDownload)
			WriteStringToOutput(header.data);
		else if(header.reqType == reqBinFileDownload)
		{
			file = header.file;
			WritefileToOutput();
		}
		else if(header.reqType == reqFileUpload)
		{
			pprintf(" \n process upload initiated on %08X", this);
			fileUploadInitiated = true;
			WriteStringToOutput(header.data);
		}
		if(header.contentType == typeJSON)
		{
			//pprintf("\n replying JSON request");
			WriteStringToOutput(header.data);
		}

		request = Http_None;
	}
	else
		SendErrorNotFound(ERR_STR_RESOURCE);

}

char * errorHtml1 =
		"<html>"
		"<head>"
		"<title>BRANSON DCX GUI</title>"
		"<script language=\"javascript\"  type=\"text/javascript\">"
		"function getCookie(c_name)"
		"{"
		"if (document.cookie.length>0)"
		  "{"
		  "c_start=document.cookie.indexOf(c_name + \"=\");"
		  "if (c_start!=-1)"
		    "{"
		    "c_start=c_start + c_name.length+1;"
		    "c_end=document.cookie.indexOf(\";\",c_start);"
		    "if (c_end==-1) c_end=document.cookie.length;"
		    "return unescape(document.cookie.substring(c_start,c_end));"
		    "}"
		  "}"
		"return \"\";"
		"}"
		"function GetLanguageAndUserID(hrefpage)"
		"{"
		   "document.getElementById(\"lang\").value= getCookie('LANGUAGE');"
			"document.getElementById(\"userid1\").value= getCookie('USERID');"
			"document.temp_form.action = hrefpage;"
			"document.temp_form.submit();"
		"}"
		"</script>"
		"</head>"
		"<body>"
		"<h2>Server Message:</h2>"
		"<form name=\"temp_form\" method=\"post\" action=\"system-information.html\">"
		"<input type=\"hidden\" name=\"lang\" id=\"lang\" value=\"\" />"
		"<input type=\"hidden\" name=\"userid1\" id=\"userid1\" value=\"\" />"
		"</form>";

char * errorHtml2 =
		"<input type=\"button\" value=\"Back\" onclick = \"GetLanguageAndUserID('system-information.html')\"/>"
		"</body>"
		"</html>";

char * errorHtml3 = "<html><head><title>BRANSON DCX GUI</title></head>"
			"<body><h2>Server Message:</h2>";

char * errorHtml4 = "</body></html>";

void HttpConnection::SendError(char * err, bool backButton, HttpError errorCode)
{
		//pprintf("\n %s %d", __FUNCTION__, errorCode);
		int content_length;
		char * err1, * err2;
		char * ip = httpServer->GetIpAddress().ToString();
//		err1 = errorHtml3;
//		err2 = errorHtml4;
		if(backButton)
		{
			err1 = errorHtml1;
			err2 = errorHtml2;
			content_length = strlen(err1) + strlen(err2) + strlen(err) + strlen(ip);
		}
		else
			content_length = strlen(err);
		char buf[30];
		sprintf(buf , "HTTP/1.1 %d OK\r\n" , errorCode);
		WriteStringToOutput(buf);
		sprintf(buf, "Content-Length: %d\r\n",content_length);
		WriteStringToOutput(buf);
		WriteStringToOutput("Connection: Keep-Alive\r\n");
		WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");
		WriteStringToOutput("\r\n");
		if(backButton){
			WriteStringToOutput(err1);
			WriteStringToOutput(err);
			WriteStringToOutput(ip);
			WriteStringToOutput(err2);
		}
		else
			WriteStringToOutput(err);
		request = Http_None;
}

void HttpConnection::SendErrorNotFound(char * err)
{
		//pprintf("\n %s", __FUNCTION__);
		int content_length;
		char * err1, * err2;
		char * ip = httpServer->GetIpAddress().ToString();
		err1 = errorHtml3;
		err2 = errorHtml4;
		WriteStringToOutput("HTTP/1.1 404 Not Found\r\n");
		char buf[30];
		content_length = strlen(err1) + strlen(err2) + strlen(err) + strlen(ip);
		sprintf(buf, "Content-Length: %d\r\n",content_length);
		WriteStringToOutput(buf);
		WriteStringToOutput("Connection: Keep-Alive\r\n");
		WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");
		WriteStringToOutput("\r\n");
		WriteStringToOutput(err1);
		WriteStringToOutput(err);
		WriteStringToOutput(ip);
		WriteStringToOutput(err2);
		request = Http_None;
}

bool HttpConnection::VerifyFileExtension(char * ext)
{
	int indx;
	bool retVal = false;
	int nExtensions = sizeof(uploadFileObj) / sizeof(uploadFileObj[0]);
	for(indx = 0; ((!retVal) && (indx < nExtensions)); indx ++)
	{
		//pprintf("\n ext = %s ", uploadFileObj[indx].extension);
		if (strnicmp(ext, uploadFileObj[indx].extension, strlen(uploadFileObj[indx].extension)) == 0)
			retVal = true;
	}
	return retVal;
}

bool HttpConnection::VerifyFileLength(char * ext, int len)
{
	int indx;
	bool retVal = false ;
	int nExtensions = sizeof(uploadFileObj) / sizeof(uploadFileObj[0]);
	for(indx = 0; ((!retVal) && (indx < nExtensions)); indx ++)
	{
		//pprintf("\n ext1=%s ext = %s maxlen-%d fileLen=%d", ext, uploadFileObj[indx].extension, uploadFileObj[indx].maxSize, len);
		if (strnicmp(ext, uploadFileObj[indx].extension, strlen(uploadFileObj[indx].extension)) == 0)
		{
			if(len <= uploadFileObj[indx].maxSize)
				retVal = true;
		}
	}
	return retVal;
}

void HttpConnection::SendBackUploadDataAsAttachMent(char * data, int dataLen , char * action)
{
	WriteStringToOutput("HTTP/1.1 200 OK\r\n");
	char buf[80];
	sprintf(buf, "Content-Length: %d\r\n", dataLen);
	WriteStringToOutput(buf);
	WriteStringToOutput("Connection: Keep-Alive\r\n");

	sprintf(buf, "Content-Disposition: attachment; filename=%s\r\n",
			(!strcmp(action , "SendBackCSVGraphDataHS"))?"HornScanData.csv":"WeldData.csv");
	WriteStringToOutput(buf);
	WriteStringToOutput("Content-Type: text/plain; charset=UTF-8\r\n");

	WriteStringToOutput("\r\n");
	UploadSendBackData = data;
	UploadSendBackDataLen = dataLen;
	//WriteStringToOutput(data);
	WriteDataToOutput();

}

void HttpConnection::ProcessFileUpload(char * data, int dataLen, char * action)
{
	bool doneUpload = false;
	char * temp = 0, * temp1 = 0, *endPtr, *ext, *ext1;
	temp = strstr(data , "filename=\"");
	if(temp){
		temp = temp + strlen("filename=\"");//point to file name start//TODO:Test It
		temp1 = strchr(temp, '\"');//point to file name end
	}
	if((temp1 - temp) < MAX_LENGTH_FILENAME)
	{
		strncpy(fileName, temp, temp1 - temp);
		//pprintf(" file name = %s", fileName);
		temp = strstr(data , "\r\n\r\n");
		if(temp)
		{	//Here we find the actual file length
			temp = temp + strlen("\r\n\r\n");
			endPtr = data + dataLen - (strlen(boundaryEnd) + strlen("\r\n\r\n"));
			ext = strchr(fileName, '.');
			ext1 = ext;
			while(ext)
			{
				ext1 = ext;
				ext = strchr(ext + 1, '.');
			}
			if((endPtr - temp) < 0){
				//UpdateTPVal(102);
				pprintf("\n negative length file upload");
			}
			if(VerifyFileLength(ext1 + 1, endPtr - temp))
			{
				//pprintf("\n verified file length = %d", endPtr - temp);
				doneUpload = true;
			}
			else
			{
				//pprintf("\n File Len Error");
				fileUploadInitiated = false;
				SendError(ERR_STR_FILELEN);
			}
		}
		else
		{
			fileUploadInitiated = false;
			SendError(ERR_FILE_START);
		}

		if(doneUpload)
		{
			fileUploadInitiated = false;
			//pprintf(" %s filName-%s ", __FUNCTION__, fileName);
			if((!strcmp(action , "SendBackCSVGraphDataHS")) || (!strcmp(action , "SendBackCSVGraphDataPS")))
			{
//				pprintf("\nfileUploadSendback = true");
				SendBackUploadDataAsAttachMent(temp , endPtr - temp, action);
			}
			else
			{
			if(httpServer->HandleFileUpload(temp , endPtr - temp, fileName,action))
				SendError(ERR_STR_FILEUPLOADED, false);
			else
				SendError(ERR_STR_FIRMWAREUPLOAD);
			}
		}
	}
	else{
		//pprintf("\n here at %d %s", __LINE__, __FUNCTION__);
	   //SendError(ERR_STR_UNKNOWN);
	   fileUploadInitiated = false;
		//Log::WriteEventLog(LEN_INVALID, (short)(4) , (short)0, 0, false);
		//Log::WriteEventLog(LEN_INVALID, (short)(4) , (short)0, 0, 0, TRUE);
	}
	request = Http_None;
}
