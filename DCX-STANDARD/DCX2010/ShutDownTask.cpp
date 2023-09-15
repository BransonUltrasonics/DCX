/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.cpp_v   1.0.2.3   13 Jun 2014 08:46:06   amaurya  $
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
 *    Rev 1.0.2.3   13 Jun 2014 08:46:06   amaurya
 * Added AlarmReasons
 * 
 *    Rev 1.0.2.2   02 Apr 2014 05:05:56   rjamloki
 * Soft reset the CPU if 24 volt is restored after detecting 24 Volt Lost from sensor.
 * 
 *    Rev 1.0.2.1   28 Mar 2014 04:39:44   rjamloki
 * Multiple Power down events prevented. Made ACPwrpresent default value to false to force ADC initialization.
 * 
 *    Rev 1.0.2.0   09 Sep 2013 08:59:54   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.0   29 Aug 2011 11:40:08   ygupta
 * Initial revision.
*/

#include "ShutDownTask.h"
#include "stdio.h"
#include "CyGOS.h"
#include "LOG.h"
#include "CPU.h"
#include "GLOBALS.h"
#include "IOTask.h"
//Static variable definition
BOOLEAN ShutDownTask::RTOSShutDown;
SINT32 ShutDownTask::AlarmReasons;
/*  ShutDownTask Constructor
 *
 *  Purpose:
 *    This is the constructor of ShutDownTask class.
 * 	  It does initializes the static variables of class to default.
 *
 *  Entry condition:
 *
 *  Exit condition: None.
 */
ShutDownTask::ShutDownTask():Task("Shut Down Task", 1024)
{
	RTOSShutDown = FALSE;
	AlarmReasons = 0;
}

/*  void ShutDownTask::Run(void)
 *
 *  Purpose:
 *	   This function implements the forever loop of ShutDownTask Task.
 *     It calls a function every ms to check whether the DCP board
 *     has lost external power.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void ShutDownTask::Run(void)
{
	DelayMs(100);

	for(;;){
		if(RTOSShutDown == TRUE)
			ShutDown();
		DelayMs(1);
	}
}

/*  void ShutDownTask::ShutDown(void)
 *
 *  Purpose:
 *	   This function checks a global flag to stop the CyGOS and
 *	   watchdog once the external power is lost. Being a low priority
 *	   task it ensures the data integrity while stooping all other tasks.
 *     This function is called by forever loop of Shutdown task
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void ShutDownTask::ShutDown(void)
{
   Log::WriteEventLog(POWER_SHUTDOWN, (unsigned short)(AlarmReasons >> 16) , (unsigned short)AlarmReasons, 0, false);
   CyGOS::DisableWatchDog();
   CyGOS::Stop();
   pprintf("\n Shutting Down the RTOS. We have lost 24V\n ");
   while(!CHECK_FOR_24V){};
   // If 24 volts gets returned and we are here. Don't do any thing until next power up
   //Other solution can be a soft reset but we are avoiding this.(At some point during DCP software
   //development we decided that there should be no soft reset from code).
   //while(1);
   MCF_RCM_RCR = 0x80;
}

/*  void CheckForPowerDown(void)
 *  Purpose:
 *    This function checks the 24V signal is present or not.
 *    In case the signal is not present it sets the shutdown
 *    flag to TRUE then the shiutdown process is haldled by
 *    Shutdown Task.
 *    This function is called every millisecond from the forever
 *    loop of DupsTask
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void CheckForPowerDown(void)
{
   //To avoid multiple log entries in the case power supply goes off
   //before DCP board we are checking that there should not be more than
   //1 overloadBit high at the same time.
   UINT8 SkipLog = 0;
   UINT8 Indx = 0;
   static BOOLEAN ACPwrPresent = FALSE;
   static BOOLEAN MultipleAlarms = FALSE;
   if(ShutDownTask::RTOSShutDown == FALSE){
      if((!CHECK_FOR_AC) && (ACPwrPresent == TRUE)){
         //Instrumentation board has lost the power.Set the flag ACPwrPresent to FALSE to avoid multiple event log entries.
         ACPwrPresent = FALSE;
      }
      else if((CHECK_FOR_AC) && (ACPwrPresent == FALSE)){
         //Instrumentation board has got the power again.Set the flag ACPwrPresent to TRUE to avoid multiple event log entries.
         ACPwrPresent = TRUE;
         AdcReInitRequired = TRUE;
         pprintf("\n Got AC.");
      }

      if(!CHECK_FOR_24V){
         // Set the flag and ShutDownTask will take care of this when ever it will get the time.
         ShutDownTask::RTOSShutDown = TRUE;
         ShutDownTask::AlarmReasons = ErrorReason;
         //Stop Timer so that no high priority task(IO and Dups) gets invoked from Timer Interrupt and
         //Shut down task get time as soon as possible.
        // printf("\n timer stopped");
         CyGOS::StopTimer(1);
      }
   }
}
