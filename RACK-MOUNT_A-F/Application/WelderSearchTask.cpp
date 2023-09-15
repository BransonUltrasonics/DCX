/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WelderSearchTask.cpp_v   1.1   21 Aug 2015 05:52:48   RJamloki  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WelderSearchTask.cpp_v  $
 * 
 *    Rev 1.1   21 Aug 2015 05:52:48   RJamloki
 * Fix for Coverity CID 11230.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:44   tayars
 * Initial revision.
 * 
 *    Rev 1.11   08 Oct 2013 06:17:18   rjamloki
 * Fixed warnings.
 * 
*/

#include "WelderSearchTask.h"
#include "Socket.h"
#include "errno.h"
#include "stdio.h"
#include "trace.h"
#include "CyGOS.h"
#include "GLOBALS.h"
#include "TYP_DEF.h"
#include "DcxManagerHandler.h"
#include "UserIO.h"
#include"GLOBALS.h"
#include "FUNC.h"

BOOLEAN ClearJSONObjects = FALSE;
extern ip_addr ConnectedDcxmAddr;

/*  WelderSearchTask :: WelderSearchTask
 *
 *  Purpose :
 *  	This is the constructor of this class.To initialize the WelderSearchTask object.
 *
 *   Entry Condition :
 *  	intf- Ethernet interface this task is running on.
 *
 *   Exit Condition :
 *   	None.
 */
WelderSearchTask :: WelderSearchTask(SINT32 Intf):Task("welder Search", WELDERSEARCHTASK_STACKSIZE)
{
	Netif = CyGOS::GetNetworkInterface(Intf);
	Fd = INVALID_SOCKET;
	DcxMIP.addr = 0;
	DcxmTimeOut = 0;
}

/*This function first checks for data on UDP port WELDERSEARCH_UDP_PORT.
 * and process it accordingly. It then processes the msgQ.
 */
void WelderSearchTask::Run(void)
{
	DelayMs(2000);
	sockaddr_in SA;
	bool Running = false;
	int SAlen = sizeof(SA), Len = 0, Flags = 0;
	WelderInfoParam Msg;
	for(;;){
		if(!Netif -> up){
			DelayMs(100);
			Running = false;
		}
		else{
			if(!Running){
				CloseSocket();
				CreateAndBindSocket();
				NonBlock();
				Running = true;
				strncpy(IpAddr, Netif->ipaddr.ToString(), sizeof(IpAddr) - 1);//Coverity CID: 11230
			}
			if(ClearJSONObjects == TRUE){
				//printf("\n Clear Json Objects");
				ClearJsonObjects();
				ClearJSONObjects = FALSE;
			}
			PrepareFDSets();
			Poll();

			if(FD_ISSET(Fd, &InputSet) != 0){
			}
			if(FD_ISSET(Fd, &ExcSet) != 0){
			}
			if((FD_ISSET(Fd, &InputSet) != 0) && (FD_ISSET(Fd, &ExcSet) == 0)){
				memset(&Msg.WelderInfo, 0, sizeof(Msg.WelderInfo));
				Len = recvfrom(Fd, &Msg.WelderInfo, sizeof(Msg.WelderInfo), Flags, (sockaddr *)&SA, &SAlen);
				if(Len < 0){
				}
				else{
					//pprintf("\n pkt type %s ", Msg.WelderInfo.PktID );
				   if(!strcmp(Msg.WelderInfo.PktID , DCXMANAGERDISCOVERY_PACKET)){
						if((SA.sin_addr.S_un.S_addr == DcxMIP.addr) || (DcxMIP.addr == 0)){
							if(SendDcxMdata(SA.sin_addr.S_un.S_addr) > 0){
								if(DcxMIP.addr == 0){
									DcxMIP.addr = SA.sin_addr.S_un.S_addr;
									ConnectedDcxmAddr = DcxMIP;
									printf("Connected to DCXM %s", DcxMIP.ToString());
								}
								DcxmTimeOut = 0;
							}
						}
					}
					else if(!strcmp(Msg.WelderInfo.PktID , DCXMANAGERLOGOUT_PACKET)){
						if(SA.sin_addr.S_un.S_addr == DcxMIP.addr) {
							   DcxMIP.addr = 0;
							   CycleDataSend(DcxMIP);
							   ClearJSONObjects = TRUE;
							   EnableJSONLogging = FALSE;
						  }
					}
					else {
							//pprintf("\n discard dcxM %08X", SA.sin_addr.S_un.S_addr);
					}
				}//else
			}//if((FD_ISSET(fd, &input_set) != 0) && (FD_ISSET(fd, &exc_set) == 0)){
		}//else
	}//for(;;)
}

/*
 * This function create and bind a UDP socket
 * on port WELDERSEARCH_UDP_PORT.
 */
