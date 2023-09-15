/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ModBusSendTask.cpp_v   1.19   25 Oct 2013 11:59:14   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ModBusSendTask.cpp_v  $
 * 
 *    Rev 1.19   25 Oct 2013 11:59:14   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.18   08 Oct 2013 06:17:34   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.17   03 Sep 2013 12:50:06   ygupta
 * Moved to tip from 1.15.1.8.1.0
 * 
 *    Rev 1.15.1.8.1.0   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.15.1.8   02 Aug 2013 10:55:46   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.15.1.5   10 Jul 2013 07:51:40   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.15.1.4   31 May 2013 10:37:16   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.15.1.3   21 May 2013 12:25:54   amaurya
 * Code review fixes.
 * 
 *    Rev 1.15.1.2   06 May 2013 09:17:28   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.15.1.1   08 Apr 2013 08:24:00   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.15.1.0   20 Mar 2013 09:37:34   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.16   20 Mar 2013 08:17:50   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.15   15 Mar 2013 10:50:30   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.14   11 Mar 2013 02:26:50   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.13   18 Feb 2013 03:08:00   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.12   11 Jan 2013 02:52:04   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.11   03 Jan 2013 07:18:28   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.10   13 Dec 2012 00:10:40   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.9   29 Nov 2012 08:45:38   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.8   24 Nov 2012 08:02:20   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.7   05 Nov 2012 08:57:22   amaurya
 * State Machine changes
 * 
 *    Rev 1.6   29 Oct 2012 02:55:16   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.5   26 Oct 2012 02:36:52   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.4   21 Sep 2012 01:32:44   rjamloki
 * Added State machine
 * 
 *    Rev 1.3   11 Sep 2012 10:13:40   rjamloki
 * Fixed the typo and comments
 * 
 *    Rev 1.2   10 Sep 2012 03:12:20   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.1   29 Aug 2012 13:28:00   rjamloki
 * WC Modbus checkin and related changes
*/


#include "ModBusSendTask.h"
#include "ModBusRcvTask.h"
#include "trace.h"
#include "CyGOS.h"
#include "GLOBALS.h"
#include "TYP_DEF.h"
#include "Socket.h"
#include "RTOS.h"
#include "ObjDictionary.h"
#include "preset.h"
#include "RTClock.h"
#include "FUNC.h"
#include "PktBuf.h"
#include "ARP.h"
BOOLEAN SocketImplementation = FALSE;

DCPPowerUpData  PowerUpData;
ModbusAck MBack;
TimeStamp SendRtc;
ModBusRTSendData RtSendData;
ModBusRTReceiveData RtReceiveData;

#define pprintf
#define printf

//Table of holding registers.
MODBusODEntry MODtable[] =
{
   /*Index					      Count					      Variable address  Precallback 	  Postcallback */
   /*-----		   			   -----					      ----------------  ---------  	      ----------   */
   {MB_POWER_UP_DATA_INDX,    POWERUPDATA_REGCOUNT,	&PowerUpData,		NULL,             NULL , sizeof(PowerUpData)},
   {MB_RT_SEND_DATA_INDX, 	   RTSENDDATA_REGCOUNT,    &RtSendData,		NULL,			  NULL,sizeof(RtSendData)},
   {MB_NRT_SEND_DATA_INDX,    0, 					      NULL,	    		NULL,  			  NULL,0},
   {MB_RT_RECEIVE_DATA_INDX,  RTRECEIVEDATA_REGCOUNT, &RtReceiveData,   NULL,  		      NULL,sizeof(RtReceiveData)},
   {MB_NRT_RECEIVE_DATA_INDX, 0, 					  NULL,       		NULL,  			  NULL,0},
   {MB_BASE_TIME_INDX, 		   BASETIME_REGCOUNT,      &SendRtc, 		NULL,  			  NULL,sizeof(SendRtc)},
   {MB_PRESET_INDX, 		      PRESET_REGCOUNT,        &DcpParameterSet, NULL,             NULL,sizeof(DcpParameterSet)},
   {MB_ACK_INDX, 			      ACK_REGCOUNT,           &MBack,			NULL,          	  NULL,sizeof(MBack)},
   {MB_MACCHIP_INDX, 		   MACCHIPDATA_REGCOUNT,   &MacchipData,   	NULL,		      NULL,sizeof(MacchipData)},
   {MB_DCPTIMEPARA_INDX, 	DCPTIMEPARAM_REGCOUNT, 	  &DCPTimeControlParam, 		NULL, NULL,sizeof(DCPTimeControlParam)},
   {0,         				   0,                      NULL,				NULL,	          NULL,0}
};

