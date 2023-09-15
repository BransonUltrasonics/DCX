/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UploadFirmWareTask.cpp_v   1.3   21 Aug 2015 05:51:56   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UploadFirmWareTask.cpp_v  $
 * 
 *    Rev 1.3   21 Aug 2015 05:51:56   RJamloki
 * Fixes for Coverity CID 10016, 11232 and 11233
 * 
 *    Rev 1.2   02 Jul 2015 10:45:06   RJamloki
 * Reverted to initial revision as we don’t need to support firmware file checksums at end
 * 
 *    Rev 1.1   25 Jun 2015 05:48:32   rjamloki
 * Changes to support the firmware files having checksums at file end
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.25   13 Mar 2014 15:55:08   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 
 *
 */

#include "UploadFirmWareTask.h"
#include "stdio.h"
#include "Board.h"
#include "CPU.h"
#include "DebugNew.h"
#include "ASSERT.h"
#include "Flash.h"
#include "CyGOS.h"
#include "Board.h"
#include "FUNC.h"

#define UPLOADFIRMWARETASK_STACKSIZE 512
#define UPLOADFIRMWARESERVERPORT_DCP 7075

//  This is the global pointer to class. Website module use this  pointer to access
//  the queue (upLoadQ)
UploadFirmware * UploadFirmware::thisPtr;

/*   Purpose :
 *	 	Constructor for UploadFirmware task called when task is created. This functiom calls the constructor of base class
 *	 	Task to which the name of the task and stack size are passed as parameter. It also initializes the class objects
 *    	declared in this class which don’t have default constructor(in this case fifo class object upLoadQ requires size and name.
 *
 *   Entry condition
 *
 *   Exit condition
 *   	None
 */
UploadFirmware::UploadFirmware():Task("UploadFirmware", UPLOADFIRMWARETASK_STACKSIZE), UpLoadQ(1, "uploadQ")
{
	thisPtr = this;
	memset(&Msg, 0, sizeof(MsgUploadFirmware));
	TempAlarmdata = new AlarmLog[MAX_ALARM_ENTRIES];
	TempEventdata = new EventLog[MAX_EVENT_ENTRIES];
	TempWeldHistory = new WeldResult[MAX_WELD_RESULT_ENTRIES];
	WebFirmware = 0;
	//coverity CID: 10016
	memset(&Sysconfigtemp, 0 , sizeof(Sysconfigtemp));
	TempCircularIndexAlarm = 0;
	TempCircularIndexEvent = 0;
	TempEventsNo = 0;
	TempAlarmLogInitialized = 0;
	TempAlarmsNo = 0;
	TempEventLogInitialized = 0;
	memset(&TempWeldresultinfo , 0 , sizeof(TempWeldresultinfo));
}


/*  void UploadFirmware::Run(void)
 *
 *   Purpose:
 *   	This function waits indefinitely for upload command to be received from website module.
 *   	Once the command is received it performas the requires action based *on received command.
 *
 *   Entry condition
 *   	None.
 *
 *   Exit condition
 *   	None.
 */
