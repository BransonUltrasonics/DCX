/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WelderSearchTask.h_v   1.1   04 Apr 2011 10:56:10   PDwivedi  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WelderSearchTask.h_v  $
 * 
 *    Rev 1.1   04 Apr 2011 10:56:10   PDwivedi
 * Added comments, defines and code cleanup
 * 
 *    Rev 1.0   02 Mar 2011 08:14:44   PDwivedi
 * Initial revision.
*/

#include "Task.h"
#include "IPv4.h"
#include "LinkedList.h"
#include "Fifo.h"
#include "Socket.h"
#include "Ethernet.h"
#include "SysConfig.h"

#ifndef WELDERSEARCHTASK_H_
#define WELDERSEARCHTASK_H_

#define WELDERSEARCH_UDP_PORT 49500//UDP port on which the welder info will be broadcasted
#define WELDERSEARCH_CHECKSUM 0xABEA//This checksum ensures that we dont receive some junk on port 49500

//Possible messages to be processed by welder
//search task Run().
enum SearchMsg
{
	errMsg,
	broadcastMsg,
	refreshMsg,
};
struct WelderInfoParam
{
	struct
	{
		UINT16 checkSum;
		UINT8 SeriallNumber[SIZE_SERAILNUMBER];
	}WelderInfo;//the information about welder which needs to be broadcasted
	ip_addr ipaddr;
	SINT32 searchTimeOut;
};

class WelderSearchTask: public Task
{
public:
	WelderSearchTask(int intf);
	static WelderSearchTask * thiPtr;
	static int GetWelderCount();
	static void GetWeldersInformation(WelderInfoParam * InfoPtr);
protected:
	virtual void Run();
	void Tick();
	void AddWelderInfo(WelderInfoParam info);
	void RefreshWelderInfo();
	void BroadcastwelderInfo();
	void CreateAndBindSocket();
	void CloseSocket();
	void PrepareFDSets();
	int NonBlock();
	bool Poll();
	Ethernet * netif;
	int fd;
	fd_set input_set;
	fd_set exc_set;
	Fifo<SearchMsg> msgQ;
	PtrList<WelderInfoParam> welderInfoList;
};


#endif /* TESTUDPSELECT_H_ */
