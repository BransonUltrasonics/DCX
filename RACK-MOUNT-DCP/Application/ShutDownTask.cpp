/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ShutDownTask.cpp_v   1.6   Dec 06 2016 15:54:50   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ShutDownTask.cpp_v  $
 * 
 *    Rev 1.6   Dec 06 2016 15:54:50   EGUAJARDO
 * Added a flag to perform a power up initialization when AC voltage comes up again.
 * 
 *    Rev 1.5   06 Jun 2016 15:28:34   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.4   04 Jun 2015 07:50:08   sesharma
 * AC Power lost alarm logged when AC power i.e. 240V is lost.Branson coding standard formatter applied.
 * 
 *    Rev 1.3   19 Mar 2015 07:03:12   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.2   01 Oct 2014 10:08:08   amaurya
 * Removed unused variable.
 * 
 *    Rev 1.1   17 Sep 2014 08:41:22   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#include "ShutDownTask.h"
#include "stdio.h"
#include "CyGOS.h"
#include "LOG.h"
#include "CPU.h"
#include "GLOBALS.h"
#include "IOTask.h"
#include "CommandsFT.h"
#include "FUNC.h"
//Static variable definition
BOOLEAN ShutDownTask::RTOSShutDown;
SINT32 ShutDownTask::AlarmReasons;

ShutDownTask::ShutDownTask() :
      Task("Shut Down Task", 1024)
/*******************************************************************************************************************************/
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
/*******************************************************************************************************************************/
{
   RTOSShutDown = FALSE;
   AlarmReasons = 0;
}

void ShutDownTask::Run(void)
/*******************************************************************************************************************************/
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
/*******************************************************************************************************************************/
{
   DelayMs(100);

   for (;;) {
      if (RTOSShutDown == TRUE) ShutDown();
      DelayMs(1);
   }
}

void ShutDownTask::ShutDown(void)
/*******************************************************************************************************************************/
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
/*******************************************************************************************************************************/
{
   if(CommandsFT::Status==CommandsFT::FT_OFF) {
   Log::WriteEventLog(POWER_SHUTDOWN, (AlarmReasons >> 16),
          AlarmReasons, 0, false);
   CyGOS::DisableWatchDog();
   CyGOS::Stop();
   pprintf("\n Shutting Down the RTOS. We have lost 24V\n ");
   while (CHECK_FOR_24V!=TWENTYFOURVOLTSPRESENT) {
   };
   // If 24 volts gets returned and we are here. Don't do any thing until next power up
   //Other solution can be a soft reset but we are avoiding this.(At some point during DCP software
   //development we decided that there should be no soft reset from code).
   //while(1);
   MCF_RCM_RCR = 0x80;
   }
}

void CheckForPowerDown(void)
/*******************************************************************************************************************************/
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
/*******************************************************************************************************************************/
{
   //To avoid multiple log entries in the case power supply goes off
   //before DCP board we are checking that there should not be more than
   //1 overloadBit high at the same time.
   static BOOLEAN ACPwrPresent = FALSE;
   static BOOLEAN AcPwrCheckAtPowerUp = TRUE;   //Need another flag for logging alarm at power up
                                                //AcPwrpresent default value shall remain FALSE to make sure that
                                                //Adc get initialized initially when ac power is present at power up
   static BOOLEAN ACPwrPowerUpSequenceFlag = FALSE; //To perform a Power up initialization when AC is gotten again
   if (ShutDownTask::RTOSShutDown == FALSE) {
      if ((!CHECK_FOR_AC)
            && ((ACPwrPresent == TRUE) || (AcPwrCheckAtPowerUp == TRUE))) {
         //Instrumentation board has lost the power.Set the flag ACPwrPresent to FALSE to avoid multiple event log entries.
         ACPwrPresent = FALSE;
         ErrorReason |= AC_POWER_LOST;
         AcPwrCheckAtPowerUp = FALSE;
         ACPwrPowerUpSequenceFlag = TRUE;
         pprintf("\n Lost AC.");
      }
      else if ((CHECK_FOR_AC) && (ACPwrPresent == FALSE)) {
         //Instrumentation board has got the power again.Set the flag ACPwrPresent to TRUE to avoid multiple event log entries.
         ACPwrPresent = TRUE;
         AdcReInitRequired = TRUE;
         if(ACPwrPowerUpSequenceFlag == TRUE)			// When AC voltage gets back again it performs a power up initialization
         {
         	ACPwrPowerUpSequenceFlag = FALSE;
         	PowerUpInitilization();
         }
         pprintf("\n Got AC.");
      }

      if (CHECK_FOR_24V!=TWENTYFOURVOLTSPRESENT) {
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
