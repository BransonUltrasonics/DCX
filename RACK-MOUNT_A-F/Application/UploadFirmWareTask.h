/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UploadFirmWareTask.h_v   1.2   02 Jul 2015 10:45:06   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/UploadFirmWareTask.h_v  $
 * 
 *    Rev 1.2   02 Jul 2015 10:45:06   RJamloki
 * Reverted to initial revision as we don’t need to support firmware file checksums at end
 * 
 *    Rev 1.1   24 Jun 2015 08:38:26   amaurya
 * Added DataStartOffset to keep track of programming data start.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.9   13 Mar 2014 16:10:08   rjamloki
 * Added data to be backed up during Copy to FRAM in class 
 * 
 *    Rev 1.8   20 Sep 2013 07:00:14   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.7   14 Jun 2013 11:21:10   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.6   05 Jun 2013 03:11:48   amaurya
 * Fixed Passcode and Limits issue.
 * 
 *    Rev 1.5   15 Mar 2013 10:53:12   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.4   11 Mar 2013 02:29:42   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.3   18 Feb 2013 03:11:24   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.2   26 Oct 2012 02:40:36   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.1   02 Jul 2012 13:04:12   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.0   21 Mar 2012 12:21:58   PDwivedi
 * Initial revision.
 *
 */

#ifndef UPLOADFIRMWARETASK_H_
#define UPLOADFIRMWARETASK_H_

#include "Task.h"
#include "FIFO.h"
#include "FirmWareUpgrade.h"
#include "SysConfig.h"
#include "LOG.h"
#include "Globals.h"
//These checksums will be fill by ADDCHECKSUM.exe during build process
//at the start of actual binary file
#define CHECKSUMDCP   	 0xAA1267BB
#define CHECKSUMWC	 	 0xEE1267DD
#define CHECKSUMLCDA	 0x8B899EFE//0xEE1267AA
#define CHECKSUMWC_COMPLETEBIN 0xFF1267CC
#define CHECKSUMDCP_COMPLETEBIN 0xDD1267AA

#define FRAM_SIZE 		 0x20000

#define ERR_UNKNOWNBINFILE 	   3
#define ERR_UNKNOWNFILETYPE 	   11
#define ERR_CONFIGDATALEN 	   5
#define SUCCESS_CONFIGDATAMODIFIEDCOPY 6
#define SUCCESS_CONFIGDATAMODIFIEDRESTORE 7

enum FileType
{
	FileTypeErr,
	FileTypeBin,
	FileTypeCyg,
	FileTypeCfg,
};

struct MsgUploadFirmware
{
	FileType Filetype;
	SINT32   FileLen;
	SINT8    * FileData;
	SINT8    * Action;
};

enum FlashArea
{
   FlashAreaInvalid,
   FlashAreaApp,
   FlashAreaWeb,
   FlashAreaBootloader,
   FlashAreaComplete,
};

class UploadFirmware: public Task
{
	public:
		UploadFirmware();
		BOOLEAN ProgramFirmware(FileType FType, SINT8 * Data, SINT32 DataLen, SINT8* Action);
		static UploadFirmware * thisPtr;
	protected:
		void Run();
		void ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea);
		void SendDCPdata(SINT8 * Data,  SINT32 DataLen, BinFileType BinType);
		void SendLCDAdata(SINT8 * Data, SINT32  DataLen);
	//	void ProgramCompressedTarRegionFlash(SINT8 * Data, SINT32 DataLen);
		void CopyDataToFRAM(SINT8  * Data, SINT32 DataLen);
		void RestoreDataToFRAM(SINT8 * Data, SINT32 DataLen);
		MsgUploadFirmware Msg;
		Fifo<MsgUploadFirmware> UpLoadQ;
		SystemConfiguration Sysconfigtemp;
		//Temp data to restore original data after copy data to FRAM.
		UINT32 TempEventLogInitialized;
		SINT16 TempCircularIndexEvent;
		UINT32 TempEventsNo;
		UINT32 TempAlarmLogInitialized;
		SINT16 TempCircularIndexAlarm;
		UINT32 TempAlarmsNo;
		WeldResultInfo TempWeldresultinfo;
		AlarmLog  * TempAlarmdata;
		EventLog  * TempEventdata;
		WeldResult * TempWeldHistory;
		SINT8  * WebFirmware;
};

void EraseCallback(int Offset, int Length);
void ProgramCallback(int Offset, int Length);

#endif /* UPLOADFIRMWARETASK_H_ */
