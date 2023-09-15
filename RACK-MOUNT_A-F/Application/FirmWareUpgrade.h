/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FirmWareUpgrade.h_v   1.2   02 Jul 2015 10:45:06   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FirmWareUpgrade.h_v  $
 * 
 *    Rev 1.2   02 Jul 2015 10:45:06   RJamloki
 * Reverted to initial revision as we don’t need to support firmware file checksums at end
 * 
 *    Rev 1.1   25 Jun 2015 05:45:18   rjamloki
 * Added Offset variable in FirmwareUpgradeCommand structure to keep track of programming data start.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.6   14 Jun 2013 11:21:08   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.5   22 Apr 2013 11:30:02   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.4   15 Mar 2013 10:48:26   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.3   13 Dec 2012 00:09:04   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.2   10 Sep 2012 03:11:36   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.1   02 Jul 2012 13:04:08   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.0   21 Mar 2012 12:20:28   PDwivedi
 * Initial revision.
 *
 */

#ifndef FIRMWAREUPGRADE_H_
#define FIRMWAREUPGRADE_H_

#include "Task.h"
#include "IPv4.h"
#include "Fifo.h"
#include "FirmWareUpgrade.h"
#include "portable.h"

enum BinFileType
{
	FileErr,
	FileDCPAppBin,
	FileLCDA,
	FileDCPCompleteBin,
};
#define PROGRESS_STRING_SIZE 	200
#define SUCCESS_FILESEND_DCP  	8
#define SUCCESS_FILESEND_LCDA 	9
#define ERR_CREATESOCKETDCP  	10
#define ERR_UPGRADESTARTCOMMANDSEND 12
#define ERR_UPGRADESTARTREJECT 	13
#define ERR_UPGRADEPACKETSEND 	14
#define ERR_FIRMWAREFILERECEIVEDCONFIRMATION 15
#define ERR_ANOTHERUPGRADEINPROGRESS 	16
#define ERR_FAILEDDCPCONNECTION 17
#define ERR_FILESEND_LCDA 	19

#define FIRMWAREUPGRADE_SRCPORT 7074
#define FIRMWAREPKTSIZE_DCP  2048
#define FIRMWAREUPGRADETASK_STACKSIZE 1024

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


class FirmWareUpgradeTask :public Task
{
	enum FUPacketType
	{
		Zero = 0,
		UpgradeStartAppBin,
		UpgradeConfirm,
		UpgradeErrorInvalidFileLength,
		UpgradeErrorTimeOut,
		UpgradeErrorInvalidRequest,
		UpgradeFileRecCompleted,
		UpgradeStartCompleteBin,
	};

	struct FUPacket
	{
		FUPacketType Pkttype;
		UINT32 FileLength;
	};
public:
	FirmWareUpgradeTask(int Intf);
	~FirmWareUpgradeTask()
	{}
	void FUWriteQueue(const FirmwareUpgradeCommand &Cmd);
	UpgradeProgress ProgInfo;
	static FirmWareUpgradeTask * thisPtr;
protected:
	void Run(void);
	SINT32  SendFileToNode(ip_addr Nodeip, UINT16 Port, UINT8 * FileDataStart, SINT32 Length, FUPacketType PktType);
	SINT32 SendFileToLCDA(UINT8 * FileDataStart , SINT32 Length);
	SINT32 FUClientSocket;
	void SendFirmwareUpgradeProgressToWebsite(SINT8 * Prog);
	BOOLEAN CreateUpgradeSocket();
	Fifo<FirmwareUpgradeCommand> Queue;
	UINT8 * UpLoadfile;
	Ethernet * NetIf;
};
#endif
