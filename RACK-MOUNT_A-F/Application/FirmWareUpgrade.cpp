/*
 $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FirmWareUpgrade.cpp_v   1.2   02 Jul 2015 10:45:06   RJamloki  $
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
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FirmWareUpgrade.cpp_v  $
 * 
 *    Rev 1.2   02 Jul 2015 10:45:06   RJamloki
 * Reverted to initial revision as we don’t need to support firmware file checksums at end
 * 
 *    Rev 1.1   25 Jun 2015 05:43:14   rjamloki
 * SendFileToNode and SendFileToLCDA function calls are modified to take programming data start offset into account.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.15   06 Nov 2013 02:49:44   rjamloki
 * Added LCD Flash verification.
 * 
 *    Rev 1.14   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.13   20 Sep 2013 07:00:12   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.12   14 Sep 2013 07:30:36   rjamloki
 * WatchDog Cant be disabled.
 * 
 *    Rev 1.11   03 Sep 2013 12:36:40   ygupta
 * Moved to tip from 1.9.1.6.1.0
 * 
 *    Rev 1.9.1.6.1.0   24 Aug 2013 17:14:00   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.9.1.6   02 Aug 2013 10:55:44   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.9.1.4   14 Jun 2013 11:21:06   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.9.1.3   05 Jun 2013 03:09:20   amaurya
 * Fixed Passcode and Limits issue.
 * 
 *    Rev 1.9.1.1   08 Apr 2013 08:22:58   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.9.1.0   20 Mar 2013 09:37:04   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.10   20 Mar 2013 08:17:22   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.9   15 Mar 2013 10:48:16   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.8   11 Mar 2013 02:24:58   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.7   18 Feb 2013 03:05:54   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.6   03 Jan 2013 07:17:58   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.5   13 Dec 2012 00:08:52   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.4   26 Oct 2012 02:34:20   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   10 Sep 2012 03:11:34   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.2   05 Jul 2012 04:45:02   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.1   02 Jul 2012 13:04:06   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.0   21 Mar 2012 12:20:24   PDwivedi
 * Initial revision.
 */

#include "Socket.h"
#include "FirmWareUpgrade.h"
#include <stdio.h>
#include "IPv4.h"
#include "LOG.h"
#include "portable.h"
#include "FUNC.h"
#include "UserIO.h"
#include "GLOBALS.h"
#include "BDMTask.h"
#include "CPU.h"

#define UPGRADEFAILMSG 		 "Upgrade failed"
#define UPGRADEINPROGRESSMSG "Upgrade in progress"
#define UPGRADECOMPLETEMSG 	 "Upgrade completed"
#define UPGRADEALREADYINPROGRESS "An Upgrade already in progress"

// static variable definition
// This is the global pointer to class. UploadFirmware module uses this pointer to call
// the FUWriteQueue() function of this Task while sending binary file.
 FirmWareUpgradeTask * FirmWareUpgradeTask::thisPtr;
BOOLEAN FirmwareFileSent = FALSE;

/*  FirmWareUpgradeTask::FirmWareUpgradeTask()
 *
 *   Purpose
 *  	constructor for FirmwareUpgradeTask called When FirmwareUpgradeTask is created.
 *		This function calls the constructor of base class Task to which the name of the task and stack size are passed as parameter.
 *  	It also initializes the class objects declared in this class which don’t have default constructor
 *  	(in this case fifo class object queue requires size and name
 *
 *   Entry condition
 *
 *   Exit condition
 *   	NOne.
 */
FirmWareUpgradeTask::FirmWareUpgradeTask(int Intf)
					: Task("FMUpgrade", FIRMWAREUPGRADETASK_STACKSIZE), Queue(1, "FirmWareUpgrade")
{
	UpLoadfile = NULL;
	thisPtr = this;
	NetIf = CyGOS::GetNetworkInterface(Intf);
	memset(&ProgInfo, 0, sizeof(UpgradeProgress));
	FUClientSocket = 0;
}


/*   SINT32 FirmWareUpgradeTask::SendFileToNode (ip_addr nodeip, UINT16 port, UINT8 * fileDataStart, SINT32 length)
 *
 *   Purpose
 *  	This function sets up TCP connection to DCP board and sends the file to DCP in form  of TCP packets.
 *    	This function is called from firmwareupgradetask Run()function after the file is received from UploadFirmware task.
 *
 *   Entry condition
 *  	nodeip- IP of the node where firmware file to be sent. DCP in this case.
 *  	port: The TCP port on node side where the TCP connection shall be established.
 *  	fileDataStart: The pointer to the file data
 *  	length: The size of file data in bytes
 *
 *   Exit condition
 *  	returns SUCCESS_FILESENT_DCP if success else return error code.
 */