void UploadFirmware::Run(void)
{
	MsgUploadFirmware TempQ;
	unsigned long Checksum = 0;
	for(;;){
		UpLoadQ.Read(this, TempQ, -1);
		pprintf(" Upload msg type %d", TempQ.Filetype);
		//TODO:Abort the weld cycle and stop
		//the state machine to avoid any other command
		//while programming
		pprintf("\n CRC=%d", ByteCRC((UINT8 *)TempQ.FileData,TempQ.FileLen));
		switch(TempQ.Filetype){
			case FileTypeBin:
				memcpy(&Checksum, TempQ.FileData, 4);
				pprintf("checksum is =%08x length = %d", Checksum, TempQ.FileLen);
				if(Checksum == CHECKSUMWC){
					pprintf("weld controller function %08x\n", Checksum);
					ProgramFlash(TempQ.FileData, TempQ.FileLen - 4, FlashAreaApp);//function to be called for wc file
				}
				else if(Checksum == CHECKSUMDCP){
					pprintf("dcp function %08x\n", Checksum);
					SendDCPdata(TempQ.FileData, TempQ.FileLen - 4, FileDCPAppBin);//function to be called for dcp file
				}
				else if(Checksum == CHECKSUMLCDA){
					SendLCDAdata(TempQ.FileData, TempQ.FileLen);//function to be called for Lcd file
				}
				else if(Checksum == CHECKSUMWC_COMPLETEBIN){
				   pprintf("weld controller complete bin function %08x\n", Checksum);
				   ProgramFlash(TempQ.FileData, TempQ.FileLen - 4, FlashAreaComplete);
				}
				else if(Checksum == CHECKSUMDCP_COMPLETEBIN){
				   pprintf("dcp controller complete bin function %08x\n", Checksum);
				   SendDCPdata(TempQ.FileData, TempQ.FileLen - 4, FileDCPCompleteBin);//function to be called for dcp file
				}
				else{
					Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(ERR_UNKNOWNBINFILE));
					pprintf("neither WC FILE NOR DCP FILE");
					if(TempQ.FileData)
					      delete TempQ.FileData;
				}
			break;
			case FileTypeCyg:
				ProgramFlash(TempQ.FileData, TempQ.FileLen, FlashAreaWeb);
			break;
		   case FileTypeCfg:
		    	if(!strcmp(TempQ.Action, "CopySystem"))
				   CopyDataToFRAM(TempQ.FileData, TempQ.FileLen);
		    	else if(!strcmp(TempQ.Action, "RestoreSystem"))
		    	   RestoreDataToFRAM(TempQ.FileData, TempQ.FileLen);
			break;
			default:{
				Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(ERR_UNKNOWNFILETYPE));
				if(TempQ.FileData)
				   delete TempQ.FileData;
			}
		}//switch(tempQ.fileType)	
	}//for(;;)
}

/*  void UploadFirmware::SendDCPdata(SINT8 * Data, SINT32 DataLen)
 *
 *   Purpose
 *   	This function passes the DCP firmware file to FirmwareUpgrade task.
 *   	This function is called from Run() function of this class.
 *
 *   Entry condition
 *   	Data - pointer to firmware file
 *   	DataLen - length of file in bytes
 *   	BinType - The binary type(App bin or complete bin)
 *
 *   Exit condition
 *   	None
 */
void UploadFirmware::SendDCPdata(SINT8 * Data, SINT32 DataLen, BinFileType BinType)
{
	//int RetVal;
	FirmwareUpgradeCommand  Cmd;
	Cmd.NodeIP.addr= MakeIP(DCPIP);
	Cmd.ServerPort= UPLOADFIRMWARESERVERPORT_DCP;
	Cmd.Type = BinType;
	Cmd.FileLength = DataLen;
	Cmd.FileData= (unsigned char *)Data;
	FirmWareUpgradeTask::thisPtr->FUWriteQueue(Cmd);
}


/*  void UploadFirmware::SendLCDAdata(SINT8 * Data, SINT32 DataLen)
 *
 *   Purpose
 *   	This function passes the DCP firmware file to FirmwareUpgradetask
 *   	This function is called from  Uploadfirmwaretask
 *
 *   Entry condition
 *   	Data - Pointer to firmware file
 *   	DataLen - length of file in bytes
 *
 *   Exit condition
 *   	None
 */
void UploadFirmware::SendLCDAdata(SINT8 * Data, SINT32 DataLen)
{
	FirmwareUpgradeCommand Cmd;
	Cmd.Type = FileLCDA;
	Cmd.FileLength = DataLen;
	Cmd.FileData = (unsigned char *)Data;
	FirmWareUpgradeTask::thisPtr -> FUWriteQueue(Cmd);
}


/*   void UploadFirmware::ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea)
 *
 *   Purpose:
 *   	This function programs the area of flash requested with received data
 *   	This function is called from the Run() function of this class.
 *
 *   Entry condition :
 *   	Data - Pointer to the data to be programmed on FLASH
 *   	DataLen - length of data to be programmed.
 *    Flasharea- Area of flash to be programmed bootloader, Application , cyg or complete
 *   Exit condition
 *   	None.
 */
