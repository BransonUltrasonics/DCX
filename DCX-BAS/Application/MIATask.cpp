/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/MIATask.cpp_v   1.11   17 Sep 2013 04:14:02   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/MIATask.cpp_v  $
 * 
 *    Rev 1.11   17 Sep 2013 04:14:02   rjamloki
 * Added Task Manager again and reduced time for WatchDogTimer.
 * 
 *    Rev 1.10   14 Sep 2013 07:30:36   rjamloki
 * WatchDog Cant be disabled.
 * 
 *    Rev 1.8   10 Sep 2013 03:46:38   ygupta
 * WC revision 6.A.
 * 
 *    Rev 1.7   03 Sep 2013 12:48:00   ygupta
 * Moved to tip from 1.5.1.0
 * 
 *    Rev 1.5.1.0   24 Aug 2013 17:14:04   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.5   17 Jul 2013 06:20:00   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.4   27 Jun 2013 05:56:04   amaurya
 * System crash fixes.
 * 
 *    Rev 1.3   21 May 2013 12:25:54   amaurya
 * Code review fixes.
 * 
 *    Rev 1.2   11 Sep 2012 10:12:24   rjamloki
 * User IO update and clean up
 * 
 *    Rev 1.1   10 Sep 2012 03:12:10   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   02 Jul 2012 12:46:44   ygupta
 * Initial revision.
 *
 */

#include "MIATask.h"
#include "GLOBALS.h"
#include "FUNC.h"

MIATask * MIATask::thisPtr;
void FbTaskMSTick(void);

MIATask::MIATask() :
	Task("MIATask", MIATASK_STACKSIZE), MIATaskQ(1, "MIATaskQ"), OutQ(2, "MIADACQ")
{
	thisPtr = this;
}
/*   void MIATask::Run()
 *
 *   Purpose:
 *   	This is the main forever loop of MIA task. It reads digital and analog signals from MIA board
 *   whenever invoked by timer interrupt handler(currently 1000us).
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void MIATask::Run()
{
	char I;
	DelayMs(100);
	CyGOS::KickWatchDog();
	for (;;){
		//CyGOS::KickWatchDog();
		MIATaskQ.Read(this, I, -1);
		if (I == 'i')
		{
			UserIO::ReadInputs();
		    FbTaskMSTick();
			WCTask::thisPtr->WCTaskQ.Write(0, 'd', 0);
		}
		else if (I == 'b')
			UserIO::WriteOutputs();
	}
}
