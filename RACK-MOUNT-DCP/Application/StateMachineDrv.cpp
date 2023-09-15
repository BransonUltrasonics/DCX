/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/StateMachineDrv.cpp_v   1.16   Oct 27 2016 11:01:34   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/StateMachineDrv.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added handling when PowerUp Scan fails
 *
 *    Rev 1.16   Oct 27 2016 11:01:34   EGUAJARDO
 * Send results to Manager Basic app in PreWaitEntry only if WeldComplete true on CheckWeldStop substate.
 * 
 *    Rev 1.15   15 Sep 2016 17:04:40   EGUAJARDO
 * Added function call to send the weld results to the Manager Basic app.
 * 
 *    Rev 1.14   03 Sep 2015 03:18:14   sesharma
 * Updated ScanCompleteExit() for Memory Clear.
 * 
 *    Rev 1.13   01 Sep 2015 02:37:36   amaurya
 * Added check for Set with Horn Scan in ScanCompleteExit().
 * 
 *    Rev 1.12   25 Jun 2015 07:07:02   amaurya
 * Added ClearWindowLimitAlarm() and WaitExit().
 * 
 *    Rev 1.11   04 Jun 2015 07:51:02   sesharma
 * Changes done to handle AC Power Lost alarm.
 * 
 *    Rev 1.10   13 May 2015 08:44:56   sesharma
 * A next start signal runs the new cycle regardless of the setting of Alarm-Reset Required option, when window limits alarm occurs and there is no overload.
 * 
 *    Rev 1.9   09 Apr 2015 05:28:26   sesharma
 * Changes done for Cycle Abort with Reset. In case SIG Special is Enabled and a window limit alarm occurs.The alarm gets cleared and the weld start immediately, if the start input is provided within 500ms.
 * 
 *    Rev 1.8   19 Mar 2015 07:07:10   sesharma
 * pprintf disabled and Branson Coding Standard Applied.
 * 
 *    Rev 1.7   09 Mar 2015 09:38:48   sesharma
 * Window limits bars should stop blinking within 500ms if SIG Special is enabled.
 * 
 *    Rev 1.6   26 Feb 2015 08:30:38   sesharma
 * Changes done to log Overload and Window Limit Alarms both if they occurr at the same time.
 * 
 *    Rev 1.5   20 Feb 2015 17:03:06   ewomack
 * Check for power window limit on weld error.
 * 
 *    Rev 1.4   13 Jan 2015 06:02:20   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.3   07 Nov 2014 16:18:58   ewomack
 * Added Weld Modes.  Updates for SIG and power window limits.
 * 
 *    Rev 1.2   17 Oct 2014 09:42:40   sesharma
 * Bug fixed DEF00770 and modifications in state Machine.
 * 
 *    Rev 1.1   17 Sep 2014 09:01:24   amaurya
 * added void in the function arguments required
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#include "StateMachineDrv.h"
#include "stdio.h"
#include "FUNC.h"
#include "Globals.h"
#include "UserIO.h"
#include "LOG.h"
#include "ReceiveCommandHandler.h"
#include "DebugNew.h"
#include "ShutDownTask.h"
#include "BroadcastWR.h"

/* Current sequence type */
enum WELD_SEQUENCE_TYPE CurrentSequence = NONEDEFINED;
enum WELDSTATES_IDX WeldState; /* Current weld state */

/*-------------------------- Global Variables -----------------------------*/
BOOLEAN SystemOK = TRUE;
BOOLEAN TimerFlag = FALSE;
SINT16 CurrentIndex; /* Index to current state pointer*/
BOOLEAN AbortFlag; /*When set to TRUE, will abort the current sequence. */
BOOLEAN FailFlag; /*Only set by state machine, cleared with every call to state machine*/
BOOLEAN StartActiveHigh;
SINT16 LastFailTrace;
SINT16 FailTrace;
STATE *WeldSequence[MAX_STATE + 1]; /* Current weld cycle definition */
BOOLEAN Energy_TimeOut_Flag;
UINT16 SigSplTick;
BOOLEAN WarningAlarm = FALSE;
/*--------------------------------------------- ---------------------------*/

#define printf
#define pprintf
#define WELDSTOPCOMPLETETRACE		166
/*-------------------------- PRIVATE GLOBAL DATA --------------------------*/
static UINT16 StateMachineFlag = STOPPED; /* Current StateMachineFlag      */
static STATE *CurrentPtr; /* Current state pointer         */
static SINT16 PushedIndex;
static SINT16 LastTrace;
static SSTATES *SSptr; /* Current sub state pointer     */
BOOLEAN WindowLimitAlarm = FALSE;
static BOOLEAN ResetReq = FALSE;
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 NumScanPoints;
extern UINT16 LCDKeyStatus;
extern BOOLEAN AlarmFlag;

/*--------------------------------------------- ---------------------------*/

/*-------------------------- Function Declarations ---------------------------*/
void WaitExit(void);
/*--------------------------------------------- ---------------------------*/

