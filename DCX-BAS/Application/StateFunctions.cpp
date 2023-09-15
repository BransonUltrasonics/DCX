/*
 $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateFunctions.cpp_v   1.42   Feb 08 2017 17:33:02   EGUAJARDO  $
 */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*   Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        	*/
/*   This program is the property of Branson Ultrasonics Corporation      	*/
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateFunctions.cpp_v  $
 * 
 *    Rev 1.42   Feb 08 2017 17:33:02   EGUAJARDO
 * Added the capability to run a seek or scan depending the power up option on Seek power up page when the AC line is restored again.
 * 
 *    Rev 1.41   03 Oct 2016 17:44:24   hasanchez
 * Updated function related to the test amplitude.
 * 
 *    Rev 1.40   26 Sep 2016 14:14:32   hasanchez
 * Updated the functions to handle the stepping and request while doing Test function.
 * 
 *    Rev 1.39   19 Sep 2016 12:32:26   hasanchez
 * Updated Test function to handle the stepping during test.
 * 
 *    Rev 1.38   30 Jul 2015 08:10:24   RJamloki
 * ScanExit modified to decide memory offset and digital tune based on SetDigTuneWithHornScan 
 * 
 *    Rev 1.37   29 Jul 2015 07:54:36   RJamloki
 * Changes for Clear memory with reset and clear memory before seek functionality
 * 
 *    Rev 1.36   21 Mar 2014 09:25:52   rjamloki
 * Tracker issue fixes.
 * 
 *    Rev 1.35   13 Mar 2014 15:55:04   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.34   06 Mar 2014 07:28:58   rjamloki
 * Changes for DCP preset update.
 * 
 *    Rev 1.33   04 Mar 2014 07:49:24   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.32   15 Nov 2013 05:51:48   rjamloki
 * Export log fixes, Preready clearing the web command for statemachine, set up alarm implemented for invalid preset.
 * 
 *    Rev 1.31   06 Nov 2013 06:20:30   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.30   25 Oct 2013 11:59:14   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.29   15 Oct 2013 07:42:12   rjamloki
 * Tracker Fix, JSON Fix, Link Status debouncing configurable from Adv R&D page.
 * 
 *    Rev 1.28   08 Oct 2013 06:17:30   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.27   03 Oct 2013 04:26:34   rjamloki
 * Watchdog related Changes
 * 
 *    Rev 1.26   05 Sep 2013 13:30:38   rjamloki
 * Alarm Code Fixes,LCD Version Dispaly Fix For SystemConfig Page,AB Delay and AB Active Synced
 * 
 *    Rev 1.25   04 Sep 2013 13:11:42   rjamloki
 * Removed Test Offset
 * 
 *    Rev 1.24   03 Sep 2013 12:56:48   ygupta
 * Moved to tip from 1.22.1.25.1.3
 * 
 *    Rev 1.22.1.25.1.3   03 Sep 2013 11:14:06   amaurya
 * Tracker fixes and Synchronization fixes for User Outputs.
 * 
 *    Rev 1.22.1.25.1.2   30 Aug 2013 15:01:34   amaurya
 * Fixed tracker issue 521,525,526,527,529,531.
 * 
 *    Rev 1.22.1.25.1.1   28 Aug 2013 12:07:10   amaurya
 * Fixed tracker issue along Restore Default of Advanced Diagnostics.
 * 
 *    Rev 1.22.1.25.1.0   24 Aug 2013 17:14:10   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.22.1.25   18 Jul 2013 12:20:00   amaurya
 * Frequency output fixed.
 * 
 *    Rev 1.22.1.24   17 Jul 2013 06:21:20   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.22.1.23   10 Jul 2013 07:52:08   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.22.1.22   08 Jul 2013 08:54:54   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.22.1.21   27 Jun 2013 05:56:48   amaurya
 * System crash fixes.
 * 
 *    Rev 1.22.1.20   14 Jun 2013 11:21:10   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.22.1.19   11 Jun 2013 09:51:12   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.22.1.18   07 Jun 2013 10:19:10   amaurya
 * MH testing round I fixes.
 * 
 *    Rev 1.22.1.17   05 Jun 2013 03:11:14   amaurya
 * Fixed Passcode and Limits issue.
 * 
 *    Rev 1.22.1.16   03 Jun 2013 08:46:00   amaurya
 * Fixed Sort by Date/Time on History page and reset overload after horn disconnect.
 * 
 *    Rev 1.22.1.14   28 May 2013 07:43:52   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.22.1.13   24 May 2013 10:16:12   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.22.1.12   22 May 2013 10:41:38   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 * 
 *    Rev 1.22.1.11   21 May 2013 12:25:58   amaurya
 * Code review fixes.
 * 
 *    Rev 1.22.1.9   07 May 2013 10:13:34   amaurya
 * Code review fixed.
 * 
 *    Rev 1.22.1.8   06 May 2013 09:18:46   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.22.1.7   30 Apr 2013 11:20:40   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.22.1.6   26 Apr 2013 11:03:38   amaurya
 * Fixed traker issue for Ver2.0U.
 * 
 *    Rev 1.22.1.5   23 Apr 2013 10:24:32   ygupta
 * Fixed issue of System Test page on website.
 * 
 *    Rev 1.22.1.4   22 Apr 2013 11:31:58   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.22.1.3   16 Apr 2013 11:17:04   amaurya
 * Fixed tracker issues in Ver2.0R
 * 
 *    Rev 1.22.1.2   10 Apr 2013 10:32:22   ygupta
 * Added and fixed EQ Alarm generation
 * 
 *    Rev 1.22.1.1   08 Apr 2013 08:25:14   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.22.1.0   20 Mar 2013 09:38:54   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.23   20 Mar 2013 08:18:56   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.22   15 Mar 2013 10:52:12   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.21   11 Mar 2013 02:28:48   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.20   18 Feb 2013 03:10:16   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.19   29 Jan 2013 02:22:26   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.18   22 Jan 2013 10:38:32   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.17   17 Jan 2013 07:26:10   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.16   16 Jan 2013 06:29:26   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.15   11 Jan 2013 02:53:20   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.14   03 Jan 2013 07:18:58   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.13   28 Dec 2012 10:11:08   ygupta
 * RTC bug fix
 * 
 *    Rev 1.12   27 Dec 2012 06:45:36   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.11   18 Dec 2012 07:38:38   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.9   17 Dec 2012 02:19:22   ygupta
 * Level 2 passwords and Alarm(WC to DCP Comm) chnged
 * 
 *    Rev 1.8   14 Dec 2012 05:28:10   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.7   13 Dec 2012 00:12:08   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.6   29 Nov 2012 08:47:20   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.5   24 Nov 2012 08:04:38   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.3   09 Nov 2012 00:35:28   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.2   08 Nov 2012 01:20:28   rjamloki
 * Website changes
 * 
 *    Rev 1.1   05 Nov 2012 09:13:54   amaurya
 * State machine changes.
 * 
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.
 */
#include "RTOS.h"
#include "StateMachineDrv.h"
#include "StateTable.h"
#include "StateFunctions.h"
#include "ModBusSendTask.h"
#include "ReceiveCommandHandler.h"
#include "preset.h"
#include "globals.h"
#include "func.h"
#include "Alarms.h"
#include "ExternalStatusTask.h"
#include "FieldBus.h"
#include "VerifyPreset.h"
#include "DcxManagerHandler.h"

#define printf
#define pprintf
UINT8 AbortReason = 0;//NONE
/*-------------------------- State Machine Variables/Flags -----------------------------*/
BOOLEAN TimeExpiredFlag = FALSE, EnergyBrakeFlag = FALSE, AfterBurstFlag = FALSE, WarningFlag = FALSE;
BOOLEAN AmpStepReachedFlag = FALSE;
BOOLEAN ABDelayFlag = FALSE, ABTimeFlag = FALSE;
BOOLEAN AbCompleted = FALSE;
BOOLEAN ScrubTimeActive = FALSE;
BOOLEAN MFTimeoutEnable = FALSE;
BOOLEAN MemoryClearAtRst = FALSE;


SINT32 EnergyBrakeTimer;
SINT32 RampTime, RampRunningTime;
SINT32 ABTimer;
UINT32 AvgPowerCount, AvgPower;

///*FB Related Flags.*/
BOOLEAN CheckForFBLoadNewPreset = FALSE;
UINT8   FBLoadPresetNumber = 0;
SINT32 TestData1 = 0;
BOOLEAN FBWeldFunction = FALSE, FBHornFunction = FALSE;
BOOLEAN PresetUpdateDone = FALSE;

UINT32  ULSTimeOutTimer = 0, TRSTimeOutTimer = 0,SonicDelayTimeOut = 0;
UINT32 EQAlarmTimer  = 0;
BOOLEAN WaitForTRS = FALSE;
BOOLEAN ULSFailFlag = FALSE;

BOOLEAN PowerUpScan = FALSE;
BOOLEAN ScanStarted = FALSE, ScanDone = FALSE, ScanAborted = FALSE;
UINT32  ScanTime = 0;
UINT32  PowerUpTimer = 0;
BOOLEAN SystemOK  = TRUE;
BOOLEAN ReadyOK   = FALSE;
BOOLEAN DummyFlag = FALSE;
BOOLEAN PowerUpDone  = FALSE;
BOOLEAN TempSonicAbortFlag = FALSE;
BOOLEAN AlarmCycleAbort = FALSE, SonicsAbortFlag = FALSE;
BOOLEAN DCPReadyFlag = FALSE;
BOOLEAN ResetFlag = FALSE;
BOOLEAN CycleAbortFlag = FALSE;
BOOLEAN WeldStopFlag   = FALSE, SonicsComplete = FALSE, WeldComplete = FALSE;
BOOLEAN ResetDone = FALSE;
BOOLEAN LoadPresetToDCP = FALSE;
BOOLEAN StartTest = FALSE, StartScan = FALSE;
BOOLEAN WaitForPresetACK = FALSE;
BOOLEAN GotoCycleStart = FALSE, GotoWeldStart = FALSE, GotToSeek = FALSE, GoToTEST = FALSE,
        GoToScan = FALSE;
BOOLEAN HistoryValid = FALSE;//If a cycle get aborted from External CycleAbort Signal. We may not have even
							//send the command to start the sonics to DCP. There is no sense to get the weld result
							//from DCP in this case.This flag keeps track of this.

BOOLEAN MFSonicsON = FALSE, MFAlarmFlag = FALSE;
BOOLEAN SeekStarted = FALSE, SeekDone = FALSE, SeekAlarmFlag = FALSE;
BOOLEAN TestStarted = FALSE, TestDone = FALSE, TestAlarmFlag = FALSE;
UINT32  SeekAccumAmplitude = 0, SeekTime = 0, TestTime = 0, ResetTimer = 0;
UINT32  AvgAmplitude = 0, SeekAmpCount = 0;
BOOLEAN ScanFailed = FALSE;
BOOLEAN GroundDetectFlag = FALSE;
UINT32  ScrubTimer = 0;
BOOLEAN NRTcmdFlag, WeldDoneWithAlarm;
SINT32  TimeOut = 0;
UINT16  AvgWeldAmplitude = 0;
AlarmLogData TempAlarmDataObj;
AlarmLogData * AlarmPtr;
UINT8   CurrentStatus = NO_SONICS;
BOOLEAN AlarmFlag = FALSE;
extern  BOOLEAN AlarmStatus;
extern  DcpWeldResult WeldHistoryData;
RFSwitchingStatus SwitchStatus = SWITCHINGINPROGRESS;
BOOLEAN RFDelayRequired = FALSE;
SINT32  RelayNumber = 0, RFSwitchTimeOut = 0, RFFeedBackTimer = 0;
BOOLEAN RFSwitchingRequired = FALSE, SonicsDisableFlag = FALSE;
typedef RFSwitchingStatus(* SelectRFRelay)(UINT16 StackNumber);
SelectRFRelay SelectRFRelayFun;
UINT32 SonicsDisableTime = 0;
SINT32 WeldStopFrequency;
SINT32 TempABDly,TempABTimer;
BOOLEAN TestComplete = FALSE;
UINT16 NRTCommand = 0;
SINT32 ULSInitialTimeOut;
extern SINT16 DCPEventLogs;
SINT16 DCPEventsRcvd = 0;
BOOLEAN EventsReceivedAtPowerUp =  FALSE;
BOOLEAN DelaytheSonics = FALSE;
UINT32 LastCmd = 0;
UINT32 ControlVariable = 0;
UINT32 SendCmdAfterACLineRestore = 0;
BOOLEAN ACLineResetFlag = FALSE;
BOOLEAN CheckCustomInput1= FALSE, CheckCustomInput2 = FALSE;
BOOLEAN ActuatorPresentAtPwrUp = FALSE;
extern UINT32 JsonWeldBuffTime;
void Nothing();
void (* PreReadyPassFuncPtr)(void) = Nothing;
void (* RdyPassFuncPtr)(void) = Nothing;
void (* SonicsEntryFuncPtr)(void) = Nothing;
void (* SonicsPassFuncPtr)(void) = Nothing;
void (* UpdatePresetPassFuncPtr)(void) = Nothing;
void (* EnergyBrakePassFuncPtr)(void) = Nothing;
void (* HoldOffTimePassFuncPtr)(void) = Nothing;
void (* ChecForSonicCompleteFun)(void) = Nothing;
NrtQ NRTcmd;
UINT32 PresetScrubTime = 0;
BOOLEAN DigitalInputConfigured = TRUE;

/* void Nothing()
 *  Purpose:
 *    All global func pointer in state machine should point to this func. Just in case
 *    they remain unassigned before execution and cause crash.
*/
void Nothing()
{

}

/* void PowerUpEntry(void)
 *
 *  Purpose:
 *    This function is called at the start of State machine at power up from Powerup State.
 *    This is the entry function of Powerup state which will be called once when WC will enter into this state.
 *    Function resets the Timeout timer and reset the PowerupDone flag to indicate that powerup is not done yet.
 *
 *  Entry condition:
 *  PowerUpDone is false.
 *
 *  Exit condition:
 *   none
 */
void PowerUpEntry(void)
{
   PowerUpDone = FALSE;
   PowerUpTimer = 0;
   TimedSeekTimerTicks = 0;
   SonicsOffPeriod = 0;
   NRTCommand = CMD_DCPEVENT_LOG_COUNT;
   NRTcmdFlag = FALSE;
   DCPEventsRcvd = 0;
   if(UserIO::CheckForActuatorPresent() == TRUE){
	   ActuatorPresentAtPwrUp = TRUE;
	   PreReadyState.PassPtr = PreReadyPassAct;
	   ReadyState.PassPtr = ReadyPassACT;
	   SonicOn.EntryPtr = SonicsEntryAct;
	   SonicOn.PassPtr = SonicsPassAct;
	   HandleUpdatePreset.PassPtr = UpdatePresetPassAct;
	   EnergyBrakeState.PassPtr = EnergyBrakePassAct;
	   HoldOffTimeState.PassPtr = HoldOffTimePassAct;
   }
   else{
	   ActuatorPresentAtPwrUp = FALSE;
	   PreReadyState.PassPtr = PreReadyPassNoAct;
	   ReadyState.PassPtr = ReadyPassNoACT;
	   SonicOn.EntryPtr = SonicsEntryNoAct;
	   SonicOn.PassPtr = SonicsPassNoAct; 
	   HandleUpdatePreset.PassPtr = UpdatePresetPassNoAct;
	   EnergyBrakeState.PassPtr = EnergyBrakePassNoAct;
	   HoldOffTimeState.PassPtr = HoldOffTimePassNoAct;
   }
}

/* void PowerUpPass(void)
 *
 *  Purpose:
 *    This function is called at the start of State machine at power up from Powerup State.
 *    It waits for Powerup communication with DCP to finish and than DCP to come into Ready state.
 *    There is a timeout of POWERUPTIMEOUT time, if DCP does not come into ready state within this time
 *    than WC will go into Error state and user will have to reset the error if Auto Reset is not enable.
 *    If DCP is ready than this function will check for PowerUp Options i.e. Seek/Scan and set appropriate flags
 *    to jump to do powerup Seek/Scan otherwise jumps to PreReady state.
 *
 *  Entry condition:
 *  PowerUpDone is false.
 *
 *  Exit condition:
 *   DCP is in ready state or Powerup Timeout.
 */
