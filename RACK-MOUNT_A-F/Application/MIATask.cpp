/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/MIATask.cpp_v   1.2   24 Jul 2015 08:37:22   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/MIATask.cpp_v  $
 * 
 *    Rev 1.2   24 Jul 2015 08:37:22   sesharma
 * Added function EnableDigitalOutputLatch for all three boards.
 * 
 *    Rev 1.1   24 Jun 2015 08:36:54   amaurya
 * Added Digital OP enabling in Run()
 * 
 *    Rev 1.0   09 Jun 2015 09:10:30   tayars
 * Initial revision.
 * 
 *    Rev 1.11   17 Sep 2013 04:14:02   rjamloki
 * Added Task Manager again and reduced time for WatchDogTimer.
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
		CyGOS::KickWatchDog();
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



/*  void EnableDigitalOutputLatchSC1054(void)
 *
 *  Purpose:
 *    This function configures pin Y3 of CPU on SC1054 board as output and
 *    drive the same low to enable digital output latch.
 *    It is called once from Run () function of MIATask.
 *
 *  Entry Condition:
 * 	Power Up
 *
 *  Exit Condition:
 * 	Digital Outputs Get Enabled
*/
void EnableDigitalOutputLatchSC1054(void)
{
   //Drive Output Enable low to enable the outputs
   MCF_PAD_PAR_FBCTL &= (~MCF_PAD_PAR_FBCTL_PAR_TS(3));   //configure Y3 as GPIO
   MCF_GPIO_PDDR_FBCTL |= MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0;   // configure data direction as out
   MCF_GPIO_PODR_FBCTL &= ~MCF_GPIO_PODR_FBCTL_PODR_FBCTL0;   //Drive low to enable latch;
   //Clear outputs after enabling so their default state isn't random
   UserIO::ClearOutputs();
}



/*  void EnableDigitalOutputLatchWC991(void)
 *
 *  Purpose:
 *    To create a placeholder for function pointer assignment.
 *    Function does nothing as no Digital Output Latch present on WC991 board.
 *
 *  Entry Condition:
 * 	Power Up
 *  Exit Condition:
 * 	None.
*/
void EnableDigitalOutputLatchWC991(void)
{
	//Do nothing as no Digital Output Latch present on WC991 board.
}



/*  void EnableDigitalOutputLatchWC1169(void)
 *
 *  Purpose:
 *    To create a placeholder for function pointer assignment.
 *    Function does nothing as no Digital Output Latch present on WC1169 board.
 *
 *  Entry Condition:
 * 	Power Up
 *
 *  Exit Condition:
 * 	None.
*/
void EnableDigitalOutputLatchWC1169(void)
{
	//Do nothing as no Digital Output Latch present on WC1169 board.
}