void PushIndex(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets PushedIndex to CurrentIndex so that CurrentIndex      */
/* could be used later.                                                     */
/****************************************************************************/
{
   PushedIndex = CurrentIndex;
}

void PopIndex(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets CurrentIndex to PushedIndex so that previos state     */
/* could be realized.                                                       */
/****************************************************************************/
{
   CurrentIndex = PushedIndex;
}

void Restart(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets CurrentIndex to 0 so that state sequence is initialize*/
/* again.                                                                   */
/****************************************************************************/
{
   CurrentIndex = 0;
}

void StartStateMachine(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets StateMachineFlag status to RUNNING.                   */
/*                                                                          */
/****************************************************************************/
{
   StateMachineFlag = RUNNING;
}

void StopStateMachine(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets StateMachineFlag status to STOPPED.                   */
/*                                                                          */
/****************************************************************************/
{
   StateMachineFlag = STOPPED;
}

UINT16 GetStateMachineFlag(void)
/*****************************************************************************/
/* Description:-                                                             */
/* This function returns StateMachineFlag status i.e. wheteher it is RUNNING */
/* or STOPPED.                                                               */
/* Returns StateMachineFlag to indicate status.										  */
/*****************************************************************************/
{
   return (StateMachineFlag);
}

void ResetStateMachine(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function Resets the State Machine to intial state and State Machine */
/* is stopped.                                                              */
/****************************************************************************/
{
   CurrentPtr = 0;
   CurrentIndex = 0;
   StateMachineFlag = STOPPED;
}

UINT16 SetStateMachineFlag(UINT16 Flag)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets StateMachineFlag to value passed in it.               */
/* Input Parameters:-                                                       */
/* Flag used to set state machine flag.                                     */
/****************************************************************************/
{
   UINT16 RetVal;

   RetVal = StateMachineFlag; /* Get old value before we change it */
   StateMachineFlag = Flag; /* Set to new passed value           */
   return (RetVal); /* Return original value             */
}

void SetCurrentSequenceType(enum WELD_SEQUENCE_TYPE Type)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets CurrentSequence type in the DCP mode.                 */
/* Input Parameters:-                                                       */
/* Type indicates type of weld sequence. 												 */
/****************************************************************************/
{
   CurrentSequence = Type;
}

enum WELD_SEQUENCE_TYPE GetCurrentSequenceType(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function returns CurrentSequence type in the DCP mode.              */
/* Returns CurrentSequence type selected.                                   */
/****************************************************************************/
{
   return (CurrentSequence);
}

void Nothing(void)
/***************************************************************************/
/*  Description:-                                                          */
/*  This is the function that will be the most used of all functions.  It  */
/*  will be the MANAGER function (i.e. it will do nothing).                */
/*                                                                         */
/*  This would be a good place to put debugging stuff, because it will get */
/*  called at somewhat regular intervals.                                  */
/*                                                                         */
/***************************************************************************/
{
   //Do nothing,used as a place holder in structures.
}

void SelectDCPSequence(void)
/****************************************************************************/
/* Description:-                                                            */
/* This is the DCP mode state sequence.                                     */
/*                                                                          */
/****************************************************************************/
{
   UINT16 i = 0;

   ResetStateMachine(); /* Turn off the state machine first    */
   CurrentIndex = 0;
   WeldSequence[i++] = &TopOfList;
   WeldSequence[i++] = &PowerUpState;
   WeldSequence[i++] = &WaitState;
   WeldSequence[i++] = &ScanStartState;
   WeldSequence[i++] = &ScanRunState;
   WeldSequence[i++] = &ScanStopState;

   WeldSequence[i++] = &ScanCalculatingState;
   WeldSequence[i++] = &ScanCompleteState;
   WeldSequence[i++] = &ScanAbortState;

   WeldSequence[i++] = &TestStartState;

   WeldSequence[i++] = &SeekStartState;
   WeldSequence[i++] = &SeekRampState;
   WeldSequence[i++] = &SeekRunState;
   WeldSequence[i++] = &SeekStopState;

   WeldSequence[i++] = &WaitNoInputState;

   WeldSequence[i++] = &SeekWeldState;

   WeldSequence[i++] = &WeldStartState;
   WeldSequence[i++] = &WeldRampState;
   WeldSequence[i++] = &WeldRunState;
   WeldSequence[i++] = &WeldStopState;
   WeldSequence[i++] = &WeldErrorState;

   WeldSequence[i++] = &ResetErrorState;
   WeldSequence[i++] = &WaitStopState;

   WeldSequence[i++] = &ManFWeldStartState;
   WeldSequence[i++] = &ManFWeldRampState;
   WeldSequence[i++] = &ManFWeldRunSonicOnState;
   WeldSequence[i++] = &ManFWeldRunSonicOffState;
   WeldSequence[i++] = &ManFWeldStopState;
   WeldSequence[i++] = &ManFWeldErrorState;
   WeldSequence[i++] = &ManFWaitStopState;
   WeldSequence[i++] = &AutoResetState;
   WeldSequence[i++] = &PreReadyState;
   WeldSequence[i++] = &WeldRampEnergyState;
   WeldSequence[i++] = &WeldRunEnergyState;
   WeldSequence[i++] = &EndOfList;

   CurrentSequence = WELD;
   StartStateMachine(); /* Start the state machine here */
}

void StateMachine(void)
{
   BOOLEAN EndFlag = FALSE;
   BOOLEAN TestValue;
   SINT16 Cond;

   // Check state machine Flag
   if (StateMachineFlag == RUNNING) {
      CurrentPtr = WeldSequence[CurrentIndex];
      SSptr = CurrentPtr->ptr; /* Get sub state pointer      */
      CurrentPtr->PassPtr(); /* Execute this pass code     */
      FailFlag = FALSE;

      while (EndFlag == FALSE) {
         TestValue = *(SSptr->ValuePtr);
         if (TestValue == FALSE) { /* Test the value          */
            Cond = SSptr->False; /* Was FALSE, use FALSE cmd */
         }
         else {
            Cond = SSptr->True; /* Was TRUE, use TRUE cmd   */
         }
         switch (Cond) {

         case NORMAL:
            SSptr++; /* Move to Next Sub State   */
            break;

         case PUSH:
            SSptr++; /* Move to Next Sub State   */
            PushedIndex = CurrentIndex;
            break;

         case FAIL:
            if (SSptr->Alarm != NONE) {
               //RecordAlarm(SSptr->Alarm);     /* Record Alarm            */
            }
            SSptr++; /* Move to Next Sub State   */
            FailFlag = TRUE;
            LastFailTrace = FailTrace;
            FailTrace = SSptr->Trace;
            break;

         case ABORT:
            if (SSptr->Alarm != NONE) {
            }
            SSptr++; /* Move to Next Sub State   */
            LastTrace = SSptr->Trace;
            CurrentIndex++; /* Increment index   */
            CurrentPtr = WeldSequence[CurrentIndex];
            if (CurrentPtr->state != NOP_IDX) WeldState = CurrentPtr->state; /* Update state variable    */
            Restart();
            EndFlag = TRUE;
            break;

         case ALNORM:
            if (SSptr->Alarm != NONE) {
            }
            SSptr++; /* Move to Next Sub State   */
            break;

         case JUMP:
            if (SSptr->Alarm != NONE) {
            }
            CurrentPtr->AlarmPtr(); /* Execute exit code       */
            CurrentPtr->JumpPtr(); /* Execute exit code       */
            LastTrace = SSptr->Trace;
            CurrentIndex++; /* Increment index         */
            CurrentPtr = WeldSequence[CurrentIndex];
            if (CurrentPtr->state != NOP_IDX) WeldState = CurrentPtr->state; /* Update state variable   */
            if (AbortFlag == FALSE) CurrentPtr->EntryPtr(); /* Execute new entry code  */
            EndFlag = TRUE;
            break;
         case SKIP:
            if (SSptr->Alarm != NONE) {
               CurrentPtr->AlarmPtr(); /* Execute Alarm code     */
            }
            CurrentPtr->JumpPtr(); /* Execute exit code       */
            LastTrace = SSptr->Trace;
            CurrentIndex = SSptr->Skip; /* Skip to next state                      */

            CurrentPtr = WeldSequence[CurrentIndex];
            if (CurrentPtr->state != NOP_IDX) WeldState = CurrentPtr->state; /* Update state variable                   */
            if (AbortFlag == FALSE) CurrentPtr->EntryPtr(); /* Execute new entry code                  */

            EndFlag = TRUE;
            break;
         case POP:
            CurrentPtr->AlarmPtr(); /* Execute alarm code     */
            LastTrace = SSptr->Trace;
            CurrentIndex = PushedIndex; /* Reset index            */
            CurrentPtr = WeldSequence[CurrentIndex];
            if (CurrentPtr->state != NOP_IDX) WeldState = CurrentPtr->state; /* Update state variable  */
            if (AbortFlag == FALSE) CurrentPtr->EntryPtr(); /* Execute new entry code */
            EndFlag = TRUE;
            break;

         case ALJUMP:
            if (SSptr->Alarm != NONE) {
            }
            CurrentPtr->AlarmPtr(); /* Execute Alarm code      */
            LastTrace = SSptr->Trace;
            CurrentIndex++; /* Increment index         */
            CurrentPtr = WeldSequence[CurrentIndex];
            if (CurrentPtr->state != NOP_IDX) WeldState = CurrentPtr->state; /* Update state variable   */

            EndFlag = TRUE;
            break;

         case END:
            EndFlag = TRUE;
            break;

         default:
            EndFlag = TRUE;
            break;
         }
      }
   }
}

void PowerUpEntry(void)
/****************************************************************************/
/* Description:- 																				 */
/* This is Entry code for PowetUp State.								             */
/****************************************************************************/
{
   TotalPowerUpDelay = POWERUPDELAY;
}

void PowerUpPass(void)
/****************************************************************************/
/* Description:- 																				 */
/* This is Pass code for Power up State.It checks for power up seek and     */
/* power up horn scan option in Running preset during Power up. If no option*/
/* is enabled ,then it moves to WAITING state.                              */
/****************************************************************************/
{
   SonicsOff();

   /* powerup seek */
   if ((PowerUpSeekStarted == FALSE)
         && ((RunningPreset.SWDip.Flags.PowerUpOptions == POWERUPSEEK) /*||
          (RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)*/)) {
      NoPowerUpSeekScan = FALSE;
      if (PowerUpDelay <= TotalPowerUpDelay)   //PowerUp Delay
            {
         PowerUpDelay++;
      }
      else {
         PowerUpDelay = 0;
         PowerUpCompleted = TRUE;
         PowerUpSeek = TRUE;
         PowerUpSeekStarted = TRUE;
      }
   }
   /* powerup scan */
   else if ((PowerUpScanStarted == FALSE)
         && (RunningPreset.SWDip.Flags.PowerUpOptions == POWERUPSCAN)) {
      NoPowerUpSeekScan = FALSE;
      if (PowerUpDelay <= TotalPowerUpDelay)   //PowerUp Delay
            {
         PowerUpDelay++;
      }
      else {
         PowerUpDelay = 0;
         PowerUpCompleted = TRUE;
         Reset_Scanbuffer();
         PowerUpScan = TRUE;
         ScanEnable = TRUE;
         PowerUpScanStarted = TRUE;
      }
   }
   else   // No power up seek /scan is set
   {
      if (PowerUpDelay <= TotalPowerUpDelay) {
         PowerUpDelay++;
      }
      else {
         PowerUpDelay = 0;
         PowerUpCompleted = TRUE;
         NoPowerUpSeekScan = TRUE;
      }
   }
}

void PowerUpExit()
/****************************************************************************/
/* Description:- 															*/
/* This is Exit code for PowetUp State.							        */
/****************************************************************************/
{
//IsIOconfigured flag is cleared here to make sure that system does not go into preready state on 
//activating external start(When Alarm-reset required is disable) after an alarm on power up seek/scan.
   IsIOConfigured = FALSE;
}

void WaitEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Waiting State.								             */
/****************************************************************************/
{
   ScanEnable = FALSE;
   WebSeek = FALSE;
   //Indicate DCP is ready.
   UserIO::SetReady(TRUE);
   // Indicates that ultrasonics are off for seek,test,weld and scan mode.
   UserIO::SetSonicsActive(FALSE);
   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);
}

void WaitPass(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Waiting State.It updates Output Status to be send  */
/* to MI board and DDSFrequency information to be send to FPGA Interfaces.  */
/* It will also update running preset if preset variable is changed from    */
/* Website/LCD to FRAM.                                                     */
/****************************************************************************/
{
   WeldError = NO_ERROR;
   PhaseCount = 0;

   //System is ready to start a new weld/seek cycle,
   //enter test mode or start a horn scan.
   SonicsOff();
   PWMAmp = 0;

   if (UpdateRPreset == TRUE)/*Update Running preset */
   {
      UpdateRPreset = FALSE;
      UpdateRunningPreset();
   }
   if (ScanInitiate) /* Check for scan initiate*/
   {
      ScanInitiate = FALSE;
      ScanEnable = TRUE;
   }
   if (PowerUpScan) {
      if (WaitDelay <= TOTALWAITDELAY) {
         WaitDelay++;
      }
      else {
         WaitDelay = 0;
         PowerUpScan = FALSE;
         /*Check for Autosonics*/
         if (MFCycleFlag == TRUE) {
            StartMFCycle = TRUE;
            EnableWebIO = TRUE;
         }
      }
   }
   else if (PowerUpSeek) {
      if (WaitDelay <= TOTALWAITDELAY) {
         WaitDelay++;
      }
      else {
         WaitDelay = 0;
         PowerUpSeek = FALSE;
         /*Check for Autosonics*/
         if (MFCycleFlag == TRUE) {
            StartMFCycle = TRUE;
            EnableWebIO = TRUE;
         }
      }
   }
   else if (NoPowerUpSeekScan) {
      if (WaitDelay <= TOTALWAITDELAY) {
         WaitDelay++;
      }
      else {
         WaitDelay = 0;
         NoPowerUpSeekScan = FALSE;
         /*Check for Autosonics*/
         if (MFCycleFlag == TRUE) {
            StartMFCycle = TRUE;
            EnableWebIO = TRUE;
         }
      }
   }
   SetFPGAFrequency();
}

void ClearWindowLimitAlarm(void)
/****************************************************************************/
/* Description:-															*/
/* This function clears window limit alarms related flags and outputs. 		*/
/****************************************************************************/
{
	WindowLimitAlarm = FALSE;
	UserIO::SetPowerWindowLimitHigh(FALSE);
	UserIO::SetPowerWindowLimitLow(FALSE);
}

void WaitExit(void)
/****************************************************************************/
/* Description:-															 */
/* This is Exit code for Waiting State. It is required to clear window limit */
/* alarm, if exist before next operation i.e. seek/scan/weld/test from any    */
/* interface as system goes to ready state with these alarms present.        */
/****************************************************************************/
{
	 if(WarningAlarm){
		 WarningAlarm = FALSE;
		 AlarmFlag = FALSE;
		 ErrorReason = 0;
		 ClearWindowLimitAlarm();
	 }
}

void ScanStartEntry(void)
/*****************************************************************************/
/* Description:-																				  */
/* This is Entry code for Scan Start State.It initializes ScanStarted Status */
/* Flag to FALSE and updated LOG entry.                                      */
/*****************************************************************************/
{
   ScanStarted = FALSE;
   // Indicate DCP is busy.
   UserIO::SetReady(FALSE);
   MemoryReset = FALSE;
   UserIO::SetSeekOut(TRUE);
   UserIO::SetSonicsActive(TRUE);
   InitializeScanbuffer();
   Log::WriteEventLog(EVENT_HORNSCAN_START, 0, 0);
}

void ScanStartExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Scan Start State.                                  */
/*                                                                          */
/****************************************************************************/
{

}

void ScanRunEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Run State.It initializes ScanRun Flag to     */
/* FALSE state .                                                            */
/****************************************************************************/
{
   ScanRun = FALSE;
   ScanAbort = FALSE;
}

void ScanRunAlarmExit(void)
{
   UINT16 data;
   //FPGA Amplitude is ramp Down
   PWMAmp = 0;
   data = HornScanPointLast;

   //End Mark of scan buffer
   ScanBuffer[HornScanGraphPointCount].Status = data;
   ScanBuffer[HornScanGraphPointCount + 1].Status = data;
}

void ScanStopEntry(void)
/*****************************************************************************/
/* Description:-																				  */
/* This is Entry code for Scan Stop State.It initializes ScanStop flag to    */
/* FALSE and updates number of scan points in FRAM.                          */
/*****************************************************************************/
{
   NumScanPoints = HornScanGraphPointCount;
   ScanStop = FALSE;
}

void ScanCalculateEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Calculate State.It initializes ScanCalculate */
/* and ScanAbort flags to FALSE.                                            */
/****************************************************************************/
{
   ScanCalculate = FALSE;
   ScanAbort = FALSE;
}

void ScanCompleteEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Complete State.It initializes ScanComplete   */
/* flag to FALSE.                                                           */
/****************************************************************************/
{
   ScanComplete = FALSE;
}

void ScanAbortEntry(void)
/****************************************************************************/
/*Description:- 																				 */
/* This is Entry code for Scan Abort State. It initializes to ScanAbort flag*/
/* to FALSE, Sets Memory Offset to zero if Horn Scan is failed.                                                                */
/****************************************************************************/
{
   ScanAbortComplete = FALSE;
   //Write Event Log
   if (WebSite::CheckForWebScanAbortCommand()) {
      Log::WriteEventLog(EVENT_HORNSCAN_ABORTED, 0, 0);
   }
   else {
      Log::WriteEventLog(EVENT_HORNSCAN_FAILED, 0, 0);
      MemoryValue = 0;
      CurrentPreset.MemoryFreq = MemoryValue;
      MemoryReset = TRUE;
      SavePreset(0);

      if ( (PowerUpScan == TRUE) &&
      		(RACKMOUNT_STD == BIT(EPromData.Systemtype)) &&
      		(EPromData.SIGSpecial)
      	 ) {
			ErrorReason |= HORN_SCAN_FAILED;
			AlarmFound = TRUE;
			AlarmFlag = TRUE;
			UserIO::SetOverloadAlarm(TRUE);   // Overload Alarm Output
			UserIO::SetGenAlarm(TRUE);   // General Alarm Output
      }
   }
   WebSite::ResetScanFlags();
}

void ScanStopExit(void)
/****************************************************************************/
/*Description:- 																				 */
/* This is Exit code for Scan Stop State.                                   */
/*                                                                          */
/****************************************************************************/
{

}

void ScanCalculateExit(void)
/****************************************************************************/
/* Description:- 																				 */
/* This is Exit code for Scan Calculate State.                              */
/*                                                                          */
/****************************************************************************/
{

}

void ScanCompleteExit(void)
/****************************************************************************/
/* Description:-															*/
/* This is Exit code for Scan Complete State. It turns the sonics to off    */
/* write event log, updates Digital Tune frequency and Memory frequency 	*/
/* depending on "Set with horn scan" is enable/disabled.                    */
/****************************************************************************/
{
   ScanInitiate = FALSE;
   ScanStarted = FALSE;
   ScanRun = FALSE;
   SonicsOff();
   SetFPGAFrequency();
   Log::WriteEventLog(EVENT_HORNSCAN_COMPLETED, MainParallelResonanceFreq, 0);
   if (ScanComplete == TRUE) {
   	if(CurrentPreset.SWDip.Flags.SetDigTuneWithScan == TRUE){
         CurrentPreset.DigitalTune = ((MainParallelResonanceFreq * 1000) / F_Factor);
         MemoryValue = 0;		// Clear stored memory frequency
   	}
   	else
   		//Set memory frequency as difference of Main parallel Resonance frequency and Digital tune frequency.
   		MemoryValue = ((MainParallelResonanceFreq * 1000) / F_Factor) - CurrentPreset.DigitalTune;

      CurrentPreset.MemoryFreq = MemoryValue;
      RunningPreset.MemoryFreq = MemoryValue;
      SavePreset(0);
   }
   WebSite::ResetScanFlags();
}

void ScanAbortExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Scan Abort State. It turns off the sonics.         */
/*                                                                          */
/****************************************************************************/
{
   ScanInitiate = FALSE;
   ScanStarted = FALSE;
   ScanRun = FALSE;
   ScanComplete = FALSE;
   ScanAbort = TRUE;
   SonicsOff();
   SetFPGAFrequency();
}

void TestStartEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Test Start State.                                 */
/*                                                                          */
/****************************************************************************/
{
   TestEnable = FALSE;
}

void SeekStartEntry(void)
/*****************************************************************************/
/* Description:-																				  */
/* This is Entry code for Seek start State.It initializes SeekStartFlag flag */
/* to FALSE.                                                                 */
/*****************************************************************************/
{
   SeekStartFlag = FALSE;
   //Indicate DCP is not ready.
   UserIO::SetReady(FALSE);
   MemoryReset = FALSE;
   if (RunningPreset.SWDip.Flags.ClrMemBfrSeek == TRUE) MemoryValue = 0;
   //Indicates in seek mode
}

void SeekStartExit(void)
/*****************************************************************************/
/* Description:-																				  */
/*This is Exit code for Seek start State.				                          */
/*                                                                           */
/*****************************************************************************/
{

}

void SeekRampEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Ramp State.It initializes SeekRampFlag and   */
/* AlarmFound flag to FALSE.                                                */
/****************************************************************************/
{
   SeekRampFlag = FALSE;
   AlarmFound = FALSE;
   UserIO::SetSeekOut(TRUE);
   UserIO::SetSonicsActive(TRUE);
}

void SeekRunEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is entry code for Seek Run State.It initializes SeekRunFlag and     */
/* AlarmFound to FALSE.                                                     */
/****************************************************************************/
{
   SeekRunFlag = FALSE;
   AlarmFound = FALSE;
}

void SeekStopEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Stop State.It initializes SeekStopFlag to    */
/* FALSE.                                                                   */
/****************************************************************************/
{
   SeekStopFlag = FALSE;
   if (!TestInputFlag) {
      //Indicates not in seek mode
      UserIO::SetSeekOut(FALSE);
      UserIO::SetSonicsActive(FALSE);
      SonicsOff();
   }
}

void SeekStopExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Seek Stop State.It updates DDSFrquency information */
/* to be send to FPGA.                                                      */
/****************************************************************************/
{
   if (OverLoadAlarm || AlarmFound) {
      MemoryValue = 0;
      CurrentPreset.MemoryFreq = MemoryValue;
      RunningPreset.MemoryFreq = MemoryValue;
      SavePreset(0);
   }
   SetFPGAFrequency();
   ScanInitiate = FALSE;
   SeekEnable = FALSE;
   TimedSeekEnabled = FALSE;
   WebSeek = FALSE;
}

void WaitNoInputEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Wait no Input State.It updates NoInput flag to    */
/* FALSE value .                                                            */
/****************************************************************************/
{
   NoInput = FALSE;
   TestInputFlag = FALSE;
}

void WaitNoInputPass(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Wait no Input State.It updates Output Status to be */
/* send to MI Board and DDSFrquency information to be send to FPGA.         */
/****************************************************************************/
{
   //The case where manufacturing cycle will start after powerup. We will ignore the external inputs
   //whether they are on or off
   if (EnableWebIO)
      NoInput = TRUE;
   else if (!(UserIO::CheckForExternalReset() || WebSite::CheckForWebReset()
         || UserIO::CheckForExternalStart() || UserIO::CheckForExternalTest()
         || (LCDKeyStatus & KB_TEST) || WebSite::CheckForWebTestCommand())
         && !(UserIO::CheckForExternalSeek()) && !(UserIO::CheckForHornScan())
         && !(UserIO::CheckForMemoryClear())) {
      NoInput = TRUE;
   }

   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);

   // Indicates that ultrasonics are off for seek mode.
   UserIO::SetSonicsActive(FALSE);

   //Turn the sonics off
   SonicsOff();

   SetFPGAFrequency();
}

void SeekWeldEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Weld State.It initializes SeekWeldFlag flag  */
/* to  FALSE.                                                               */
/****************************************************************************/
{
   SeekWeldFlag = FALSE;
}

void WeldStartEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Start State.It initializes WeldStartFlag flag*/
/* to  FALSE.                                                               */
/****************************************************************************/
{
   WeldStartFlag = FALSE;
   WeldStartEnerFlag = FALSE;
   // Indicate DCP is busy.
   UserIO::SetReady(FALSE);
   MemoryReset = FALSE;
   RampRequired = FALSE;
   // Indicates that ultrasonics are on for weld mode.
   UserIO::SetSonicsActive(TRUE);
}

void ManFWeldStartEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Manufacturing Weld Start State.It initializes     */
/* WeldStartFlag flag to FALSE and cycle status to Cycling.                 */
/****************************************************************************/
{
   WeldStartFlag = FALSE;
   MFTimeoutEnable = FALSE;
   // Indicate DCP is busy.
   UserIO::SetReady(FALSE);
   MemoryReset = FALSE;
   // Indicates that ultrasonics are on for weld mode.
   UserIO::SetSonicsActive(TRUE);
   MFcycleResults.cycleStatus = statCycling;
}

void WeldStartExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Weld Start State.        									 */
/*                              															 */
/****************************************************************************/
{

}

void WeldRampEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Ramp State.It initializes WeldRampFlag flag  */
/* and AlarmFound flag to FALSE.                                            */
/****************************************************************************/
{
   WeldRampFlag = FALSE;
   AlarmFound = FALSE;
   NoInput = FALSE;
   WeldRunComplete = FALSE;
}

void WeldRunEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Run State.It initializes WeldRunComplete flag*/
/* and AlarmFound Flag to FALSE.                                            */
/****************************************************************************/
{
   WeldRunComplete = FALSE;
   AlarmFound = FALSE;
   RampRequired = FALSE;
   Energy_TimeOut_Flag = FALSE;
}

void ManFWeldRunSonicOnEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Manufacturing WeldRunSonicOn State.It initializes */
/* WeldRunComplete flag ,AlarmFound Flag to FALSE and SonicsOnOffTimer to   */
/* Zero.																							*/
/****************************************************************************/
{
   WeldRunComplete = FALSE;
   AlarmFound = FALSE;
}

void ManFWeldRunSonicOffEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Manufacturing WeldRunSonicOff State.It initializes*/
/* WeldRunComplete flag ,AlarmFound Flag to FALSE and SonicsOnOffTimer to   */
/* Zero.																							 */
/****************************************************************************/
{
   WeldRunComplete = FALSE;
}

void WeldStopEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop State.It initializes WeldComplete flag  */
/* to FALSE value.                                                          */
/****************************************************************************/
{
   WeldComplete = FALSE;
   UserIO::SetSonicsActive(FALSE);
   SonicsOff();
   AlarmFlag = FALSE;
}

void WeldStopExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is normal Exit code for Weld Stop State.It updates DDS frequency    */
/* information to be send to FPGA.                                          */
/****************************************************************************/
{
   SetFPGAFrequency();
   ScanInitiate = FALSE;
   if(TestInputFlag && !(OverLoadAlarm || AlarmFound))
	   TestInputFlag = FALSE;
   StartInputEnable = FALSE;
}

void ResetEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Reset State.It initializes ResetComplete and      */
/* PowerUpRequired flag to FALSE .                                          */
/****************************************************************************/
{
   FPGAOverloadReset();
   ResetComplete = FALSE;
   PowerUpRequired = FALSE;
}

void ResetExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is exit code for Reset State.                                       */
/****************************************************************************/
{
   ResetEnable = FALSE;
}

void ResetPass(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Reset State.It updates DDSFrquency                 */
/* information to be send to FPGA. It also initializes ScanInitiate flag    */
/* to zero. 						                                              */
/****************************************************************************/
{
   FPGAOverloadReset();

   if ((fpga_input & OL_MASK) == FALSE) {
      WeldError = NO_ERROR;

      //Indicates that general alarm is cleared
      UserIO::SetGenAlarm(FALSE);
      //Indicated power supply have been reseted.
      UserIO::SetOverloadAlarm(FALSE);

      SetFPGAFrequency();
      ScanInitiate = FALSE;
      SeekEnable = FALSE;
      TimedSeekEnabled = FALSE;
      TestEnable = FALSE;
      StartInputEnable = FALSE;
      OverLoadAlarm = FALSE;
      // Check if power up state is completed at start up
      if (PowerUpCompleted == FALSE) {
         // if not reset state machine index to zero.
         Restart();
         PowerUpRequired = TRUE;
      }
      else if (!StartActiveHigh || !CheckForInputs()) {
         ResetComplete = TRUE;
         PowerUpSeek = FALSE;
         PowerUpScan = FALSE;
         NoPowerUpSeekScan = FALSE;
         if (MFCycleFlag) {
            StartMFCycle = TRUE;
         }
      }
   }
}

void WeldErrorEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Error state. it initializes WeldErrorFinish  */
/* Flag to Zero.                                                            */
/****************************************************************************/
{
   WeldErrorFinish = FALSE;
   SeekEnable = FALSE;
   TimedSeekEnabled = FALSE;
   StartInputEnable = FALSE;
   // Indicate DCP is busy.
   UserIO::SetReady(FALSE);
   // Indicates that ultrasonics are off for weld or test mode.
   UserIO::SetSonicsActive(FALSE);
   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);
   if (UserIO::CheckForExternalStart()) {
      StartActiveHigh = TRUE;
   }
   else
      StartActiveHigh = FALSE;
   WriteAlarmLog();
   if (MFCycleFlag) {
      StartMFCycle = FALSE;
      SonicsOnCounter = 0;
      SonicsOffCounter = 0;
//		pprintf("\n cycle stopped 1");
      if (MFcycleResults.cycleStatus == statCompleted) {
      }
      else
         MFcycleResults.cycleStatus = statStopped;
   }
   SigSplTick = 0;
   if (WRSendFlag==TRUE)
         	BroadcastWR::SendWR();    // This to send Weld Result via ethernet
}

void ManFWeldErrorEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Error state. it initializes WeldErrorFinish  */
/* Flag to Zero.                                                            */
/****************************************************************************/
{
   WeldErrorFinish = FALSE;
   SeekEnable = FALSE;
   TimedSeekEnabled = FALSE;
   StartInputEnable = FALSE;
   WriteAlarmLog();
   // Indicate DCP is busy.
   UserIO::SetReady(FALSE);
   // Indicates that ultrasonics are off for weld or test mode.
   UserIO::SetSonicsActive(FALSE);
   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);
   StartMFCycle = FALSE;
   SonicsOnCounter = 0;
   SonicsOffCounter = 0;
   if (MFcycleResults.cycleStatus == statCompleted) {
   }
   else
      MFcycleResults.cycleStatus = statStopped;
}

void WeldErrorExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is normal Exit code for Weld Error state.                           */
/*                                                                          */
/****************************************************************************/
{

}

void WaitStopEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
   AutoResetEnable = FALSE;
}

void ManFWaitStopEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
   AutoResetEnable = FALSE;
}

void AutoResetEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
	//Warning alarm indicates only window limit alarm(s) is present and system should be in ready state with alarm.
	if((ErrorReason & ~WINDOW_LIMITS_MASK) == 0)
		WarningAlarm = TRUE;
	else
		WarningAlarm = FALSE;
}

void PreWaitEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Pre Wait state.                                   */
/*                                                                          */
/****************************************************************************/
{
   IsInputNotPresent = FALSE;
   IsIOConfigured = FALSE;

   //Indicate DCP is not ready.
   UserIO::SetReady(FALSE);

   // Indicates that ultrasonics are off for seek,test,weld and scan mode.
   UserIO::SetSonicsActive(FALSE);

   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);
   if (WRSendFlag==TRUE && LastTrace == WELDSTOPCOMPLETETRACE ) //if last state flag was WeldComplete
      	BroadcastWR::SendWR();    // This to send Weld Result via ethernet
}

void Test_start(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Test Start State. It initializes TestInputFlag flag*/
/* to TRUE.                                                                 */
/****************************************************************************/
{
   BOOLEAN PresetVerifiedSuccess = TRUE;
   if (PresetVerifiedRequired) {
      PresetVerifiedRequired = FALSE;
      PresetVerifiedSuccess = VerifyPreset();
   }
   if (!PresetVerifiedSuccess) {
      CheckAndAssignSeekCycleBoundaryValue();
      CheckAndAssignWeldCycleBoundaryValue();
   }
   TestInputFlag = TRUE;
   Log::WriteEventLog(TEST_ACTIVATED, 0, 0);
}

void TopOfListEntry(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for NOP State.								                */
/****************************************************************************/
{

}

void TopOfListExit(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for NOP State.			    					                */
/****************************************************************************/
{

}

void EndOfListEntrycode(void)
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for NOP State.			    					                */
/****************************************************************************/
{
//	pprintf("\n EndOfList Entry Test");
}

void AutoResetAlarm(void)
/****************************************************************************/
/* Description:-																				 */
/* This is pass code for Auto Reset state.                                  */
/*                                                                          */
/****************************************************************************/
{
   // A new start signal should run new cycle when window limits occur regardless of Alarm-Reset required or not(provided there is no overload alarm)
   if ((UserIO::GetGenAlarmFlag() == FALSE)
         && ((ErrorReason & WINDOW_LIMITS_MASK) != 0))
      ResetReq = FALSE;
   else
      ResetReq = RunningPreset.AlarmResetReq;

   if (((ResetReq == FALSE) && !EnableWebIO && UserIO::CheckForExternalStart()
         && !WeldStoppedbyReset)
   // If only window limit alarms occurs, system should be in ready state with alarm to accept any input i.e. seek/scan/weld/test from all the available interfaces.
		 || (WarningAlarm && !WeldStoppedbyReset)
   //If power up is not completed and AC is restored we need to leave this state without waiting for any input.
         || ((PowerUpCompleted == FALSE) && CHECK_FOR_AC)) {
      UserIO::SetGenAlarm(FALSE);
      WebSite::ResetWebResetFlag();
      //Clear these Flags and outputs if an overload alarm or multiple alarms exist.
      //These should not be cleared if only Window limit alarm occurs.
      if(!WarningAlarm){
    	  AlarmFlag = FALSE; //Required to handle Reset input when in Ready with alarm
    	  //Required to blink violated window limit segment on LCD when in system is in Ready state with alarm
    	  ErrorReason = 0;
    	  ClearWindowLimitAlarm();
      }
      TimedSeekTimerTicks = 0;
      ResetEnable = TRUE;
      WeldStoppedbyReset = FALSE;
   }

   if (WeldStoppedbyReset) {
      if (!CheckforResetInput() && !CheckForInputs())
         WeldStoppedbyReset = FALSE;
      else if (WebSite::CheckForWebReset())   //Forcefully clear Reset flag from website
         WebSite::ResetWebResetFlag();
   }
}

void PreWait(void)
/****************************************************************************/
/* Description:-																				 */
/* This is pass code for PreWait state.                                     */
/*                                                                          */
/****************************************************************************/
{
   //Indicates not in seek mode
   UserIO::SetSeekOut(FALSE);

   // Indicates that ultrasonics are off for seek mode.
   UserIO::SetSonicsActive(FALSE);

   //Turn the sonics off
   SonicsOff();

   SetFPGAFrequency();

   if (EnableWebIO) {
      IsInputNotPresent = TRUE;
   }
   if (!CheckForInputs() && !EnableWebIO) {
      IsInputNotPresent = TRUE;
   }
}

CurrentState CheckForCurrentFunction(void)
/****************************************************************************/
/* Description:-																				 */
/* This function returns Current Index/state of State machine and it is     */
/* called by website.                                                       */
/* Returns Current State of machine.                                        */
/****************************************************************************/
{
   CurrentState state = stateError;
   switch (CurrentIndex) {
   case WAITING_IDX:
      state = stateWait;
      break;
   case TEST_START_IDX:
   case SEEK_START_IDX:
   case SEEK_RAMP_IDX:
   case SEEK_RUN_IDX:
   case SEEK_STOP_IDX:
      state = stateSeek;
      break;
   case SEEK_WELD_IDX:
   case WELD_START_IDX:
   case WELD_RAMP_IDX:
   case WELD_RUN_IDX:
   case WELD_STOP_IDX:
      state = stateRun;
      break;
   case WELD_ERROR_IDX:
   case RESET_ERROR_IDX:
      state = stateError;
      break;
   case SCAN_START_IDX:
   case SCAN_RUN_IDX:
   case SCAN_STOP_IDX:
   case SCAN_CALCULATING_IDX:
   case SCAN_COMPLETED_IDX:
   case SCAN_ABORT_IDX:
      state = stateScan;
      break;
   };
   return state;
}

/**********************Sub State of State Machine for DCP***************************************/

/*                         Variable           TRUE    FALSE   Alarm      Skip      		Trace  */
/*                         pointer            Action  Action  Code       Index     		Code   */
/*                         ----------         ------- ------- -------    -----     		------ */
SSTATES Initial[] = { { &SystemOK, JUMP, JUMP, NONE, NONE, 100 } };

SSTATES WaitForPowerUp[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 102 }, { &PowerUpSeek, SKIP, NORMAL, NONE, SEEK_START_IDX,
      102 }, { &PowerUpScan, SKIP, NORMAL, NONE, SCAN_START_IDX, 103 }, {
      &NoPowerUpSeekScan, SKIP, NORMAL, NONE, PREWAIT_IDX, 104 }, { &TimerFlag,
      END, END, NONE, NONE, 105 } };

SSTATES WaitForInput[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &IsIOConfigured, SKIP, NORMAL, NONE, PREWAIT_IDX, 106 }, { &StartMFCycle,
      SKIP, NORMAL, NONE, MANF_WELD_START_IDX, 106 }, { &TestEnable, SKIP,
      NORMAL, NONE, TEST_START_IDX, 106 }, { &StartInputEnable, SKIP, NORMAL,
      NONE, WELD_START_IDX, 106 },
      { &ScanEnable, JUMP, NORMAL, NONE, NONE, 107 }, { &SeekEnable, SKIP,
            NORMAL, NONE, SEEK_START_IDX, 106 }, { &TimedSeekEnabled, SKIP,
            NORMAL, NONE, SEEK_START_IDX, 106 }, { &TimerFlag, END, END, NONE,
            NONE, 109 } };

SSTATES CheckScanStart[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      SCAN_ABORT_IDX, 102 }, { &ScanStarted, JUMP, NORMAL, NONE, NONE, 111 }, {
      &TimerFlag, END, END, NONE, NONE, 112 } };

SSTATES CheckScanRun[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, OVERLOAD, SCAN_ABORT_IDX, 102 }, {
      &ScanAbort, SKIP, NORMAL, NONE, SCAN_ABORT_IDX, 121 }, { &ScanRun, JUMP,
      NORMAL, NONE, NONE, 114 }, { &TimerFlag, END, END, NONE, NONE, 115 } };

SSTATES CheckScanStop[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, SCAN_ABORT_IDX, 102 }, {
      &ScanStop, SKIP, NORMAL, NONE, SCAN_CALCULATING_IDX, 117 }, { &TimerFlag,
      END, END, NONE, NONE, 118 } };

