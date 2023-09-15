/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ModBusRcvTask.cpp_v   1.0   09 Jun 2015 09:10:30   tayars  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ModBusRcvTask.cpp_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:30   tayars
 * Initial revision.
 * 
 *    Rev 1.11   23 Jan 2015 15:13:30   ygupta
 * Added logic to count the missed packets from powerup.
 * 
 *    Rev 1.10   08 Oct 2013 06:17:34   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.9   03 Sep 2013 12:48:56   ygupta
 * Moved to tip from 1.7.1.5.1.0
 * 
 *    Rev 1.7.1.5.1.0   24 Aug 2013 17:14:04   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.7.1.5   02 Aug 2013 10:55:46   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.7.1.3   21 May 2013 12:25:54   amaurya
 * Code review fixes.
 * 
 *    Rev 1.7.1.2   07 May 2013 10:13:22   amaurya
 * Code review fixed.
 * 
 *    Rev 1.7.1.1   22 Apr 2013 11:30:58   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.7.1.0   20 Mar 2013 09:37:20   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.8   20 Mar 2013 08:17:34   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.7   15 Mar 2013 10:50:16   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.6   11 Mar 2013 02:26:38   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.5   18 Feb 2013 03:07:46   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   29 Nov 2012 08:45:22   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.3   26 Oct 2012 02:36:40   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   21 Sep 2012 01:32:30   rjamloki
 * Added State machine
 * 
 *    Rev 1.1   10 Sep 2012 03:12:16   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/

#include "ModBusRcvTask.h"
#include "Socket.h"
#include "GLOBALS.h"
#include "TYP_DEF.h"
#include "Socket.h"
#include "FUNC.h"
#include "TcpIp.h"
#include "PktBuf.h"

#define pprintf
#define printf

UINT32 TotalMissedPkt = 0;
UINT32 LastTID = 0;
Fifo<MBMessage> MBMsgQ(100, "mbmsg");
UINT8 CrashMBRcvPktbuf[MB_UDP_BUF_SIZE];
extern BOOLEAN SocketImplementation;
//to keep the track of The OD address where replied register values from DCP are to be written
Fifo<UINT16> MBwriteAddrLocal(10 , "WAddrLocal") ;
BOOLEAN ModBusErrorFlag = FALSE;
/*   Constructor.
 *   Purpose:
 *   	To initialize the task stack and member variables of class.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 * 		None
 */
ModBusRcvTask::ModBusRcvTask(SINT32 Intf):Task("Modbus", MODBUSTASK_STACKSIZE)
{
   Netif = CyGOS::GetNetworkInterface(Intf);
   //The listening socket descriptor default init.
   Fd = INVALID_SOCKET;
	if(SocketImplementation == FALSE)
		 Netif->SetCustomQueue(&MBMsgQ);
}

