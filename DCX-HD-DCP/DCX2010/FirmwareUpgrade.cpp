/* $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FirmwareUpgrade.cpp_v   1.0.1.0   17 Mar 2016 16:23:38   hasanchez  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FirmwareUpgrade.cpp_v  $
 * 
 *    Rev 1.0.1.0   17 Mar 2016 16:23:38   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.1   17 Mar 2016 16:07:20   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.0   19 Sep 2014 06:49:48   amaurya
 * Initial revision.
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
#include "CPU.h"
#include "FIFO.h"

// static variable definition
// This is the global pointer to class. UploadFirmware module uses this pointer to call
// the FUWriteQueue() function of this Task while sending binary file.
FirmwareUpgradeTask * FirmwareUpgradeTask::thisPtr;
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
FirmwareUpgradeTask::FirmwareUpgradeTask(int Intf)
					: Task("FMUpgrade", FIRMWAREUPGRADETASK_STACKSIZE), Queue(1, "FirmWareUpgrade")
{
	UpLoadfile = NULL;
	thisPtr = this;
	NetIf = CyGOS::GetNetworkInterface(Intf);
	memset(&ProgInfo, 0, sizeof(UpgradeProgress));
	FUClientSocket = 0;
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
void FirmwareUpgradeTask::SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog)
{
	Prog = 0;
}

void FirmwareUpgradeTask::FUWriteQueue(const FirmwareUpgradeCommand &Cmd)
{
	 printf(" received firmware upgrade command");
	 if(!Queue.Write(Cmd, 0))
		 Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, ERR_ANOTHERUPGRADEINPROGRESS,0);

	 SendFirmwareUpgradeProgressToWebsite(UPGRADEALREADYINPROGRESS);
}

void FirmwareUpgradeTask::Run(void){
	FirmwareUpgradeCommand Cmd;
	int Sent;
	for(;;){
		Queue.Read(Cmd, -1);
		UpLoadfile = Cmd.FileData;
		if(Cmd.Type == FileLCDA){
			Sent = 0; //SendFileToLCDA(Cmd.FileData, Cmd.FileLength);
			if(Sent == SUCCESS_FILESEND_LCDA)
				SendFirmwareUpgradeProgressToWebsite(UPGRADECOMPLETEMSG);
			else
				SendFirmwareUpgradeProgressToWebsite(UPGRADEFAILMSG);
		}
		if(UpLoadfile){
			delete UpLoadfile;
			UpLoadfile = 0;
		}
	}
}