SSTATES CheckScanCalculating[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      SCAN_ABORT_IDX, 102 }, { &ScanCalculate, SKIP, NORMAL, NONE,
      SCAN_COMPLETED_IDX, 120 }, { &ScanAbort, SKIP, NORMAL, NONE,
      SCAN_ABORT_IDX, 121 }, { &TimerFlag, END, END, NONE, NONE, 122 } };

SSTATES CheckScanCompleted[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      SCAN_ABORT_IDX, 102 }, { &ScanAbort, SKIP, NORMAL, NONE, SCAN_ABORT_IDX,
      121 }, { &ScanComplete, SKIP, NORMAL, NONE, PREWAIT_IDX, 124 }, {
      &TimerFlag, END, END, NONE, NONE, 125 } };

SSTATES CheckScanAbort[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 102 }, { &IsIOConfigured, SKIP, NORMAL, NONE, PREWAIT_IDX,
      106 }, { &AlarmFound, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 126 }, { &ScanAbortComplete, SKIP, NORMAL, NONE, WAITING_IDX, 127 }, {
      &TimerFlag, END, END, NONE, NONE, 128 } };

SSTATES CheckTestStart[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 102 }, { &TestInputFlag, SKIP, NORMAL, NONE,
      SEEK_START_IDX, 130 }, { &TimerFlag, END, END, NONE, NONE, 131 } };