SINT32 FirmWareUpgradeTask::SendFileToNode (ip_addr Nodeip, UINT16 Port, UINT8 * FileDataStart, SINT32 Length, FUPacketType PktType)
{
	int RetValFunc = SUCCESS_FILESEND_DCP;
	int UpgardePacketSize = FIRMWAREPKTSIZE_DCP;
	FUPacket Cpacket; // user packet for confirmation.
	BOOLEAN ErrorFlag = FALSE;
	SINT32 FileSize;
	SINT32 RetVal;
	sockaddr_in ClientService;
	FileDataStart += 4;
	//1. Connect to the node and ask for confirmation.
	ClientService.sin_family = AF_INET;
	ClientService.sin_addr.s_addr = Nodeip.addr;
	ClientService.sin_port = htons(Port);
	FirmwareFileSent = TRUE;
	AddToNetIfList(NetIf);

	if(lwip_connect(FUClientSocket, (sockaddr *) &ClientService, sizeof(ClientService)) < 0){
		RetValFunc = RetVal = ERR_FAILEDDCPCONNECTION;
	}
	else{
		//To make sure website doesn't time out
		SendFirmwareUpgradeProgressToWebsite(UPGRADEINPROGRESSMSG);
		FileSize = Length;
		Cpacket.Pkttype = (FUPacketType)htonl(PktType);
		Cpacket.FileLength = htonl(FileSize);

		if(send(FUClientSocket,(const void*)&Cpacket, sizeof(FUPacket), 0) < 0){
		   RetValFunc = ERR_UPGRADESTARTCOMMANDSEND;
		}
		else{
			recv(FUClientSocket, &Cpacket, sizeof(FUPacket), 0);
			if(ntohl(Cpacket.Pkttype) != UpgradeConfirm){
				RetValFunc = ERR_UPGRADESTARTREJECT;
			}
		   // Now we got confirmation from DCP. Start sending the
			//file in TCP packets
			else{
				while((Length > 0) && (ErrorFlag == FALSE)){
					CyGOS::KickWatchDog();
					if(send (FUClientSocket, FileDataStart, UpgardePacketSize, 0) < 0){
			        	RetValFunc =  ERR_UPGRADEPACKETSEND;
			        	ErrorFlag = TRUE;
			 	    }
					else{
						// Advance fileDataStart to upgardePacketSize bytes.
						FileDataStart += UpgardePacketSize;
						// Decrement length by upgardePacketSize
						Length -= UpgardePacketSize;
						if(Length <  FIRMWAREPKTSIZE_DCP){
							UpgardePacketSize = Length;
			  			}
					}//else
			    }//while((length > 0) && ( errorFlag == FALSE))

			    if(ErrorFlag == FALSE){
			       recv(FUClientSocket, &Cpacket, sizeof(FUPacket), 0);
			       if(Cpacket.Pkttype !=  UpgradeFileRecCompleted){
			    	   RetValFunc = ERR_FIRMWAREFILERECEIVEDCONFIRMATION;
			       }
			   }//if(errorFlag == FALSE)
		   }//else
		}//else
	}//else
	RemoveFromNetIfList(NetIf);
	//TODO:
	//if(Cpacket.Pkttype == UpgradeFileRecCompleted)
	//wait for bit telling upgrade completed in real time data extraflags to become true
	//and soft reset the CPU
	return RetValFunc;
}


/*  SINT32 FirmWareUpgradeTask::SendFileToLCDA(UINT8 * FileDataStart, SINT32 Length)
 *
 *  Purpose
 *  	This function is used to send file to LCDA using BDM bit bang.
 *
 *  Entry condition
 *  	FileDataStart- The pointer to file data.
 *  	Length – The length of file data
 *
 *  Exit condition
 *  	returns SUCCESS_FILESENT_LCDA if  success else return ERROR code.
 */
SINT32 FirmWareUpgradeTask::SendFileToLCDA(UINT8 * FileDataStart, SINT32 Length)
{
	SINT32  RetVal = SUCCESS_FILESEND_LCDA;
	//Seup bit bang and send file
//	CyGOS::DisableWatchDog();
	CyGOS::Stop();
	CyGOS::KickWatchDog();
	pprintf("\nStart Upload");
	BdmTest();
	if(BDMExitFlag == FALSE)
	{
		*(FileDataStart + 0x7FAE) = 0x00;
		*(FileDataStart + 0x7FAF) = 0x98;
		*(FileDataStart + 0x7FBF) = 0xFE;
		//*(FileDataStart + 0x7FAE) = 0xAE;
		//*(FileDataStart + 0x7FAF) = 0xE6;

		BdmProgram(FileDataStart,Length);

		if(BDMExitFlag == FALSE) {
			pprintf("\nProgramming Done\n");
			//BdmTest();
			pprintf("\n************** SUCCESSFULLY PROGRAMMED *****************\n");
			Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FILESEND_LCDA);
		}
	}
	if(BDMExitFlag == TRUE) {
		pprintf("\n************** LCD PROGRAM FAILED PLEASE POWER UP THE LCD BOARD *****************\n");
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_FILESEND_LCDA);
	}
	CPU::Restart();
	return 	RetVal;
}

