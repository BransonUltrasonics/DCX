// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/TftpClient.cpp_v   1.3   16 Aug 2011 08:53:34   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements TFTP Client task object.

// FIXME When two interfaces connect to same DHCP/TFTP server, second client uses first interface.
// When two Ethernet interfaces use same DHCP server as TFTP server, TFTP client
// attached to the second interface still uses first Ethernet interface to
// communicate because first TFTP client in the list is attached to the Ethernet
//interface with same DHCP/TFTP server IP address.

#include "TftpClient.h"
#include "RTOS.h"
#include "DhcpClient.h"
#include "string.h"
#include "Socket.h"

PtrList<TftpClient> TftpClient::clients;

TftpClient::TftpClient(Ethernet * eth) :
	Task("TftpClient", 10240), requests(5, "TftpReq")
{
	clients.AddTail(this);
	netif = eth;
}

void TftpClient::Run()
{
	for (;;)
	{
		TftpRequest request;
		requests.Read(this, request, -1);
		switch (request.type)
		{
		case TftpTransmit:
			TransmitFile(request);
			break;
		case TftpReceive:
			ReceiveFile(request);
			break;
		case TftpGetLength:
		default:
			CRASH("Bad TFTP client request type");
		}
	}
}

void TftpClient::Reset(TftpRequest & req)
{
	ErrorOccured = false;
	Tsize = req.size;
	File = req.data;
	Buffer = req.data;
	BufferIndex = 0;
	BlkSize = 512;
	NoOfOptions = 0;
	Done = false;
	Timeout = 5;
	LastDataPkt = -1;
	fileSize = 0;
}

void TftpClient::TransmitFile(TftpRequest & req)
{
	Reset(req);
	SendFileFlag = true;

	// some random port
	CreateSocket();

	SendWriteRequest(req.name, req.size, req.server);
	// Wait 5 secs for reply initially
	int length = Receive(5);
	if (length != -1)
	{
		bool ReTransmitFlag = ProcessPacket(length);
		if (ErrorOccured)
		{
			req.result->Write(this, -5, -1);
		}
		else
		{
			// this point on wait for Xms to detect timeout.
			// Retransmit after timeout occurs.
			// Maximum retransmission attempts is 3
			int tries = 0;
			do
			{
				// wait for Timeout seconds before timing out
				if ((length = Receive(Timeout)) != -1)
				{
					tries = 0;
					ReTransmitFlag = ProcessPacket(length);
					if (ErrorOccured)
					{
						req.result->Write(this, -1, -1);
						closesocket(sock);
						return;
					}
					if (Done)
					{
						req.result->Write(this, 0, -1);
						closesocket(sock);
						return;
					}
				}
				else
				{
					if (ReTransmitFlag)
					{
						if (sendto(sock, sendBuffer, LenSendBuffer, 0,
								(sockaddr *) &from, sizeof(from)) < 0)
							CRASH("Error sending data");
					}
					tries++;
				}
			} while (tries < 2);
			req.result->Write(this, -3, -1);
		}
	}
	else
	{
		req.result->Write(this, -4, -1);
	}
	closesocket(sock);
}

void TftpClient::ReceiveFile(TftpRequest & req)
{
	SendFileFlag = false;
	int tries = 0;

	// some random port
	CreateSocket();

	SendReadRequest(req.name, req.size, req.server);
	Reset(req);
	// Wait 5 secs for reply
	int length = Receive(5);
	if (length != -1)
	{
		bool ReTransmitFlag = ProcessPacket(length);
		if (ErrorOccured)
		{
			req.result->Write(this, -5, -1);
		}
		else
		{
			do
			{
				// wait for Timeout seconds before timing out
				if ((length = Receive(Timeout)) != -1)
				{
					tries = 0;
					ReTransmitFlag = ProcessPacket(length);
					if (ErrorOccured)
					{
						req.result->Write(this, -1, -1);
						closesocket(sock);
						return;
					}
					if (Done)
					{
						req.result->Write(this, fileSize, -1);
						closesocket(sock);
						return;
					}
				}
				else
				{
					if (ReTransmitFlag)
					{
						if (sendto(sock, sendBuffer, LenSendBuffer, 0,
								(sockaddr *) &from, sizeof(from)) < 0)
							CRASH("Error sending data");
					}
					tries++;
				}
			} while (tries < 5);
			req.result->Write(this, -3, -1);
		}
	}
	else
	{
		req.result->Write(this, -4, -1);
	}
	closesocket(sock);
}