SSTATES CheckSeekStart[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 102 }, { &SeekStartFlag, JUMP, NORMAL, NONE, NONE, 133 },
      { &TimerFlag, END, END, NONE, NONE, 134 } };

SSTATES CheckSeekRamp[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &SeekRampFlag, JUMP, NORMAL, NONE, NONE, 136 }, { &AlarmFound, SKIP,
      NORMAL, NONE, WELD_ERROR_IDX, 137 }, { &TimerFlag, END, END, NONE, NONE,
      138 } };

SSTATES CheckSeekRun[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &SeekRunFlag, JUMP, NORMAL, NONE, NONE, 140 }, { &AlarmFound, SKIP,
      NORMAL, NONE, WELD_ERROR_IDX, 141 }, { &TimerFlag, END, END, NONE, NONE,
      142 } };

SSTATES CheckSeekStop[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &SeekStopFlag, JUMP, NORMAL, NONE, NONE, 144 }, { &TestInputFlag, SKIP,
      NORMAL, NONE, SEEK_WELD_IDX, 145 }, { &TimerFlag, END, END, NONE, NONE,
      146 } };

SSTATES WaitForNoInput[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 102 }, { &IsIOConfigured, SKIP, NORMAL, NONE, PREWAIT_IDX,
      106 }, { &NoInput, SKIP, NORMAL, NONE, WAITING_IDX, 148 }, { &TimerFlag,
      END, END, NONE, NONE, 149 } };