/*  void FirmWareUpgradeTask::FUWriteQueue(const FirmwareUpgradeCommand &Cmd)
 *
 *   Purpose
 *  	This function is used to pass the firmware file information received in cmd argument to FirmwareUpgarde task fifo.
 *  	This function  is called from SendDCPdata() and SendLCDAdata() functions of UploadFirmware() task.
 *
 *   Entry condition
 *  	Cmd- Object contains information (i.e. file Data pointer, file length in bytes, TCP *server port information, TCP server IP, binFileType) which nneds to be passed to
 *  	FirmwareUpgradeCommand queue of this class.
 *
 *   Exit condition
 *		None.
 */
void FirmWareUpgradeTask::FUWriteQueue(const FirmwareUpgradeCommand &Cmd)
{
	 printf(" received firmware upgrade command");
	 if(!Queue.Write(Cmd, 0))
		 Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_ANOTHERUPGRADEINPROGRESS);

	 SendFirmwareUpgradeProgressToWebsite(UPGRADEALREADYINPROGRESS);
}


/*  void FirmWareUpgradeTask::SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog)
 *
 *   Purpose
 *  	This function shall send the firmware upgrade progress information to the website.
 *  	Currently this function does nothing.File sending to DCP and LCDA may take some
 *		mean while the webpage should dispaly the progress of file sending.
 *
 *   Entry condition
 *  	prog-  The current progress information string which need to send to website
 *
 *   Exit condition
 *		None.
 */
void FirmWareUpgradeTask::SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog)
{
	Prog = 0;
}


/*  BOOLEAN FirmWareUpgradeTask::CreateUpgradeSocket()
 *
 *   Purpose
 *  	This function creates a TCP socket to be used for file transfer to DCP.
 *  	This function  is called  once from Run() function ,every time
 *  	the file transfer to DCP is required.
 *
 *   Entry condition
 *		None.
 *
 *   Exit condition
 *  	returns Boolean value whether a socket is created or not
 */
BOOLEAN FirmWareUpgradeTask::CreateUpgradeSocket()
{
	BOOLEAN RetVal = TRUE;
	SINT32 Error;
   sockaddr_in Sa;
	FUClientSocket = lwip_socket(AF_INET, SOCK_STREAM, PF_INET);
	if(FUClientSocket < 0)
		RetVal = FALSE;

   Sa.sin_family = AF_INET;
   Sa.sin_addr.S_un.S_addr = NetIf->ipaddr.addr;
   Sa.sin_port = htons(FIRMWAREUPGRADE_SRCPORT);
   Error = lwip_bind(FUClientSocket, (sockaddr *) &Sa, sizeof(Sa));
   if(Error == SOCKET_ERROR){
      printf(" \n fupgrade socet bind error");
      RetVal = FALSE;
   }

	return RetVal;
}


/*   void FirmWareUpgradeTask::Run()
 *
 *   Purpose
 *   	This function waits infinitely for firmwareupde message. On receiving
 *   	message it sends the file to either DCP or LCDA depends on message type.
 *
 *   Entry condition
 *		None.
 *
 *   Exit condition
 *   	None.
 */
void FirmWareUpgradeTask::Run(void)
{
	FirmwareUpgradeCommand Cmd;
	FUPacketType PktTypeDCP;
	int Sent;
	for(;;){
		Queue.Read(Cmd, -1);
		UpLoadfile = Cmd.FileData;
		//CyGOS::StopTimer(1);
		switch(Cmd.Type){
			case FileDCPAppBin:
			case FileDCPCompleteBin:
			   pprintf(" rec fileDCP1");
				if(CreateUpgradeSocket() == FALSE){
					Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_CREATESOCKETDCP);
					SendFirmwareUpgradeProgressToWebsite(UPGRADEFAILMSG);
					pprintf(" rec fileDCP2");
				}
				else{
				   pprintf(" rec fileDCP3");
				   PktTypeDCP = Zero;
				   if(Cmd.Type == FileDCPAppBin)
					   PktTypeDCP = UpgradeStartAppBin;
				   else if(Cmd.Type == FileDCPCompleteBin)
					   PktTypeDCP = UpgradeStartCompleteBin;

				  Sent = SendFileToNode(Cmd.NodeIP, Cmd.ServerPort, Cmd.FileData, Cmd.FileLength, PktTypeDCP);
				  pprintf(" rec fileDCP3 %d", Sent);
				  if(Sent == SUCCESS_FILESEND_DCP){
					  SendFirmwareUpgradeProgressToWebsite(UPGRADECOMPLETEMSG);
				  }
				  else
					  SendFirmwareUpgradeProgressToWebsite(UPGRADEFAILMSG);
				  Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, Sent);
				}
				lwip_close(FUClientSocket);
			break;
			case FileLCDA:
				Sent = SendFileToLCDA(Cmd.FileData, Cmd.FileLength);
				if(Sent == SUCCESS_FILESEND_LCDA)
					SendFirmwareUpgradeProgressToWebsite(UPGRADECOMPLETEMSG);
				else
					SendFirmwareUpgradeProgressToWebsite(UPGRADEFAILMSG);

			//	Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, Sent);
			break;
			default:
			break;
		}//switch(Cmd.Type)
		if(UpLoadfile){
			delete UpLoadfile;
			UpLoadfile = 0;
		}
		//CyGOS::StartTimer(1);
	}//for(;;)
}