void TftpClient::GetFileLength(TftpRequest & req)
{
	CRASH("GetFileLength");
	ASSERT(req.size);
}

/**
 * 	Function creates a UDP Client on the specified port.
 * 	@param port : Client port number
 */
void TftpClient::CreateSocket()
{
	sock = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(sock != INVALID_SOCKET);
	sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = 0;
	socketAddress.sin_addr.S_un.S_addr = ip_addr_any.addr;
	if (bind(sock, (sockaddr *) &socketAddress, sizeof(socketAddress)) < 0)
		CRASH("Bind");
	RcvdBlkNo = 0;
	RcvdACKNo = 0;
}

int TftpClient::TransmitFile(char * name, int size, void * data, ip_addr server)
{
	TftpRequest request;
	request.name = name;
	request.data = (unsigned char *) data;
	request.size = size;
	request.server = server;
	if (server.addr == 0)
	{
		if (!netif)
			return 0;
		if (!netif->dhcp)
			return 0;
		request.server = netif->dhcp->server_ip_addr;
	}
	request.type = TftpTransmit;
	Fifo<int> result(1, "Reply");
	request.result = &result;
	requests.Write(RTOS::GetCurrentTask(), request, -1);
	int ret;
	result.Read(RTOS::GetCurrentTask(), ret, -1);
	return ret;
}

int TftpClient::ReceiveFile(char * name, int * size, void * data,
		ip_addr server)
{
	TftpRequest request;
	request.name = name;
	request.data = (unsigned char *) data;
	request.size = *size;
	request.server = server;
	if (server.addr == 0)
	{
		if (!netif)
			return 0;
		if (!netif->dhcp)
			return 0;
		request.server = netif->dhcp->server_ip_addr;
	}
	request.type = TftpReceive;
	Fifo<int> result(1, "Reply");
	request.result = &result;
	requests.Write(RTOS::GetCurrentTask(), request, -1);
	int ret;
	result.Read(RTOS::GetCurrentTask(), ret, -1);
	*size = ret < 0 ? 0 : ret;
	return ret >= 0 ? 0 : ret;
}

int TftpClient::GetFileLength(char * name, ip_addr server)
{
	CRASH("GetFileLength");
	ASSERT(name);
	ASSERT(server.addr);
}

bool TftpClient::SendReadRequest(char *name, int size, ip_addr addr)
{
	int len = CreateRRQPkt(name, size);
	from.sin_family = AF_INET;
	from.sin_port = htons(69);
	from.sin_addr.S_un.S_addr = addr.addr;
	if (sendto(sock, sendBuffer, len, 0, (sockaddr *) &from, sizeof(from)) < 0)
		CRASH("Error sending data");
	return true;
}

bool TftpClient::SendWriteRequest(char *name, int size, ip_addr addr)
{
	int len = CreateWRQPkt(name, size);
	from.sin_family = AF_INET;
	from.sin_port = htons(69);
	from.sin_addr.S_un.S_addr = addr.addr;
	if (sendto(sock, sendBuffer, len, 0, (sockaddr *) &from, sizeof(from)) < 0)
		CRASH("Error sending data");
	return true;
}

/**
 *	Create Write Request Packet
 *	@param buffer: pointer to pre-allocated packet
 *	@param name : file name
 *	@param size : size of the file in bytes
 *	@param return : length of the packet
 */
int TftpClient::CreateWRQPkt(char *name, int size)
{
	RRQWRQPkt * packet = (RRQWRQPkt *) sendBuffer;
	memset(packet, 0, sizeof(RRQWRQPkt));
	packet->Opcode = WRQOpcode;

	uint8 * DataPtr = packet->Data;
	// Filename
	strcpy((char *) DataPtr, name);
	DataPtr += strlen(name) + 1;
	// Mode
	strcpy((char *) DataPtr, "octet");
	DataPtr += strlen("octet") + 1;
	// Maximum supported block size
	strcpy((char *) DataPtr, "blksize");
	DataPtr += strlen("blksize") + 1;
	// Blocksize
	strcpy((char *) DataPtr, "1432");
	DataPtr += strlen("1432") + 1;
	// Transfer Size option
	strcpy((char *) DataPtr, "tsize");
	DataPtr += strlen("tsize") + 1;
	// Tsize
	char temp[26];
	sprintf(temp, "%d", size);
	strcpy((char *) DataPtr, temp);
	DataPtr += strlen(temp) + 1;

	return (DataPtr - packet->Data) + sizeof(short);
}