/*   void ModBusRcvTask::Run()
 *
 *   Purpose:
 *   	This function first checks for data on UDP port MB_UDP_DEFAULT_PORT
 *     and then call the ProcessModBusReply() function for processing the same.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void ModBusRcvTask::Run(void)
{
	//Some delay to make sure all driver components are initialized properly
	DelayMs(100);
	MBMessage MbMsg;
	BOOLEAN Running = FALSE;
	//socket address containing port, IP and family of the slave
	sockaddr_in SA;
	SINT32 SAlen = sizeof(SA), Len = 0, Flags = 0;
	ModBusErrorFlag = FALSE;
	for(;;){
		if(!Netif -> up){
			DelayMs(100);
			Running = FALSE;
			//UpdateTPVal(119);
		}
		else{
			if(!Running){
				//UpdateTPVal(120);
	          if(SocketImplementation == TRUE){
			   CloseSocket();
			   CreateAndBindSocket();
			   Block();
	          }
			   Running = TRUE;
		   }
	        if(SocketImplementation == TRUE)
	        {
		   		Len = recvfrom(Fd, MBRcvPkt.Query, sizeof(MBRcvPkt.Query), Flags, (sockaddr *)&SA, (int *)&SAlen);
			   if(Len < 0){
				  //For Blocking socket recvfrom function call should only return if some data is received.
				  CRASH("receive data error");
			   }
               else{
			  		ProcessModBusReply(Len);
               }
	        }
	        else{
               if(MBMsgQ.Read(this , MbMsg, 100)){
                  Len = MbMsg.packet->tot_len - sizeof(EthHeader) - EthCrcLen;
                  if(Len > MB_UDP_BUF_SIZE){
                	  Len = MB_UDP_BUF_SIZE;
                	 // UpdateTPVal(98);
                  }
                  memcpy(MBRcvPkt.Query, MbMsg.packet->payload + sizeof(EthHeader), Len);
                  memcpy(CrashMBRcvPktbuf, MbMsg.packet->payload + sizeof(EthHeader), Len);

                  pbuf_free(MbMsg.packet);
                  if((LastTID != MBRcvPkt.MBAdu.MBAPheader.TID - 1)&&(LastTID != 0)){
                	  //pprintf("\n Missed TID %d ",LastTID);
                	  TotalMissedPkt++;
                	  //Log::WriteEventLog(EVENT_CALIBRATION_PASS, LastTID,MBRcvPkt.MBAdu.MBAPheader.TID,TotalMissedPkt);

                  }
                  LastTID = MBRcvPkt.MBAdu.MBAPheader.TID;
                  Len = MBRcvPkt.MBAdu.MBAPheader.Length + MB_UDP_UID_OFFSET;
                  ProcessModBusReply(Len);
               }
	        }
		}//else
	}
}

/*  void ModBusRcvTask::ProcessModBusReply(SINT32 MBUDPLen)
 *
 *  Purpose:
 *    This function is used to process the Modbus Command Reply
 *    Function is called by Run() function.
 *
 *  Entry condition:
 *
 *	  MBUDPLen:The length of Modbus ADU
 *  Exit condition:
 *	  None.
 */
void ModBusRcvTask::ProcessModBusReply(SINT32 MBUDPLen)
{
	UINT16 PDURegsLength;//Total size of received registers in bytes
	MBException ECode = MB_EX_ILLEGAL_DATA_ADDRESS;
	UINT16 WriteAddress = 0;
	UINT8  ByteCount;
	UINT16 RegsNo = 0;
	if(PowerupCommunicationDone == TRUE){
	   while(MBwriteAddrLocal.Read(0, WriteAddress, 0));//empty queue
	   WriteAddress = MB_RT_RECEIVE_DATA_INDX;
	}
	else{
	  MBwriteAddrLocal.Read(0, WriteAddress, 0);
	}
	if(WriteAddress > 0){
      if(MBRcvPkt.MBAdu.MBAPheader.ProtocolID == MB_UDP_PROTOCOL_ID){	  // Indicates modbus TCP/UDP protocol
         if(MBRcvPkt.MBAdu.MBAPheader.SlaveId == DCPMBSLAVEADDRESS){
            if(MBRcvPkt.MBAdu.MBAPheader.FunctinCode == (MB_FUNC_READWRITE_MULTIPLE_REGISTERS | MB_FUNC_ERROR)){	// Check msb for error
               ECode = (MBException)MBRcvPkt.MBAdu.MBReadWriteMultiple.MBReply.Exception;	// Save Error code for log purpose
               // In case NRTModbus task is waiting for some reply. Notify the error
               ModBusSendTask::thisptr -> NotifyReadWriteEvent(NRT_READWRITE_ERROR);
            }
            else{
               if(MBRcvPkt.MBAdu.MBAPheader.FunctinCode == MB_FUNC_READWRITE_MULTIPLE_REGISTERS){
                  ByteCount = MBRcvPkt.MBAdu.MBReadWriteMultiple.MBReply.ByteCount;	// no. of bytes rcvd
                  PDURegsLength = MBUDPLen - MB_UDP_FUNC_OFFSET - MB_PDU_REGVALUES_OFFSET;	// Compute regs size in bytes
                  RegsNo = (ByteCount / HOLDING_REG_SIZE) + (ByteCount % HOLDING_REG_SIZE);	// Calculate no of registers

                  // Verify that we received the complete data
                  if(PDURegsLength == ByteCount){
                     if(MBRegHoldingCB(0, WriteAddress, RegsNo, MB_REG_WRITE, &MBRcvPkt.Query[MB_UDP_FUNC_OFFSET + MB_PDU_REGVALUES_OFFSET],
                    		 MB_UDP_BUF_SIZE - (MB_UDP_FUNC_OFFSET + MB_PDU_REGVALUES_OFFSET)) == MB_ENOERR)
                        ECode = MB_EX_NONE;
                  }
                  else
                     ECode = MB_EX_ILLEGAL_DATA_VALUE;
                  // Any data written to OD should be notified to NRT Task in case it is waiting
                  ModBusSendTask::thisptr -> NotifyReadWriteEvent(ByteCount - sizeof(ModBusRTReceiveData));
               }
               else
                  ECode = MB_EX_ILLEGAL_FUNCTION;
            }
         }
      }
      if(ECode != MB_EX_NONE){
         //Log::WriteEventLog(EVENT_MB_ENOREG, ECode, 0);
         ModBusErrorFlag = TRUE;
      }
      else{
    	 ModBusErrorFlag = FALSE;
         StoreWeldData();
         if((RtSendData.CmdFlags & MB_HSCAN) == MB_HSCAN)
            CollectScanPoints();
         if(((RtReceiveData.StatusFlags & SIGNAL_EVENTCLEARED) == SIGNAL_EVENTCLEARED)
        	&& ((RtSendData.CmdFlags & MB_CLEAREVENTLOG) == MB_CLEAREVENTLOG)){
        	 RtSendData.CmdFlags &= ~MB_CLEAREVENTLOG;        	
         }
      }
	}
}

