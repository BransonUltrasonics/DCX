/*
$Header:
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
 * $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ShutDownTask.cpp_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:50   tayars
 * Initial revision.
 * 
 *    Rev 1.6   14 Sep 2013 07:30:36   rjamloki
 * WatchDog Cant be disabled.
 * 
 *    Rev 1.5   03 Sep 2013 12:55:50   ygupta
 * Moved to tip from 1.3.1.0
 * 
 *    Rev 1.3.1.0   24 Aug 2013 17:14:10   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.3   17 Jul 2013 06:21:10   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.2   31 May 2013 10:37:18   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.1   22 Jan 2013 10:38:32   ygupta
 * Profibus Bug Fixes
 */

#include "ShutDownTask.h"
#include "CyGOS.h"
#include "LOG.h"
#include "CPU.h"
#include "GLOBALS.h"
#include "FUNC.h"
//Static variable definition
BOOLEAN ShutDownTask::RTOSShutDown;


/*  ShutDownTask Constructor
 *
 *  Purpose:
 *    This is the constructor of ShutDownTask class.
 *      It does initializes the static variables of class to default.
 *
 *  Entry condition:
 *
 *  Exit condition: None.
 */
ShutDownTask::ShutDownTask():Task("Shut Down Task", 1024)
{
   RTOSShutDown = FALSE;
}

/*  void ShutDownTask::Run(void)
 *
 *  Purpose:
 *    This function implements the forever loop of ShutDownTask Task.
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
 *    This function checks a global flag to stop the CyGOS and
 *    watchdog once the external power is lost. Being a low priority
 *    task it ensures the data integrity while stooping all other tasks.
 *     This function is called by forever loop of Shutdown task
 *
 *  Entry condition: None.
 *
 *  Exit condition: 24 volts is restored..
 */
void ShutDownTask::ShutDown(void)
{
   CyGOS::DisableWatchDog();
   Log::WriteEventLog(EVENT_POWER_SHUT_DOWN);
   CyGOS::Stop();
   pprintf("\n\n Shutting Down the RTOS. We have lost 24V\n\n ");
   while(!CHECK_FOR_24V){CyGOS::KickWatchDog();};
   // Wait for 24V to return then restart processor.
   CPU::Restart();
}