ObjDictionary OdHoldingRegisters(MODtable);
ModBusSendTask * ModBusSendTask::thisptr;
//Flag becomes true after powerup communication is done
BOOLEAN PowerupCommunicationDone = FALSE;

/*   Constructor.
 *   Purpose:
 *   	To initialize the task stack, fifo objects and
 *		member variables of class.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 * 		None
 */
ModBusSendTask::ModBusSendTask(SINT32 Intf):Task("ModRealBusSend", MODBUSTASK_STACKSIZE),
					ModBusRealTimeQ(MBRTQ_SIZE, "ModRealQ"), ReqQueue(REQ_QUEUESIZE, "rxq")
{
   Netif = CyGOS::GetNetworkInterface(Intf);
   //Socket descriptor to be uses to send Modbus packet
   Fd = INVALID_SOCKET;
   thisptr = this;
   memset(&RtSendData, 0 , sizeof(RtSendData));
   //No NRT command at power up
   Cmd.Reply = 0;
   memcpy(Dest.addr, "\xFF\xFF\xFF\xFF\xFF\xFF", MAC_ADDRESS_LEN);
}

/*  void ModBusSendTask::Run(void)
 *
 *  Purpose:
 *    This is the run function of the ModBusSendTask. At power up, RTc is written to DCP,
 *    Power up data is read and then preset data is written to DCP.
 *    Real time communication takes place every 1ms and non-real time based upon occurrence of
 *    any request made by other tasks by writing into the ReqQueue queue of this task.
 *
 *  Entry condition:
 *	  None
 *
 *  Exit condition:
 *	  None.
 */
void ModBusSendTask::Run(void)
{
	//Some delay to make sure all driver components are initialized properly
	DelayMs(100);
	SINT8 DummyChar;//To read the queue invoked by timer interrupt
	UINT16 ReadRegNRT  = 0;
	UINT16 WriteRegNRT = 0;
	UINT16 ReadAddrRemote  = MB_RT_RECEIVE_DATA_INDX;//Remote OD read index
	UINT16 RegReadCount  = RTRECEIVEDATA_REGCOUNT;
	UINT16 WriteAddrRemote = MB_RT_SEND_DATA_INDX;//Remote OD write index
	UINT16 RegWriteCount = RTSENDDATA_REGCOUNT;
	UINT16 ReadAddrLocal  =  MB_RT_SEND_DATA_INDX;
	Running = FALSE;
	for(;;){
		if(!Netif -> up){
		   ModBusErrorFlag = TRUE;
		   if(PowerupCommunicationDone == TRUE){
		      AlarmQueueWrite(CF01);
		      Log::WriteEventLog(EVENT_MODBUSCOMMFAILURE, 0, 0, 1);
		   }
		   RtReceiveData.DigOutputs = 0;
		   PowerupCommunicationDone = FALSE;
		   Running = FALSE;
		   FirmwareFileSent = FALSE;
		   DelayMs(100);
		}
		else{
			if(!Running){
			   if(SocketImplementation == TRUE){
				CloseSocket();
				CreateAndBindSocket();
				NonBlock();
			   }
				Running = TRUE;
				DelayMs(1000);//Some delay before DCP set it up after firmware upload
				StartPowerupCommunicationWithDCP();
				while(PowerupCommunicationDone == FALSE)
				{
					StartPowerupCommunicationWithDCP();
				}
			}//if(!running
			if((ModBusRealTimeQ.Read(this, DummyChar , -1)) && (FirmwareFileSent == FALSE)){
				//Check if we have some non real time data request
				if(ReqQueue.Read(this, Cmd , 0)){
					RtSendData.ReqReg = Cmd.Req;
					RtSendData.IndxReg = Cmd.Indx;
					ReadRegNRT = Cmd.RegsReadCount;
					WriteRegNRT = Cmd.RegsWriteCount;
				}
				else{
					RtSendData.ReqReg = 0;
					RtSendData.IndxReg = 0;
					ReadRegNRT = 0;
					WriteRegNRT = 0;
				}
				ReadWriteMultipleRegisters(DCPMBSLAVEADDRESS, ReadAddrRemote,
					(RegReadCount + ReadRegNRT), WriteAddrRemote, (RegWriteCount + WriteRegNRT), ReadAddrLocal);
			}
		}
	}
}

