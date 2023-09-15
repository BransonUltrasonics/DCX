//$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/TftpClient.h_v   1.0   21 Mar 2012 12:43:24   PDwivedi  $
//
//This program is the property of Cyber Group Inc
//Copying of this software is expressly forbidden, without the prior written
//consent of Cyber Group Inc
//
//This file is part of CyGOS-3.
//
//This module declares TFTP Client task object.

#ifndef TFTPCLIENT_H_
#define TFTPCLIENT_H_
#include "Task.h"
#include "LinkedList.h"
#include "Ethernet.h"
#include "UDP.h"
#include "Socket.h"
#include "ArchDefs.h"

class TftpClient: public Task
{
	friend class CyGOS;
	/**
	 * 	TFTP supports 5 types of packets. The opcode value of each of these
	 * 	packet types are as defined.
	 */
	enum TFTPOpcode
	{
		/*
		 * Opcode for Read Request
		 */
		RRQOpcode = 1,
		/*
		 * Opcode for Write Request
		 */
		WRQOpcode = 2,
		/*
		 * Opcode for Data Packet
		 */
		DATAOpcode = 3,
		/*
		 * Opcode for Data Acknowledgment
		 */
		ACKOpcode = 4,
		/*
		 * Opcode for Error
		 */
		ERROROpcode = 5,
		/*
		 * 	Opcode for Options Acknowledge
		 */
		OACKOpcode = 6,
	};
	/**
	 * 	TFTP Error codes and their meaning
	 */
	enum TFTPErr
	{
		UndefinedErr = 0,
		FileNotFoundErr = 1,
		AccessViolationErr = 2,
		DiskFullErr = 3,
		IllegalOperationErr = 4,
		UnknownTransferIdErr = 5,
		FileExistsErr = 6,
		NoSuchUserErr = 7,
		OptionUnacceptableErr = 8,
		NoErr = 9,
	};

	enum TftpRequestType
	{
		TftpTransmit, TftpReceive, TftpGetLength,
	};
	struct TftpRequest
	{
		TftpRequestType type;
		char * name;
		int size;
		unsigned char * data;
		ip_addr server;
		Fifo<int> * result;
	};
	/**
	 * 	Defines the RRQ/WRQ packet format
	 */
	struct RRQWRQPkt
	{
		/*
		 * 	Opcode should be either RRQOpcode/WRQOpcode
		 */
		NetShort Opcode;
		/*
		 * 	Data is subdivided into the following blocks(RFC 1350)
		 *	   string   1 byte   string  1 byte
		 *	 ---------------------------------
		 * 	| Filename |   0   |  Mode  |  0  |
		 *	 ---------------------------------
		 * 	Option Extension (RFC 2347)
		 *	 ------------------------------------------------------
		 * 	| opt1 | 0 | value1 | 0 | .... | optN | 0 | valueN | 0 |
		 *	 ------------------------------------------------------
		 * 	The options and extensions are NULL terminated strings.
		 * 	Maximum size of a request packet is 512 octets.
		 *
		 * 	Blocksize Option (RFC 2348) Allows block size negotiation
		 * 	to improve throughput.
		 * 	opt = "blksize" value = 8 to 65464 # of octets
		 *
		 * 	Timeout Option (RFC 2349)
		 * 	opt = "timeout" value = secs 1 to 255 which says the # of
		 * 	seconds to wait before re-transmitting
		 *
		 * 	Transfer Size Option (RFC 2349). Pre-asks if a client can
		 * 	accept this size file.
		 * 	opt = "tsize" value = # of octets to be transferred
		 */
		uint8 Data[510];
	};

	/**
	 * 	Defines the DATA packet format.
	 */
	struct DATAPkt
	{
		/*
		 * 	Opcode should be either RRQOpcode/WRQOpcode
		 */
		NetShort Opcode;
		/*
		 * 	Block Number
		 */
		NetShort BlockNo;
		/*
		 * 	Data of size 512 bytes. (RFC 1350)
		 * 	Extension for blksize support. Safely fits in 1 Ethernet frame.
		 */
		uint8 Data[1432];
	};