void UploadFirmware::ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea)
{
	pprintf("\n %s", __FUNCTION__);
	SINT32 DataLenLimit = 0;
	SINT32 WebFirmwareLen = 0;
	BOOLEAN ValidFlashArea = TRUE;
	void * FlashAreaStartAddr;
	Flash * FlashPtr = Board::GetFlash();
	SINT8 * FlashData = Data;
	SINT32 Indx = 0;
	ASSERT(FlashPtr);

   CyGOS::StopTimer(1);
   CyGOS::KickWatchDog();
   if(Flasharea == FlashAreaApp){
      FlashData += 4;
      DataLenLimit = (SINT32)__FLASH_SIZE;
      FlashAreaStartAddr = (void *)__FLASH_ROM;
   }
   else if(Flasharea == FlashAreaWeb){
      DataLenLimit = (SINT32)__TAR_SIZE;
      FlashAreaStartAddr = (void*)__TAR_START;
      if(DataLen > (DataLenLimit - 4))
    	  ValidFlashArea = FALSE;
      else{
		  WebFirmware = new SINT8[DataLen + 4];
		  memset(WebFirmware, 0 , DataLen + 4);
		  //convert to Little Endian
		  WebFirmwareLen = (((DataLen & 0x000000FFUL) << 24) |
						   ((DataLen & 0x0000FF00UL) << 8)   |
						   ((DataLen & 0x00FF0000UL) >> 8)   |
						   ((DataLen & 0xFF000000UL) >> 24));
		  memcpy(WebFirmware , &WebFirmwareLen, sizeof(WebFirmwareLen));
		  memcpy(WebFirmware + 4 , Data, DataLen);
		  FlashData = WebFirmware;
		  DataLen = DataLen + 4;
      }
   }
   else if(Flasharea == FlashAreaComplete){
      FlashData += 4;
      DataLenLimit = (SINT32)__FLASH_SIZE + (SINT32)__BOOTLOADER_SIZE + (SINT32)__TAR_SIZE;
      FlashAreaStartAddr = (void *)0;
   }
   else
      ValidFlashArea = FALSE;

	if(ValidFlashArea == TRUE){
      if((DataLen > 0) && (DataLen <= DataLenLimit)){
         CyGOS::Stop();
         CyGOS::KickWatchDog();
         pprintf("Programming Address %08X\n", FlashAreaStartAddr);
         pprintf("Flash %08X\n", FlashPtr);
         FlashPtr -> Unlock(FlashAreaStartAddr, DataLen);
         FlashPtr -> Erase(FlashAreaStartAddr, DataLen,EraseCallback);
         FlashPtr -> Program(FlashAreaStartAddr, FlashData, DataLen, ProgramCallback);
         FlashPtr -> Lock(FlashAreaStartAddr, DataLen);
		 for(Indx = 0; Indx < 10; Indx++){
         	CyGOS::KickWatchDog();
         	DelayMs(100);
		 }         
         Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT16)(Flasharea));
         pprintf("\n Waiting for Restart after flash program...\n ");
         CyGOS::Restart() ;
         while(1);
      }
	}
	CyGOS::StartTimer(1);
	if(WebFirmware)
		delete[] WebFirmware;//coverity CID: 11232 , 11233
	delete FlashPtr;
	delete Data;
}



/*   bool UploadFirmware::ProgramFirmware(FileType Ftype, SINT8 * Data, SINT32 DataLen,SINT8 * Action)
 *
 *   Purpose
 *   	This function gets called by website module after receiving the firmware file.
 *
 *   Entry condition
 *   	Ftype: type of file
 *   	Data:  data to be programmed
 *   	DataLen: Length of data
 *
 *   Exit condition
 *   	Returns TRUE if data was written into fifo else return FALSE
 */
bool UploadFirmware::ProgramFirmware(FileType Ftype, SINT8 * Data, SINT32 DataLen,SINT8 * Action)
{
	bool RetVal;
	pprintf("\n %s %d,%s", __FUNCTION__, Ftype,Action);
	Msg.Filetype = Ftype;
	Msg.FileLen = DataLen;
	Msg.FileData = Data;
	Msg.Action = Action;
	RetVal = UpLoadQ.Write(0, Msg ,0);

	if(!RetVal)
	   delete[](Data);  //delete Data;

	return RetVal;
}

/*  void UploadFirmware:: RestoreDataToFRAM(SINT8 * Data, SINT32 DataLen)
 *
 *   Purpose:
 *   	This function modifies the whole FRAM with received file .All tasks which does Access the fram shall be of
 *   	higher priority than upload firmware. It will make sure that no other task was in middle of accessing the
 *  	FRAM because before copying the data critical section is used for avoiding being preempted by any other high
 *  	priority tasks and the data integrity maintained.
 *
 *   Entry condition
 *  	Data: data to be copied to FRAM.
 *  	DataLen: Length of data
 *
 *   Exit condition
 *		None.
 */