int TftpClient::CreateRRQPkt(char *name, int size)
{
	RRQWRQPkt * packet = (RRQWRQPkt *) sendBuffer;
	memset(packet, 0, sizeof(RRQWRQPkt));
	packet->Opcode = RRQOpcode;

	uint8 * DataPtr = packet->Data;
	// Filename
	strcpy((char *) DataPtr, name);
	DataPtr += strlen(name) + 1;
	// Mode
	strcpy((char *) DataPtr, "octet");
	DataPtr += strlen("octet") + 1;
	// Maximum supported block size
	strcpy((char *) DataPtr, "blksize");
	DataPtr += strlen("blksize") + 1;
	// Blocksize
	strcpy((char *) DataPtr, "1432");
	DataPtr += strlen("1432") + 1;
	// Transfer Size option
	strcpy((char *) DataPtr, "tsize");
	DataPtr += strlen("tsize") + 1;
	// Tsize
	strcpy((char *) DataPtr, "0");
	DataPtr += strlen("0") + 1;

	return (DataPtr - packet->Data) + sizeof(short);
	size = 0;
}

void TftpClient::SendErrorPkt(TFTPErr code, char * string)
{
	LenSendBuffer = 2 + 2 + strlen(string);
	ERRORPkt * packet = (ERRORPkt *) sendBuffer;
	packet->Opcode = ERROROpcode;
	packet->ErrorCode = code;
	strcpy(packet->ErrorMsg, string);
	if (sendto(sock, sendBuffer, LenSendBuffer, 0, (sockaddr *) &from,
			sizeof(from)) < 0)
		CRASH("Error sending data");
}

bool TftpClient::ProcessWRQOpcode(int length)
{
	// extract options from the packet
	ExtractOptions(length);
	// filename looks ok?
	// mode is "octet"
	if (strcmp("octet", mode))
	{
		SendErrorPkt(UndefinedErr, "Only octet mode supported");
		ErrorOccured = true;
		// error packets are not acknowledged nor retransmitted
		return false;
	}
	if (!CheckOptions())
		return false;
	// If number of options greater than 0 send OACK
	// else send ACK packet
	if (NoOfOptions > 0)
		SendOAckPkt();
	else
		SendAckPkt(sendBuffer, 0);
	//	printf("Receiving %s\n", "filename");
	return true;
}

bool TftpClient::ProcessDATAOpcode(int length)
{
	DATAPkt *packet = (DATAPkt *) recvBuffer;
	if ((packet->BlockNo - RcvdBlkNo) == 1)
	{
		// copy data into Buffer array
		memcpy(&Buffer[BufferIndex], packet->Data, length - 4);
		BufferIndex += length - 4;
		// less than max packet size then we are done with transfer
		if ((length - 4) < BlkSize)
		{
			Done = true;
		}
		RcvdBlkNo = packet->BlockNo;
	}
	SendAckPkt(sendBuffer, packet->BlockNo);
	return true;
}

bool TftpClient::ProcessOACKOpcode(int length)
{
	// extract options from the packet
	ExtractOptions(length);
	// Check if they are all ok
	if (!CheckOptions())
		return false;
	// For a Write Request this need not be sent
	if (!SendFileFlag)
	{
		SendAckPkt(sendBuffer, 0);
		return true;
	}
	// send first data packet
	RcvdACKNo = 0;
	if (Tsize)
	{
		// Send a Data packet now
		int size = (Tsize > 1432) ? 1432 : Tsize;
		SendDataPkt(File, size, 1);
		File += size;
		Tsize -= size;
		if (size < 1432)
			LastDataPkt = RcvdACKNo + 1;
		return true;
	}
	return false;
}

