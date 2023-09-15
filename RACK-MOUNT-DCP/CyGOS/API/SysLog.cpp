// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/API/SysLog.cpp_v   1.0   18 Aug 2014 09:15:24   amaurya  $
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
	if((sysLogCallBack))
		sysLogCallBack(eid, data1, data2, data3,valid);
}

