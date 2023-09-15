/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FirmwareUpgrade.cpp_v   1.4   21 Sep 2015 06:45:48   RJamloki  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FirmwareUpgrade.cpp_v  $
 * 
 *    Rev 1.4   21 Sep 2015 06:45:48   RJamloki
 * Added SendFileToLCDAviaBDM() to program the secured LCD target processor.
 * 
 *    Rev 1.3   19 Mar 2015 06:44:24   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.2   28 Jan 2015 05:30:38   sesharma
 * Disable Watchdog before starting programming LCD
 * 
 *    Rev 1.1   17 Sep 2014 08:47:16   amaurya
 * Added code review changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

#include "Socket.h"
#include "FirmwareUpgrade.h"
#include <stdio.h>
#include "IPv4.h"
#include "LOG.h"
#include "portable.h"
#include "FUNC.h"
#include "UserIO.h"
#include "GLOBALS.h"
#include "BDMTask.h"
#include "CPU.h"
#include "FIFO.h"

// static variable definition
// This is the global pointer to class. UploadFirmware module uses this pointer to call
// the FUWriteQueue() function of this Task while sending binary file.
FirmwareUpgradeTask * FirmwareUpgradeTask::thisPtr;
BOOLEAN FirmwareFileSent = FALSE;

FirmwareUpgradeTask::FirmwareUpgradeTask(int Intf) :
		Task("FMUpgrade", FIRMWAREUPGRADETASK_STACKSIZE), Queue(1,
		      "FirmWareUpgrade")
/***********************************************************************************************/
/*
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
/**************************************************************************************************/
{
	UpLoadfile = NULL;
	thisPtr = this;
	NetIf = CyGOS::GetNetworkInterface(Intf);
	memset(&ProgInfo, 0, sizeof(UpgradeProgress));
	FUClientSocket = 0;
}
void FirmwareUpgradeTask::SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog)
/*********************************************************************************************/
/*
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
/**********************************************************************************************/
{
	Prog = 0;
}

void FirmwareUpgradeTask::FUWriteQueue(const FirmwareUpgradeCommand &Cmd) {
	printf(" received firmware upgrade command");
	if (!Queue.Write(Cmd, 0))
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_ANOTHERUPGRADEINPROGRESS,
		      0);

	SendFirmwareUpgradeProgressToWebsite(UPGRADEALREADYINPROGRESS);
}

/*  SINT32 FirmWareUpgradeTask::SendFileToLCDAviaBDM(UINT8 * FileDataStart, SINT32 Length)
 *
 *  Purpose
 *  	To program the LCD processor flash by first disabling the security.
 *
 *  Entry condition
 *  	FileDataStart- The pointer to file data.
 *  	Length – The length of file data
 *
 *  Exit condition
 *  	EVENT_PROGRAMFIRMWARE event is logged with data1 as SUCCESS_FILESENT_LCDA if the programming is successful else
 *  	the same event with data1 as ERR_FILESEND_LCDA is logged. Processor gets soft reset in any case at the end.
 */
SINT32 FirmwareUpgradeTask::SendFileToLCDAviaBDM(UINT8 * FileDataStart , SINT32 Length)
{
	printf("\n %s;   %08x;    len=%d;  \n", __FUNCTION__, FileDataStart, Length);
	SINT32  RetVal = SUCCESS_FILESEND_LCDA;
	CyGOS::DisableWatchDog();
	CyGOS::Stop();
	CyGOS::KickWatchDog();
	pprintf("\nStart Upload");
	//trim reg values
	*(FileDataStart + 0x7FAE) = 0x00;
	*(FileDataStart + 0x7FAF) = 0x98;
	*(FileDataStart + 0x7FBF) = 0xFE;//Program SEC[1:0] = 1:0 in NVOPT to avoid returning to secure mode
	BDMProgramWithMassEraseAndBlankCheck(FileDataStart);

	if(BDMExitFlag == FALSE) {
		pprintf("\nProgramming Done\n");
		pprintf("\n************** SUCCESSFULLY PROGRAMMED *****************\n");
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FILESEND_LCDA, 0);
	}
	else
	{
		pprintf("\n************** LCD PROGRAM FAILED PLEASE POWER UP THE LCD BOARD *****************\n");
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_FILESEND_LCDA, 0);
	}
	CPU::Restart();
	return RetVal;
}

SINT32 FirmwareUpgradeTask::SendFileToLCDA(UINT8 * FileDataStart, SINT32 Length)
/***************************************************************************************/
/*
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
/******************************************************************************************/
{
	SINT32 RetVal = SUCCESS_FILESEND_LCDA;
	CyGOS::Stop();
	CyGOS::DisableWatchDog();
//	CyGOS::KickWatchDog();
	pprintf("\nStart Upload  ");
	if (BDMExitFlag == FALSE) {
		*(FileDataStart + 0x7FAE) = 0x00;
		*(FileDataStart + 0x7FAF) = 0x98;
		*(FileDataStart + 0x7FBF) = 0xFE;
		BdmProgram(FileDataStart, Length);

		if (BDMExitFlag == FALSE) {
			pprintf("\nProgramming Done\n");
			pprintf(
			      "\n************** SUCCESSFULLY PROGRAMMED *****************\n");
			Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, SUCCESS_FILESEND_LCDA, 0);
		}
	}
	if (BDMExitFlag == TRUE) {
		pprintf(
		      "\n************** LCD PROGRAM FAILED PLEASE POWER UP THE LCD BOARD *****************\n");
		Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_FILESEND_LCDA, 0);
	}
	CPU::Restart();
	return RetVal;
}

void FirmwareUpgradeTask::Run(void) {
	FirmwareUpgradeCommand Cmd;
	SINT32 Sent;
	for (;;) {
		Queue.Read(Cmd, -1);
		UpLoadfile = Cmd.FileData;
		if (Cmd.Type == FileLCDA) {
			Sent = SendFileToLCDAviaBDM(Cmd.FileData, Cmd.FileLength);
			if (Sent == SUCCESS_FILESEND_LCDA)
				SendFirmwareUpgradeProgressToWebsite(UPGRADECOMPLETEMSG);
			else
				SendFirmwareUpgradeProgressToWebsite(UPGRADEFAILMSG);
		}
		if (UpLoadfile) {
			delete UpLoadfile;
			UpLoadfile = 0;
		}
	}
}