void PowerUpPass(void)
{
   if(PowerupCommunicationDone == TRUE){
	   if(EventsReceivedAtPowerUp == FALSE){
		  if(NRTcmdFlag == FALSE){
			 NRTcmdFlag = TRUE;
			 NRTResponse = NRT_NORESPONSE;
			 NRTcmd = (NrtQ){NRTCommand, READ_REQ, 0};
			 NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
		  }
		  else if(NRTResponse == REPLY_RECEIVED){
			 if(DCPEventsRcvd++ < DCPEventLogs){
				NRTcmdFlag = FALSE;
				NRTCommand = CMD_GETEVENTLOG;
				NRTResponse = NRT_NORESPONSE;
			 }
			 else
				 EventsReceivedAtPowerUp = TRUE;
		  }
	   }
       else if(DCPReadyFlag){
         /* powerup seek */
			  if((ControlVariable & POWERUPSEEK) != 0){
				 DCXFunc.SeekStart = TRUE;
				 LastCmd = POWERUPSEEK;
			  }
			  else if((ControlVariable & POWERUPSCAN) != 0){
				 PowerUpScan = TRUE;
				 DCXFunc.ScanStart = TRUE;
				 LastCmd = POWERUPSCAN;
			  }
			 else{ // No power up seek /scan is set
				PowerUpDone = TRUE;
				RtSendData.Reserved16 = 0;    //only for testing
				RtSendData.CmdFlagsReserved = 0; //only for testing
			 }
    	  }
   }
   if((PowerupCommunicationDone == FALSE) || (DCPReadyFlag == FALSE)){
      PowerUpTimer += WCTIME;
      //If Power up communication is not done and DCP is not ready than there is a problem.
      if(PowerUpTimer > POWERUPTIMEOUT){
         AlarmQueueWrite(CF01);
         Log::WriteEventLog(EVENT_MODBUSCOMMFAILURE, PowerupCommunicationDone, DCPReadyFlag, 3);
      }
   }
}

/* void PreReadyEntry(void)
 *
 *  Purpose:
 *    This function the entry function of PreReady state which will be called once when WC will enter
 *    into this state.
 *    Function resets the Flags and timeout variables before PreReadyPass function is called.
 *
 *  Entry condition:
 *  PowerUpDone is True at powerup. After a Weld/Horn function or Error reset to verify all the inputs are
 *  inactive.
 *
 *  Exit condition:
 *   none
 */
void PreReadyEntry(void)
{
   ResetFlag = FALSE;
   ReadyOK = FALSE;
   ULSFailFlag = FALSE;
   ULSTimeOutTimer = 0;
   CycleAbortFlag = FALSE;
   EQAlarmTimer = 0;
}

/* void PreReadyPassAct(void)
 *
 *  Purpose:
 *    This function the Pass function of PreReady state When actuator is present at power up.
 *    This function will wait for NoInput before jumps to ready state that means
 *    all External/WebPage or FB input must be clear to WC to come into ReadyState.
 *    If Actuator is present then this function will check for ULS to comeup before
 *    UPPERLIMIT_TIMEOUT otherwise generate an Alarm.
 *
 *  Entry condition:
 *  Jumps from Successful Horn/Weld Function State or from Powerup or after Error Reset.
 *
 *  Exit condition:
 *   General alarm or NoInput with powerupDone and DCPReady.
 */
void PreReadyPassAct(void)
{

	if((UserIO::CheckForULS()) == FALSE){
		ULSTimeOutTimer += WCTIME;
		if(ULSTimeOutTimer >= UPPERLIMIT_TIMEOUT){
			ULSFailFlag = TRUE;
			AlarmQueueWrite(EQ11);
			UserIO::SetActuaterHome(FALSE);
		}
	}
	else
		UserIO::SetActuaterHome(TRUE);
   RtSendData.CmdFlags = 0;
   if(WebSite::diagFlag == FALSE){
	   if((CheckForNoInput() == TRUE) && (DCPReadyFlag == TRUE) && (PowerUpDone == TRUE)){
		  ReadyOK = TRUE;
		  UserIO::SetActuaterHome(TRUE);
	   }
	   else{
		   if((ControlVariable & WELDSTART_MASKACT) != 0){
			   EQAlarmTimer += WCTIME;
			   if(EQAlarmTimer >= STARTSIGNAL_TIMEOUT)
				   AlarmQueueWrite(EQ01);
		   }
		   else if((DCPReadyFlag == FALSE) && ((ControlVariable & RESET_MASK) != 0))
			   ResetFlag = TRUE;
		   else
			   ControlVariable &= ~WEBMASK;//Web initiated commands should not cause the system to remain in pre ready
	   }
   }
}

/* void PreReadyPassNoAct(void)
 *
 *  Purpose:
 *    This function the Pass function of PreReady state When actuator is not present at power up.
 *    This function will wait for NoInput before jumps to ready state that means
 *    all External/WebPage or FB input must be clear to WC to come into ReadyState.
 *    If Actuator is present then this function will check for ULS to comeup before
 *    UPPERLIMIT_TIMEOUT otherwise generate an Alarm.
 *
 *  Entry condition:
 *  Jumps from Successful Horn/Weld Function State or from Powerup or after Error Reset.
 *
 *  Exit condition:
 *   General alarm or NoInput with powerupDone and DCPReady.
 */
void PreReadyPassNoAct(void)
{

   RtSendData.CmdFlags = 0;
   if(WebSite::diagFlag == FALSE){
	   if((CheckForNoInput() == TRUE) && ((RtSendData.CmdFlags & 0xFF) == 0x00)
			 && (DCPReadyFlag == TRUE) && (PowerUpDone == TRUE)){
		  ReadyOK = TRUE;
	   }
	   else{
		   if((ControlVariable & WELDSTART_MASKNOACT) != 0){
			   EQAlarmTimer += WCTIME;
			   if(EQAlarmTimer >= STARTSIGNAL_TIMEOUT)
				   AlarmQueueWrite(EQ01);
		   }
		   else if((DCPReadyFlag == FALSE) && ((ControlVariable & RESET_MASK) != 0))
			    ResetFlag = TRUE;
		   else
			   ControlVariable &= ~WEBMASK;//Web initiated commands should not cause the system to remain in pre ready
	   }
   }
}


/* void PreReadyExit(void)
 *
 *  Purpose:
 *    This function the Exit function of PreReady state.
 *
 *  Entry condition:
 *  Preready Pass Function has determined that no input is active before going to Ready State.
 *
 *  Exit condition:
 *   None
 */
void PreReadyExit()
{
	DigitalInputConfigured = FALSE;
}

/* void ReadyEntry(void)
 *
 *  Purpose:
 *    This function the entry function of Ready state which will be called once when WC will enter
 *    into this state.
 *    Function resets the Flags and timeout variables before ReadyPass function is called.
 *    It will also set the Ready output.
 *
 *  Entry condition:
 *  Jumps from PreReady State.
 *
 *  Exit condition:
 *   none
 */
void ReadyEntry(void)
{
   //Clear All the Flags that we are using in StateMachine for different functions.

   LastCmd = ControlVariable = 0;
   DCXFunc.CycleStart = FALSE;
   DCXFunc.WeldStart = FALSE;
  //DCXFunc.MFCycleStart = CurrentCycleParam->MfCycling;
   DCXFunc.ScanStart = FALSE;
   DCXFunc.SeekStart = FALSE;
   DCXFunc.TestStart = FALSE;
   WebIOEnabled = FALSE;
   WeldStopFlag  = FALSE;
   //Indicate WC is ready.
   UserIO::SetReady(TRUE);
   RtSendData.CmdFlags &= ~MB_USEREALTIMEDATA;
   UserIO::SetPresetChanged(FALSE);
   FBStat.ParameterChangeDone = FALSE;
   TimedSeekTimerTicks = 0;
   DelaytheSonics = FALSE;
}

/* void ReadyPassACT(void)
 *
 *  Purpose:
 *    This function the Pass function of Ready state when Actuator is present at power up.
 *    This function will wait for some Input before jumps to appropriate state.
 *    Function will check for WeldStart/CycleStart/Seek/Scan/Test/PresetRecall/MFCycling
 *    or Alarms before jumping to appropriate state.
 *
 *  Entry condition
 *  Jumps from PreReady State.
 *
 *  Exit condition:
 *   Alarm or Weld/Horn Function required or PresetRecall is required.
 */
void ReadyPassACT(void)
{
	if(SendCmdAfterACLineRestore != 0)
		{
			ControlVariable |= SendCmdAfterACLineRestore;
			SendCmdAfterACLineRestore = 0;
		}
		else
			ControlVariable &= ~AC_RESTORE_MASK;

   if(ControlVariable != 0){
      if((ControlVariable & WELDSTART_MASKACT) != 0){
         if((ControlVariable & IOCYCLE_START) == IOCYCLE_START){
            if((UserIO::CheckForPartInPlace() == FALSE)
               ||(UserIO::CheckForInterLockInPlace() == FALSE)){
               if(UserIO::CheckForPartInPlace() == FALSE)
                  AlarmQueueWrite(NC06);
               if(UserIO::CheckForInterLockInPlace() == FALSE)
                  AlarmQueueWrite(NC04);
            }
            else
               DCXFunc.CycleStart = TRUE;              
         }
         else
            DCXFunc.CycleStart = TRUE;
		   
         WriteCycleCounterForDCXM();
         LastCmd = ControlVariable & WELDSTART_MASKACT;
      }
      else if((ControlVariable & TESTSTART_MASK) != 0){
         DCXFunc.TestStart = TRUE;
         LastCmd = ControlVariable & TESTSTART_MASK;
         if(Sysconfig->ClearMemoryBeforeSeek == TRUE)
        	 ClearMemoryFrequency();
      }
      else if((ControlVariable & SEEKSTART_MASK) != 0){
         DCXFunc.SeekStart = TRUE;
         if(Sysconfig->ClearMemoryBeforeSeek == TRUE)
        	 ClearMemoryFrequency();
         LastCmd = ControlVariable & SEEKSTART_MASK;
      }
      else if((ControlVariable & SCANSTART_MASK) != 0){
         DCXFunc.ScanStart = TRUE;
         LastCmd = ControlVariable & SCANSTART_MASK;
      }
      else if((ControlVariable & RESET_MASK) != 0){
         ClearAlarmOutputs();
         AlarmIndex = 0;
         AlarmLogIndx = 0;
         FBStat.Error = 0;
         FBStat.CycleOK = FALSE;
         ControlVariable &= ~AC_RESTORE_RESET;
         FBStat.EmergencyStopActive = 0;
         WebSite::ResetWebResetFlag();
         memset(FBAlarms, 0, sizeof(FBAlarms));
         if(Sysconfig->ClearMemoryWithReset == TRUE)
        	 ClearMemoryFrequency();
         AlarmStatus = FALSE;    //stop pulse
      }
      if((ControlVariable & IOTRIGGER) == IOTRIGGER)
         AlarmQueueWrite(EQ03);
   }
   if(WebIOEnabled == FALSE){
      if((UserIO::CheckForGroundDetect() == TRUE) && (RunningPreset.WcParam.WeldMode == GroundDetectMode))
         AlarmQueueWrite(EQ05);
      if(UserIO::CheckForULS() == FALSE){
         AlarmQueueWrite(EQ04);
         UserIO::SetActuaterHome(FALSE);
      }
      else
    	  UserIO::SetActuaterHome(TRUE);
      if(UserIO::CheckForCycleAbort() == TRUE)
    	  AlarmQueueWrite(EQ10);

      if(UserIO::CheckForMemoryClear() || FieldBus::CheckForMemoryClear() || MemoryClearAtRst)
    	  ClearMemoryFrequency();
   }
   CheckForPresetChange();
   CheckforRFSwitching();
}

/* void ReadyPassNoACT(void)
 *
 *  Purpose:
 *    This function the Pass function of Ready state when Actuator is not present at power up.
 *    This function will wait for some Input before jumps to appropriate state.
 *    Function will check for WeldStart/CycleStart/Seek/Scan/Test/PresetRecall/MFCycling
 *    or Alarms before jumping to appropriate state.
 *
 *  Entry condition
 *  Jumps from PreReady State.
 *
 *  Exit condition:
 *   Alarm or Weld/Horn Function required or PresetRecall is required.
 */
void ReadyPassNoACT(void)
{
	if(SendCmdAfterACLineRestore != 0)
		{
			ControlVariable |= SendCmdAfterACLineRestore;
			SendCmdAfterACLineRestore = 0;
		}
		else
			ControlVariable &= ~AC_RESTORE_MASK;

   if(ControlVariable != 0){
      if((ControlVariable & WELDSTART_MASKNOACT) != 0){
         DCXFunc.WeldStart = TRUE;
		  WriteCycleCounterForDCXM();
         LastCmd = ControlVariable & WELDSTART_MASKNOACT;
      }
      else if((ControlVariable & TESTSTART_MASK) != 0){
         DCXFunc.TestStart = TRUE;
         LastCmd = ControlVariable & TESTSTART_MASK;
         if(Sysconfig->ClearMemoryBeforeSeek == TRUE)
        	 ClearMemoryFrequency();
      }
      else if((ControlVariable & SEEKSTART_MASK) != 0){
         DCXFunc.SeekStart = TRUE;
         LastCmd = ControlVariable & SEEKSTART_MASK;
         if(Sysconfig->ClearMemoryBeforeSeek == TRUE)
        	 ClearMemoryFrequency();
      }
      else if((ControlVariable & SCANSTART_MASK) != 0){
         DCXFunc.ScanStart = TRUE;
         LastCmd = ControlVariable & SCANSTART_MASK;
      }
      else if((ControlVariable & RESET_MASK) != 0){
         ClearAlarmOutputs();
         AlarmIndex = 0;
         AlarmLogIndx = 0;
         FBStat.Error = 0;
         FBStat.CycleOK = FALSE;
         ControlVariable &= ~AC_RESTORE_RESET;
         FBStat.EmergencyStopActive = 0;
         WebSite::ResetWebResetFlag();
         memset(FBAlarms, 0, sizeof(FBAlarms));
         if(Sysconfig->ClearMemoryWithReset == TRUE)
        	 ClearMemoryFrequency();
         AlarmStatus = FALSE;    //stop pulse
      }
      else if(((ControlVariable & MANUFACT_STARTMASK) != 0)&&(CurrentCycleParam->MfCycling == TRUE)){
         WebIOEnabled = TRUE;		//flag is cleared by Website
         LoadPresetToDCP = TRUE;
         UpdateRPreset = TRUE;
         LastCmd = ControlVariable & MANUFACT_STARTMASK;
      }
   }
   if(WebIOEnabled == FALSE){
	  if((UserIO::CheckForGroundDetect() == TRUE) && (RunningPreset.WcParam.WeldMode == GroundDetectMode))
         AlarmQueueWrite(EQ05);
	  if(UserIO::CheckForCycleAbort() == TRUE)
	     AlarmQueueWrite(EQ10);

      if(UserIO::CheckForMemoryClear() || FieldBus::CheckForMemoryClear() || MemoryClearAtRst)
    	  ClearMemoryFrequency();
   }
   CheckForPresetChange();
   CheckforRFSwitching();   
}

/* void ClearMemoryFrequency(void)
 *
 *  Purpose:
 *    To clear the memory frequency and update DCP about the same.
 *
 *  Entry condition
 *    Memory clear initiated while the system is in ready state
 *
 *  Exit condition:
 *    Memory frequency going live to DCP is cleared.
 */
void ClearMemoryFrequency(void)
{
	 RtSendData.CmdFlags |= MB_MEMCLR;
	 HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
	 HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
	 MemoryReset = TRUE;
	 MemoryClearAtRst = FALSE;
}

/* void ReadyExit(void)
 *
 *  Purpose:
 *    This function the Exit function of Ready state.
 *    This function will clear the Ready and Cycle complete output.
 *
 *  Entry condition
 *    None
 *
 *  Exit condition:
 *    Exit from ready state.
 */