bool TftpClient::ProcessACKOpcode()
{
	ACKPkt *packet = (ACKPkt *) recvBuffer;
	if ((packet->BlockNo - RcvdACKNo) > 1) // if ACK is repeated then retransmit
	{
		RcvdACKNo = packet->BlockNo;
		return true;
	}
	if (packet->BlockNo == LastDataPkt)
	{
		Done = true;
		return false;
	}
	// send next data packet
	RcvdACKNo = packet->BlockNo;
	// Send a Data packet now
	int size = (Tsize > 1432) ? 1432 : Tsize;
	SendDataPkt(File, size, RcvdACKNo + 1);
	if (size < 1432)
		LastDataPkt = RcvdACKNo + 1;
	File += size;
	Tsize -= size;
	return true;
}

/**
 * 	Process Incoming packets.
 * 	@param pkt : pointer to packet buffer
 * 	@param length : length of the packet
 * 	@return : true if packet was sent. False otherwise.
 */
bool TftpClient::ProcessPacket(int length)
{
	NetShort type;
	memcpy(&type, recvBuffer, 2);

	switch (type)
	{
	case WRQOpcode:
		return ProcessWRQOpcode(length);
	case DATAOpcode:
		return ProcessDATAOpcode(length);
	case ERROROpcode:
	{
		//ERRORPkt *packet = (ERRORPkt *) recvBuffer;
		//		printf("Client returned error %d. ", (uint16)packet->ErrorCode);
		//		if (length > 4)
		//			printf("%s.\n", packet->ErrorMsg);
		ErrorOccured = true;
		break;
	}
	case OACKOpcode:
		return ProcessOACKOpcode(length);
	case ACKOpcode:
		return ProcessACKOpcode();
	default:
		printf("Unknown Packet type\n");
	}
	return false;
}

/**
 * 	Extract options from a WRQ packet
 * 	@param pkt: pointer to a buffer holding WRQ packet
 * 	@param length : length of the buffer
 */
void TftpClient::ExtractOptions(int length)
{
	uint8 * pkt = recvBuffer;
	char temp[26];
	RRQWRQPkt *packet = (RRQWRQPkt *) pkt;

	int word = 0, index = 0;
	length -= 2; // skip over opcode

	int opt = 0, val = 0;
	// extract the string for words and put them in the right place
	while (length > 0)
	{
		int i = 0;
		// extract a word
		while (packet->Data[index])
		{
			temp[i++] = packet->Data[index++];
			length--;
		}
		temp[i++] = '\0';
		// figure out the kind of word
		switch (word)
		{
		//		case 0: // filename
		//			strcpy(filename, temp);
		//			printf("File: %s\n", filename);
		//			break;
		//		case 1: // mode
		//			strcpy(mode, temp);
		//			printf("Mode: %s\n", mode);
		//			break;
		default: // type of option/value
			if (word % 2)
			{
				strcpy(values[val++], temp);
				//				printf("%s\n", values[val - 1]);
				NoOfOptions++;
			}
			else
			{
				strcpy(options[opt++], temp);
				//				printf("%s\n", options[opt - 1]);
			}
		};
		word++; // finished a word
		index++; // skip '\0'
		length--;
	}
}

/**
 * 	Check options and see if we support it. For each of the options
 * 	check to see if they are in bounds. If not send an Error Packet
 * 	with appropriate error code.
 * 	@return : false if option not supported true otherwise
 */