SSTATES CheckSeekWeld[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &SeekWeldFlag, SKIP, NORMAL, NONE, WELD_RAMP_IDX, 151 }, { &TimerFlag,
      END, END, NONE, NONE, 152 } };

SSTATES CheckWeldStart[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_STOP_IDX, 102 }, { &WeldStartFlag, JUMP, NORMAL, NONE, NONE, 154 }, {
      &WeldStartEnerFlag, SKIP, NORMAL, NONE, WELD_RAMP_ENER_IDX, 154 }, {
      &TimerFlag, END, END, NONE, NONE, 155 } };

SSTATES CheckWeldRamp[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_STOP_IDX, 102 }, {
      &WeldRampFlag, JUMP, NORMAL, NONE, NONE, 157 }, { &WeldRunComplete, SKIP,
      NORMAL, NONE, WELD_STOP_IDX, 162 }, { &AlarmFound, SKIP, NORMAL, NONE,
      WELD_ERROR_IDX, 158 },
      { &NoInput, SKIP, NORMAL, NONE, WELD_STOP_IDX, 159 }, { &TimerFlag, END,
            END, NONE, NONE, 160 } };

SSTATES CheckWeldRun[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_STOP_IDX, 102 }, {
      &WeldRunComplete, SKIP, NORMAL, NONE, WELD_STOP_IDX, 162 }, { &AlarmFound,
      SKIP, NORMAL, NONE, WELD_ERROR_IDX, 163 }, { &RampRequired, SKIP, NORMAL,
      NONE, WELD_RAMP_IDX, 151 }, { &TimerFlag, END, END, NONE, NONE, 164 } };