void UploadFirmware::RestoreDataToFRAM(SINT8 * Data, SINT32 DataLen)
{
	pprintf(" %s %d ",__FUNCTION__, DataLen);
	SR SRObj;
	if(DataLen != FRAM_SIZE)
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(ERR_CONFIGDATALEN));
	else{
		SRObj = EnterCritical();
		memcpy((UINT8 *)FRAM_BASEADDRESS, Data, DataLen);
		ExitCritical(SRObj);
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(SUCCESS_CONFIGDATAMODIFIEDRESTORE));
		CyGOS::Restart() ;
		while(1);
	}
}


/*  void UploadFirmware:: CopyDataToFRAM(SINT8 * Data, SINT32 DataLen)
 *
 *   Purpose:
 *   	This function Does the same thing as RestoreDataToFRAM. However it doesn't modify the IP address,Fieldbus address
 *   event, alarm and history section of fram.
 *
 *   Entry condition
 *  	Data: data to be copied to FRAM.
 *  	DataLen: Length of data
 *
 *   Exit condition
 *		None.
 */
void UploadFirmware::CopyDataToFRAM(SINT8 * Data, SINT32 DataLen)
{
	pprintf(" %s %d ",__FUNCTION__, DataLen);
	SR SRObj;
	DCPSpeedControl SpeedControl;
	if(DataLen != FRAM_SIZE)
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(ERR_CONFIGDATALEN));
	else{
		SRObj = EnterCritical();
		memcpy(&Sysconfigtemp,Sysconfig,sizeof(SystemConfiguration));
		memcpy(TempAlarmdata, &AlarmLogFram->AlarmLogObjs[0],sizeof(AlarmLogFram->AlarmLogObjs));
		memcpy(TempEventdata, &EventLogFram->EventLogObjs[0],sizeof(EventLogFram->EventLogObjs));
		memcpy(TempWeldHistory, WeldResFram->WeldResultObjs, sizeof(WeldResFram->WeldResultObjs));
		TempEventLogInitialized = EventLogFram->LogInitialized;
		TempCircularIndexEvent =  EventLogFram->CircularIndexEvent;
		TempEventsNo = EventLogFram->EventsNo;
		TempAlarmLogInitialized = AlarmLogFram->LogInitialized;
		TempCircularIndexAlarm = AlarmLogFram->CircularIndexAlarm;
		TempAlarmsNo = AlarmLogFram->AlarmsNo;
		TempWeldresultinfo = WeldResFram->Weldresultinfo;
		memcpy((UINT8 *)FRAM_BASEADDRESS, Data, DataLen);
		SpeedControl = Sysconfig->DcpSpeedControl;//Copy the value from new power supply.
		memcpy(Sysconfig, &Sysconfigtemp, sizeof(SystemConfiguration));
		Sysconfig->DcpSpeedControl = SpeedControl;//Restore every thing in system config from old power supply but loop speed control.
		memcpy(&AlarmLogFram->AlarmLogObjs[0], TempAlarmdata, sizeof(AlarmLogFram->AlarmLogObjs));
		memcpy(&EventLogFram->EventLogObjs[0], TempEventdata, sizeof(EventLogFram->EventLogObjs));
		memcpy(WeldResFram->WeldResultObjs, TempWeldHistory, sizeof(WeldResFram->WeldResultObjs));
		EventLogFram->LogInitialized = TempEventLogInitialized;
		EventLogFram->CircularIndexEvent = TempCircularIndexEvent;
		EventLogFram->EventsNo = TempEventsNo;
		AlarmLogFram->LogInitialized = TempAlarmLogInitialized;
		AlarmLogFram->CircularIndexAlarm = TempCircularIndexAlarm;
		AlarmLogFram->AlarmsNo = TempAlarmsNo;
		WeldResFram->Weldresultinfo = TempWeldresultinfo;
		ExitCritical(SRObj);
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (unsigned short)(SUCCESS_CONFIGDATAMODIFIEDCOPY));
		CyGOS::Restart() ;
		while(1);
	}
}