void ReadyExit(void)
{
   TempPeakPower = 0;
   UserIO::SetReady(FALSE);
   UserIO::SetWeldCycleComplete(FALSE);
   UserIO::SetEndOfHoldTime(FALSE);
   UserIO::SetStartSignalRelease(FALSE);
   HistoryValid = FALSE;
   if((SonicsAbortFlag == FALSE) && (AlarmCycleAbort == FALSE)){
	  ClearAlarmOutputs();
      AlarmIndex = 0;
      AlarmLogIndx = 0;
      AlarmStatus = FALSE;    //stop pulse
      memset(&TempAlarmDataObj, 0, sizeof(AlarmLogData));
      FBStat.Error = 0;
      FBStat.CycleOK = FALSE;
      UserIO::SetCycleOK(FALSE);
      FBStat.EmergencyStopActive = 0;
      memset(FBAlarms, 0, sizeof(FBAlarms));
      CycleCountFlag = TRUE;
   }
}

/* void UpdatePresetEntry(void)
 *
 *  Purpose:
 *    This function the entry function of HandleUpdatePreset state which will be called once when
 *    WC will enter into this state.
 *    Function resets the Flags and timeout variables before UpdatePresetPass function is called.
 *    Function will check for the preset number to be recalled and verify that. State will jump to
 *    Alarm state in case of alarm otherwise preset will be send to DCP.
 *
 *  Entry condition:
 *  Jumps from Ready State in case of LoadPresetToDCP is set.
 *
 *  Exit condition:
 *   none
 */
void UpdatePresetEntry(void)
{
   SINT8 PresetNumber = -1;
   SINT32 ExtPresetNum = UserIO::CheckForExtRecallPreset();
   NRTResponse = NRT_NORESPONSE;
   CommIfc Data2 = COMM_IFC_NO_IFC;
   GotoCycleStart = FALSE;
   GotoWeldStart = FALSE;
   GotToSeek = FALSE;
   GoToTEST = FALSE;
   GoToScan = FALSE;
   PresetUpdateDone = FALSE;
   static SINT32 LastDCXMPSNum = -1;

   //Check if New Preset needs to be loaded. Initiated by External signal or from FB.
   if(UserIO::CheckForLoadNewPreset() == TRUE){
      PresetNumber = ExtPresetNum;
      Data2 = COMM_IFC_USERIO;
   }
   else if (FBWeldFunction|| CheckForFBLoadNewPreset||FBHornFunction){
      PresetNumber = FBLoadPresetNumber;
      if(FieldBusType == HW_ASSEMBLY_PROFIBUS)
    	  Data2 = COMM_IFC_PB;
      else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
    	  Data2 = COMM_IFC_ETHIP;
   }
   //Checkif New preset needs to be loaded in case of cycle start or trigger from user I/O.
   else if((ControlVariable & (IOCYCLE_START | IOWELD_START))&&(ExtPresetNum != 0)){
	   PresetNumber = ExtPresetNum;
       Data2 = COMM_IFC_USERIO;
   }

   if(PresetNumber > CurrPreset){
      if(RecallAndVerifyPreset((PRESET_NUMBER)PresetNumber) == TRUE){
    	 if(AdvanceConfigData->Advrnd.Flag2)
    		Log::WriteEventLog(EVENT_PRESET_RECALL, PresetNumber, Data2);
         if(LastDCXMPSNum != PresetNumber){
        	 LastDCXMPSNum = PresetNumber;
        	 SendPresetToDCXM = TRUE;
         }
         else
        	 SendPresetToDCXM = SendToDCXM[(UINT8)PresetNumber];
         SendToDCXM[(UINT8)PresetNumber] = FALSE;
         NRTcmd = (NrtQ){CMD_PRESETWRITE, WRITE_REQ, 0};
         NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
         WaitForPresetACK = TRUE;
      }
      else
         AlarmQueueWrite(HW02);
   }
   //Check if Running preset needs to be updated. Because of Parameter change from Website or from FB or from LCD.
   else if((UpdateRPreset == TRUE)||(Sysconfig->PresetChanged == TRUE)){
       PresetNumber = 0;
       UpdateRunningPreset();
       if(DCXFunc.MFCycleStart == TRUE)
          PrepareDCPPreset(TRUE);
       //if(VerifyPreset(TRUE) == TRUE){
     	 NRTcmd = (NrtQ){CMD_PRESETWRITE, WRITE_REQ, 0};
          NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
          WaitForPresetACK = TRUE;
 //      }
 //      else
 //         AlarmQueueWrite(SE05);
   }
   if(RFSwitchingRequired == TRUE){
	   //Initialize Timeout variables for RF switching.
		 RFSwitchTimeOut = 0;
		 RFFeedBackTimer = 0;
		 //Initialize RF Switching Status
		 SwitchStatus = SWITCHINGINPROGRESS;
		 //Initialize Relay Number based on Switching type(Direct/with Off)
		 if(Sysconfig->RFSwitch.RFOption == SWITCHWITHOFF)
			RelayNumber = 0;
		 else
			RelayNumber = ParameterSet[CurrPreset].InfoParam.HornPresetNumber;
		 //Check if RF Delay Required and set the flag accordingly.
		 if(Sysconfig->RFSwitch.RFSwitchDelay > 0)
			RFDelayRequired = TRUE;
   }
   else
	   SwitchStatus = SWITCHINGDONE;
}

/* void UpdatePresetPassAct(void)
 *
 *  Purpose:
 *    This function the Pass function of HandleUpdatePreset state which will be called every ms when WC
 *    is in this state and Actuator is present at power up.
 *    Function will wait for ACK for preset upload from DCP to make sure preset is uploaded on DCP.
 *    After a ACK from DCP this function will check for all input to see if any weld or horn function is
 *    required and set the flag accordingly. Otherwise it will set PresetUpdateDone flag to goback to
 *    preready state.
 *
 *  Entry condition:
 *   None.
 *
 *  Exit condition:
 *   Preset is uploaded to DCP, any Horn/Weld function is required.
 */
void UpdatePresetPassAct(void)
{
   if(WaitForPresetACK == TRUE){
      WaitForPresetACK = FALSE;
      Sysconfig->PresetChanged = FALSE;
   }
   else{
      if(SwitchStatus == SWITCHINGDONE){
    	    if(ControlVariable & WELDSTART_MASKACT){
        		GotoCycleStart = TRUE;
        		LastCmd = (ControlVariable & (WELDSTART_MASKACT));
        	}
            if((ControlVariable & SEEKSTART_MASK) != 0){
               GotToSeek = TRUE;
               LastCmd = (ControlVariable & SEEKSTART_MASK);
            }
            if((ControlVariable & TESTSTART_MASK) != 0){
				GoToTEST = TRUE;
				LastCmd = (ControlVariable & TESTSTART_MASK);
            }
            if((ControlVariable & SCANSTART_MASK) != 0){
               GoToScan = TRUE;
               LastCmd = (ControlVariable & SCANSTART_MASK);
            }
            PresetUpdateDone = TRUE;
            if(Sysconfig->ActivePreset != CurrPreset){
				UserIO::SetPresetChanged(TRUE);
				FBStat.ParameterChangeDone = TRUE;
            }
            if((ControlVariable & IOCYCLE_START) == IOCYCLE_START)
            	UserIO::SetCycleStartOut(TRUE);
      }
      else
    	  HandleRFSwitching();
   }
}

/* void UpdatePresetPassNoAct(void)
 *
 *  Purpose:
 *    This function the Pass function of HandleUpdatePreset state which will be called every ms when WC
 *    is in this state and Actuator is not present at power up.
 *    Function will wait for ACK for preset upload from DCP to make sure preset is uploaded on DCP.
 *    After a ACK from DCP this function will check for all input to see if any weld or horn function is
 *    required and set the flag accordingly. Otherwise it will set PresetUpdateDone flag to goback to
 *    preready state.
 *
 *  Entry condition:
 *   None.
 *
 *  Exit condition:
 *   Preset is uploaded to DCP, any Horn/Weld function is required.
 */
void UpdatePresetPassNoAct(void)
{
   if(WaitForPresetACK == TRUE){
      WaitForPresetACK = FALSE;
      Sysconfig->PresetChanged = FALSE;
   }
   else{
      if(SwitchStatus == SWITCHINGDONE){
        	if((ControlVariable & WELDSTART_MASKNOACT) != 0){
        		GotoWeldStart = TRUE;
        		LastCmd = (ControlVariable & WELDSTART_MASKNOACT);
        	}
            if((ControlVariable & SEEKSTART_MASK) != 0){
               GotToSeek = TRUE;
               LastCmd = (ControlVariable & SEEKSTART_MASK);
            }
            if((ControlVariable & TESTSTART_MASK) != 0){
				GoToTEST = TRUE;
				LastCmd = (ControlVariable & TESTSTART_MASK);
            }
            if((ControlVariable & SCANSTART_MASK) != 0){
               GoToScan = TRUE;
               LastCmd = (ControlVariable & SCANSTART_MASK);
            }
            PresetUpdateDone = TRUE;
            if(Sysconfig->ActivePreset != CurrPreset){
				UserIO::SetPresetChanged(TRUE);
				FBStat.ParameterChangeDone = TRUE;
            }
            if((ControlVariable & IOCYCLE_START) == IOCYCLE_START)
            	UserIO::SetCycleStartOut(TRUE);
      }
      else
    	  HandleRFSwitching();
   }
}

/* void UpdatePresetExit(void)
 *
 *  Purpose:
 *    This function the Exit function of HandleUpdatePreset state.
 *    Function will clear LoadPresetToDCP Flag.
 *
 *  Entry condition:
 *   None.
 *
 *  Exit condition:
 *   None.
 */
void UpdatePresetExit(void)
{
   LoadPresetToDCP = FALSE;
   CheckForFBLoadNewPreset = FALSE;
   CheckForCustomCutoffSetup();
}

/* void WaitforULSTRSEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of WaitforULSTRS state which will be called once when WC
 *    will enter into this state.
 *    Function resets the Flags and timeout variables before WaitforULSTRSPass function is called.
 *
 *  Entry condition:
 *  Jumps from Ready State or from HandleUpdatePreset state in case of weld required when
 *  Actuator is present.
 *
 *  Exit condition:
 *   None
 */
void WaitforULSTRSEntry(void)
{
   ULSInitialTimeOut = CHECKEQ02_TIMEOUT;
   ULSTimeOutTimer = 0;
   TRSTimeOutTimer = 0;
   WaitForTRS = FALSE;
   GotoCycleStart = FALSE;
   UserIO::SetCycleStartOut(TRUE);
   WritePresetForDCXM();
}

/* void WaitforULSTRSPass(void)
 *
 *  Purpose:
 *    This function the Pass function of WaitforULSTRS state which will be called every ms when WC
 *    is in this state.
 *    Function will first wait for ULS to go away and than wait for TRS to comeup. UPPERLIMIT_TIMEOUT
 *    and TRIGGER_TIMEOUT will be tested and alarm will be raised in case of timeout. If both
 *    condition matches than only it will set WeldStartFlag to jump to SonicOn state.
 *
 *  Entry condition:
 *  Jumps from Ready State or from HandleUpdatePreset state in case of weld required when
 *  Actuator is present.
 *
 *  Exit condition:
 *   ULS or TRS timeout or both condition matches.
 */
void WaitforULSTRSPass(void)
{
	  if((ControlVariable & LastCmd) != LastCmd)
		  AlarmQueueWrite(EQ09);
	  else if((UserIO::CheckForULS() == TRUE) && ((ControlVariable & IOTRIGGER) == IOTRIGGER)){
            ULSInitialTimeOut -= WCTIME;
            if(ULSInitialTimeOut <= 0)
            	AlarmQueueWrite(EQ02);
      }
      else{
         if(WaitForTRS == TRUE){ // Wait for TRS
        	 if(UserIO::CheckForULS() == TRUE){
        		 AlarmQueueWrite(EQ12);
        	 	 UserIO::SetActuaterHome(TRUE);
        	 }
        	 else if((ControlVariable & IOTRIGGER) != IOTRIGGER){
                  TRSTimeOutTimer += WCTIME;
                  if(TRSTimeOutTimer >= TRIGGER_TIMEOUT)
                        AlarmQueueWrite(NC02);
              }
              else{
                  if((Sysconfig->Externalstatus == TRUE) && (Sysconfig->ErrorFlag == TRUE)){
                        ExternalStatusTask::thisPtr -> ExtStatusTaskQ.Write(0, 't', 0);
                  }
                  LastCmd |= IOTRIGGER;
                  GotoCycleStart = TRUE;
              }
         }
         else{
              if(UserIO::CheckForULS() == TRUE){
                  ULSTimeOutTimer += WCTIME;
                  if(ULSTimeOutTimer >= UPPERLIMIT_TIMEOUT)
                        AlarmQueueWrite(NC01);
              }
              else{
                  WaitForTRS = TRUE;
                  UserIO::SetActuaterHome(FALSE);
              }
         }
      }
}


/* void CheckForCustomCutoffSetup(void)
 *
 *  Purpose: This function sets or clear global flags CheckCustomInput1, CheckCustomInput2 depending upon they are activated or not.
 *  It is called by UpdatePresetExit function.
 *
 *  Entry condition:
 *   None.
 *
 *  Exit condition:
 *   None
 */
void CheckForCustomCutoffSetup(void)
{
   CheckCustomInput1= FALSE; CheckCustomInput2 = FALSE;
   if(RunningPreset.WcParam.CutOffEnabled == TRUE){
      if((CustomInput1Configured == TRUE) && (RunningPreset.ActParam.CustomAIn1CutoffFlag))//chk before weld
    	  CheckCustomInput1 = TRUE;
      if((CustomInput2Configured == TRUE) && (RunningPreset.ActParam.CustomAIn2CutoffFlag))
    	  CheckCustomInput2 = TRUE;
   }
}

/* void CheckCustomCutoffs(void)
 *
 *  Purpose: This function checks values of Custom Inputs, if any of them exceeds their cutoff value
 *     defined in preset,respective alarm is raised.It is called by SonicsPass function irrespective of
 *     Actuator presence.
 *
 *  Entry condition:
 *     None.
 *
 *  Exit condition:
 *     None
 */
void CheckCustomCutoffs(void)
{
	if(CheckCustomInput1 == TRUE){
		if(UserIO::GetCustomAIn1() >= (RunningPreset.ActParam.CustomAIn1Cutoff))
			AlarmQueueWrite(CU04);
	}
	if(CheckCustomInput2 == TRUE){
		if(UserIO::GetCustomAIn2() >= (RunningPreset.ActParam.CustomAIn2Cutoff))
			AlarmQueueWrite(CU08);
	}
}

/* void SonicsEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of SonicOn state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the timer variables and flags used in this state and
 *    function will set Amplitude and MB_USEREALTIMEDATA bit in DCP real time command based
 *    on following conditions:
 *    1.> Use External Amplitude if External Amplitude is selected and set MB_USEREALTIMEDATA to
 *    use this amplitude.
 *    2.> Use Amplitude1 if Step at External is selected and set MB_USEREALTIMEDATA to
 *    use this amplitude.
 *    3.> Reset MB_USEREALTIMEDATA to use preset Amplitude1 on DCP side in all other step conditions.
 *    This function will also set the MB_START bit to start sonics on DCP side and set Sonics Active output
 *    and Amplitude1 digital output signals.
 *
 *  Entry condition:
 *  WC is entering into SonicsOn state.
 *
 *  Exit condition:
 *   None
 */