SSTATES CheckWeldEnerRamp[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_STOP_IDX, 102 }, { &WeldRampFlag, JUMP, NORMAL, NONE, NONE, 157 }, {
      &AlarmFound, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 158 }, { &NoInput, SKIP,
      NORMAL, NONE, WELD_STOP_IDX, 159 }, { &TimerFlag, END, END, NONE, NONE,
      160 } };

SSTATES CheckWeldEnerRun[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      WELD_STOP_IDX, 102 }, { &WeldRunComplete, SKIP, NORMAL, NONE,
      WELD_STOP_IDX, 162 }, { &AlarmFound, SKIP, NORMAL, NONE, WELD_ERROR_IDX,
      163 }, { &RampRequired, SKIP, NORMAL, NONE, WELD_RAMP_ENER_IDX, 154 }, {
      &TimerFlag, END, END, NONE, NONE, 164 } };

SSTATES CheckWeldStop[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 102 }, {
      &AlarmFlag, SKIP, NORMAL, NONE, WELD_ERROR_IDX, 103 }, { &WeldComplete,
      SKIP, NORMAL, NONE, PREWAIT_IDX, 166 }, { &TimerFlag, END, END, NONE,
      NONE, 167 } };

SSTATES CheckManFWeldStart[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      MANF_WELD_RUNSONICOFF_IDX, 102 }, { &MFTimeoutEnable, SKIP, NORMAL, NONE,
      MANF_WELD_STOP_IDX, 159 },
      { &WeldStartFlag, JUMP, NORMAL, NONE, NONE, 154 }, { &TimerFlag, END, END,
            NONE, NONE, 155 } };