/*   void ModBusRcvTask::CreateAndBindSocket()
 *
 *   Purpose:
 *   	This function create and bind a UDP socket on UDP port MB_UDP_DEFAULT_PORT.
 *  	This function is called by ModBusRcvTask::Run() function.
 *
 *   Entry condition:
 *		None
 *
 *   Exit condition:
 *		None.
 */
void ModBusRcvTask::CreateAndBindSocket(void)
{
   Fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
   ASSERT(Fd != INVALID_SOCKET);
   //Socket address to be bind with socket descriptor
   sockaddr_in SA;
   //socket family
   SA.sin_family = AF_INET;
   //this becomes the listening port for receiving MB packet
   SA.sin_port = htons(MB_UDP_DEFAULT_PORT);
   //IP on which MB packet will be received
   SA.sin_addr.s_addr = Netif -> ipaddr.addr;
   if(bind(Fd, (sockaddr *) &SA, sizeof(SA)) < 0)
      ASSERT(0);
}

/*  void ModBusRcvTask::CloseSocket()
 *
 *  Purpose:
 *   	This function closes the socket initially binded at 502 UDP port.
 *   	(It is used in case connection is lost i.e network cable is unplugged).
 *   	This function is called by Run() function:
 *
 *  Entry condition:
 *		None
 *
 *  Exit condition:
 *		None.
 */
void ModBusRcvTask::CloseSocket(void)
{
   if(Fd != INVALID_SOCKET){
      lwip_close(Fd);
      Fd = INVALID_SOCKET;
   }
}

/*  void ModBusRcvTask::Block()
 *
 *   Purpose:
 *   This function makes the socket binded at MB_UDP_DEFAULT_PORT blocking.
 *   This function is called by Run functions once the
 *    Ethernet link goes up
 *
 *   Entry condition:
 *		None
 *
 *   Exit condition:
 *		None.
 */
void ModBusRcvTask::Block(void)
{
   SINT32 Val = 0;
   if(lwip_ioctl(Fd, FIONBIO, &Val) < 0)
	   CRASH("socket blocking failed");
}