void SonicsEntry(void)
{
   UINT16 CmdFlags = 0;
   WeldStopFrequency = 0;
   WeldComplete = FALSE;
   CurrentStatus = NO_SONICS;
   SonicsComplete = FALSE;
   EnergyBrakeFlag = FALSE;
   GroundDetectFlag = FALSE;
   ScrubTimeActive = FALSE;
   ScrubTimer = 0;
   WeldBufferIndex = -1;
   WeldBufferTime = 0;
   JsonWeldBuffTime = 0;
   SonicsDisableFlag = FALSE; 
   AmpStepReachedFlag = FALSE;
   WritePresetForDCXM();
   if((RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_EXTERNALAIN)
      &&(RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_EXTERNAL)){
      //Use Preset Amplitude in case of Stepping at Time/Energy/Power
      RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude1;
      CmdFlags &= ~MB_USEREALTIMEDATA;
   }
   else if (RunningPreset.WcParam.AmpStepCondition  == AMPSTEP_COND_EXTERNALAIN){
      //Use External Amplitude in case of External Amplitude.
      CmdFlags |= MB_USEREALTIMEDATA;
      if(IsManualFunctionAvailable() == TRUE)
         RtSendData.AmplitudeIn = UserIO::GetExternalAmplitude();
      else
         RtSendData.AmplitudeIn = FieldBus::GetExternalAmplitude();
   }
   else if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_EXTERNAL){
      //Use real time Amplitude in case of stepping at external
      RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude1;
      CmdFlags |= MB_USEREALTIMEDATA;
   }
   else {
         //Use real time Amplitude in case of stepping at external
         RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude1;
   }
   RtSendData.RampTime = RunningPreset.DcpParam.WeldRampTime;
   CmdFlags |= MB_ACCUM_REQ;
   CmdFlags &= ~MB_SONICS_DISABLE;
   CmdFlags |= MB_START;
   HistoryValid = TRUE;
   if(UserIO::CheckForSonicsDisable() == TRUE){
      SonicsDisableFlag = TRUE;
      CmdFlags |= MB_SONICS_DISABLE;
      AlarmQueueWrite(WA05);
      SonicsDisableTime = 0;
   }
   else if (UserIO::CheckForExternalSonicsDelay() == TRUE)
   {
	   HistoryValid = FALSE;
	   CmdFlags &= ~MB_START;
	   DelaytheSonics = TRUE;
	   SonicDelayTimeOut = 0;
   }
   UserIO::SetAmplitude1_2(AMP1);	// FALSE for Amp1

   AvgPowerCount = 0;
   AvgPower = 0;
   MemoryReset = FALSE;
   if((ControlVariable & IOCYCLE_START) == IOCYCLE_START)
      	UserIO::SetCycleStartOut(TRUE);
   RtSendData.CmdFlags = CmdFlags;
}

/* void ActSonicCheckContMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is continuous.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void ActSonicCheckContMode(void)
{
   if((ControlVariable & IOTRIGGER) != IOTRIGGER){
	 CurrentStatus = WELD_COMPLETE;
     RtSendData.CmdFlags &= ~MB_START;   //Stop weld
     RtSendData.CmdFlags &= ~MB_USEREALTIMEDATA;
   }
}

/* void ActSonicCheckTimeEngPwrMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is Time/Energy/Peak Power mode.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void ActSonicCheckTimeEngPwrMode(void)
{
   if((((RtReceiveData.StatusFlags & SIGNAL_PRIMARYPARAMETER_REACHED) == SIGNAL_PRIMARYPARAMETER_REACHED))) {
      if(RunningPreset.WcParam.EBFlag == TRUE){
         EnergyBrakeFlag = TRUE;
      }
      else{
    	 CurrentStatus = SONICS_COMPLETE;
         RtSendData.CmdFlags &= ~MB_START;   //Stop weld
      }
   }
   else if((DCXFunc.CycleStart == TRUE)&& ((ControlVariable & IOTRIGGER) != IOTRIGGER))
      AlarmQueueWrite(CM01);
}

/* void ActSonicCheckGDDetctMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is Ground detect mode.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void ActSonicCheckGDDetctMode(void)
{
   if((UserIO::CheckForGroundDetect() == TRUE)||(FBCmd.GndDetect)){
     if (GroundDetectFlag == FALSE){
       GroundDetectFlag = TRUE;
       if((RunningPreset.WcParam.ScrubTime > 0) /*&&
            (RunningPreset.WcParam.ScrubTimeFlag == TRUE)*/){  //Scrub Time(s) isn't Off
    	  ScrubTimeActive = TRUE;
          ScrubTimer += WCTIME;
          RtSendData.CmdFlags |= (MB_USEREALTIMEDATA | MB_DISABLESTEPPING);
          RtSendData.AmplitudeIn = RunningPreset.WcParam.ScrubAmp;
       }
       /* Stop sonics, ground detect is active*/
       else if(RunningPreset.WcParam.EBFlag == TRUE){
          EnergyBrakeFlag = TRUE;
          RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
       }
       else{
          CurrentStatus = SONICS_COMPLETE;
          RtSendData.CmdFlags &= ~MB_START;   //Stop weld
       }
     }
     else if(ScrubTimer >= PresetScrubTime){
       /* Stop sonics, ground detect is active*/
       if(RunningPreset.WcParam.EBFlag == TRUE){
          EnergyBrakeFlag = TRUE;
          RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
       }
       else{
    	  CurrentStatus = SONICS_COMPLETE;
          RtSendData.CmdFlags &= ~MB_START;   //Stop weld
       }
     }
     ScrubTimer += WCTIME;
   }
   else if(GroundDetectFlag == TRUE){ /* Had Gnd Detect, but lost it, check anyway */
     if(ScrubTimer >= PresetScrubTime){
       /* Stop sonics, ground detect is active */
       if(RunningPreset.WcParam.EBFlag == TRUE){
          EnergyBrakeFlag = TRUE;
          RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
       }
       else{
    	  CurrentStatus = SONICS_COMPLETE;
          RtSendData.CmdFlags &= ~MB_START;   //Stop weld
       }
     }
     ScrubTimer += WCTIME;
   }
   if((DCXFunc.CycleStart == TRUE)&&((ControlVariable & IOTRIGGER) != IOTRIGGER))
      AlarmQueueWrite(CM01);
}

/* void SonicsEntryAct(void)
 *
 *  Purpose:
 *    This function the Entry function of SonicOn state when Actuator is present at power up.
 *    It is called from entry function pointer of SonicsOn State. It assigns pass function to
 *    pass function pointer ChecForSonicCompleteFun based on weld mode.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void SonicsEntryAct(void)
{
   SonicsEntry();
   UserIO::SetStartSignalRelease(TRUE);//Cycle Start can be released now.
   switch(RunningPreset.WcParam.WeldMode){
      case ContinuousMode:
         ChecForSonicCompleteFun = ActSonicCheckContMode;
      break;
      case TimeMode:
      case EnergyMode:
      case PeakpowerMode:
         ChecForSonicCompleteFun = ActSonicCheckTimeEngPwrMode;
      break;
      case GroundDetectMode:
         PresetScrubTime = CONV_MILLI_MICROSEC(RunningPreset.WcParam.ScrubTime);
         ChecForSonicCompleteFun = ActSonicCheckGDDetctMode;
      break;
      default:break;
   }
}

/* void NoActSonicCheckContMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryNoAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is Continuous.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void NoActSonicCheckContMode(void)
{
   if((LastCmd & ControlVariable) != LastCmd){
	   CurrentStatus = WELD_COMPLETE;
     RtSendData.CmdFlags &= ~MB_START;   //Stop weld
     RtSendData.CmdFlags &= ~MB_USEREALTIMEDATA;
   }
}

/* void NoActSonicCheckTimeEngPwrMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryNoAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is Time/Energy/PeakPower.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void NoActSonicCheckTimeEngPwrMode(void)
{
   if((((RtReceiveData.StatusFlags & SIGNAL_PRIMARYPARAMETER_REACHED) == SIGNAL_PRIMARYPARAMETER_REACHED))) {
      if(RunningPreset.WcParam.EBFlag == TRUE){
         EnergyBrakeFlag = TRUE;
      }
      else{
    	  CurrentStatus = SONICS_COMPLETE;
         RtSendData.CmdFlags &= ~MB_START;   //Stop weld
      }
   }
}

/* void NoActSonicCheckGDDetctMode(void)
 *
 *  Purpose:
 *    This function the Pass function of SonicOn state. It is called from SonicsEntryNoAct function using
 *    ChecForSonicCompleteFun function pointer when weld mode is Ground detect.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void NoActSonicCheckGDDetctMode(void)
{
   if((UserIO::CheckForGroundDetect() == TRUE)||(FBCmd.GndDetect)){
       if (GroundDetectFlag == FALSE){
          GroundDetectFlag = TRUE;
          if((RunningPreset.WcParam.ScrubTime > 0) /*&&
             (RunningPreset.WcParam.ScrubTimeFlag == TRUE)*/){  //Scrub Time(s) isn't Off
        	 ScrubTimeActive = TRUE;
             ScrubTimer += WCTIME;
             RtSendData.CmdFlags |= (MB_USEREALTIMEDATA | MB_DISABLESTEPPING);
             RtSendData.AmplitudeIn = RunningPreset.WcParam.ScrubAmp;
          }
          /* Stop sonics, ground detect is active*/
          else if(RunningPreset.WcParam.EBFlag == TRUE){
             EnergyBrakeFlag = TRUE;
             RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
          }
          else{
             CurrentStatus = SONICS_COMPLETE;
             RtSendData.CmdFlags &= ~MB_START;   //Stop weld
          }
       }
       else if(ScrubTimer >= PresetScrubTime){
          /* Stop sonics, ground detect is active*/
          if(RunningPreset.WcParam.EBFlag == TRUE){
             EnergyBrakeFlag = TRUE;
             RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
          }
          else{
        	  CurrentStatus = SONICS_COMPLETE;
             RtSendData.CmdFlags &= ~MB_START;   //Stop weld
          }
       }
       ScrubTimer += WCTIME;
    }
    else if(GroundDetectFlag == TRUE){ /* Had Gnd Detect, but lost it, check anyway */
       if(ScrubTimer >= PresetScrubTime){
          /* Stop sonics, ground detect is active */
          if(RunningPreset.WcParam.EBFlag == TRUE){
             EnergyBrakeFlag = TRUE;
             RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
          }
          else{
        	  CurrentStatus = SONICS_COMPLETE;
             RtSendData.CmdFlags &= ~MB_START;   //Stop weld
          }
       }
       ScrubTimer += WCTIME;
    }
}

/* void SonicsEntryNoAct(void)
 *
 *  Purpose:
 *    This function the Entry function of SonicOn state when Actuator is not present at power up.
 *    It is called by entry function pointer of SonicsOn State. It assigns pass function to
 *    pass function pointer ChecForSonicCompleteFun based on weld mode.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *   None
 */
void SonicsEntryNoAct(void)
{
   SonicsEntry();
   switch(RunningPreset.WcParam.WeldMode){
      case ContinuousMode:
         ChecForSonicCompleteFun = NoActSonicCheckContMode;
      break;
      case TimeMode:
      case EnergyMode:
      case PeakpowerMode:
         ChecForSonicCompleteFun = NoActSonicCheckTimeEngPwrMode;
      break;
      case GroundDetectMode:
         PresetScrubTime = CONV_MILLI_MICROSEC(RunningPreset.WcParam.ScrubTime);
         ChecForSonicCompleteFun = NoActSonicCheckGDDetctMode;
      break;
      default:break;
   }
}

/* void SonicsPassAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of SonicOn state which will be called every ms when WC is in this state.
 *    Function will check for trigger or FBCycleStart all the time while in this state.
 *    if both of them are not present than it will stop the cycle.
 *    Function will also check for stepping and will set the profileramp time in real time
 *    command in case of stepping needs to be done.
 *    Function will also check for externalstart/FBCycleStart signal in case of Continuous mode to stop the cycle.
 *    In case of Time/Energy/Power mode SIGNAL_PRIMARYPARAMETER_REACHED bit will be checked to jump to next state.
 *    In case of GroundDetect mode if GroundDetect input is active than ScrubAmplitude will be sent to
 *    DCP for ScrubTime and jump to next state after scrubtime complete.
 *    If EnergyBraking is required than set EnergyBrakeFlag to jump to that state otherwise
 *    set SonicsComplete flag to jump to HoldTime state.
 *
 *  Entry condition:
 *    WC is in SonicsOn state.
 *
 *  Exit condition:
 *    None
 */
void SonicsPassAct(void)
{
   if(DelaytheSonics == FALSE)
   {
	   if(CurrentStatus == NO_SONICS){
		   if(RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_OFF){
		     if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT)
				CheckForStepping();
		   }
		   CheckCustomCutoffs();
		   ChecForSonicCompleteFun();
	   }
	   else{
		  if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT){
			 if(CurrentStatus == WELD_COMPLETE)
				WeldComplete = TRUE;
			 else if(CurrentStatus == SONICS_COMPLETE)
				SonicsComplete = TRUE;
		  }
	   }
	   UpdatePeakPower();
   }
   else
   {
	   if (UserIO::CheckForExternalSonicsDelay() == TRUE)
	   {
		   SonicDelayTimeOut += WCTIME;
		   if(SonicDelayTimeOut >= SONICDELAY_TIMEOUTUS)
			  AlarmQueueWrite(NC03);
	   }
	   else{
		   DelaytheSonics = FALSE;
		   HistoryValid = TRUE;
		   RtSendData.CmdFlags |= MB_START;
	   }
   }
}

/* void SonicsPassNoAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of SonicOn state which will be called every ms when WC is in this state.
 *    Function will check for trigger or FBCycleStart all the time while in this state.
 *    if both of them are not present than it will stop the cycle.
 *    Function will also check for stepping and will set the profileramp time in real time
 *    command in case of stepping needs to be done.
 *    Function will also check for externalstart/FBCycleStart signal in case of Continuous mode to stop the cycle.
 *    In case of Time/Energy/Power mode SIGNAL_PRIMARYPARAMETER_REACHED bit will be checked to jump to next state.
 *    In case of GroundDetect mode if GroundDetect input is active than ScrubAmplitude will be sent to
 *    DCP for ScrubTime and jump to next state after scrubtime complete.
 *    If EnergyBraking is required than set EnergyBrakeFlag to jump to that state otherwise
 *    set SonicsComplete flag to jump to HoldTime state.
 *
 *  Entry condition:
 *    WC is in SonicsOn state.
 *
 *  Exit condition:
 *    None
 */
void SonicsPassNoAct(void)
{
   if(DelaytheSonics == FALSE)
   {
	   if(CurrentStatus == NO_SONICS){
		   if(RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_OFF){
			   if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT)
				CheckForStepping();
		   }
		   CheckCustomCutoffs();
		   ChecForSonicCompleteFun();
	   }
      else{
         if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT){
            if(CurrentStatus == WELD_COMPLETE)
               WeldComplete = TRUE;
            else if(CurrentStatus == SONICS_COMPLETE)
               SonicsComplete = TRUE;
         }
      }
	   UpdatePeakPower();
   }
   else
   {
	   if (UserIO::CheckForExternalSonicsDelay() == TRUE){
		   SonicDelayTimeOut += WCTIME;
		   if(SonicDelayTimeOut >= SONICDELAY_TIMEOUTUS)
			  AlarmQueueWrite(NC03);
	   }
	   else{
		   DelaytheSonics = FALSE;
		   HistoryValid = TRUE;
		   RtSendData.CmdFlags |= MB_START;
	   }
   }
}

/* void SonicsExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of SonicOn state which will be called once when WC
 *    will leave this state.
 *    Function will reset SonicsActive output and reset the MB_START bit if energyBraking
 *    is not required. it will also reset the MB_ACCUM_REQ bit in this case so that DCP will Stop
 *    looking at Primary parameters.
 *
 *  Entry condition:
 *  WC is leaving SonicsOn state.
 *
 *  Exit condition:
 *   None
 */
void SonicsExit(void)
{
   SINT32 Temp;
   if(EnergyBrakeFlag == FALSE){
      RtSendData.AmplitudeIn = 0;         //stop sonics
      RtSendData.CmdFlags &= ~MB_START;   //Stop weld
      RtSendData.CmdFlags &= ~MB_ACCUM_REQ;
   }
   if(OverLoadAlarm == FALSE){
      if(ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag == TRUE){
         WeldStopFrequency = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) / FREQ_SCALING_FACTOR);
         Temp = (RtReceiveData.MemOut - RunningHSPreset.DigitalTuneScaledUP);
         HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = Temp;
         HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = Temp;
      }
   }
   SonicsOffPeriod = 0;
}

/* void EnergyBrakeEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of EnergyBrakeState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the timer variables and flags used in this state and function will set Amplitude
 *    and Ramptime in DCP real time command based on EBAmp and EBTime in Preset. It will also clear
 *    the MB_ACCUM_REQ bit and will set the SonicsActive output.
 *
 *  Entry condition:
 *    WC is entering into EnergyBrakeState state.
 *
 *  Exit condition:
 *    None
 */