/* void ModBusSendTask::StartPowerupCommunicationWithDCP(void)
 *
 * Purpose:
 *     This function is called once at power up and once the Ethernet link get up
 *     on Ethernet interface 0 connected to DCP. It is called by Run() function of the task.
 *     It sends preset and RTC time to DCP and reads the power up data from DCP containing the
 *     power supply frequency, power, DCP CRC etc(see DCPPowerupData structure).
 *     Power Up communication sequence is following
 *	   1 - Read Power up data and write Timestamp for sync
 *	   2- If freq, power doesn't match with the one in WC . Write MAC chip data and read ack
 *	   3- Write preset and read ack.
 *  Entry condition:
 *		  None.
 *
 *  Exit condition:
 *		  None.
 */
void ModBusSendTask::StartPowerupCommunicationWithDCP(void)
{
	RTCTime TM;
	UINT16 Calcrc = 0;
	UINT16 FailureCount = 0;
	PowerupCommunicationDone = FALSE;
	UINT16 ReadAddrRemote = MB_POWER_UP_DATA_INDX;//Remote/slave OD read index
	UINT16 RegReadCount = POWERUPDATA_REGCOUNT;
	UINT16 WriteAddrRemote = MB_BASE_TIME_INDX;//Remote OD write index
	UINT16 RegWriteCount = BASETIME_REGCOUNT;
	UINT16 ReadAddrLocal = MB_BASE_TIME_INDX;//Local OD read index

	RTClock::GetCurrentTime(&TM);//Read RTC
	SendRtc.Second = TM.Second;
	SendRtc.Minute = TM.Minute;
	SendRtc.Hour   = TM.Hour;
	SendRtc.Day    = TM.Day;
	SendRtc.Month  = TM.Century_Month;
	SendRtc.Year   = TM.Years;
	MBack.ACK = 0;
   while((PowerupCommunicationDone == FALSE) && (FailureCount < NUMBER_OF_RETRIES)){
	  MBwriteAddrLocal.Write(0 ,ReadAddrRemote, 0);
      ReadWriteMultipleRegisters(DCPMBSLAVEADDRESS,
								   ReadAddrRemote, RegReadCount, WriteAddrRemote, RegWriteCount,
								   ReadAddrLocal);
	  //Delay for a while before checking that ModbusReceiveTask has received and processed the response
      DelayMs(500);
      switch(WriteAddrRemote){
			case MB_BASE_TIME_INDX:
				Calcrc = ByteCRC((UINT8 *)&PowerUpData, (sizeof(PowerUpData) - sizeof(PowerUpData.CRC)));
				if((Calcrc != PowerUpData.CRC)
					|| (memcmp(PowerUpData.SWVersionLabel, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
							SWVERSION_LEN) == 0)){
				   FailureCount++;
				}
				else{
					DCPTimeSyncCounter = 0;
					ReadAddrRemote = MB_ACK_INDX;
					RegReadCount = ACK_REGCOUNT;
//					pprintf("\nDCP Mac Address %02X %02X %02X %02X %02X %02X %s",
//							PowerUpData.DCPMACAddr[0], PowerUpData.DCPMACAddr[1], PowerUpData.DCPMACAddr[2],
//							PowerUpData.DCPMACAddr[3], PowerUpData.DCPMACAddr[4], PowerUpData.DCPMACAddr[5],
//							PowerUpData.SWVersionLabel);

					memcpy(Dest.addr, PowerUpData.DCPMACAddr ,MAC_ADDRESS_LEN);
					if((PowerUpData.Freq != MacchipData.Freq) || (PowerUpData.Pwr != MacchipData.Power)
							|| (PowerUpData.CurrSystemType != MacchipData.CurrSystemType)){
						//In case the received data doesn't match with MACchipp data
						WriteAddrRemote = MB_MACCHIP_INDX;
						RegWriteCount = MACCHIPDATA_REGCOUNT;
						MBack.ACK = 0;
					}
					else{
						WriteAddrRemote = MB_PRESET_INDX;
						RegWriteCount = PRESET_REGCOUNT;
						MBack.ACK = 0;
					}
					ReadAddrLocal = WriteAddrRemote;
				}//else
				break;
			case MB_MACCHIP_INDX:
				if(MBack.ACK == 0)	// if MAC chip data not written successfully
					FailureCount++;
				else{
					WriteAddrRemote = MB_PRESET_INDX;
					RegWriteCount = PRESET_REGCOUNT;
					ReadAddrLocal = WriteAddrRemote;
					MBack.ACK = 0;
				}
				break;
			case MB_PRESET_INDX:
				if(MBack.ACK == 0)	// if Preset data not written successfully
				   FailureCount++;
				else{
					WriteAddrRemote = MB_DCPTIMEPARA_INDX;
					RegWriteCount = DCPTIMEPARAM_REGCOUNT;
					ReadAddrLocal = WriteAddrRemote;
					DCPTimeControlParam.CRC = ByteCRC((UINT8 *)&DCPTimeControlParam, (sizeof(DCPTimeControlParam) - sizeof(DCPTimeControlParam.CRC)));
					MBack.ACK = 0;
				}
				break;
			case MB_DCPTIMEPARA_INDX:
				if(MBack.ACK == 0)	// if DCPTimeParama data not written successfully
				   FailureCount++;
				else{
					PowerupCommunicationDone = TRUE;
					MBack.ACK = 0;
				}
				break;
			default:
				break;
		}
	}
	if(PowerupCommunicationDone == FALSE){
		//TODO:Raise Alarm or write event.Discuss
//		pprintf("\n DCP power up communication failure \n");
	}
}

