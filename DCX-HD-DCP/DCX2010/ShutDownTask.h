/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.h_v   1.1.1.0   17 Mar 2016 16:23:46   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.h_v  $
 * 
 *    Rev 1.1.1.0   17 Mar 2016 16:23:46   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.2   17 Mar 2016 16:07:24   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.1   16 Dec 2011 15:09:10   tayars
 * Changed power down routine to accomodate the 814 board which is to be released. We now sense 15V directly to the processor.
 * 
 *    Rev 1.0   29 Aug 2011 11:40:08   ygupta
 * Initial revision.
*/

#include "Task.h"
#include "portable.h"

#ifndef SHUTDOWNTASK_H_
#define SHUTDOWNTASK_H_

#define CHECK_FOR_15V	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)

class ShutDownTask: public Task
{
public:
	ShutDownTask();
	static BOOLEAN RTOSShutDown;
	static SINT32 AlarmReasons;
protected:
	virtual void Run();
	void CheckForShutDown();

};


#endif /* TESTUDPSELECT_H_ */