void EnergyBrakeEntry(void)
{
   SonicsComplete = FALSE;
   RtSendData.AmplitudeIn = RunningPreset.WcParam.EBAmp; //energy brake amplitude
   RtSendData.RampTime = RunningPreset.WcParam.EBTime;
   RtSendData.CmdFlags &= ~MB_ACCUM_REQ;  //clear accumulation bit
   RtSendData.CmdFlags |= (MB_USEREALTIMEDATA | MB_DISABLESTEPPING);
   EnergyBrakeTimer = CONV_MILLI_MICROSEC(RunningPreset.WcParam.EBTime) - ENERGYBRAKETIME_REDUCTION;
}

/* void EnergyBrakePassAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of EnergyBrakeState state which will be called every ms
 *    when WC is in this state.
 *    Function will check for Time and AmplitudeOut to reach to EBTime and EBAmp all the time while
 *    in this state.SonicsComplete will be set to exit from the state if any of these conditions matches.
 *
 *  Entry condition:
 *  WC is in EnergyBrakeState state.
 *
 *  Exit condition:
 *   EBTime is complete or EBAmp is reached.
 */
void EnergyBrakePassAct(void)
{
   UINT32 TempAmpIn;
   TempAmpIn = RtReceiveData.AmplitudeOut;
   EnergyBrakeTimer -= WCTIME;
   if(EnergyBrakeTimer <= 0){
      SonicsComplete = TRUE;
      RtSendData.CmdFlags &= ~MB_START;   //Stop weld
   }
   //TBD CurrentAmpIn is the Amplitude coming from DCP.
   if(TempAmpIn <= RunningPreset.WcParam.EBAmp){
      SonicsComplete = TRUE;
      RtSendData.CmdFlags &= ~MB_START;   //Stop weld
   }
   if((DCXFunc.CycleStart == TRUE)&&((ControlVariable & IOTRIGGER) != IOTRIGGER))
	  AlarmQueueWrite(CM01);
   UpdatePeakPower();
}

/* void EnergyBrakePassNoAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of EnergyBrakeState state which will be called every ms
 *    when WC is in this state.
 *    Function will check for Time and AmplitudeOut to reach to EBTime and EBAmp all the time while
 *    in this state.SonicsComplete will be set to exit from the state if any of these conditions matches.
 *
 *  Entry condition:
 *  WC is in EnergyBrakeState state.
 *
 *  Exit condition:
 *   EBTime is complete or EBAmp is reached.
 */
void EnergyBrakePassNoAct(void)
{
   EnergyBrakeTimer -= WCTIME;
   //RtReceiveData.AmplitudeOut is the Amplitude coming from DCP.
   if((EnergyBrakeTimer <= 0)||(RtReceiveData.AmplitudeOut <= RunningPreset.WcParam.EBAmp)){
      SonicsComplete = TRUE;
      RtSendData.CmdFlags &= ~MB_START;   //Stop weld
   }
   UpdatePeakPower();
}
/* void EnergyBrakeExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of EnergyBrakeState state which will be called once when WC
 *    will leave this state.
 *    Function will reset SonicsActive output and reset the MB_START bit.
 *    it will also send the Alarm reset command to DCP in case of alarm so that DCP will be ready for next cycle.
 *
 *  Entry condition:
 *  WC is leaving EnergyBrakeState state.
 *
 *  Exit condition:
 *   None
 */
void EnergyBrakeExit(void)
{
   RtSendData.AmplitudeIn = 0;
   RtSendData.RampTime = 0;
   RtSendData.CmdFlags &= ~MB_START;   //Stop weld
   SonicsOffPeriod = 0;
}

/* void HoldOffTimeEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of HoldTimeState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the timer variables and flags used in this state and
 *    function will set the timeout to HoldTime and also set the HoldTimeActive digital output.
 *
 *  Entry condition:
 *  WC is entering into HoldTimeState state.
 *
 *  Exit condition:
 *   None
 */
void HoldOffTimeEntry(void)
{
   TimeExpiredFlag = FALSE;
   TimeOut = 0;
   if(RunningPreset.WcParam.HoldTimeFlag == TRUE)
     TimeOut = RunningPreset.WcParam.HoldTime;
   UserIO::SetHoldTimeActive(TRUE);
   TimeOut = CONV_MILLI_MICROSEC(TimeOut);
}

/* void HoldOffTimePassAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of HoldTimeState state which will be called every ms
 *    when WC is in this state.
 *    Function will check for HoldTime to expire all the time while in this state.
 *    TimeExpiredFlag will be set to exit from the state if hold time expires.
 *
 *  Entry condition:
 *  WC is in HoldTimeState state.
 *
 *  Exit condition:
 *   HoldTime is completed.
 */
void HoldOffTimePassAct(void)
{
   TimeOut -= WCTIME;
   if(TimeOut <= 0)
      TimeExpiredFlag = TRUE;
   if((DCXFunc.CycleStart == TRUE)&&((ControlVariable & IOTRIGGER) != IOTRIGGER))
	  AlarmQueueWrite(CM01);
}

/* void HoldOffTimePassNoAct(void)
 *
 *  Purpose:
 *    This function is the Pass function of HoldTimeState state which will be called every ms
 *    when WC is in this state.
 *    Function will check for HoldTime to expire all the time while in this state.
 *    TimeExpiredFlag will be set to exit from the state if hold time expires.
 *
 *  Entry condition:
 *  WC is in HoldTimeState state.
 *
 *  Exit condition:
 *   HoldTime is completed.
 */
void HoldOffTimePassNoAct(void)
{
   TimeOut -= WCTIME;
   if(TimeOut <= 0)
      TimeExpiredFlag = TRUE;
}


/* void HoldOffTimeExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of HoldTimeState state which will be called once when WC
 *    will leave this state.
 *    Function will reset HoldTimeActive output and set the EndOfHoldTime output.
 *
 *  Entry condition:
 *  WC is leaving HoldTimeState state.
 *
 *  Exit condition:
 *   None
 */
void HoldOffTimeExit(void)
{
   UserIO::SetHoldTimeActive(FALSE);
   UserIO::SetEndOfHoldTime(TRUE);
   UserIO::SetCycleStartOut(FALSE);
}

/* void CheckLimitEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of ChkLimitState state which will be called once when WC
 *    will jump into this state.
 *    Function will send the command to DCP to get the weldresults and also reset the flags used in
 *    this state.
 *
 *  Entry condition:
 *  WC is entering into ChkLimitState state.
 *
 *  Exit condition:
 *   None
 */
void CheckLimitsEntry(void)
{
   WeldStopFlag = FALSE;
   AfterBurstFlag = FALSE;
   NRTcmdFlag = FALSE;
   NRTResponse = NRT_NORESPONSE;
   TempSonicAbortFlag = SonicsAbortFlag;
   SonicsAbortFlag = FALSE;
   WarningFlag = FALSE;
   if((SampleRate > 1) && (JsonWeldBuffTime > 1 ) && ((JsonWeldBuffTime % SampleRate) != 0) && (WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)){//Make sure we don't miss the last weld point for DCX Manager
      	 WriteWeldDatForDCXM(&WeldBuffer[WeldBufferIndex], JsonWeldBuffTime);
      	 JsonWeldBuffTime = 0;
    }
}

/* void CheckLimitsPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of ChkLimitState state which will be called every ms
 *    when WC is in this state.
 *    Function will wait for the weld result from DCP and then it will check for limit alarms
 *    applicable in current weld mode and create the alarm accordingly.
 *    At the end it will look if Afterburst is enabled and set AfterBurstFlag to jumps to that
 *    state if enabled otherwise set WeldStopFlag to go to the PreReady State.
 *
 *  Entry condition:
 *  WC is in ChkLimitState state.
 *
 *  Exit condition:
 *   limit check is completed.
 */
void CheckLimitsPass(void)
{
   if((NRTcmdFlag == FALSE) && (RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT){
      NRTcmdFlag = TRUE;
      NRTcmd = (NrtQ){CMD_GETWELDRESULT, READ_REQ, 0};
      NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
   }
   else{
      if(NRTResponse == REPLY_RECEIVED){//Got the results from DCP.
         /* *  **  First we deal with Energy  **  * */
         if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) == CONTROL_LIMITS_ENABLE){
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYMLR) == CONTROL_LIMITS_ENERGYMLR){
               if(RunningPreset.LimitsParam.EnergyMLR > WeldHistoryData.WeldEnergy){
            	  UserIO::SetMinusEnergyLimitAlarm(TRUE);
                  AlarmQueueWrite(LM07);
               }
            }
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYPLR) == CONTROL_LIMITS_ENERGYPLR){
               if (RunningPreset.LimitsParam.EnergyPLR < WeldHistoryData.WeldEnergy){
            	  UserIO::SetPlusEnergyLimitAlarm(TRUE);
                  AlarmQueueWrite(LM08);
               }
            }
            /**  **  Next we deal with Peak Power    **  **/
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_POWERMLR) == CONTROL_LIMITS_POWERMLR){
               if (RunningPreset.LimitsParam.PowerMLR > WeldHistoryData.PeakPower){
            	  UserIO::SetMinusPeakPowerLimitAlarm(TRUE);
                  AlarmQueueWrite(LM03);
               }
            }
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_POWERPLR) == CONTROL_LIMITS_POWERPLR){
               if (RunningPreset.LimitsParam.PowerPLR < WeldHistoryData.PeakPower){
            	  UserIO::SetPlusPeakPowerLimitAlarm(TRUE);
                  AlarmQueueWrite(LM04);
               }
            }
            /**  **  Finally we deal with Time    **  **/
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEMLR) == CONTROL_LIMITS_TIMEMLR){
               if (RunningPreset.LimitsParam.WeldTimeMLR > WeldHistoryData.WeldTime){
            	  UserIO::SetMinusTimeLimitAlarm(TRUE);
                  AlarmQueueWrite(LM05);
               }
            }
            if((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEPLR) == CONTROL_LIMITS_TIMEPLR){
               if (RunningPreset.LimitsParam.WeldTimePLR < WeldHistoryData.WeldTime){
            	  UserIO::SetPlusTimeLimitAlarm(TRUE);
                  AlarmQueueWrite(LM06);
               }
            }
         }
         /**  **  Finally we deal with amplitude step not occurring  **  **/
         if((RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_OFF) &&  /* If stepping on and */
               (RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_EXTERNALAIN) && /* amp is internal &  */
               (AmpStepReachedFlag == FALSE)){         /* not stepped yet    */
        	 if((RunningPreset.WcParam.ABFlag == TRUE) && (SonicsAbortFlag == FALSE) && (TempSonicAbortFlag == FALSE))
        	        WarningFlag = TRUE;
            AlarmQueueWrite(WA04);
         }
         if((TempSonicAbortFlag == TRUE)|| (SonicsDisableFlag == TRUE)){
        	WarningFlag = FALSE;
            SonicsAbortFlag = TRUE;
            WriteWeldHistory();
         }
         else if(RunningPreset.WcParam.ABFlag == TRUE)
            AfterBurstFlag = TRUE;
         else if(AfterBurstFlag == FALSE){
            WeldStopFlag = TRUE;

            WriteWeldHistory();
         }
      }
   }
}

/* void CheckLimitExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of ChkLimitState state which will be called once when WC
 *    will leave this state.
 *    Function will store the weld results in history and will set the WeldCycleComplete output
 *    and reset the CycleStart Output only if AfterBurst is not enabled.
 *
 *  Entry condition:
 *  WC is leaving ChkLimitState state.
 *
 *  Exit condition:
 *   None
 */
void CheckLimitsExit(void)
{
   if(AfterBurstFlag == FALSE){
	  UserIO::SetWeldCycleComplete(TRUE);
      if(AlarmIndex == 0){
    	  FBStat.CycleOK = TRUE;
    	  UserIO::SetCycleOK(TRUE);
      }
   }
   UserIO::SetCycleStartOut(FALSE);
   if(SonicsAbortFlag == TRUE){
      if(WeldHistoryData.AvgAmplitude2 > 0)
         AvgWeldAmplitude = (WeldHistoryData.AvgAmplitude1 + WeldHistoryData.AvgAmplitude2)/2;
      else
         AvgWeldAmplitude = WeldHistoryData.AvgAmplitude1;
      TempAlarmDataObj.AvgWeldAmplitude = AvgWeldAmplitude;
      TempAlarmDataObj.WeldStartFrequency = WeldHistoryData.WeldStartFrequency;
      TempAlarmDataObj.WeldStopFrequency = WeldHistoryData.WeldStopFrequency;
      TempAlarmDataObj.WeldTime = WeldHistoryData.WeldTime;
      TempAlarmDataObj.WeldEnergy = WeldHistoryData.WeldEnergy;
      TempAlarmDataObj.WeldPower = WeldHistoryData.PeakPower;
   }
}

/* void AfterBurstEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of AfterBurstState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *
 *  Entry condition:
 *  WC is entering into AfterBurstState state.
 *
 *  Exit condition:
 *   None
 */
void AfterBurstEntry(void)
{
   SonicsAbortFlag = FALSE;
   ABDelayFlag = TRUE;
   TempABDly = CONV_MILLI_MICROSEC(RunningPreset.WcParam.ABDelay);
   AbCompleted = FALSE;
   ABTimer = RunningPreset.WcParam.ABTime - ABRAMPDOWNTIME;
   TempABTimer = CONV_MILLI_MICROSEC(ABTimer);
   RtSendData.CmdFlags |= MB_DISABLESTEPPING;
   UserIO::SetAfterBurstDelayActive(TRUE);
}

/* void AfterBurstPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of AfterBurstState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for ABDelay to expire.
 *    2.> Set Amplitude to ABAmp and start Sonics.
 *    3.> Wait for ABTime to expire.
 *    4.> Set the Amplitude to 5% and RampTime to 10ms and than wait for ramp time to finish.
 *    5.> Set AbCompleted flag to jump to preready state.
 *
 *  Entry condition:
 *  WC is in AfterBurstState state.
 *
 *  Exit condition:
 *   Afterburst is completed.
 */
void AfterBurstPass(void)
{
   if(ABDelayFlag == TRUE){
      TempABDly -= WCTIME;
      if(TempABDly <= 0){
         ABDelayFlag = FALSE;
         UserIO::SetAfterBurstDelayActive(FALSE);
         UserIO::SetAfterBurstTimeActive(TRUE);
         /******* After Burst Time State Start**************/
         RtSendData.AmplitudeIn = RunningPreset.WcParam.ABAmp;
         RtSendData.CmdFlags |= (MB_START | MB_USEREALTIMEDATA);
         RtSendData.CmdFlags &= ~MB_ACCUM_REQ;
         ABTimeFlag = TRUE;
         /***********************************************/
      }      
   }
   else if(ABTimeFlag == TRUE){
	  TempABTimer -= WCTIME;
      if(TempABTimer <= 0){
         ABTimeFlag = FALSE;
         RtSendData.AmplitudeIn = ABAMPLITUDE;             /* 5% amplitude              */
         RtSendData.RampTime = ABRAMPDOWNTIME;
         RampTime = ABRAMPDOWNTIME;                      /* How long to ramp down(10ms)*/
      	 RampRunningTime = WCTIME;                            /* Initial ramp counter       */
      }
   }
   else{
      if(CONVMICRO_MILLI(RampRunningTime) >= RampTime){    //10ms check
         RtSendData.AmplitudeIn = 0;
         RtSendData.CmdFlags &= ~MB_START; //Stop weld 
         if(WarningFlag == TRUE)
            SonicsAbortFlag = TRUE;
         else
        	 AbCompleted = TRUE;
         UserIO::SetAfterBurstTimeActive(FALSE);
      }
      RampRunningTime += WCTIME;
   }
   UpdatePeakPower();
}


/* void AfterBurstExit(void)
 *
 *  Purpose:
 *    This function the Exit function of AfterBurstState state.
 *
 *  Entry condition:
 *  WC is leaving AfterBurst State.
 *
 *  Exit condition:
 *   None
 */
void AfterBurstExit(void)
{
	WarningFlag = FALSE;
	UserIO::SetAfterBurstDelayActive(FALSE);
	UserIO::SetAfterBurstTimeActive(FALSE);
	UserIO::SetWeldCycleComplete(TRUE);
	WriteWeldHistory();
	if(AlarmIndex == 0){
		FBStat.CycleOK = TRUE;
		UserIO::SetCycleOK(TRUE);
	}
}