SSTATES CheckManFWeldRamp[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      MANF_WELD_RUNSONICOFF_IDX, 102 }, { &MFTimeoutEnable, SKIP, NORMAL, NONE,
      MANF_WELD_STOP_IDX, 159 },
      { &WeldRampFlag, JUMP, NORMAL, NONE, NONE, 157 }, { &AlarmFound, SKIP,
            NORMAL, NONE, MANF_WELD_RUNSONICOFF_IDX, 158 }, { &WeldRunComplete,
            SKIP, NORMAL, NONE, MANF_WELD_RUNSONICOFF_IDX, 158 }, { &NoInput,
            SKIP, NORMAL, NONE, MANF_WELD_STOP_IDX, 159 }, { &TimerFlag, END,
            END, NONE, NONE, 160 } };

SSTATES CheckManFWeldRunSonicOn[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      MANF_WELD_RUNSONICOFF_IDX, 102 }, { &MFTimeoutEnable, SKIP, NORMAL, NONE,
      MANF_WELD_STOP_IDX, 159 }, { &WeldRunComplete, JUMP, NORMAL, NONE, NONE,
      162 },
      { &AlarmFound, SKIP, NORMAL, NONE, MANF_WELD_RUNSONICOFF_IDX, 163 }, {
            &TimerFlag, END, END, NONE, NONE, 164 } };

SSTATES CheckManFWeldRunSonicOff[] = { { &MFTimeoutEnable, JUMP, NORMAL, NONE,
      NONE, 162 }, { &WeldRunComplete, JUMP, NORMAL, NONE, NONE, 162 }, {
      &TimerFlag, END, END, NONE, NONE, 164 } };

SSTATES CheckManFWeldStop[] = { { &AlarmFound, SKIP, NORMAL, NONE,
      MANF_WELD_ERROR_IDX, 102 }, { &OverLoadAlarm, SKIP, NORMAL, NONE,
      MANF_WELD_ERROR_IDX, 102 }, { &WeldComplete, SKIP, NORMAL, NONE,
      WAITING_IDX, 166 }, { &TimerFlag, END, END, NONE, NONE, 167 } };

SSTATES CheckWeldError[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &WeldErrorFinish, SKIP, NORMAL, NONE,
      WAIT_STOP_IDX, 169 }, { &TimerFlag, END, END, NONE, NONE, 170 } };

