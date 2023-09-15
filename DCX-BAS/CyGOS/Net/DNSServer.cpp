// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Net/DNSServer.cpp_v   1.3   03 Sep 2013 18:54:54   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2009
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
//
// This module declares DNS Server.
//

#include "DNSServer.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "Socket.h"
#include "Ethernet.h"
#include "DebugNew.h"

DNSServer::DNSServer(Ethernet * eth) :
	Task("DNSServer", 2048)
{
	netif = eth;
}

DNSServer::~DNSServer()
{
	delete Rcvdata;
}

/**
 * 	Timer handling for Timeouts.
 */
void DNSServer::Tick()
{

}
//void UpdateTPVal(int PointIndex);
/**
 * 	Main Run loop of RTP
 */
void DNSServer::Run()
{
	DelayMs(3000);
	sockaddr_in sa;
	fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(fd != INVALID_SOCKET);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(DNS_SERVER_PORT);
	sa.sin_addr.s_addr = ip_addr_any.addr;;
	if (bind(fd, (sockaddr *) &sa, sizeof(sa)) < 0)
		CRASH("Bind DNS server socket");
	else
		printf("DNS Bind OK\n");

	Rcvdata = new char[65535];
	pprintf("Listening on UDP port %d\n", DNS_SERVER_PORT);
	for (;;)
	{
		memset(&client, 0, sizeof(client));
		memset(Rcvdata, 0, 65535);
		int fromlen = sizeof(client);
		int flags = 0;
		int ByteRecv = recvfrom(fd, Rcvdata, 65535, flags,
				(sockaddr *) &client, &fromlen);
		if (ByteRecv < 0)
		{
			DelayMs(1);
			continue;
		}
		else
		{
			ParsePacket((unsigned char*)Rcvdata);
			// Handle the each request (questions).
			for (int index = 0; index<ntohs(DNSPkt->PktHdr.q_count); index++)
			{
				// Make sure we have an IP address A record in the query.
				if ((DNSPkt->QDList[index].qtype == 1 )
						&&(DNSPkt->QDList[index].qclass == 1))
				{
					if (DNS_hasEntry(DNSPkt->QDList[index].name))
					{
						SendReply(DNSPkt->QDList[index].name);
					}
					else
					{
						// TODO.. LookUp for next dns server and send request;
						// For now we will send Name error in the reply.
						SendErrorReply(NAME_ERROR);
					}
				}
				else // We don't support any other type of query.
				{
					SendErrorReply(NOT_IMPLEMENTED);
				}
			}
			Cleanup();
		}
	}
}

//this will convert www.google.com to 3www6google3com 
void DNSServer::ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host)
{
	int lock=0, i;

	strcat((char*)host, ".");

	for (i=0; i<(int)strlen((char*)host); i++)
	{
		if (host[i]=='.')
		{
			*dns++=i-lock;
			for (; lock<i; lock++)
			{
				*dns++=host[lock];
			}
			lock++; //or lock=i+1;
		}
	}
	*dns++='\0';
}

unsigned char* DNSServer::ReadName(unsigned char* reader,
		unsigned char* buffer, int* count)
{
	unsigned char *name;
	unsigned int p=0, jumped=0, offset;
	int i, j;

	*count = 1;
	name = new unsigned char[256];

	name[0]='\0';

	//read the names in 3www6google3com format
	while (*reader!=0)
	{
		if (*reader>=192)
		{
			offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000  ;)
			reader = buffer + offset - 1;
			jumped = 1; //we have jumped to another location so counting wont go up!
		}
		else
			name[p++]=*reader;

		reader=reader+1;

		if (jumped==0)
			*count = *count + 1; //if we havent jumped to another location then we can count up
	}

	name[p]='\0'; //string complete
	if (jumped==1)
		*count = *count + 1; //number of steps we actually moved forward in the packet

	//now convert 3www6google3com0 to www.google.com
	for (i=0; i<(int)strlen((const char*)name); i++)
	{
		p=name[i];
		for (j=0; j<(int)p; j++)
		{
			name[i]=name[i+1];
			i=i+1;
		}
		name[i]='.';
	}
	name[i-1]='\0'; //remove the last dot
	return name;
}

// If we have X.Y.Z in query then parse the first part only i.e. X
// TODO Support the domain part.
unsigned char* DNSServer::ParseHostNameOnly(unsigned char* QueryName,
		unsigned char* ParsedName)
{
	int len = strlen((const char*)QueryName);
	//unsigned char S[256];
	int i = 0;
	while (len>0)
	{
		if (*QueryName == '.')
			break;
		else
			ParsedName[i++]= *QueryName;
		QueryName++;
		len--;
	}
	ParsedName[i] = '\0';
	//pprintf("\n\n ParseHostNameOnly: %s",ParsedName);
	return ParsedName;
}