/* void CycleAbortEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of CycleAbortState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *    It will also reset the SonicsActive output.
 *
 *  Entry condition:
 *  WC is entering into CycleAbortState state.
 *
 *  Exit condition:
 *   None
 */
void CycleAbortEntry(void)
{
   WeldStopFlag = FALSE;
   RtSendData.CmdFlags &= ~MB_START;
   NRTcmdFlag = FALSE;
   NRTResponse = NRT_NORESPONSE;
   WeldDoneWithAlarm = FALSE;
   SonicsOffPeriod = 0;
   if((SampleRate > 1) && (JsonWeldBuffTime > 1 ) && ((JsonWeldBuffTime % SampleRate) != 0) && (WeldBufferIndex >= 0) && (WeldBufferIndex < SIZE_WELDBUFFER)){//Make sure we don't miss the last weld point for DCX Manager
   		 WriteWeldDatForDCXM(&WeldBuffer[WeldBufferIndex], JsonWeldBuffTime);
   		JsonWeldBuffTime = 0;
   }
}

/* void CycleAbortPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of CycleAbortState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for DCP to stop Sonics.
 *    2.> Send Get Weld Result command to DCP.
 *    3.> Wait for Weld Results from DCP.
 *    4.> Prepare and store Final Weld Results into History.
 *    5.> Set WeldStopFlag flag to jump to next state i.e. PreReady.
 *
 *  Entry condition:
 *  WC is in CycleAbortState state.
 *
 *  Exit condition:
 *   CycleAbort process is completed.
 */
void CycleAbortPass(void)
{
   if(HistoryValid == FALSE){
	   memset(&WeldHistoryData, 0, sizeof(WeldHistoryData));
	   WriteWeldHistory();
	   if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE))
		   WeldDoneWithAlarm = TRUE;
	   else
		   WeldStopFlag = TRUE;
   }
   else{
	   if((NRTcmdFlag == FALSE) && (RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT){
		  NRTcmdFlag = TRUE;
		  NRTcmd = (NrtQ){CMD_GETWELDRESULT, READ_REQ, 0};
		  NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
	   }
	   if(NRTResponse == REPLY_RECEIVED)
		   WriteWeldHistory();
	   if(NRTResponse != NRT_NORESPONSE){
		  if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE))
			 WeldDoneWithAlarm = TRUE;
		  else
			 WeldStopFlag = TRUE;
	   }
   }
}

/* void CycleAbortExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of CycleAbortState state which will be called once when WC
 *    will leave this state.
 *    Function will set the WeldCycleComplete output and reset the CycleStart Output.
 *
 *  Entry condition:
 *  WC is leaving CycleAbortState state.
 *
 *  Exit condition:
 *   None
 */
void CycleAbortExit(void)
{
   UserIO::SetWeldCycleComplete(TRUE);
   UserIO::SetCycleStartOut(FALSE);
//   if((ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag == TRUE)
//         &&(AfterBurstFlag == FALSE))
//      StoredFrequencyAbs[ParameterSet[CurrPreset].InfoParam.HornPresetNumber] = RtReceiveData.MemOut;

   if((NRTResponse == REPLY_RECEIVED) && (WeldDoneWithAlarm == TRUE)){
      if(WeldHistoryData.AvgAmplitude2 > 0)
         AvgWeldAmplitude = (WeldHistoryData.AvgAmplitude1 + WeldHistoryData.AvgAmplitude2)/2;
      else
         AvgWeldAmplitude = WeldHistoryData.AvgAmplitude1;
      TempAlarmDataObj.AvgWeldAmplitude = AvgWeldAmplitude;
      TempAlarmDataObj.WeldStartFrequency = WeldHistoryData.WeldStartFrequency;
      TempAlarmDataObj.WeldStopFrequency = WeldHistoryData.WeldStopFrequency;
      TempAlarmDataObj.WeldTime = WeldHistoryData.WeldTime;
      TempAlarmDataObj.WeldEnergy = WeldHistoryData.WeldEnergy;
      TempAlarmDataObj.WeldPower = WeldHistoryData.PeakPower;
   }
//   else{ 
//	   FBStat.CycleOK = TRUE;
//	   UserIO::SetCycleOK(TRUE);
//   }
}

/* void ErrorEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of ErrorState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *    It will reset the CycleStart output and set the AlarmCycleAbort or SonicsAbortFlag
 *    output signal based on the current Alarm. It will send the reset command to DCP to reset
 *    the Alarm on DCP side anyway so that DCP will come in ready State.
 *
 *  Entry condition:
 *  WC is entering into ErrorState state.
 *
 *  Exit condition:
 *   None
 */
void ErrorEntry(void)
{
   if((Sysconfig->Externalstatus == TRUE) && (Sysconfig->AlarmFlag == TRUE)){
      ExternalStatusTask::thisPtr -> ExtStatusTaskQ.Write(0, 'a', 0);   //Output 3Second Pulse
   }
   AlarmFlag = TRUE;
   ResetDone = FALSE;
   CurrentAlarm = AlarmsArray[0];
   UserIO::SetCycleStartOut(FALSE);
   //UserIO::SetGenAlarm(TRUE);

   CreateAlarm();
   WriteAlarmForDCXM();
   RtSendData.CmdFlags = 0; //Clear all the flags to DCP
   RtSendData.CmdFlags |= MB_RESET; //Reset DCP anyway.
   RtSendData.CmdFlags &= ~MB_START; //Reset DCP anyway.
   if(MFAlarmFlag == TRUE){      //If Alarm comes from Manufacturing Page
      if(CurrentCycleParam->StopOnAlarm == TRUE)
         ResetReqFlag = TRUE;
      else
         ResetReqFlag = FALSE;
   }
   ResetTimer = 0;
}

/* void ErrorPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of ErrorState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for Alarm Reset from LCD/WebPage/External I/O or from FB.
 *    2.> Clear the reset signal to DCP and wait for DCP to come into ready state.
 *    3.> Set ResetDone flag to jump to PreReady state.
 *
 *  Entry condition:
 *    WC is in ErrorState state.
 *
 *  Exit condition:
 *    Reset process is completed.
 */
void ErrorPass(void)
{
   RtSendData.CmdFlags |= MB_RESET; //To be sure DCP is getting reset Added the statement in pass function
   if(ResetReqFlag == TRUE){
	  if((ControlVariable & RESET_MASK) != 0){
         ResetFlag = TRUE;
      }
   }
   else
      ResetFlag = TRUE;

   if((Sysconfig->ClearMemoryWithReset == TRUE) || (MemoryClearAtRst))//Once MemoryClearAtRst is true make sure it doesn't go false
	   MemoryClearAtRst = TRUE;//Using same flag(MemoryClearAtRst) here which is used for checking whether reset and memory clear on user IO are shorted and active
   else
	   MemoryClearAtRst = CheckMemClearReset();
   ResetTimer += WCTIME;
   if((ResetFlag == TRUE)&&(DCPReadyFlag == TRUE)&&(ResetTimer >= MAX_RESETTIME)){
         AlarmFlag = FALSE;
         if(ResetReqFlag == TRUE || ACLineResetFlag == TRUE){
               ResetDone = TRUE;
               ClearAlarmOutputs();
               AlarmIndex = 0;
               AlarmLogIndx = 0;
               FBStat.Error = 0;
               FBStat.CycleOK = FALSE;
               FBStat.EmergencyStopActive = 0;
               memset(FBAlarms, 0, sizeof(FBAlarms));
               AlarmStatus = FALSE;    //stop pulse
               ACLineResetFlag = FALSE;
         }//if(ResetReqFlag == TRUE)
         else
            ResetDone = TRUE;
      }  //if(ResetFlag == TRUE)
}

/* void ErrorExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of ErrorState state which will be called once when WC
 *    will leave this state.
 *    Function will Reset the Overload and General Alarm output.
 *
 *  Entry condition:
 *  WC is leaving ErrorState state.
 *
 *  Exit condition:
 *   None
 */
void ErrorExit(void)
{
   WebSite::ResetWebResetFlag();
   ControlVariable &= ~AC_RESTORE_RESET;
   RtSendData.CmdFlags &= ~MB_RESET;
   ResetFlag = FALSE;
   AlarmCycleAbort = FALSE;
   SonicsAbortFlag = FALSE;
   ResetReqFlag = FALSE;
}

/* void SeekEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of SeekState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *    It will set the MB_SEEK bit to DCP to start Seek.
 *
 *  Entry condition:
 *    WC is entering into SeekState state.
 *
 *  Exit condition:
 *    None.
 */
void SeekEntry(void)
{
   PowerUpDone = TRUE;     // Powerup done in case of power up seek.
   RtSendData.CmdFlags = MB_SEEK;
   RtSendData.CmdFlags |= MB_ACCUM_REQ;
   SeekStarted = FALSE;
   SeekDone = FALSE;
   AvgAmplitude = 0;
   SeekAccumAmplitude = 0;
   SeekTime = 0;
   SeekAmpCount = 0;
   WeldBufferIndex = -1;
   WeldBufferTime = 0;
   NRTcmdFlag = FALSE;
   NRTResponse = NRT_NORESPONSE;
   MemoryReset = FALSE;
   SeekAlarmFlag = FALSE;
   SonicsDisableFlag = FALSE;
   RtSendData.CmdFlags &= ~MB_SONICS_DISABLE;
   DCXFunc.SeekStart = FALSE;
   if(UserIO::CheckForSonicsDisable() == TRUE){
      SonicsDisableFlag = TRUE;
      RtSendData.CmdFlags |= MB_SONICS_DISABLE;
      AlarmQueueWrite(WA05);
   }
}

/* void SeekPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of SeekState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for MB_SEEKSCAN_OUT bit from DCP and than Set Seek output to digital I/O.
 *    2.> Wait for MB_SEEKSCAN_OUT bit to be cleared and than fill the seek status data and store it.
 *    3.> Set SeekDone flag to jump to PreReady state.
 *
 *  Entry condition:
 *  WC is in SeekState state.
 *
 *  Exit condition:
 *   Seek process is completed.
 */
void SeekPass(void)
{
   SINT32 Data2 = 0, Temp;
   if(SeekStarted == FALSE){
      if((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) == MB_SEEKSCAN_OUT){
         SeekStarted = TRUE;
         SeekAccumAmplitude += RtReceiveData.AmplitudeOut;
		 SeekAmpCount += WCTIME;
		 RtSendData.CmdFlags &= ~MB_SEEK;
      }
   }
   else{
      if(((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) != MB_SEEKSCAN_OUT) && (NRTcmdFlag == FALSE)){
         //Collect Seek Stop Freq and Memory Freq here.Collect End parameters here.
         //Calculate Average Amplitude here.
       	 SeekAmpCount = (SeekAmpCount / WCTIME);
         if(SeekAmpCount > 0)
            AvgAmplitude = SeekAccumAmplitude/SeekAmpCount;
         else
            AvgAmplitude = SeekAccumAmplitude;
         RtSendData.CmdFlags &= ~MB_SEEK;
         NRTcmdFlag = TRUE;
         NRTcmd = (NrtQ) {CMD_GETSEEKDATA, READ_REQ, 0};
         NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
      }
      else if(NRTResponse == REPLY_RECEIVED){
         if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE))
            SeekAlarmFlag = TRUE;
         else
            SeekDone = TRUE;

         SeekTime = WeldBufferTime;
         if(SeekAlarmFlag == FALSE){
        	 Temp = (RtReceiveData.MemOut - RunningHSPreset.DigitalTuneScaledUP);
        	 HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = Temp;
        	 HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = Temp;
        	 Data2 = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut)) /FREQ_SCALING_FACTOR;
         }
         else{
        	 HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
        	 HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
        	 MemoryReset = TRUE;
         }

         //Fill the HORNSTATUS Set and store
         SaveHSStatusSet(HFSEEK);
         if(SeekAlarmFlag == TRUE){
            TempAlarmDataObj.SeekStartFrequency = SeekData.SeekStartFrequencyAbs;
            TempAlarmDataObj.SeekStopFrequency = SeekData.SeekStopFrequencyAbs;
            Data2 = SeekData.SeekStopFrequencyAbs;
         }
         else
            FBStat.CycleOK = TRUE;
         Log::WriteEventLog(EVENT_SEEK, SeekData.SeekStartFrequencyAbs, Data2,ParameterSet[CurrPreset].InfoParam.HornPresetNumber);
      }
      else if(NRTResponse == TIMEOUT)
         SeekAlarmFlag = TRUE;
      else{
    	  SeekAccumAmplitude += RtReceiveData.AmplitudeOut;
    	  SeekAmpCount += WCTIME;
      }

   }
   UpdatePeakPower();
}

/* void SeekExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of SeekState state which will be called once when WC
 *    will leave this state.
 *    Function will clear MB_SEEK bit to DCP.
 *
 *  Entry condition:
 *  WC is leaving SeekState state.
 *
 *  Exit condition:
 *   None
 */
void SeekExit(void)
{
   RtSendData.CmdFlags &= ~MB_SEEK;
   RtSendData.CmdFlags &= ~MB_ACCUM_REQ;
   SonicsOffPeriod = 0;
   TimedSeekTimerTicks = 0;
   ControlVariable &= ~POWERUPSEEK;
   ControlVariable &= ~WEB_SEEK;
   ControlVariable &= ~TIMEDSEEK;
   ControlVariable &= ~AC_RESTORE_SEEK;
}

/* void TestEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of TestState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *    It will set the MB_TSET bit to DCP to start TEST. It will also write the Event in eventlog.
 *
 *  Entry condition:
 *  WC is entering into TestState state.
 *
 *  Exit condition:
 *   None
 */
void TestEntry(void)
{
   RtSendData.CmdFlags = MB_TEST;
   if (HsParameterSet[CurrHsPreset].TestSelection == TEST_AMP1_EXT_AIN)   //If require to run a test with the preset parameter verify configure the
	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	  //flags to run the test.
   {

	   if((RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_EXTERNALAIN)
	            &&(RunningPreset.WcParam.AmpStepCondition != AMPSTEP_COND_EXTERNAL)){
	            //Use Preset Amplitude in case of Stepping at Time/Energy/Power
	   	   	   RtSendData.CmdFlags &= ~MB_USEREALTIMEDATA;
	   	   }
	   	   else if (RunningPreset.WcParam.AmpStepCondition  == AMPSTEP_COND_EXTERNALAIN){
	            //Use External Amplitude in case of External Amplitude.


	       	  if(IsManualFunctionAvailable() == TRUE)
	       	  {
	       		 RtSendData.AmplitudeIn = UserIO::GetExternalAmplitude();
	       		 RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
	       	  }
	       	  else
	       		  RtSendData.AmplitudeIn = FieldBus::GetExternalAmplitude();


	   	   }
   }
   DCXFunc.TestStart = FALSE;
   TestStarted = FALSE;
   TestDone = FALSE;
   AvgAmplitude = 0;
   TestTime = 0;
   WeldBufferIndex = -1;
   WeldBufferTime = 0;
   if((ControlVariable & FBTEST) == FBTEST){
		if(FieldBusType == HW_ASSEMBLY_PROFIBUS)
			TestData1 = COMM_IFC_PB;
		else if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
			TestData1 = COMM_IFC_ETHIP;
   }
   else if((LastCmd & WEBTEST) == WEBTEST)
      TestData1 = COMM_IFC_WEB;
   else if((LastCmd & LCDTEST) == LCDTEST)
	   TestData1 = COMM_IFC_LCD;
   else if((LastCmd & IOEXTTEST) == IOEXTTEST)
	   TestData1 = COMM_IFC_USERIO;
   else
	   TestData1 = LastCmd;

   Log::WriteEventLog(EVENT_TEST_ACTIVATED, TestData1);
   NRTcmdFlag = FALSE;
   NRTResponse = NRT_NORESPONSE;
   MemoryReset = FALSE;
   TestAlarmFlag = FALSE;
   TestComplete = FALSE;
   SonicsDisableFlag = FALSE;
   RtSendData.CmdFlags &= ~MB_SONICS_DISABLE;
   if(UserIO::CheckForSonicsDisable() == TRUE){
      SonicsDisableFlag = TRUE;
      RtSendData.CmdFlags |= MB_SONICS_DISABLE;
      AlarmQueueWrite(WA05);
   }
}

