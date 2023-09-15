/* $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FirmwareUpgrade.h_v   1.0.1.0   17 Mar 2016 16:23:40   hasanchez  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FirmwareUpgrade.h_v  $
 * 
 *    Rev 1.0.1.0   17 Mar 2016 16:23:40   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.1   17 Mar 2016 16:07:20   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.0   19 Sep 2014 06:49:48   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
*/


#ifndef FIRMWAREUPGRADE_H_
#define FIRMWAREUPGRADE_H_
#include "Task.h"
#include "IPv4.h"
#include "Fifo.h"
#include "FirmWareUpgrade.h"
#include "portable.h"

#define SUCCESS_FILESEND_LCDA 			9
#define ERR_FILESEND_LCDA 				19
#define ERR_FAILEDDCPCONNECTION 		17
#define ERR_ANOTHERUPGRADEINPROGRESS 	16

#define FIRMWAREUPGRADETASK_STACKSIZE 	1024
#define PROGRESS_STRING_SIZE 	200

#define UPGRADEALREADYINPROGRESS 		"An Upgrade already in progress"
#define UPGRADEFAILMSG 		 			"Upgrade failed"
#define UPGRADEINPROGRESSMSG 			"Upgrade in progress"
#define UPGRADECOMPLETEMSG 	 			"Upgrade completed"
enum BinFileType
{
	FileErr,
	FileDCPAppBin,
	FileLCDA,
	FileDCPCompleteBin,
};
struct FirmwareUpgradeCommand
{
	ip_addr NodeIP;
	UINT16  ServerPort;
	BinFileType Type;
	UINT32 FileLength;
	UINT8  * FileData;
};

struct UpgradeProgress
{
	SINT8 ProgressInfo[PROGRESS_STRING_SIZE];
};


class FirmwareUpgradeTask :public Task
{
public:
	FirmwareUpgradeTask(int Intf);
	~FirmwareUpgradeTask()
	{}
	void FUWriteQueue(const FirmwareUpgradeCommand &Cmd);
	UpgradeProgress ProgInfo;
	static FirmwareUpgradeTask * thisPtr;
protected:
	void Run(void);
	SINT32 FUClientSocket;
	SINT32 SendFileToLCDA(UINT8 * FileDataStart , SINT32 Length);
	void SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog);
	Fifo<FirmwareUpgradeCommand> Queue;
	UINT8 * UpLoadfile;
	Ethernet * NetIf;
};

#endif /* FIRMWAREUPGRADE_H_ */
