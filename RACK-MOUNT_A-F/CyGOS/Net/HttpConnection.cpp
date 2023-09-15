// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Net/HttpConnection.cpp_v   1.1   24 Jul 2015 08:51:04   sesharma  $
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
	content_length = 0;
	//	search = 0;
	httpServer = svrPtr;
	memset(modifiedSinceStr , 0, sizeof(modifiedSinceStr));
	memset(boundaryEnd , 0, sizeof(boundaryEnd));
	content_type[0] = 0;
	//file upload related variables
	fileUpload = false;
	rxBuf = rxBuff;
	http = 0;
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
			content_length = 0;
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
		    printf(".");
		    //TODO: Don't call GetMaxUploadLength function every time
			if((content_length > (httpServer->GetMaxUploadLength())) || ((rxPtr + length) >= (content_length + 1024)))
			{
				rxPtr = 0;
				fileUpload = false;
				fileUploadInitiated = false;
				ready = false;
				pprintf("\n file len error");
				request = Http_None;
				content_length = 0;
				rxBuf = rxBuff;
				SendError(httpServer->FileLenInvalidMsg);

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
            content_length = 0;
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
			if (crlf) {
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
				if (fileUpload && ready) {
					if (CheckForFileEnd() == true) {
						request = Http_POST;
						ready = true;
					} else {
						request = Http_None;
						ready = false;
					}
					break;
				}
				/////////////////////////
				else if (ready)               // && !fileUpload)
				{
					//printf("\n leng %d content len %d rxBuf %s", rxPtr + length, content_length, rxBuf);
					if ((content_length > 0)
							&& ((rxPtr + length) < content_length))
						ready = false;

					break;
				}

			} else if ((content_length > 0) && (rxPtr >= content_length)) {
				ready = true;
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
				content_length = 0;
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
				//SendError(ERR_NOT_IMPLEMNETED, httpNotImplemented);
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
	//pprintf("*");
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
				POSITION pos = httpServer->SendBackUploadList.GetHeadPosition();
				SendBackUploadData  * Temp;
				while (pos)
				{
					Temp = httpServer->SendBackUploadList.GetNext(pos);
					if(!stricmp(uri + 1, Temp->HtmlFormAction))
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
		         if(content_length < (httpServer->GetMaxUploadLength())){
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

			  request = Http_None;
			  return true;
		 }
	 }
	 else{

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
		SendErrorNotFound(httpServer->ResourceUnavailableMsg);
}

void HttpConnection::HandleRequestPost()
{
	//printf("POST '%s%s'\n", root, uri);
	memset(&header, 0, sizeof(header));
	if(httpServer->HandlePostEvent(rxBuf, uri + 1, header, PeerAddr, content_length))
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
		SendErrorNotFound(httpServer->ResourceUnavailableMsg);

}


void HttpConnection::SendError(char * err, HttpError errorCode)
{
		int content_length;
		content_length = strlen(err);
		char buf[30];
		sprintf(buf , "HTTP/1.1 %d OK\r\n" , errorCode);
		WriteStringToOutput(buf);
		sprintf(buf, "Content-Length: %d\r\n",content_length);
		WriteStringToOutput(buf);
		WriteStringToOutput("Connection: Keep-Alive\r\n");
		WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");
		WriteStringToOutput("\r\n");
		WriteStringToOutput(err);
		request = Http_None;
}

void HttpConnection::SendErrorNotFound(char * err)
{
		int content_length;
		WriteStringToOutput("HTTP/1.1 404 Not Found\r\n");
		char buf[30];
		content_length = strlen(err);
		sprintf(buf, "Content-Length: %d\r\n",content_length);
		WriteStringToOutput(buf);
		WriteStringToOutput("Connection: Keep-Alive\r\n");
		WriteStringToOutput("Content-Type: text/html; charset=UTF-8\r\n");
		WriteStringToOutput("\r\n");
		WriteStringToOutput(err);
		request = Http_None;
}

bool HttpConnection::VerifyFileExtension(char * ext)
{
	POSITION pos = httpServer->UploadSupportedList.GetHeadPosition();
	UploadSupportedFile  * Temp;
	while (pos)
	{
		Temp = httpServer->UploadSupportedList.GetNext(pos);
		//printf("\nTemp.maxSize = %d", Temp->maxSize);
		if (stricmp(ext, Temp->extension) == 0)
			return true;
	}
	return false;
}

bool HttpConnection::VerifyFileLength(char * ext, int len)
{
	POSITION pos = httpServer->UploadSupportedList.GetHeadPosition();
	UploadSupportedFile  * Temp;
	while (pos)
	{
		Temp = httpServer->UploadSupportedList.GetNext(pos);
		if (stricmp(ext, Temp->extension) == 0)
		{
			if(len <= Temp->maxSize)
				return true;
		}
	}
	return false;
}

void HttpConnection::SendBackUploadDataAsAttachMent(char * data, int dataLen , char * action, char * FileName)
{
	WriteStringToOutput("HTTP/1.1 200 OK\r\n");
	char buf[80];
	sprintf(buf, "Content-Length: %d\r\n", dataLen);
	WriteStringToOutput(buf);
	WriteStringToOutput("Connection: Keep-Alive\r\n");

	sprintf(buf, "Content-Disposition: attachment; filename=%s\r\n", FileName);
	WriteStringToOutput(buf);
	WriteStringToOutput("Content-Type: text/plain; charset=UTF-8\r\n");

	WriteStringToOutput("\r\n");
	UploadSendBackData = data;
	UploadSendBackDataLen = dataLen;
	WriteDataToOutput();

}

void HttpConnection::ProcessFileUpload(char * data, int dataLen, char * action)
{
   //pprintf(" %s %X", __FUNCTION__, this);
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

				pprintf("\n negative length file upload");
			}

			//pprintf("\n ext1 = %s", ext1);
			//pprintf("\n action = %s", action);
			//pprintf("\n URI+1 = %s", uri+1);
			if(!(ext1) || VerifyFileExtension(ext1 + 1))
			{
				if(!(ext1) || VerifyFileLength(ext1 + 1, endPtr - temp))
				{
					//pprintf("\n verified file length = %d", endPtr - temp);
					doneUpload = true;
				}
				else
				{
					//pprintf("\n File Len Error");
					fileUploadInitiated = false;
					SendError(httpServer->FileLenInvalidMsg);
				}
			}
			else
			{
				//pprintf("\n File Len Error");
				fileUploadInitiated = false;
				SendError(httpServer->FileExtensionInvalidMsg);
			}
		}
		else
		{
			fileUploadInitiated = false;
			SendError(httpServer->FileUploadInvalidStartMsg);
		}

		if(doneUpload)
		{
			fileUploadInitiated = false;
			bool UploadSendBack = false;
			POSITION pos = httpServer->SendBackUploadList.GetHeadPosition();
			SendBackUploadData  * Temp;
			while (pos)
			{
				Temp = httpServer->SendBackUploadList.GetNext(pos);
				if(!stricmp(uri + 1, Temp->HtmlFormAction))
				{
					SendBackUploadDataAsAttachMent(temp , endPtr - temp, action, Temp->SendBackFileName);
					UploadSendBack = true;
					break;
				}
			}
			if(!UploadSendBack)
			{
				if(httpServer->HandleFileUpload(temp , endPtr - temp, fileName,action))
					SendError(httpServer->FileUploadSuccessMsg);
				else
					SendError(httpServer->FileUploadRejectMsg);
			}
		}
	}
	else{
	   fileUploadInitiated = false;
	   SendError(httpServer->FileUploadInvalidFileNameMsg);
	}
	request = Http_None;
}