/*  void ModBusSendTask::NotifyReadWriteEvent(SINT32 Length)
 *
 *  Purpose:
 *        This function notify the occurrence of reading or writing Non real time
 *        data to NRTModbus task.
 *        It is called by ProcessModBusCommand() function
 *
 *   Entry condition:
 *		  length: no. of data bytes read other than real time data.
 *		  contains 0 in case the extra bytes written successfully on remote OD.
 *   Exit condition:
 *		  None.
 */
void ModBusSendTask::NotifyReadWriteEvent(SINT32 Length)
{
	//Cmd.Reply pointer will only have value
	//when some extra data other than real time
	//data is requested from DCP or written on DCP
   if(Cmd.Reply && Length > 0){
      Cmd.Reply -> Write(RTOS::GetCurrentTask(), Length, 0);
      Cmd.Reply = 0;
   }
}

/*   Purpose:
 *        This function create and bind a UDP socket on port MB_SOURCE_UDPPORT.
 *        This function is called by Run function of ModBusSendTask task.
 *
 *   Entry condition:
 *		  None
 *
 *   Exit condition:
 *		  None.
 */
void ModBusSendTask::CreateAndBindSocket(void)
{
	Fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(Fd != INVALID_SOCKET);
	//Socket address to be bind with socket descriptor
	sockaddr_in Sa;
	//socket family
	Sa.sin_family = AF_INET;
	//this becomes the source port for sending MB ppacket
	Sa.sin_port = htons((UINT16)MB_SOURCE_UDPPORT);
	//source ip from which MB packet will be send
	Sa.sin_addr.s_addr = Netif -> ipaddr.addr;
	if (bind(Fd, (sockaddr *) &Sa, sizeof(Sa)) < 0)
		CRASH("Bind ModBusSendTask socket");
}

