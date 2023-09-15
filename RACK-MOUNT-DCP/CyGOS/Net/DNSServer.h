#ifndef DNSSERVER_H_
#define DNSSERVER_H_


#include "Task.h"
#include "socket.h"
#include "Fifo.h"


/** DNS server port address */
#ifndef DNS_SERVER_PORT
#define DNS_SERVER_PORT           53
#endif

enum RCODE
{
    NO_DNS_ERROR, //               No error condition

	FORMAT_ERROR, //           Format error - The name server was unable to interpret the query.

	SERVER_FAILURE,//          Server failure - The name server was
                   //             unable to process this query due to a
                   //             problem with the name server.
	NAME_ERROR,		//          Name Error - Meaningful only for
                    //            responses from an authoritative name
                    //            server, this code signifies that the
                    //            domain name referenced in the query does not exist.

	NOT_IMPLEMENTED,//          Not Implemented - The name server does
                    //            not support the requested kind of query.

	REFUSED //                 Refused - The name server refuses to
             //                   perform the specified operation for
             //                   policy reasons.  For example, a name
             //                   server may not wish to provide the
             //                   information to the particular requester,
             //                   or a name server may not wish to perform
             //                   a particular operation (e.g., zone transfer) for particular data.
};

enum QueryResponseFLAG
{
	QUERY_FLAG,
	RESPONSE_FLAG
};

//DNS header structure
struct DNS_HEADER
{
	unsigned	short id;		    // identification number
	
	unsigned	char rd     :1;		// recursion desired
	unsigned	char tc     :1;		// truncated message
	unsigned	char aa     :1;		// authoritive answer
	unsigned	char opcode :4;	    // purpose of message
	unsigned	char qr     :1;		// query/response flag
	
	unsigned	char rcode  :4;	    // response code
	unsigned	char cd     :1;	    // checking disabled
	unsigned	char ad     :1;	    // authenticated data
	unsigned	char z      :1;		// its z! reserved
	unsigned	char ra     :1;		// recursion available
	
	unsigned    short q_count;	    // number of question entries
	unsigned	short ans_count;	// number of answer entries
	unsigned	short auth_count;	// number of authority entries
	unsigned	short add_count;	// number of resource entries
};

//Constant sized fields of query structure
struct QUESTION
{
	unsigned short qtype;
	unsigned short qclass;
};

//Constant sized fields of the resource record structure
struct  R_DATA
{
	unsigned short type;
	unsigned short _class;
	unsigned int   ttl;
	unsigned short data_len;
};

//Pointers to resource record contents
struct RES_RECORD
{
	unsigned char  *name;
	unsigned short type;
	unsigned short _class;
	unsigned int   ttl;
	unsigned short data_len;
	//struct R_DATA  *resource;
	unsigned char  *rdata;
};

//Structure of a Query
struct QUERY
{
	unsigned char *name;
	unsigned short qtype;
	unsigned short qclass;
	//struct QUESTION  *ques;
} ;

struct DNSPacket
{
	struct DNS_HEADER PktHdr;
	struct QUERY *QDList;
	struct RES_RECORD *ANList;
	struct RES_RECORD *NSList;
	struct RES_RECORD *ARList;

};

struct DNSTableEntry
{
	uint8 ClientMACAddr[6];
	char HostName[256];
	unsigned char Addr[4];
	int TTL;
};

class Ethernet;
class DNSServer: public Task
{
public:
	DNSServer(Ethernet * eth);
	~DNSServer();
protected:
	void Run();
	void Tick();
	void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host);
	bool DNS_hasEntry(unsigned char* QueryName);
	void AddAnswers(struct RES_RECORD *res_records ,unsigned char *Name,
			unsigned char* Addr );
	void ParsePacket(unsigned char * buf);
	unsigned char* ReadName(unsigned char* reader,unsigned char* buffer,int* count);
	void SendReply(unsigned char * QueryName);
	unsigned char* ParseHostNameOnly(unsigned char* QueryName,unsigned char* ParsedName);
	void SendErrorReply(unsigned char);
	unsigned short dns_get_short(uint8 *ptr);
	void Cleanup();
	struct DNSPacket *DNSPkt ;
	sockaddr_in client;
	int fd;
	struct QUERY *Query;
	Ethernet * netif;
	char *Rcvdata;
	char tempName[256];
};

#endif /*DNSSERVER_H_*/
