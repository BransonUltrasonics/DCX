/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastFT.cpp_v   1.2   25 Jul 2016 11:54:32   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastFT.cpp_v  $
 * 
 *    Rev 1.2   25 Jul 2016 11:54:32   GButron
 * Run function changed to avoid making assignments within a conditional expression. Break removed from while loop
 * 
 *    Rev 1.1   07 Jun 2016 11:31:54   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   03 Jun 2016 13:27:18   GButron
 * Initial revision.
*/

#include "BroadcastFT.h"
#include "Socket.h"
#include "Board.h"
#include "Ethernet.h"

#define LENGTHOFTHEQUEUE                       1

/*  BroadcastFT::BroadcastFT(UINT8 Eth):Task("BROADCASTFT", BROADCAST_FT_STACKSIZE)
 *
 *  Purpose:
 *   	This is the constructor of BroadcastFT class.
 *
 *  Entry condition:
 *		Eth - Ethernet interface the task is running on.
 *
 *  Exit condition:
 *  	None.
 */
BroadcastFT::BroadcastFT(UINT8 Eth):Task("BROADCASTFT", BROADCAST_FT_STACKSIZE)
{
	// TODO Auto-generated constructor stub
    this->Eth = Eth;
}


/*  BroadcastFT::~BroadcastFT()
 *
 *  Purpose:
 *   	This is the destructor of BroadcastFT class.
 *
 *  Entry condition:
 *		None.
 *
 *  Exit condition:
 *  	None.
 */
BroadcastFT::~BroadcastFT()
{
	// TODO Auto-generated destructor stub
}


/*  void BroadcastFT::Run(void)
 *
 *  Purpose:
 *		  This function implements the forever loop of FirmwareUpgrade Task. This function
 *      accepts incoming connections at port 7075 to receive the firmware file. Checks
 *      the file size against the space available in Flash. Writes event logs in case of
 *      some error. Finally call the function to program the flash.
 *
 *  Entry condition:None
 *
 *  Exit condition:None.
 */
void BroadcastFT::Run(void)
{
	BOOLEAN ControllingLoopFlag = TRUE;
	BOOLEAN ExitFlag = FALSE;
	sockaddr_in ServerAddr, ClientAddr;
	SINT32 Sockfeed = 0;
	SINT32 ClientAddrSize = sizeof(ClientAddr);
	STR BufferStr[BROADCAST_FT_BUFFERSIZE] = {0};
	SINT32 BroadcastPermission = 1;

	UtilsFT::Clean(BufferStr);
	Sockfeed = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(Sockfeed < 0){
	   printf("socket() failed");
	   ExitFlag = TRUE;
   }
	if(ExitFlag==FALSE) {
	   if (setsockopt(Sockfeed,SOL_SOCKET,SO_BROADCAST,(void*)&BroadcastPermission,sizeof(BroadcastPermission)) < 0) {
	      printf("setsockopt() failed");
	      ExitFlag = TRUE;
	   }
	}
	if(ExitFlag==FALSE) {
	   memset(&ServerAddr,0,sizeof(ServerAddr));
	   ServerAddr.sin_family = AF_INET;
	   ServerAddr.sin_port = BROADCAST_FT_PORT;
	   ServerAddr.sin_addr.S_un.S_addr=Board::eth[this->Eth]->ipaddr.addr; //NetIfPtr->ipaddr.addr;
	   bind(Sockfeed,(sockaddr*)&ServerAddr,sizeof(ServerAddr));
	   listen(Sockfeed,LENGTHOFTHEQUEUE);
	   while(ControllingLoopFlag == TRUE) {
	      recvfrom(Sockfeed,BufferStr,BROADCAST_FT_BUFFERSIZE, 0,(sockaddr*)&ClientAddr,&ClientAddrSize);
	      if (sendto(Sockfeed,BufferStr,BROADCAST_FT_BUFFERSIZE,0,(sockaddr*)&ClientAddr,ClientAddrSize) < 0) {
	      	ControllingLoopFlag = FALSE;
	      	pprintf("sendto() failed");
	      }
	   }
	}
	closesocket(Sockfeed);
}