/* void TestPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of TestState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for MB_SEEKSCAN_OUT/MB_SONICACTIVE_OUT bit from DCP.
 *    2.> Wait for StartTest condition to become false (Test disabled from LCD/WebPage/FB/External I/O).
 *    3.> Wait for MB_SEEKSCAN_OUT/MB_SONICACTIVE_OUT bit to be cleared and than send GetWeldResults
 *    command to DCP to get Test Results.
 *    4.> Wait for Test Results and store that in TEST Status set.
 *    3.> Set TestDone flag to jump to PreReady state.
 *
 *  Entry condition:
 *  WC is in TestState state.
 *
 *  Exit condition:
 *   Test process is completed.
 */
void TestPass(void)
{
   if(TestStarted == FALSE){
      if(((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) == MB_SEEKSCAN_OUT)
           && ((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) == MB_SONICACTIVE_OUT)){
         TestStarted = TRUE;
         TestComplete = FALSE;
       }
   }
   else{
    	if(TestComplete == TRUE){
         if((RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT) != MB_SONICACTIVE_OUT){
            if(NRTcmdFlag == FALSE){   //first send NRT command
               NRTcmdFlag = TRUE;
               NRTcmd = (NrtQ){CMD_GETWELDRESULT, READ_REQ, 0};
               NRTModbus::thisPtr -> Nrtq.Write(RTOS::GetCurrentTask(), NRTcmd, 0);
            }
            else if(NRTResponse == REPLY_RECEIVED){//wait for results here
               if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE))
                  TestAlarmFlag = TRUE;
               else
                  TestDone = TRUE;
            }
            else if(NRTResponse == TIMEOUT)
               TestAlarmFlag = TRUE;

         }
      }
      else
      {
    	  if((ControlVariable & TESTSTART_MASK) == 0){
    		  RtSendData.CmdFlags &= ~MB_TEST;
    		  TestComplete = TRUE;
    	  }

      }
   }
   if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE)){
      RtSendData.CmdFlags = 0;
      TestComplete = TRUE;
   }
   UpdatePeakPower();
   if (HsParameterSet[CurrHsPreset].TestSelection == TEST_AMP1_EXT_AIN && RunningPreset.WcParam.AmpStepCondition  == AMPSTEP_COND_EXTERNALAIN)  //If require to run a test with the preset parameter verify the stepping
	   CheckForStepping();
}

/* void TextExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of TestState state which will be called once when WC
 *    will leave this state.
 *    Function will write a event in case of Test Overload.
 *
 *  Entry condition:
 *  WC is leaving TestState state.
 *
 *  Exit condition:
 *   None
 */
void TestExit(void)
{
   WebSite::ResetTestFlag();
   SINT32 Temp;
   if(OverLoadAlarm == TRUE){
      Log::WriteEventLog(EVENT_TEST_OVERLOAD);
      MemoryReset = TRUE;
      HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
      HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
   }
   else{
	   Temp = (RtReceiveData.MemOut - RunningHSPreset.DigitalTuneScaledUP);
      HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = Temp;
      HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP =  Temp;
   }
   if(TestAlarmFlag == TRUE){
      if(WeldHistoryData.AvgAmplitude2 > 0)
         AvgWeldAmplitude = (WeldHistoryData.AvgAmplitude1 + WeldHistoryData.AvgAmplitude2)/2;
      else
         AvgWeldAmplitude = WeldHistoryData.AvgAmplitude1;
      TempAlarmDataObj.AvgWeldAmplitude = AvgWeldAmplitude;
      TempAlarmDataObj.WeldStartFrequency = WeldHistoryData.WeldStartFrequency;
      TempAlarmDataObj.WeldStopFrequency = WeldHistoryData.WeldStopFrequency;
      TempAlarmDataObj.WeldTime = WeldHistoryData.WeldTime;
      TempAlarmDataObj.WeldEnergy = WeldHistoryData.WeldEnergy;
      TempAlarmDataObj.WeldPower = WeldHistoryData.PeakPower;
   }
   else
	   FBStat.CycleOK = TRUE;
   TestTime = WeldBufferTime;
   SaveHSStatusSet(HFTEST);
   SonicsOffPeriod = 0;
}

/* void ScanEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of ScanState state which will be called once when WC
 *    will jump into this state.
 *    Function will reset the flags and initialize the variables used in this state.
 *    It will set the MB_HSCAN bit to DCP to start SCAN. It will also write the Event in eventlog.
 *
 *  Entry condition:
 *    WC is entering into ScanState state.
 *
 *  Exit condition:
 *    None.
 */
void ScanEntry(void)
{
   RtSendData.CmdFlags = MB_HSCAN;
   NumScanPoints = -1;
   ScanDone = FALSE;
   ScanFailed = FALSE;
   ScanStarted = FALSE;
   ScanAborted = FALSE;
   ScanTime = 0;
   PowerUpDone = TRUE;     // Powerup done in case of power up scan .
   Log::WriteEventLog(EVENT_HORN_SCAN_START, (HsParameterSet[CurrHsPreset].ParaScan.FrequencyStart/SCANFREQFACTOR));
   MemoryReset = FALSE;
   AbortReason = 0;
}

/* void ScanPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of ScanState state which will be called every ms
 *    when WC is in this state.
 *    Function is doing following functionalities in following sequence:
 *    1.> Wait for MB_SEEKSCAN_OUT bit from DCP. *
 *    2.> Wait for MB_SEEKSCAN_OUT bit to be cleared and also check for ScanTime to reach to Timeout.
 *    3.> Also check for ScanAbort conditions (Abort from user input or from webpage/FB).
 *    4.> Calculate the resonance if not aborted and scan is completed from DCP.
 *    3.> Set ScanDone flag to jump to PreReady state.
 *
 *  Entry condition:
 *     WC is in ScanState state.
 *
 *  Exit condition:
 *     Scan process is completed.
 */
void ScanPass(void)
{
   static BOOLEAN AbortSCAN = FALSE;
   if(ScanStarted == FALSE){
      if((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) == MB_SEEKSCAN_OUT){
         ScanStarted = TRUE;
         ScanTime += WCTIME;
         AbortSCAN  = FALSE;
      }
   }
   else{
      ScanTime += WCTIME;
      if((RtReceiveData.DigOutputs & MB_SEEKSCAN_OUT) != MB_SEEKSCAN_OUT){
         RtSendData.CmdFlags &= ~MB_HSCAN;
         if(AbortSCAN == TRUE){
            ScanAborted = TRUE;
            WebSite::ScanStartFlag = FALSE;
         }
         else{
            if((RtReceiveData.StatusFlags & SIGNAL_COLLECTHORNSCANPOINT) != SIGNAL_COLLECTHORNSCANPOINT){
               DetectResonance();
               WebSite::ResetScanFlags(FALSE);
               ScanDone = TRUE;
            }
         }
      }

      if ((ScanTime > SCANTIMEOUT) ||((ControlVariable & SCANSTART_MASK) != LastCmd)){
         WebSite::ResetScanFlags(FALSE);
         RtSendData.CmdFlags &= ~MB_HSCAN;
         AbortSCAN = TRUE;
      }
      else
         AbortSCAN  = FALSE;
   }
}

/* void ScanExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of ScanState state which will be called once when WC
 *    will leave this state.
 *    Function will write a event in case of scan completed or aborted. It will also store
 *    the Scan Status set into memory.
 *
 *  Entry condition:
 *  WC is leaving ScanState state.
 *
 *  Exit condition:
 *   None
 */
void ScanExit(void)
{
   SINT32 Temp;
   ControlVariable &= ~AC_RESTORE_SCAN;
   if(PowerUpScan == TRUE){
      ControlVariable &= ~POWERUPSCAN;
      PowerUpScan = FALSE;
   }
   if(SonicsAbortFlag == TRUE)
      ScanFailed = TRUE;
   if(ScanAborted == TRUE){
      if(ScanTime > SCANTIMEOUT)
          AbortReason = SCANABORT_TIMEOUT;
      else if((ControlVariable & SCANSTART_MASK) != LastCmd)
      {
         if((LastCmd & WEBSCAN) == WEBSCAN){
            AbortReason = SCANABORT_USER;
         }
         else if((LastCmd & IOSCAN) == IOSCAN)
            AbortReason = SCANABORT_IO;
         else if((LastCmd & FBSCAN) == FBSCAN)
            AbortReason = SCANABORT_FB;
      }
      Log::WriteEventLog(EVENT_HORN_SCAN_ABORTED, ScanBuffer[NumScanPoints].Frequency, AbortReason);
   }
   else if(ScanFailed == TRUE){
	  HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
	  HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
	  Log::WriteEventLog(EVENT_HORN_SCAN_FAILED, ScanBuffer[NumScanPoints].Frequency);
	  MemoryReset = TRUE;
   }
   ScanStarted = FALSE;
   //Store Scan Status set here.
   WebSite::ResetScanFlags();
   if((ScanAborted == TRUE) || (ScanFailed == TRUE) || (MemoryValue <= 0)){
	 //  StoredFrequencyAbs[ParameterSet[CurrPreset].InfoParam.HornPresetNumber] = SCALE_UP_FOR_N_BITS(Midband);
   }
   else{
		if(Sysconfig->SetDigTuneWithHornScan)
		{
			HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].DigitalTuneScaledUP =
			SCALE_UP_FOR_N_BITS(MainParallelResonanceFreq * FREQ_SCALING_FACTOR);			
			CHECKANDLOG_HSPARMCHANGE(CurrHsPreset, DigitalTuneScaledUP, SCALE_UP_FOR_N_BITS(MainParallelResonanceFreq * FREQ_SCALING_FACTOR), DigitalTune, MainParallelResonanceFreq, 0);
			HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
			HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
			Sysconfig->PresetChanged = TRUE;//To make sure preset is transferred to DCP on next weld,seek,test start
		}
		else
		{
			Temp = (SCALE_UP_FOR_N_BITS(MainParallelResonanceFreq * FREQ_SCALING_FACTOR) - RunningHSPreset.DigitalTuneScaledUP);
			HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = Temp;
			HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = Temp;
		}
   }
   SonicsOffPeriod = 0;
   SaveHSStatusSet(HFSCAN);
}

/* void MfgCycleStartEntry(void)
 *
 *  Purpose:
 *    This function the Entry function of ManFCycleState state which will be called once when WC
 *    will jump into this state.
 *    Function will increment the MFcycleResults->nCycles and also reset the timer variables
 *    and flags used in this state and
 *    function will set Amplitude and MB_USEREALTIMEDATA bit in DCP real time command
 *    This function will also set the MB_START bit to start Sonics on DCP side and set Sonics Active output
 *
 *  Entry condition:
 *  WC is entering into ManFCycleState state.
 *
 *  Exit condition:
 *   None
 */
void MfgCycleStartEntry(void)
{
   WeldBufferIndex = -1;
   RtSendData.AmplitudeIn = CurrentCycleParam->Amplitude;
   RtSendData.CmdFlags |= MB_ACCUM_REQ;
   if(MFAlarmFlag == TRUE)
      MFSonicsON = FALSE;
   else{
      RtSendData.CmdFlags |= MB_START;
      MFcycleResults->CyclesCount++;
      MFSonicsON = TRUE;
   }
   SonicsOnCounter = 0;
   SonicsOffCounter = 0;
   TimeOut = 0;
   MFTimeoutEnable = FALSE;
   MFcycleResults->Cyclestatus = STATECYCLING;
   MFAlarmFlag = FALSE;
}/* ManFCycle_Start() */

/* void MfgCycleRunPass(void)
 *
 *  Purpose:
 *    This function is the Pass function of ManFCycleState state which will be called everyms when WC
 *    is in this state.
 *    Function will do the following things in sequence:
 *    1.> Check for totalCycleTime is expired. If expired then set MFCycle state to STATECOMPLETED.
 *    and store CurrentCycleParams. Set MFTimeoutEnable to jump to PreReady state.
 *    2.> check if MFSonicsON is set (OnTime is active) and SIGNAL_PRIMARYPARAMETER_REACHED is set from DCP.
 *    If set than start SonicsOff Time.
 *    3.> Check for Sonics Off time to expire and than set Sonics to ON for next cycle and increment
 *    cycle count.
 *
 *  Entry condition:
 *    WC is in ManFCycleState state.
 *
 *  Exit condition:
 *   None
 */
void MfgCycleRunPass(void)
{
   if(CurrentCycleParam->MfCycling == TRUE){
	   MFcycleResults->CurrCycleTime++;
	   if(SCALEDTIMETOWEBSITE(MFcycleResults->CurrCycleTime) >= CurrentCycleParam->TotalCycleTime){
	      DCXFunc.MFCycleStart = FALSE;
	      MFcycleResults->Cyclestatus = STATECOMPLETED;
	      CurrentCycleParam->MfCycling = FALSE;
	      SonicsOnCounter = 0;
	      SonicsOffCounter = 0;
	      CurrentCycleParam->CRC = ByteCRC((UINT8*)CurrentCycleParam, sizeof(CycleParam) - sizeof(CurrentCycleParam->CRC));
	      MFTimeoutEnable = TRUE;
	   }
	   else if(MFSonicsON == TRUE){//Sonics are Already On. Check for On Time.
	      if((RtReceiveData.StatusFlags & SIGNAL_PRIMARYPARAMETER_REACHED) == SIGNAL_PRIMARYPARAMETER_REACHED){
	         RtSendData.CmdFlags &= ~MB_START;
	         SonicsOnCounter = CurrentCycleParam->SonicsOnTimePerWeldCycle;
	         if(!(RtReceiveData.DigOutputs & MB_SONICACTIVE_OUT)){
	            MFSonicsON = FALSE;
	            MFcycleResults->SonicsOnTime += SonicsOnCounter;
	         }
	      }
         MFcycleResults->ResultantFrequency = RtReceiveData.FreqOut;
         MFcycleResults->ResultantPower = RtReceiveData.PowerOut;
	   }
	   else{
	      SonicsOffCounter++;
	      if(SCALEDTIMETOWEBSITE(SonicsOffCounter) >= CurrentCycleParam->SonicsOffTimePerWeldCycle){
	         RtSendData.CmdFlags |= MB_START;
	         MFSonicsON = TRUE;
	         SonicsOnCounter = 0;
	         SonicsOffCounter = 0;
	         MFcycleResults->CyclesCount++;
	      }
	   }
   }
   else{
      RtSendData.CmdFlags &= ~MB_START;
      MFTimeoutEnable = TRUE;
   }
   UpdatePeakPower();
}/* ManFCycle_Run() */

/* void MfgCycleExit(void)
 *
 *  Purpose:
 *    This function is the Exit function of ManFCycleState state which will be called once when WC
 *    will leave this state.
 *    Function will clear the MB_START bit and other variables used in this state.
 *
 *  Entry condition:
 *    WC is leaving ManFCycleState state.
 *
 *  Exit condition:
 *    None
 */
void MfgCycleExit(void)
{
   if((AlarmCycleAbort == TRUE) || (SonicsAbortFlag == TRUE)){
      MFAlarmFlag = TRUE;
      IncrementManFErrorCycleCounter();
   }
   else
      MFAlarmFlag = FALSE;
   RtSendData.CmdFlags &= ~MB_ACCUM_REQ;   //clear accumulation bit
   RtSendData.CmdFlags &= ~MB_START;   //Stop weld
   RtSendData.CmdFlags &= ~MB_USEREALTIMEDATA;
   SonicsOffPeriod = 0;
}

/* void CheckForPresetChange(void)
 *
 *  Purpose:
 *    This function is called from ReadyPass function of the state machine.
 *    Function will check if Preset needs to be loaded to DCP in any case:
 *    1.> LoadNewPreset input from External I/O
 *    2.> FB Weld or Horn function or command to load new preset.
 *    3.> CycleStartFlag is set and ExternalRecall input is non zero.
 *    4.> UpdateRPreset Flag is set (that means current preset has updated from webpage/LCD/FB)
 *    Function will set LoadPresetToDCP if preset update is required and than Statemachine
 *    will switch to new state based upon this flag.
 *
 *  Entry condition:
 *  None.
 *
 *  Exit condition:
 *   None
 */