bool TftpClient::CheckOptions()
{
	for (int k = 0; k < NoOfOptions; k++)
	{
		if (!strcmp("blksize", options[k]))
		{
			// block size greater than what we can accept
			if (atoi(values[k]) > 1432)
			{
				//printf("Blk Size greater than 1432\n");
				SendErrorPkt(OptionUnacceptableErr,
						"Blk Size greater than 1432");
				ErrorOccured = true;
				// error packets are not acknowledged nor retransmitted
				return false;
			}
			BlkSize = atoi(values[k]);
		}
		else if (!strcmp("tsize", options[k]))
		{
			// transfer size greater than what we can accept
			//if (atoi(values[k]) > (int)__USER_FLASH_SIZE)
			if (atoi(values[k]) > 21000000)
			{
				char str[100];
				sprintf(str, "Transfer Size greater than %d bytes",
						(int) 21000000);
				SendErrorPkt(DiskFullErr, str);
				ErrorOccured = true;
				// error packets are not acknowledged nor retransmitted
				return false;
			}
			fileSize = atoi(values[k]);
		}
		else if (!strcmp("timeout", options[k]))
		{
			// timeout greater than 3 seconds.
			if (atoi(values[k]) > 3)
			{
				SendErrorPkt(OptionUnacceptableErr,
						"Timeout value greater than 3 secs");
				ErrorOccured = true;
				// error packets are not acknowledged nor retransmitted
				return false;
			}
			Timeout = atoi(values[k]);
		}
		else
		{
			if (NoOfOptions > 0)
			{
				SendErrorPkt(OptionUnacceptableErr, "Unknown Option");
				ErrorOccured = true;
				// error packets are not acknowledged nor retransmitted
				return false;
			}
		}
	}
	return true;
}

/**
 * 	Create Options Acknowledgment packet
 * 	@param buffer : pointer to pre-allocated packet
 * 	@return : returns the size of the packet
 */
void TftpClient::SendOAckPkt()
{
	int size = 0;
	OACKPkt * packet = (OACKPkt *) sendBuffer;
	packet->Opcode = OACKOpcode;
	size += 2;
	int temp = 0;
	for (int i = 0; i < NoOfOptions; i++)
	{
		strcpy((char *) &packet->Data[temp], options[i]);
		temp += strlen(options[i]) + 1;
		strcpy((char *) &packet->Data[temp], values[i]);
		temp += strlen(values[i]) + 1;
	}
	LenSendBuffer = size + temp;
	// Send OACK packet
	if (sendto(sock, sendBuffer, LenSendBuffer, 0, (sockaddr *) &from,
			sizeof(from)) < 0)
		CRASH("Error sending data");
}

/**
 * 	Create Acknowledge packet
 * 	@param buffer : pointer to pre-allocated packet
 * 	@param blk : block number to ack
 */
void TftpClient::SendAckPkt(uint8 *buffer, uint16 blk)
{
	LenSendBuffer = sizeof(ACKPkt);
	ACKPkt * packet = (ACKPkt *) buffer;
	packet->Opcode = ACKOpcode;
	packet->BlockNo = blk;
	if (sendto(sock, sendBuffer, LenSendBuffer, 0, (sockaddr *) &from,
			sizeof(from)) < 0)
		CRASH("Error sending ACK");
}

/**
 * 	Create Data packet
 * 	@param buffer : pointer to pre-allocated packet
 * 	@param fileptr : pointer to file which needs to be transferred
 * 	@param size : # of bytes to transfer in this packet
 * 	@param blk : block number to send
 * 	@param return : length of the packet in bytes
 */
void TftpClient::SendDataPkt(uint8 *fileptr, int size, uint16 blk)
{
	DATAPkt *packet = (DATAPkt *) sendBuffer;
	packet->Opcode = DATAOpcode;
	packet->BlockNo = blk;
	if (size)
		memcpy(packet->Data, fileptr, size);
	LenSendBuffer = size + 4;
	if (sendto(sock, sendBuffer, LenSendBuffer, 0, (sockaddr *) &from,
			sizeof(from)) < 0)
		CRASH("Error sending Data packet.");
}

/**
 * 	Receive: Funtion waits for timeoutSeconds before timing out
 * 	@param timeoutSeconds: Timeout in Seconds
 * 	@param return : -1 on error else the number of bytes received
 */
int TftpClient::Receive(int timeoutSeconds)
{
	int dataRecvd;
	int len = sizeof(from);

	// convert to msecs
	timeoutSeconds *= 1000;

	// try for timeoutSeconds
	while ((dataRecvd = recvfrom(sock, recvBuffer, sizeof(recvBuffer),
			MSG_DONTWAIT, (sockaddr *) &from, &len)) <= 0)
	{
		// 1 ms choosen to be responsive, otherwise on good cnxns things
		// will become terribly slow
		timeoutSeconds -= 1;
		if (timeoutSeconds <= 0)
			break;
		DelayMs(1);
	}

	// return
	return dataRecvd;
}