void WelderSearchTask::CreateAndBindSocket(void)
{
	Fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(Fd != INVALID_SOCKET);
	sockaddr_in SA;
	SA.sin_family = AF_INET;
	SA.sin_port = htons(WELDERSEARCH_UDP_PORT);
	SA.sin_addr.s_addr = Netif -> ipaddr.addr;
	if (bind(Fd, (sockaddr *)&SA, sizeof(SA)) < 0)
		CRASH("Bind welderSearch server socket");
}

/*
 * This fuction is called from Run() in case the link is lost
 * (i.e network cable is unplugged)
 * It closes the socket initially binded at 49500 UDP port
 */
void WelderSearchTask::CloseSocket(void)
{
	if(Fd != INVALID_SOCKET){
		lwip_close(Fd);
		Fd = INVALID_SOCKET;
	}
}


/*   SINT32 WelderSearchTask :: NonBlock (void)
 *
 *   Purpose :
 *   	To make the binded socket non-blocking
 *  	This function is called once by Run() function after binding the socket.
 *
 *   Entry Condition :
 *   	None.
 *
 *   Exit Condition :
 *   	None.
 */
SINT32 WelderSearchTask :: NonBlock (void)
{
	unsigned long Val = 1;
	return lwip_ioctl(Fd, FIONBIO, &Val);
}


/*
 * Prepare input and error FD sets to be used in socket
 * recvfrom function.
 */
void WelderSearchTask::PrepareFDSets(void)
{
	FD_ZERO(&InputSet);
	FD_ZERO(&ExcSet);
	FD_SET(Fd, &InputSet);
}

/*
 * This function waits for something to receive on port 49500
 * for WELDERSEARCHMSG_SOCKET_TIMEOUT uSecs. It returns true
 * if select socket API executes without any error otherwise returns
 * false.
 */
bool WelderSearchTask::Poll(void)
{
	timeval Tval;
	Tval.tv_sec = 0;
	Tval.tv_usec = WELDERSEARCHMSG_SOCKET_TIMEOUT;
	if(lwip_select(Fd + 1, &InputSet, 0, &ExcSet, &Tval) < 0){
		return false;
	}
	return true;
}


int WelderSearchTask :: SendDcxMdata(UINT32 DestAddr)
{
	sockaddr_in SA;
	BYTE MacAddr[MAC_ADDRESS_LEN];
	SINT8 Type[MAX_DCXTYPE_LEN];
	memset(&Dcxmdata, 0, sizeof(DcxMData));
	if(MacchipData.CurrSystemType == SYSTEM_ADVANCED)
	 strcpy(Type, "DCX-A");
	else if(MacchipData.CurrSystemType == SYSTEM_FIELDBUS)
	 strcpy(Type, "DCX-F");
	else if(MacchipData.CurrSystemType == SYSTEM_ADVANCED_HD)
	 strcpy(Type, "DCX-AHD");
	else if(MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD)
	 strcpy(Type, "DCX-FHD");
	else
	 strcpy(Type, "DCX UNKNOWN");

	memcpy(MacAddr,  Netif->GetHardwareAddress(), MAC_ADDRESS_LEN);

	sprintf(Dcxmdata.PktData,"%s,%02X%02X%02X%02X%02X%02X,%s,%s,%d,%d,%d,%lu,%s"
		, DCXMANAGERDISCOVERY_PACKET,MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],
		MacAddr[4],MacAddr[5],MacchipData.SerialNo,Type,(int)MacchipData.Freq, (int)MacchipData.Power,MAXDCPADCRAWVALUE,Midband / FREQ_SCALING_FACTOR,
		IpAddr);
	SA.sin_family = AF_INET;
	SA.sin_port = htons(WELDERSEARCH_UDP_PORT);
	SA.sin_addr.s_addr = DestAddr;//need to broadcast the packet
	//sendto(Fd, &Dcxmdata, sizeof(DcxMData), 0, (sockaddr *)&SA, sizeof(SA));
	//Include null in response by adding 1
	return sendto(Fd, Dcxmdata.PktData, strlen(Dcxmdata.PktData) + 1, 0, (sockaddr *)&SA, sizeof(SA));
}

/*
 * This function is called on every RTOS tick.
 * On every WELDERINFO_BROADCAST_INTERVAL it marks the sending of welder
 * information broadcast by writing into msgQ. On every WELDERINFOREFRESH_TIMEOUT
 * interval it marks to refresh the welderInfoList by writing into msgQ.
 */
void WelderSearchTask::Tick(void)
{
	DcxmTimeOut += usecPerTick;
	JSonServiceTimeOut += usecPerTick;
	if((DcxMIP.addr > 0) && (DcxmTimeOut >= DCXM_TIMEOUT)){
		DcxMIP.addr = 0;
		ClearJSONObjects = TRUE;
		EnableJSONLogging = FALSE;
		CycleDataSend(DcxMIP);
	}
	if((DcxMIP.addr > 0) && (JSonServiceTimeOut >= DCXM_TIMEOUT) && (EnableJSONLogging == TRUE)){
		ClearJSONObjects = TRUE;
		EnableJSONLogging = FALSE;
	}
}