// Search in the DNS Table to look the requested entry.
bool DNSServer::DNS_hasEntry(unsigned char* QueryName)
{
	bool NameFound = false;
	unsigned char ParsedName[256];
	ParseHostNameOnly(QueryName, ParsedName);
	POSITION pos = netif->DNSTable.GetHeadPosition();
	while (pos)
	{
		DNSTableEntry * DNS = netif->DNSTable.GetNext(pos);
		if (stricmp(DNS->HostName, (const char*)ParsedName) == 0)
			NameFound = true;
	}
	return NameFound;

}

void DNSServer::AddAnswers(struct RES_RECORD *res_records, unsigned char *Name,
		unsigned char* Addr)
{
	res_records->_class = htons(1);
	res_records->data_len = htons(4);
	res_records->type = htons(1);
	res_records->ttl = htons(5);
	res_records->rdata = Addr;
	res_records->name = Name;

}
void DNSServer::SendErrorReply(unsigned char Err)
{
	struct DNS_HEADER DNSHeader;

	DNSHeader.id = DNSPkt->PktHdr.id; // identification number
	DNSHeader.rd = 1;
	DNSHeader.tc = 0;
	DNSHeader.aa =0;
	DNSHeader.opcode = 0;
	DNSHeader.qr = RESPONSE_FLAG;

	DNSHeader.rcode = Err;
	DNSHeader.cd =0;
	DNSHeader.ad =0;
	DNSHeader.z =0;
	DNSHeader.ra =0;

	DNSHeader.q_count =DNSPkt->PktHdr.q_count;
	DNSHeader.ans_count =0;
	DNSHeader.auth_count =0;
	DNSHeader.add_count = 0;

	unsigned char *ReplyBuf;//[1000];
	int offset =sizeof(DNS_HEADER);
	unsigned char buff[1000];

	//Copy Header
	memcpy(buff, &DNSHeader, offset);

	//Copy Questions
	for (int Qcount =0; Qcount < ntohs(DNSPkt->PktHdr.q_count) ; Qcount++)
	{
		ReplyBuf = (unsigned char *)&buff[offset];
		ChangetoDnsNameFormat(ReplyBuf,
				(unsigned char *)DNSPkt->QDList[Qcount].name);

		offset += (strlen((const char*)ReplyBuf) + 1);

		unsigned short type = htons(DNSPkt->QDList[Qcount].qtype);
		memcpy(&buff[offset], &type, sizeof(unsigned short));
		offset += sizeof(unsigned short);
		unsigned short Class = htons(DNSPkt->QDList[Qcount].qclass);
		memcpy(&buff[offset], &Class, sizeof(unsigned short));
		offset += sizeof(unsigned short);
	}

	sendto(fd, &buff, offset, 0, (sockaddr *) &client, sizeof(client));
}
void DNSServer::SendReply(unsigned char * QueryName)
{

	struct DNS_HEADER PktHdr;

	//Send Reply
	//Prepare Header for the requested query
	PktHdr.id = DNSPkt->PktHdr.id; // identification number

	PktHdr.rd = 1;
	PktHdr.tc = 0;
	PktHdr.aa =0;
	PktHdr.opcode = 0;
	PktHdr.qr = RESPONSE_FLAG;

	PktHdr.rcode = 0;
	PktHdr.cd =0;
	PktHdr.ad =0;
	PktHdr.z =0;
	PktHdr.ra =0;

	PktHdr.q_count =DNSPkt->PktHdr.q_count;
	PktHdr.ans_count =0;
	PktHdr.auth_count =0;
	PktHdr.add_count = 0;

	int AnsCount = 0;

	int i =0;
	unsigned char ParsedName[256];
	ParseHostNameOnly(QueryName, ParsedName);


	//' Build the answer block.
	POSITION pos = netif->DNSTable.GetHeadPosition();
	while (pos)
	{
		DNSTableEntry * DNS = netif->DNSTable.GetNext(pos);
		if (stricmp(DNS->HostName, (const char*)ParsedName) == 0)
			AnsCount++;
	}

	PktHdr.ans_count = htons(AnsCount);
	struct RES_RECORD * res_records = new RES_RECORD[AnsCount];

	pos = netif->DNSTable.GetHeadPosition();
	while (pos)
	{
		DNSTableEntry * DNS = netif->DNSTable.GetNext(pos);
		if (stricmp(DNS->HostName, (const char*)ParsedName) == 0)
		{
			//char * temp = DNS->HostName;
			strcpy(tempName, DNS->HostName);
			// Add an answer from the cache to the reply packet.
			AddAnswers(&res_records[i], (unsigned char*)tempName, DNS->Addr);
		}
	}

	//printf("TTL %x %x %x %x  ",res_records[0].ttl ,ntohs(res_records[0].type),
	//		ntohs(res_records[0].data_len),ntohs(res_records[0]._class)	);

	unsigned char *ReplyBuf;//[1000];
	int offset =sizeof(DNS_HEADER);
	unsigned char buff[1000];

	//Copy Header
	memcpy(buff, &PktHdr, offset);

	//Copy Questions
	for (int Qcount =0; Qcount < ntohs(PktHdr.q_count) ; Qcount++)
	{
		ReplyBuf = (unsigned char *)&buff[offset];
		ChangetoDnsNameFormat(ReplyBuf,
				(unsigned char *)DNSPkt->QDList[Qcount].name);
		offset += (strlen((const char*)ReplyBuf) + 1);

		unsigned short type = htons(DNSPkt->QDList[Qcount].qtype);
		memcpy(&buff[offset], &type, sizeof(unsigned short));
		offset += sizeof(unsigned short);

		unsigned short Class = htons(DNSPkt->QDList[Qcount].qclass);
		memcpy(&buff[offset], &Class, sizeof(unsigned short));
		offset += sizeof(unsigned short);
	}

	//Copy Answers.
	for (int Anscount =0; Anscount < ntohs(PktHdr.ans_count); Anscount++)
	{

		ReplyBuf = (unsigned char *)&buff[offset];
		ChangetoDnsNameFormat(ReplyBuf,
				(unsigned char *)res_records[Anscount].name );
		offset += (strlen((const char*)ReplyBuf) + 1);

		unsigned short type = (res_records[Anscount].type);
		memcpy(&buff[offset], &type, sizeof(unsigned short));
		offset += sizeof(unsigned short);

		unsigned short Class = (res_records[Anscount]._class);
		memcpy(&buff[offset], &Class, sizeof(unsigned short));
		offset += sizeof(unsigned short);

		int ttl = (res_records[Anscount].ttl);
		memcpy(&buff[offset], &ttl, sizeof(int));
		offset += sizeof(int);

		unsigned short datalen = (res_records[Anscount].data_len );
		memcpy(&buff[offset], &datalen, sizeof(unsigned short));
		offset += sizeof(unsigned short);

		for (int RDatalen = 0; RDatalen < ntohs(res_records[Anscount].data_len); RDatalen++)
			buff[offset++] = res_records[Anscount].rdata[RDatalen];
	}

	//for( int x=0;x<offset;x++)
	//	printf(" %d/%d ",ReplyBuf[x],buff[x]);

	sendto(fd, &buff, offset, 0, (sockaddr *) &client, sizeof(client));
	delete res_records;
}
unsigned short DNSServer::dns_get_short(uint8 *ptr)
{
	unsigned short value;
	value = (unsigned short) (*ptr++) << 8;
	value |= (unsigned short) (*ptr++) ;
	return value;
}