SSTATES CheckManFWeldError[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 },
      { &WeldErrorFinish, JUMP, NORMAL, NONE, NONE, 157 }, { &TimerFlag, END,
            END, NONE, NONE, 170 } };

SSTATES CheckResetError[] = { { &ResetComplete, SKIP, NORMAL, NONE, WAITING_IDX,
      172 }, { &PowerUpRequired, SKIP, NORMAL, NONE, POWERUP_IDX, 173 }, {
      &TimerFlag, END, END, NONE, NONE, 174 } };

SSTATES CheckWaitStop[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &AutoResetEnable, SKIP, NORMAL, NONE, AUTO_RESET_IDX, 102 }, {
      &TimerFlag, END, END, NONE, NONE, 176 } };

SSTATES CheckAutoReset[] = { { &ResetEnable, SKIP, NORMAL, NONE,
      RESET_ERROR_IDX, 102 }, { &TimerFlag, END, END, NONE, NONE, 176 } };

SSTATES CheckPreWait[] = { { &ResetEnable, SKIP, NORMAL, NONE, RESET_ERROR_IDX,
      102 }, { &IsInputNotPresent, SKIP, NORMAL, NONE, WAITING_IDX, 166 }, {
      &TimerFlag, END, END, NONE, NONE, 176 } };

SSTATES Nop[] = { { &TimerFlag, END, END, NONE, NONE, 177 } };

/**************************State Machine for DCP*******************************************************/

/*                   Substate              Pass Code       Entry Code  		JUMP Code  		    ALJUMP Code      	State        */
/*                   Name                                                                                        		          */
/*                   --------              ----------      ----------  		---------  	    	----------       		------------ */
STATE TopOfList = { Initial, Nothing, TopOfListEntry, TopOfListExit, Nothing,
      NOP_IDX };

STATE PowerUpState = { WaitForPowerUp, PowerUpPass, PowerUpEntry, PowerUpExit,
      Nothing, POWERUP_IDX };

STATE WaitState = { WaitForInput, WaitPass, WaitEntry, WaitExit, Nothing,
      WAITING_IDX };

STATE ScanStartState = { CheckScanStart, Scan_Start, ScanStartEntry,
      ScanStartExit, Nothing, SCAN_START_IDX };

STATE ScanRunState = { CheckScanRun, Scan_Run, ScanRunEntry, Nothing,
      ScanRunAlarmExit, SCAN_RUN_IDX };

STATE ScanStopState = { CheckScanStop, Scan_Stop, ScanStopEntry, ScanStopExit,
      Nothing, SCAN_STOP_IDX };

STATE ScanCalculatingState = { CheckScanCalculating, HS_Resonance,
      ScanCalculateEntry, ScanCalculateExit, Nothing, SCAN_CALCULATING_IDX };

STATE ScanCompleteState = { CheckScanCompleted, Scan_Complete,
      ScanCompleteEntry, ScanCompleteExit, Nothing, SCAN_COMPLETED_IDX };

STATE ScanAbortState = { CheckScanAbort, Scan_Abort, ScanAbortEntry,
      ScanAbortExit, Nothing, SCAN_ABORT_IDX };

STATE TestStartState = { CheckTestStart, Test_start, TestStartEntry, Nothing,
      Nothing, TEST_START_IDX };

STATE SeekStartState = { CheckSeekStart, Seek_Start, SeekStartEntry,
      SeekStartExit, Nothing, SEEK_START_IDX };
STATE SeekRampState = { CheckSeekRamp, Seek_Ramp, SeekRampEntry, Nothing,
      Nothing, SEEK_RAMP_IDX };
STATE SeekRunState = { CheckSeekRun, Seek_Run, SeekRunEntry, Nothing, Nothing,
      SEEK_RUN_IDX };
STATE SeekStopState = { CheckSeekStop, Seek_Stop, SeekStopEntry, SeekStopExit,
      Nothing, SEEK_STOP_IDX };
STATE WaitNoInputState = { WaitForNoInput, WaitNoInputPass, WaitNoInputEntry,
      Nothing, Nothing, WAITNOINPUT_IDX };
STATE SeekWeldState = { CheckSeekWeld, Seek_Weld, SeekWeldEntry, Nothing,
      Nothing, SEEK_WELD_IDX };
STATE WeldStartState = { CheckWeldStart, Weld_Start, WeldStartEntry,
      WeldStartExit, Nothing, WELD_START_IDX };
STATE WeldRampState = { CheckWeldRamp, Weld_Ramp, WeldRampEntry, Nothing,
      Nothing, WELD_RAMP_IDX };
STATE WeldRunState = { CheckWeldRun, Weld_Run, WeldRunEntry, Nothing, Nothing,
      WELD_RUN_IDX };
STATE WeldStopState = { CheckWeldStop, Weld_Stop, WeldStopEntry, WeldStopExit,
      Nothing, WELD_STOP_IDX };
STATE WeldErrorState = { CheckWeldError, Weld_Error, WeldErrorEntry,
      WeldErrorExit, Nothing, WELD_ERROR_IDX };
STATE ResetErrorState = { CheckResetError, ResetPass, ResetEntry, ResetExit,
      Nothing, RESET_ERROR_IDX };
STATE WaitStopState = { CheckWaitStop, PS_Error, WaitStopEntry, Nothing,
      Nothing, WAIT_STOP_IDX };

STATE ManFWeldStartState = { CheckManFWeldStart, ManFWeld_Start,
      ManFWeldStartEntry, WeldStartExit, Nothing, MANF_WELD_START_IDX };
STATE ManFWeldRampState = { CheckManFWeldRamp, ManFWeld_Ramp, WeldRampEntry,
      Nothing, Nothing, MANF_WELD_RAMP_IDX };
STATE ManFWeldRunSonicOnState = { CheckManFWeldRunSonicOn, ManFWeldSonicOn_Run,
      ManFWeldRunSonicOnEntry, Nothing, Nothing, MANF_WELD_RUNSONICON_IDX };
STATE ManFWeldRunSonicOffState = { CheckManFWeldRunSonicOff,
      ManFWeldSonicOff_Run, ManFWeldRunSonicOffEntry, Nothing, Nothing,
      MANF_WELD_RUNSONICOFF_IDX };

STATE ManFWeldStopState = { CheckManFWeldStop, ManFWeld_Stop, WeldStopEntry,
      WeldStopExit, Nothing, MANF_WELD_STOP_IDX };

STATE ManFWeldErrorState = { CheckManFWeldError, ManFWeld_Error,
      ManFWeldErrorEntry, WeldErrorExit, Nothing, MANF_WELD_ERROR_IDX };

STATE ManFWaitStopState = { CheckWaitStop, ManFPS_Error, ManFWaitStopEntry,
      Nothing, Nothing, MANF_WAIT_STOP_IDX };

STATE AutoResetState = { CheckAutoReset, AutoResetAlarm, AutoResetEntry,
      Nothing, Nothing, AUTO_RESET_IDX };

STATE PreReadyState = { CheckPreWait, PreWait, PreWaitEntry, Nothing, Nothing,
      PREWAIT_IDX };

STATE WeldRampEnergyState = { CheckWeldEnerRamp, Weld_EnerRamp, WeldRampEntry,
      Nothing, Nothing, WELD_RAMP_ENER_IDX };

STATE WeldRunEnergyState = { CheckWeldEnerRun, Weld_EnerRun, WeldRunEntry,
      Nothing, Nothing, WELD_RUN_ENER_IDX };

STATE EndOfList =
      { Nop, Nothing, EndOfListEntrycode, Restart, Nothing, NOP_IDX };
