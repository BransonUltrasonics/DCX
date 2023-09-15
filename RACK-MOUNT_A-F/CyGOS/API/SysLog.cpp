// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/API/SysLog.cpp_v   1.1   24 Jul 2015 09:01:48   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements System Logs
#include "CyGOS.h"
LogCallback sysLogCallBack = 0;
BoardTestCallback BoardTestCallBack = 0;

//An object of this structure keeps the event description
//in string format corresponding to an system event ID
struct SystemEvent
{
	char * eventStr;
	int eid;
};

const SystemEvent sysEvents[] =
{
		{"System Fault", FAULT_ERROR},
		{"CRLF not found", CRLF_INVALID},
		{"Invalid Length", LEN_INVALID},
		{"CPU restart from SW", CPU_RESTART},
		{"Link Up", LINK_UP},
		{"Link Down", LINK_DOWN},
		{"File Upload Aborted", UPLOAD_ABORT},
		{"Assert Error",ASSERT_ERROR},
		{"Crash Error",CRASH_ERROR},
		{"Watchdog Error", WD_ERROR},		
		{"String Length Error", STRLENGTH_ERROR},
		{"Allocation Failure", ALLOCATION_ERROR},
		{"IP Config Changed", EVENT_IPCONFIG_CHANGED},
		{"Eth. Link Status Changed", EVENT_NETIF},
		{"Task Load", EVENT_TASKLOAD},
		{"Int Load", EVENT_INTLOAD},
		{0,0},
};
void CyGOS::RegisterBoardTestCallback(BoardTestCallback callback)
{
	BoardTestCallBack = callback;
}
void CyGOS::DOBoardTest()
{
	if(BoardTestCallBack)
		BoardTestCallBack();
}

void CyGOS::RegisterSysLogCallback(LogCallback callback)
{
	sysLogCallBack = callback;
}

/*function is called upon some system event which application code may use*/
//@eid: aystem event ID
//@data1: data related to event
//@data2: data related to event
//@data3: data related to event
//@valid: Is data3 is meaningful or not
/**/
void CyGOS::WriteSystemEvent(int eid, int data1, int data2,unsigned int data3,bool valid)
{
	if(sysLogCallBack)
		sysLogCallBack(eid, data1, data2, data3,valid);
}

char * CyGOS::GetSysEventStrigFromID(int eid)
{
	static char EventString[50];
	int indx = 0;
	strcpy(EventString , "Undefined");
	while(sysEvents[indx].eventStr)
	{
		if(eid == sysEvents[indx].eid)
		{
			if(strlen(sysEvents[indx].eventStr) < 50)
				strcpy(EventString , (sysEvents[indx].eventStr));
			else
			     strcpy(EventString , "SysEvent Invalid Length");
			break;
		}
		indx++;
	}
	return EventString;
}