//Parse DNS packet.
void DNSServer::ParsePacket(unsigned char * buf)
{
	unsigned char  *reader;
	int i,  stop;



	DNSPkt=(struct DNSPacket*)buf;

	//move ahead of the dns header and the query field
	reader=&buf[sizeof(struct DNS_HEADER)];

	int qcount = ntohs(DNSPkt->PktHdr.q_count);
	int AnsCount = ntohs(DNSPkt->PktHdr.ans_count);


	//reading questions
	stop=0;
	Query = new QUERY[qcount];

	for (i=0; i<qcount; i++)
	{
		Query[i].name=ReadName(reader, buf, &stop);
		reader = reader + stop;

		struct QUESTION *Quest;
		Quest = (struct QUESTION*)reader;//&buf[sizeof(struct DNS_HEADER)+stop];
		//int a = sizeof(struct DNS_HEADER)+stop;
		Query[i].qtype = dns_get_short(reader);
		Query[i].qclass = dns_get_short(reader+2);
		reader = reader + 4;
	}
	DNSPkt->QDList = Query;

	//reading answers
	stop=0;
	if (AnsCount >0)
	{
		//TODO Support Answers.
		//for now we don't need it.
		/**		answers = new RES_RECORD[AnsCount];
		 for(i=0;i<AnsCount;i++)
		 {
		 answers[i].name=ReadName(reader,buf,&stop);
		 reader = reader + stop;
		 struct R_DATA * r_data = (struct R_DATA*)(reader);
		 answers[i].type = r_data->type ;
		 answers[i]._class = r_data->_class ;
		 answers[i].ttl = r_data->ttl ;
		 answers[i].data_len = r_data->data_len ;

		 //answers[i].resource = (struct R_DATA*)(reader);
		 reader = reader + sizeof(struct R_DATA);

		 if(ntohs(answers[i].type) == 1) //if its an ipv4 address
		 {
		 answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].data_len));
		 
		 for(j=0 ; j<ntohs(answers[i].data_len) ; j++)
		 answers[i].rdata[j]=reader[j];
		 
		 answers[i].rdata[ntohs(answers[i].data_len)] = '\0';
		 
		 reader = reader + ntohs(answers[i].data_len);
		 
		 }
		 else
		 {
		 answers[i].rdata = ReadName(reader,buf,&stop);
		 reader = reader + stop;
		 }
		 }**/
	}
	//TODO read authorities
	/**for(i=0;i<ntohs(DNSPkt->PktHdr.auth_count);i++)
	 {
	 auth[i].name=ReadName(reader,buf,&stop);
	 reader+=stop;

	 auth[i].resource=(struct R_DATA*)(reader);
	 reader+=sizeof(struct R_DATA);

	 auth[i].rdata=ReadName(reader,buf,&stop);
	 reader+=stop;
	 }*/

	//TODO read additional
	/**for(i=0;i<ntohs(DNSPkt->PktHdr.add_count);i++)
	 {
	 addit[i].name=ReadName(reader,buf,&stop);
	 reader+=stop;

	 addit[i].resource=(struct R_DATA*)(reader);
	 reader+=sizeof(struct R_DATA);

	 if(ntohs(addit[i].resource->type)==1)
	 {
	 addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
	 for(j=0;j<ntohs(addit[i].resource->data_len);j++)
	 addit[i].rdata[j]=reader[j];

	 addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
	 reader+=ntohs(addit[i].resource->data_len);

	 }
	 else
	 {
	 addit[i].rdata=ReadName(reader,buf,&stop);
	 reader+=stop;
	 }
	 }*/
	//print questions
	/*	for(i=0;i<ntohs(DNSPkt->PktHdr.q_count );i++)
	 {
	 printf("Question : %s ",DNSPkt->QDList[0].name );
	 printf("Type : %d",ntohs(DNSPkt->QDList[0].qtype) );
	 printf("Class : %d",ntohs(DNSPkt->QDList[0].qclass ) );
	 }

	 //print answers
	 for(i=0;i<ntohs(DNSPkt->PktHdr.ans_count);i++)
	 {
	 //printf("\nAnswer : %d",i+1);
	 printf("Name  :  %s ",answers[i].name);
	 
	 if(ntohs(answers[i].type)==1)   //IPv4 address
	 {
	 
	 long *p;
	 p=(long*)answers[i].rdata;
	 a.sin_addr.s_addr=(*p);    //working without ntohl
	 //printf("has IPv4 address :  %s",inet_ntoa(a.sin_addr));
	 }
	 if(ntohs(answers[i].type)==5)   //Canonical name for an alias
	 printf("has alias name : %s",answers[i].rdata);
	 
	 printf("\n");
	 }

	 //print authorities
	 for(i=0;i<ntohs(DNSPkt->PktHdr.auth_count);i++)
	 {
	 //printf("\nAuthorities : %d",i+1);
	 printf("Name  :  %s ",auth[i].name);
	 if(ntohs(auth[i].type)==2)
	 printf("has authoritative nameserver : %s",auth[i].rdata);
	 printf("\n");
	 }

	 //print additional resource records
	 for(i=0;i<ntohs(DNSPkt->PktHdr.add_count);i++)
	 {
	 //printf("\nAdditional : %d",i+1);
	 printf("Name  :  %s ",addit[i].name);
	 if(ntohs(addit[i].type)==1)
	 {
	 long *p;
	 p=(long*)addit[i].rdata;
	 a.sin_addr.s_addr=(*p);    //working without ntohl
	 //printf("has IPv4 address :  %s",inet_ntoa(a.sin_addr));
	 }
	 printf("\n");
	 }*/

}
void DNSServer::Cleanup()
{
	for (int i = 0; i<ntohs(DNSPkt->PktHdr.q_count); i++)
		delete Query[i].name;
	delete Query;
}