	/**
	 * 	Defines the ACK packet format.
	 */
	struct ACKPkt
	{
		/*
		 * 	Opcode should be either RRQOpcode/WRQOpcode
		 */
		NetShort Opcode;
		/*
		 * 	Block Number
		 */
		NetShort BlockNo;
	};

	/**
	 * 	Defines the ACK packet format.
	 */
	struct ERRORPkt
	{
		/*
		 * 	Opcode should be either RRQOpcode/WRQOpcode
		 */
		NetShort Opcode;
		/*
		 * 	Error Code
		 */
		NetShort ErrorCode;
		/*
		 * 	Error Message which is a NULL terminated string
		 */
		char ErrorMsg[510];
	};

	/**
	 * 	Defines the Options Acknowledge packet format. (RFC 2347)
	 */
	struct OACKPkt
	{
		/*
		 * 	Opcode should be either RRQOpcode/WRQOpcode
		 */
		NetShort Opcode;
		/*
		 * 	Data is subdivided into the following blocks.
		 *	 ------------------------------------------------------
		 * 	| opt1 | 0 | value1 | 0 | .... | optN | 0 | valueN | 0 |
		 *	 ------------------------------------------------------
		 * 	opt1 : The first option acknowledged, copied from the original
		 * 	request.
		 * 	value1 : The acknowledged value associated with the first
		 * 	option.
		 */
		uint8 Data[510];
	};

	enum Configuration
	{
		MaxBufSize = EthMaxData - sizeof(IPv4Header) - sizeof(UdpHeader),
		MaxOptions = 10,
	};
public:
	TftpClient(Ethernet * eth = 0);
	int TransmitFile(char * name, int size, void * data, ip_addr server);
	int ReceiveFile(char * name, int * size, void * data, ip_addr server);
	int GetFileLength(char * name, ip_addr server);
protected:
	void Run();
	void TransmitFile(TftpRequest & req);
	void ReceiveFile(TftpRequest & req);
	void GetFileLength(TftpRequest & req);
	void CreateSocket();
	bool SendReadRequest(char *name, int size, ip_addr addr);
	bool SendWriteRequest(char *name, int size, ip_addr addr);
	int CreateRRQPkt(char *name, int size);
	int CreateWRQPkt(char *name, int size);
	void SendErrorPkt(TFTPErr code, char *string);
	void SendAckPkt(uint8 *buffer, uint16 blk);
	void SendOAckPkt();
	void SendDataPkt(uint8 *fileptr, int size, uint16 blk);
	bool ProcessPacket(int length);
	bool ProcessWRQOpcode(int length);
	bool ProcessDATAOpcode(int length);
	bool ProcessOACKOpcode(int length);
	bool ProcessACKOpcode();
	bool CheckOptions();
	void ExtractOptions(int length);
	void Reset(TftpRequest & req);
	int Receive(int timeoutSeconds);
	Fifo<TftpRequest> requests;
	int sock;
	unsigned char sendBuffer[MaxBufSize];
	// Size of Send Buffer
	int LenSendBuffer;
	unsigned char recvBuffer[MaxBufSize];
	// address information of the incoming packet
	sockaddr_in from;
	uint32 RcvdBlkNo;
	uint32 RcvdACKNo;
	// type of transfer. Only octet is supported
	char mode[10];
	// Flag indicating whether error occured during transfer
	bool ErrorOccured;
	// If send file has been called
	bool SendFileFlag;
	// Pointer to file which needs to be sent
	uint8 * File;
	int fileSize;
	// Transfer Size of the file to send
	int Tsize;
	// number of different options in the packet
	int NoOfOptions;
	// options can be any of blksize timeout tsize
	char options[MaxOptions][10];
	// values for each of the options
	char values[MaxOptions][10];
	// Pointer to Buffer which holds the transferred file
	uint8 *Buffer;
	// Index into file storage buffer
	unsigned int BufferIndex;
	// Block Size of each data block
	int BlkSize;
	// flag indicating whether transfer is completed
	bool Done;
	// Time out associated with a retry
	int Timeout;
	// filename of the incoming file
	//	char filename[26];
	int LastDataPkt;
	Ethernet * netif;
	static PtrList<TftpClient> clients;
};
#endif