/*   Purpose:
 *
 *        This function closes the socket initially binded at MB_SOURCE_UDPPORT UDP port.
 *        (It is used in case connection is lost i.e network cable is unplugged).
 *         This function is called by "void ModBusSendTask::Run()" functions:
 *
 *   Entry condition:
 *		  None
 *
 *   Exit condition:
 *		  None.
 */
void ModBusSendTask::CloseSocket(void)
{
	if(Fd != INVALID_SOCKET){
		lwip_close(Fd);
		Fd = INVALID_SOCKET;
	}
}

/*   Purpose:
 *
 *        This function makes the socket binded at MB_SOURCE_UDPPORT nonblocking.
 *        This function is called by Run functions:
 *
 *   Entry condition:
 *		  None
 *
 *   Exit condition:
 *		  None.
 */
void ModBusSendTask::NonBlock(void)
{
	UINT32 Val = 1;
	if(lwip_ioctl(Fd, FIONBIO, &Val) < 0)
		CRASH("socket nonblocking failed");
}

/*   void BuildQueryBasis(UINT32 Slave, UINT32 Function, UINT16 ReadAddr, UINT16 NRegsRead, UINT16 WriteAddr,
 *		UINT16 NRegWrite)
 *  Purpose:
 *     It builds a UDP query header for Modbus UDP packet for function code 0x17(ReadWriteMultipleRegs).
 *     This function is called by ReadWriteMultipleRegisters() functions:
 *
 *  Entry condition:
 *	   Slave: Slave ID
 *		  Function: function Code
 *		  ReadAddr: Read Address of slave OD
 *		  NRegsRead: no. of registers to be read from slave
 *        WriteAddr: Write address of slave OD
 *        NRegWrite: no of registers to be written on slave
 *
 *   Exit condition:
 *		  None.
 */
void ModBusSendTask::BuildQueryBasis(UINT32 Slave, UINT32 Function, UINT16 ReadAddr, UINT16 NRegsRead, UINT16 WriteAddr,
		UINT16 NRegWrite)
{
   /* Extract from MODBUS Messaging on TCP/IP Implementation
	  Guide V1.0b (page 23/46):
	  The transaction identifier is used to associate the future
	  response with the request. So, at a time,
	   this identifier must be unique.
   */
   static UINT16 Tid = 0;

   MBPkt.MBAdu.MBAPheader.TID = Tid;
   /* Protocol Identifier Modbus UDP*/
   MBPkt.MBAdu.MBAPheader.ProtocolID = MB_UDP_PROTOCOL_ID;
   /* Length to fix later with set_Query_length_udp (index 4 and 5) */
   MBPkt.MBAdu.MBAPheader.SlaveId = Slave;	//slave ID
   MBPkt.MBAdu.MBAPheader.FunctinCode = Function;	//function code
   MBPkt.MBAdu.MBReadWriteMultiple.MBPDUReadWriteMultipleReq.Readaddr = ReadAddr;	// Read OD address
   MBPkt.MBAdu.MBReadWriteMultiple.MBPDUReadWriteMultipleReq.ReadRegs = NRegsRead;	//Read register count
   MBPkt.MBAdu.MBReadWriteMultiple.MBPDUReadWriteMultipleReq.WriteAddr = WriteAddr;	//write OD address
   MBPkt.MBAdu.MBReadWriteMultiple.MBPDUReadWriteMultipleReq.WriteRegs = NRegWrite;	//write register count

   MBPkt.MBAdu.MBReadWriteMultiple.MBPDUReadWriteMultipleReq.WriteByteCount = NRegWrite * HOLDING_REG_SIZE;  //WriteByteCount
   //value gets wrapped ,after reaching the maximum value of 16 bit variable.
   Tid++;
}

