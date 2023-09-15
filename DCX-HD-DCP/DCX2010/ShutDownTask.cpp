/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.cpp_v   1.2.1.0   17 Mar 2016 16:23:44   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.cpp_v  $
 * 
 *    Rev 1.2.1.0   17 Mar 2016 16:23:44   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.3   17 Mar 2016 16:07:24   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.2   13 Feb 2013 09:21:20   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.0   29 Aug 2011 11:40:08   ygupta
 * Initial revision.
*/

#include "ShutDownTask.h"
#include "stdio.h"
#include "CyGOS.h"
#include "TYP_DEF.h"
#include "LOG.h"

BOOLEAN ShutDownTask::RTOSShutDown;
SINT32 ShutDownTask::AlarmReasons;
ShutDownTask::ShutDownTask():Task("Shut Down Task", 1024)
{
	RTOSShutDown = FALSE;
	AlarmReasons = 0;
}

/*This function first checks for data on UDP port WELDERSEARCH_UDP_PORT.
 * and process it accordingly. It then processes the msgQ.
 */
void ShutDownTask::Run()
{
	DelayMs(100);
	pprintf("\n ShutDownTask::Run()");
	for(;;)
	{
		CheckForShutDown();
		DelayMs(1);
	}
}

void ShutDownTask::CheckForShutDown()
{
	if(RTOSShutDown == TRUE)
	{
		Log::WriteEventLog(POWER_SHUTDOWN, (unsigned short)(AlarmReasons >> 16) , (unsigned short)AlarmReasons, 0, false);
		pprintf("\n Shutting Down the RTOS.Multiple Alarms %d\n ",AlarmReasons);
		CyGOS::DisableWatchDog();
		CyGOS::Stop();

		while(!CHECK_FOR_15V){};
		// Wait for 15V to return then restart processor.
		MCF_RCM_RCR |= MCF_RCM_RCR_SOFTRST;
	}
}