void CheckForPresetChange(void)
{
   //Check if New Preset needs to be loaded. Initiated by External signal or from FB.
   if((UserIO::CheckForExtRecallPreset() != 0)||(FBWeldFunction)|| (CheckForFBLoadNewPreset)||(FBHornFunction)){
      LoadPresetToDCP = TRUE;
   }
   //Checkif New preset needs to be loaded in case of cycle start or trigger from user I/O.
   else if(((DCXFunc.WeldStart == TRUE )||(DCXFunc.CycleStart == TRUE))&&(UserIO::CheckForExtRecallPreset() != 0)){
      LoadPresetToDCP = TRUE;
   }
   //Check if Running preset needs to be updated.Because of Parameter change from Website or from FB or from LCD.
   else if(UpdateRPreset == TRUE){ /*Update Running preset */
      LoadPresetToDCP = TRUE;
   }
   else if((Sysconfig->PresetChanged == TRUE)&&((LoadPresetToDCP == TRUE)||(DCXFunc.ScanStart == TRUE) || (DCXFunc.SeekStart == TRUE)
  		 ||(DCXFunc.TestStart == TRUE)||(DCXFunc.CycleStart == TRUE) || (DCXFunc.WeldStart == TRUE))){
	  LoadPresetToDCP = TRUE;
   }
}

/* void CheckForStepping(void)
 *
 *  Purpose:
 *    This function is called from SonicsPass function of the state machine.
 *    Function will check if Stepping needs to be done based on following conditions:
 *    1.> Check for distance if step at distance is enabled. If reached to the condition than set the Amp2 to DCP
 *    and set Amplitude digital output to Amp2.
 *    2.> Check for Step at external condition and switch to Amplitude 1 or on Amplitude 2 based on the input.
 *    3.> If Amplitude control is external than set the amplitude to external input and set the ramp time to
 *    Amplitude profile ramp time.
 *    4.> If Step at time/energy/power is set than stepping is done on DCP side. If SIGNAL_STEPPING_DONE is set
 *    from DCP than set the profile time and also set the Amplitude digital output to Amp2.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None
 */
void CheckForStepping(void)
{
   if(ScrubTimeActive == FALSE){
	   if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_EXTERNAL  && DCXFunc.TestStart == FALSE){ /* If stepping on for external */
		  //Stepping can be done multiple time in a cycle.
		  // Switch to Amp2 if external input is active otherwise switch to Amp1.
		  if((UserIO::CheckForExternalAmpStep() == TRUE) || (FBCmd.AmpProfiling == TRUE)){
			 AmpStepReachedFlag = TRUE;
			 RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude2;
			 UserIO::SetAmplitude1_2(AMP2);//TRUE for AMP2
		  }
		  else{
			 RtSendData.AmplitudeIn = RunningPreset.WcParam.Amplitude1;
			 UserIO::SetAmplitude1_2(AMP1);//FALSE for AMP1
		  }
		  RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
		  RtSendData.RampTime = RunningPreset.DcpParam.WeldRampTime;
	   }
	   else if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_EXTERNALAIN){
		   if (DCXFunc.TestStart == FALSE)
		   {
			   if(IsManualFunctionAvailable() == TRUE)
				   RtSendData.AmplitudeIn = UserIO::GetExternalAmplitude();
			   else
				   RtSendData.AmplitudeIn = FieldBus::GetExternalAmplitude();
			   RtSendData.RampTime = RunningPreset.DcpParam.WeldRampTime;
			   RtSendData.CmdFlags |= MB_USEREALTIMEDATA;
		   }
	   }
	   //All other stepping i.e. Time, Energy and PeakPower is done on DCP side.
	   else if((RtReceiveData.StatusFlags & SIGNAL_STEPPING_DONE) == SIGNAL_STEPPING_DONE){
		  AmpStepReachedFlag = TRUE;
		  UserIO::SetAmplitude1_2(AMP2);//TRUE for AMP2
		  RtSendData.RampTime = RunningPreset.DcpParam.WeldRampTime;
	   }
   }
}



/* void CheckForNoInput(void)
 *
 *  Purpose:
 *    This function is called from PreReadyPass function of the state machine.
 *    Function will check any input is active from user I/O or from FB.
 *    If manufacturing is enabled than all other inputs will be ignored.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    Returns True if there is no external input
 */
BOOLEAN CheckForNoInput(void)
{
   BOOLEAN NoInput = FALSE;
   //The case where manufacturing cycle will start after powerup. We will ignore the external inputs
   //whether they are on or off
   if(WebIOEnabled)
	  NoInput = TRUE;
   else if(!(ControlVariable
	  || UserIO::CheckForMemoryClear()
   	  || FieldBus::CheckForMemoryClear()
	  || CheckForLoadNewPreset()
	  || UserIO::CheckForCycleAbort()
	  || UserIO::CheckForGroundDetect()
	  || FBCmd.GndDetect
	  ||((ActuatorPresentAtPwrUp == TRUE) && (UserIO::CheckForULS() == FALSE)))){
	  NoInput = TRUE;
   }
   return NoInput;
}

/* BOOLEAN CheckMemClearReset(void)
 *
 *  Purpose:
 *    This function is called from ErrorPass function of the state machine.
 *    Function will return TRUE if Reset and Memory Clear both signals are active simultaneously on IO
 *    also no other IO - signal is active when system is in error state.
 *
 *  Entry condition:
 *    System in error state.
 *
 *  Exit condition:
 *    Function will return TRUE if Reset and Memory Clear both signals are active simultaneously on IO
 *    also no other IO - signal is active when system is in error state
 */
BOOLEAN CheckMemClearReset(void){
   BOOLEAN Result = FALSE;
   if(((ControlVariable & IORESET) == IORESET) && (UserIO::CheckForMemoryClear() == TRUE) && (WebIOEnabled == FALSE)){
   if(!((ControlVariable & ~IORESET)
	  || CheckForLoadNewPreset()
	  || UserIO::CheckForCycleAbort()
	  || UserIO::CheckForGroundDetect()
      || FBCmd.GndDetect
	  || ((ActuatorPresentAtPwrUp == TRUE) && (UserIO::CheckForULS() == FALSE))))
		 Result = TRUE;
   }
   return Result;
}




/* void CheckforRFSwitching(void)
 *
 *  Purpose:
 *    This function is called from ReadyPass function of the state machine.
 *    Function will check if RF Switching is required or not based on the RF Switch
 *    Setting in Sysconfig and the input received from IO or FB.
 *    If RFSwitching is required then set a flag "RFSwitchingRequired" and initialize
 *    RF Timeout variables, Relay number and Switching status. Also initialize the
 *    SelectRFRelayFun function pointer to call SelectRFRelayIO or SelectRFRelayFB functions.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None
 */
void CheckforRFSwitching(void)
{
   RFSwitchingRequired = FALSE;
   //Check if Switching is enabled or not.
   if(Sysconfig->RFSwitch.RFSwitchingType != NORFSWITCHING){
      //If RF Switching is ON then Power-up Options will be OFF and user will not be allowed to make any changes in power-up options.
      Sysconfig->PowerupOption = POWERUP_NONE;
      //Check if Switching is enabled for IO only.
      if(Sysconfig->RFSwitch.RFSwitchingType == RFSWITCHING_DIO){
         //If RF Switching is ON then External preset recall will be set and user will not be allowed to change it.
         Sysconfig->ExternalPresetFlag = TRUE;
         //RF Switching will be required in case of Load new preset, Ext Weld Start, Ext Cycle Start
         // Ext Seek,Ext Scan or Ext Test from User I/O.
         if((LoadPresetToDCP == TRUE)||(DCXFunc.ScanStart == TRUE) || (DCXFunc.SeekStart == TRUE)
        		 ||(DCXFunc.TestStart == TRUE)||(DCXFunc.CycleStart == TRUE) || (DCXFunc.WeldStart == TRUE)){
            RFSwitchingRequired = TRUE;
            SelectRFRelayFun = SelectRFRelayIO;
         }
      }
      //Check if Switching is enabled for FB only.
      else if(Sysconfig->RFSwitch.RFSwitchingType == RFSWITCHING_FB){
         //Switching will be required in case of Weld/Horn function or in case of load new preset from FB.
          if((LoadPresetToDCP == TRUE)||(DCXFunc.ScanStart == TRUE) || (DCXFunc.SeekStart == TRUE)
          		 ||(DCXFunc.TestStart == TRUE)||(DCXFunc.CycleStart == TRUE) || (DCXFunc.WeldStart == TRUE)){
            RFSwitchingRequired = TRUE;
            SelectRFRelayFun = SelectRFRelayFB;
         }
      }
      if(RFSwitchingRequired == TRUE){
          //Set "LoadPresetToDCP" to jump to update preset state of state machine to do RF Switching.
         LoadPresetToDCP = TRUE;
      }
   }
}

/* RFSwitchingStatus SelectRFRelayIO(UINT16 StackNumber)
 *
 *  Purpose:
 *    This function is called from HandleRFSwitching function if RF Switching is required from IO.
 *    Function will check for valid StackNumber first and then set the requested stackNumber to
 *    User output pins.
 *    If Feedback is not required then wait for RealySwitchTime before declaring the SwitchingDone.
 *    If Feedback is required then check for coded/uncoded feedback based on sysconfig setting of RF Switching
 *    within Feedback timeout before declaring alarm or success.
 *
 *  Entry condition:
 *    UINT16 StackNumber: StackNumber to be switched ON.
 *
 *  Exit condition:
 *    SWITCHINGABORTED_FAILED: if Alarm condition.
 *    SWITCHINGDONE: if Switching is completed for required stack.
 *    SWITCHINGINPROGRESS: if switching is under progress.
 */
RFSwitchingStatus SelectRFRelayIO(UINT16 StackNumber)
{
   UINT16 MaxHornPresets = HORNPRESETNUMBER_MAX;
   SINT32 ExpectedUncodedFeedBackVal = 0;
   RFSwitchingStatus ReturnStatus = SWITCHINGINPROGRESS;
   if((Sysconfig->RFSwitch.RFOutputType == UNCODED) || (Sysconfig->RFSwitch.RFFeedback == UNCODEDFEEDBACK))
      MaxHornPresets = HORNPRESETNUMBER_MAX_UNCODED;

   if(StackNumber > MaxHornPresets){
      AlarmQueueWrite(NC07);
      ReturnStatus = SWITCHINGABORTED_FAILED;
   }
   else{
      UserIO::SetRfselect(StackNumber);
      if(Sysconfig->RFSwitch.RFFeedback != NOFEEDBACK){
         if(CONVMICRO_MILLI(RFFeedBackTimer) <= Sysconfig->RFSwitch.FeedBackTimeOut){
            if(Sysconfig->RFSwitch.RFFeedback == CODEDFEEDBACK){
               if(StackNumber == UserIO::CheckForRfswitchfeedback())
                  ReturnStatus = SWITCHINGDONE;
            }
            else if(Sysconfig->RFSwitch.RFFeedback == UNCODEDFEEDBACK){
            	ExpectedUncodedFeedBackVal = BIT((StackNumber - 1));
            	if(ExpectedUncodedFeedBackVal == (UserIO::CheckForRfswitchfeedback()))
            		ReturnStatus = SWITCHINGDONE;
            }
            else if(Sysconfig->RFSwitch.RFFeedback == STATUSFEEDBACK){
              if(UserIO::CheckForRfStatusfeedback())
            	  ReturnStatus = SWITCHINGDONE;
            }
         }
         else{
            AlarmQueueWrite(NC05);
            ReturnStatus = SWITCHINGABORTED_FAILED;
         }
         RFFeedBackTimer += WCTIME;
      }
      else {
         if(CONVMICRO_MILLI(RFSwitchTimeOut) > Sysconfig->RFSwitch.RelaySwitchTime)
            ReturnStatus = SWITCHINGDONE;
         RFSwitchTimeOut += WCTIME;
      }
   }
   return ReturnStatus;
}

/* RFSwitchingStatus SelectRFRelayFB(UINT16 StackNumber)
 *
 *  Purpose:
 *    This function is called from HandleRFSwitching function if RF Switching is required from FB.
 *    Function will check for valid StackNumber first and then set the requested stackNumber to
 *    FB HFS0-3 bits.
 *    If Feedback is not required then wait for RealySwitchTime before declaring the SwitchingDone.
 *    If Feedback is required then check for coded/uncoded feedback based on sysconfig setting of RF Switching
 *    within Feedback timeout before declaring alarm or success.
 *
 *  Entry condition:
 *    UINT16 StackNumber: StackNumber to be switched ON.
 *
 *  Exit condition:
 *    SWITCHINGABORTED_FAILED: if Alarm condition.
 *    SWITCHINGDONE: if Switching is completed for required stack.
 *    SWITCHINGINPROGRESS: if switching is under progress.
 */
RFSwitchingStatus SelectRFRelayFB(UINT16 StackNumber)
{
   UINT16 MaxHornPresets = HORNPRESETNUMBER_MAX;
   SINT32 ExpectedUncodedFeedBackVal = 0;
   RFSwitchingStatus ReturnStatus = SWITCHINGINPROGRESS;
   if((Sysconfig->RFSwitch.RFOutputType == UNCODED) || (Sysconfig->RFSwitch.RFFeedback == UNCODEDFEEDBACK))
      MaxHornPresets = HORNPRESETNUMBER_MAX_UNCODED;
   if(StackNumber > MaxHornPresets){
      AlarmQueueWrite(NC07);
      ReturnStatus = SWITCHINGABORTED_FAILED;
   }
   else
   {
      FieldBus::SetRfSelect(StackNumber);
      if(Sysconfig->RFSwitch.RFFeedback != NOFEEDBACK)
      {
         if(CONVMICRO_MILLI(RFFeedBackTimer) <= Sysconfig->RFSwitch.FeedBackTimeOut)
         {
            if(Sysconfig->RFSwitch.RFFeedback == CODEDFEEDBACK) {
               if(StackNumber == FieldBus::CheckForRfswitchfeedback())
                  ReturnStatus = SWITCHINGDONE;
            }
            else if (Sysconfig->RFSwitch.RFFeedback == UNCODEDFEEDBACK)
            {
               ExpectedUncodedFeedBackVal = BIT((StackNumber - 1));
               if(ExpectedUncodedFeedBackVal == (FieldBus::CheckForRfswitchfeedback()))
            	   ReturnStatus = SWITCHINGDONE;
            }
         }
         else
            AlarmQueueWrite(NC05);
         RFFeedBackTimer += WCTIME;
      }
      else {
    	  if(CONVMICRO_MILLI(RFSwitchTimeOut) > Sysconfig->RFSwitch.RelaySwitchTime)
            ReturnStatus = SWITCHINGDONE;
    	  RFSwitchTimeOut += WCTIME;
      }
   }
   return ReturnStatus;
}

/* void HandleRFSwitching(void)
 *
 *  Purpose:
 *    This function is called from UpdatePresetPass function of state machine to handle RF Switching.
 *    Function will wait for RFDelay period if set and then Call SelectRFRelayFun function which was initialized in
 *    CheckforRFSwitching function.
 *    If Switch With Off is enabled then switch to stack zero first and then the preset Stack number otherwise
 *    switch to Preset Stack number directly.
 *
 *  Entry condition:
 *    RelayNumber: Global variable contains the relay number to switch to.
 *
 *  Exit condition:
 *     RelayNumber: Global variable contains stack zero horn number once the switching is done
 */
void HandleRFSwitching(void)
{
   //Check if RFDelay is required.
   if(RFDelayRequired == TRUE){
      //Checkif RFDelay period is already lapsed otherwise wait for RFSwitchDelay
      // SonicsOffPeriod is getting incremented from StateMachine driver.
      if(SonicsOffPeriod > Sysconfig->RFSwitch.RFSwitchDelay)
         RFDelayRequired = FALSE;
   }
   else{
      //Call SelectRFRelayFB/SelectRFRelayIO function to complete the switching.
      if(SwitchStatus == SWITCHINGINPROGRESS)
         SwitchStatus = SelectRFRelayFun(RelayNumber);
      //If Switching with Off is set and stack zero is switched then switch new stack from Preset.
      if((SwitchStatus == SWITCHINGDONE)&&(Sysconfig->RFSwitch.RFOption == SWITCHWITHOFF)
            &&(RelayNumber != ParameterSet[CurrPreset].InfoParam.HornPresetNumber)){
            RelayNumber = ParameterSet[CurrPreset].InfoParam.HornPresetNumber;
            SwitchStatus = SelectRFRelayFun(RelayNumber);
      }
   }
}