/* void ReadWriteMultipleRegisters(UINT32 SlaveAddress, UINT16 ReadAddrSlave, UINT16 NRegRead, UINT16 WriteAddrSlave,
 *  UINT16 NRegWrite, UINT16 ReadAddrLocal)
 * Purpose:
 *    To call the functions to prepare the Modbus packet using the passed arguments and send the same
 *    to slave. This function is called by "void ModBusSendTask::Run()"
 *
 *  Entry condition:
 *  SlaveAddress: slave id
 *	ReadAddrSlave:Od index/read address on DCP side
 *	NRegRead: No of registers to be read from Slave
 *	WriteAddrSlave:Od index/write address on DCP side.
 *	NRegWrite: of registers to be written on Slave OD.
 *	ReadAddrLocal: OD index/read address on local OD.
 *
 *  Exit condition:
 *  None.
 */
void ModBusSendTask::ReadWriteMultipleRegisters(UINT32 SlaveAddress, UINT16 ReadAddrSlave, UINT16 NRegRead,
		UINT16 WriteAddrSlave, UINT16 NRegWrite, UINT16 ReadAddrLocal)
{
   PktBuf MbPkt;
   sockaddr_in To;
   if(SocketImplementation == TRUE){
	   To.sin_family = AF_INET;
	   To.sin_port = htons(MB_UDP_DEFAULT_PORT);
	   To.sin_addr.s_addr = MakeIP(DCPIP);
   }
   if((NRegRead > MAX_REGISTERS_READ) || (NRegWrite > MAX_REGISTERS_WRITE)){
//      pprintf("ERROR Trying to Read/write to too many registers");
   }
   else{
	  BuildQueryBasis(SlaveAddress, MB_FUNC_READWRITE_MULTIPLE_REGISTERS, ReadAddrSlave, NRegRead, WriteAddrSlave, NRegWrite);//, Query);
	  //Read the registers from local OD which needs to we written on slave OD
	  if(MBRegHoldingCB(&MBPkt.Query[MB_UDP_WRITEDATA_OFFSET], ReadAddrLocal, NRegWrite, MB_REG_READ, 0,
			  (MB_UDP_BUF_SIZE - MB_UDP_WRITEDATA_OFFSET)) != MB_ENOERR){
	     Log::WriteEventLog(EVENT_MB_ENOREG);
	     ModBusErrorFlag = TRUE;
	  }
	  else{
        //set length field equals to bytes followed by length field
        MBPkt.MBAdu.MBAPheader.Length = (MB_UDP_WRITEDATA_OFFSET + (NRegWrite * HOLDING_REG_SIZE) - MB_UDP_UID_OFFSET);
         if(Netif -> up){
      		if(SocketImplementation == TRUE){
           		if(sendto(Fd, (void *)MBPkt.Query, (MB_UDP_WRITEDATA_OFFSET + (NRegWrite * HOLDING_REG_SIZE)), 0, (sockaddr *)&To, sizeof(To)) < 0)
             	CRASH(" Error sending Modbus packet");
			 }
			 else{
				 int len = (MB_UDP_WRITEDATA_OFFSET + (NRegWrite * HOLDING_REG_SIZE));
				 if(len > MB_UDP_BUF_SIZE){
					 len = MB_UDP_BUF_SIZE;
					// UpdateTPVal(96);
				 }
		         memcpy(Query + sizeof(EthHeader), (void *)MBPkt.Query, len);
		         MbPkt.payload = Query;
		         MbPkt.tot_len = len + sizeof(EthHeader);
		         MbPkt.len = MbPkt.tot_len;
		         MbPkt.next = 0;
		         //memcpy(Dest.addr, "\x00\x04\xA3\x2D\xFA\xBB", 6);
		         if(etharp_send_mb(Netif, &MbPkt, (EthAddr *)Netif->GetHardwareAddress(), &Dest) != 0)
		            ASSERT(0);
		      }
         }
	  }
   }
}

/* MBRegHoldingCB( UINT8 * RegBuffer, UINT16 UsAddress, UINT16 UsNRegs, MBRegisterMode EMode, UINT8 * DataBuff)
 *
 * 	 Purpose:
 *      This function is used to read data(Requested by WC) from related buffer
 *      and write data transmitted by WC using Modbus ReadWrite Multiple register command.
 *		This function is called by ReadWriteMultipleRegisters() function.
 *
 *   Entry condition:
 *		RegBuffer = pointer to buffer where data is stored before transmission
 *		UsAddress = Modbus OD table Index
 *		UsNRegs = No. of registers
 *		EMode = read or write
 *		DataBuff = pointer to buffer that contain data to be written
 *
 *   Exit condition:
 *		Returns status of the process: if successful, returns MB_ENOERR else returns MB_ENOREG
 */
Mutex MBHold("MbHoldCb");
MBErrorCode MBRegHoldingCB(UINT8 * RegBuffer, UINT16 UsAddress, UINT16 UsNRegs,
		MBRegisterMode EMode, UINT8 * DataBuff, int MaxBuffSize)
{
   MBErrorCode EStatus = MB_ENOERR;
   UINT16 RegIndex = 0;
   const MODBusODEntry * Odobj;
   UINT8 * RegPtr;
   UINT16 Nregs = 0;
   int CpySize;
   int tempCpySize =0;
   //UpdateTPVal(140);
   MBHold.Acquire(0, -1);
	   if(UsAddress >= REG_HOLDING_START){	
		  Odobj = OdHoldingRegisters.FindOdEntry(UsAddress, &RegIndex);
		  while(UsNRegs != 0){
			 if(Odobj){
				if(Odobj->Precallback)	
				   Odobj->Precallback();

				Nregs = Odobj->Count - RegIndex;
				if(Nregs > UsNRegs)
				   Nregs = UsNRegs;
				UsNRegs -= Nregs;
				RegPtr = (UINT8 *)((UINT8 *)Odobj->Variable + (RegIndex * HOLDING_REG_SIZE));
				if(EMode == MB_REG_READ){
				   CpySize = Nregs * HOLDING_REG_SIZE;
				   if(CpySize > MaxBuffSize){
					   CpySize = MaxBuffSize;
					 //  UpdateTPVal(100);
				   }
				   tempCpySize += CpySize;
				   memcpy(RegBuffer, RegPtr, CpySize);
				}
				else if(EMode == MB_REG_WRITE){
				   CpySize = Nregs * HOLDING_REG_SIZE;
				   tempCpySize = CpySize;
				   if(CpySize > MaxBuffSize){
//					   pprintf("\n Reading extra from Source buffer %d %d %x %x", MaxBuffSize - CpySize, UsAddress, RegPtr, DataBuff);
					}
				   if(tempCpySize > (Odobj->VarSize)){
//					   pprintf("\n\n Copying more size then expected at index %d,%x,%d,%d,%d",UsAddress,RegPtr,Odobj->VarSize,CpySize,RegIndex * HOLDING_REG_SIZE);
					}
				   memcpy(RegPtr, DataBuff, CpySize);
				}
				if(Odobj->Postcallback)
					 Odobj->Postcallback();
				if(UsNRegs > 0){
				   UsAddress += Nregs;		//modify index
				   if(EMode == MB_REG_READ){
					  RegBuffer += (Nregs * HOLDING_REG_SIZE);
					  MaxBuffSize -= (Nregs * HOLDING_REG_SIZE);
				   }
				   else if(EMode == MB_REG_WRITE){
					 DataBuff += (Nregs * HOLDING_REG_SIZE);
					 MaxBuffSize -= (Nregs * HOLDING_REG_SIZE);
				   }
				   RegIndex = 0;
				   Odobj = OdHoldingRegisters.FindOdEntry(UsAddress);
				}
			 }
			 else{
				EStatus = MB_ENOREG;
				UsNRegs = 0;
			 }
		  }
	   }
	   else
		  EStatus = MB_ENOREG;
	MBHold.Release(0);
   return EStatus;
}

void ModBusSendTask::Tick(void)
{
   if(PowerupCommunicationDone == TRUE){
      if(!Netif -> up){
         PowerupCommunicationDone = FALSE;
         ModBusErrorFlag = TRUE;
         AlarmQueueWrite(CF01);
         Running = FALSE;
         RtReceiveData.DigOutputs = 0;
      }
   }
}
