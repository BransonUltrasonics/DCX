/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/StateMachineDrv.cpp_v   1.11.1.5   01 May 2014 10:53:32   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/StateMachineDrv.cpp_v  $
 * 
 *    Rev 1.11.1.5   01 May 2014 10:53:32   amaurya
 * Added check for Alarm
 * 
 *    Rev 1.11.1.4   30 Apr 2014 06:19:40   amaurya
 * Added change for Digital Tune
 * 
 *    Rev 1.11.1.3   12 Mar 2014 07:52:28   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.11.1.2   24 Jan 2014 06:47:28   rjamloki
 * Fixed User I/O Signal Sequence Error when Alarms are Generated.
 * 
 *    Rev 1.11.1.1   04 Dec 2013 06:57:12   rjamloki
 * Fixed PowerUpScan graph issue.
 * 
 *    Rev 1.11.1.0   09 Sep 2013 08:59:56   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.12   14 Aug 2013 09:31:04   amaurya
 * Added Seek Frequency Offset and Amplitude ramping.
 * 
 *    Rev 1.11   15 Jan 2013 04:41:58   amaurya
 * Energy Special.
 * 
 *    Rev 1.8.1.11   16 Sep 2011 08:17:16   RatneshA
 * changes regarding amplitude based Horn Scan alogorithm
 * 
 *    Rev 1.8.1.10   01 Sep 2011 13:06:16   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.8.1.9   16 Aug 2011 08:28:44   pdwivedi
 * code cleanup
 * 
 *    Rev 1.8.1.8   05 Aug 2011 10:34:04   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.8.1.7   02 Aug 2011 10:54:06   pdwivedi
 * Added changes for Memory management and removing warnings.
 * 
 *    Rev 1.8.1.6   02 Aug 2011 07:39:34   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.8.1.5   15 Jul 2011 09:48:18   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.8.1.4   13 Jun 2011 09:22:10   PDwivedi
 * Scan buffer initialization before very scan.
 * 
 *    Rev 1.8.1.3   11 May 2011 08:41:36   ygupta
 * sonic out fix during seek,scan and TEST
 * 
 *    Rev 1.8.1.2   09 May 2011 08:11:58   ygupta
 * Manufacturing counters error fixed
 * 
 *    Rev 1.8.1.0   27 Apr 2011 10:48:18   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.8   19 Apr 2011 10:49:04   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   04 Apr 2011 10:58:24   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.6   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.5   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:49:30   nhahmed
 * Changed to support TEST through LCD.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#include "StateMachineDrv.h"
#include "stdio.h"
#include "FUNC.h"
#include "Globals.h"
#include "UserIO.h"
#include "LOG.h"
#include "ReceiveCommandHandler.h"
#include "DebugNew.h"

/* Current sequence type */
enum WELD_SEQUENCE_TYPE CurrentSequence = NONEDEFINED;
enum WELDSTATES_IDX WeldState;          /* Current weld state */

/*-------------------------- Global Variables -----------------------------*/
BOOLEAN SystemOK =  TRUE;
BOOLEAN TimerFlag = FALSE;
SINT16 CurrentIndex; 			/* Index to current state pointer*/
BOOLEAN AbortFlag; 				/*When set to TRUE, will abort the current sequence. */
BOOLEAN FailFlag; 				/*Only set by state machine, cleared with every call to state machine*/
SINT16 LastFailTrace;
SINT16 FailTrace;
STATE *WeldSequence[MAX_STATE + 1]; /* Current weld cycle definition */
/*--------------------------------------------- ---------------------------*/

#define printf
#define pprintf
/*-------------------------- PRIVATE GLOBAL DATA --------------------------*/
static UINT16 StateMachineFlag = STOPPED;	/* Current StateMachineFlag      */
static STATE *CurrentPtr;  					/* Current state pointer         */
static SINT16 PushedIndex;
static SINT16 LastTrace;
static SSTATES *SSptr; 							/* Current sub state pointer     */
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16   NumScanPoints;
extern UINT16   LCDKeyStatus;
extern BOOLEAN  AlarmFlag;
extern UINT32   RampStepTimer;
extern SINT32   RampTime, WeldAmplitudeLimit;
extern BOOLEAN RampDown;
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

void StateMachine()
{
	BOOLEAN EndFlag= FALSE;
	BOOLEAN TestValue;
	SINT16 Cond;

	// Check state machine Flag
	if (StateMachineFlag == RUNNING) {
		CurrentPtr = WeldSequence[CurrentIndex];
		SSptr = CurrentPtr->ptr; /* Get sub state pointer      */
		CurrentPtr->PassPtr();   /* Execute this pass code     */
		FailFlag = FALSE;

		while (EndFlag == FALSE) {
			TestValue = *(SSptr->ValuePtr);
			if (TestValue == FALSE) { /* Test the value          */
				Cond = SSptr->False;  /* Was FALSE, use FALSE cmd */
			} else {
				Cond = SSptr->True;   /* Was TRUE, use TRUE cmd   */
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
				SSptr++;        /* Move to Next Sub State   */
				LastTrace = SSptr->Trace;
				CurrentIndex++; /* Increment index   */
				CurrentPtr = WeldSequence[CurrentIndex];
				if (CurrentPtr->state != NOP_IDX)
					WeldState = CurrentPtr->state; /* Update state variable    */
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
				if (CurrentPtr->state != NOP_IDX)
					WeldState = CurrentPtr->state; /* Update state variable   */
				if (AbortFlag == FALSE)
					CurrentPtr->EntryPtr(); /* Execute new entry code  */
				EndFlag = TRUE;
				break;
			case SKIP:
				if (SSptr->Alarm != NONE) {
					CurrentPtr->AlarmPtr();             /* Execute Alarm code     */
				}
				CurrentPtr->JumpPtr(); /* Execute exit code       */
				LastTrace = SSptr->Trace;
				CurrentIndex = SSptr->Skip; /* Skip to next state                      */

				CurrentPtr = WeldSequence[CurrentIndex];
				if (CurrentPtr->state != NOP_IDX)
					WeldState = CurrentPtr->state; /* Update state variable                   */
				if (AbortFlag == FALSE)
					CurrentPtr->EntryPtr(); /* Execute new entry code                  */

				EndFlag = TRUE;
				break;
			case POP:
				CurrentPtr->AlarmPtr(); /* Execute alarm code     */
				LastTrace = SSptr->Trace;
				CurrentIndex = PushedIndex; /* Reset index            */
				CurrentPtr = WeldSequence[CurrentIndex];
				if (CurrentPtr->state != NOP_IDX)
					WeldState = CurrentPtr->state; /* Update state variable  */
				if (AbortFlag == FALSE)
					CurrentPtr->EntryPtr(); /* Execute new entry code */
				EndFlag = TRUE;
				break;

			case ALJUMP:
				if (SSptr->Alarm != NONE) {
				}
				CurrentPtr->AlarmPtr(); /* Execute Alarm code      */
				LastTrace = SSptr->Trace;
				CurrentIndex++; /* Increment index         */
				CurrentPtr = WeldSequence[CurrentIndex];
				if (CurrentPtr->state != NOP_IDX)
					WeldState = CurrentPtr->state; /* Update state variable   */

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

void PowerUpEntry()
/****************************************************************************/
/* Description:- 																				 */
/* This is Entry code for PowetUp State.								             */
/****************************************************************************/
{
//	pprintf("%s",__FUNCTION__);
//	if(RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)
//	{
//		TotalPowerUpDelay = (POWERUPDELAY + AUTOSONICSDELAY);
//	}
//	else
		TotalPowerUpDelay = POWERUPDELAY;
}

void PowerUpPass()
/****************************************************************************/
/* Description:- 																				 */
/* This is Pass code for Power up State.It checks for power up seek and     */
/* power up horn scan option in Running preset during Power up. If no option*/
/* is enabled ,then it moves to WAITING state.                              */
/****************************************************************************/
{
	SonicsOff();

	/* powerup seek */
	if((PowerUpSeekStarted == FALSE)&&((RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEK) /*||
			(RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)*/))
	{
		NoPowerUpSeekScan = FALSE;
		if(PowerUpDelay <= TotalPowerUpDelay) //PowerUp Delay
		{
		   PowerUpDelay++;
		}
		else
		{
			PowerUpDelay = 0;
			PowerUpCompleted = TRUE;
			PowerUpSeek = TRUE;
			PowerUpSeekStarted = TRUE;
		}
	}
	/* powerup scan */
	else if((PowerUpScanStarted == FALSE) && (RunningPreset.SWDip.Flags.PowerUpOptions == POWERUPSCAN))
	{
		NoPowerUpSeekScan = FALSE;
		if(PowerUpDelay <= TotalPowerUpDelay)  //PowerUp Delay
		{
		 PowerUpDelay++;
		}
		else
		{
			PowerUpDelay = 0;
		 PowerUpCompleted = TRUE;
		 Reset_Scanbuffer();
		 PowerUpScan = TRUE;
		 ScanEnable = TRUE;
		 PowerUpScanStarted = TRUE;
		}
	}
	else // No power up seek /scan is set
	{
	  if(PowerUpDelay <= TotalPowerUpDelay)
	  {
		 PowerUpDelay++;
	  }
	  else
	  {
		  PowerUpDelay = 0;
		 PowerUpCompleted = TRUE;
		 NoPowerUpSeekScan = TRUE;
	  }
	}
}

void WaitEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Waiting State.								             */
/****************************************************************************/
{
	ScanEnable = FALSE;
	WebSeek = FALSE;
//	AutoSonicsEnable = FALSE;
	//Indicate DCP is ready.
	UserIO::SetReady(TRUE);

	 // Indicates that ultrasonics are off for seek,test,weld and scan mode.
	UserIO::SetSonicsActive(FALSE);

	//Indicates not in seek mode
	UserIO::SetSeekOut(FALSE);
}

void WaitPass()
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

	if(UpdateRPreset == TRUE)/*Update Running preset */
	{
	  UpdateRPreset = FALSE;
	  UpdateRunningPreset();
	}
	if(ScanInitiate)         /* Check for scan initiate*/
	{
	  ScanInitiate = FALSE;
	  ScanEnable = TRUE;
	}
	if(PowerUpScan)
	{
		if(WaitDelay <= TOTALWAITDELAY)
	   {
			WaitDelay++;
	   }
	   else
	   {
		   WaitDelay = 0;
		  PowerUpScan = FALSE;
		  /*Check for Autosonics*/
//		  if(RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)
//		  {
//			 AutoSonicsEnable = TRUE;
//		  }
		  if(MFCycleFlag == TRUE)
		  {
		  	StartMFCycle = TRUE;
		  	EnableWebIO = TRUE;
		  }
	   }
	}
	else if(PowerUpSeek)
	{
		if(WaitDelay <= TOTALWAITDELAY)
		{
			WaitDelay++;
		}
		else
		{
			WaitDelay = 0;
	  PowerUpSeek = FALSE;
	  /*Check for Autosonics*/
//	  if(RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)
//	  {
//		 AutoSonicsEnable = TRUE;
//	  }
			if(MFCycleFlag == TRUE)
			{
				StartMFCycle = TRUE;
				EnableWebIO = TRUE;
			}
		}
	}
	else if(NoPowerUpSeekScan)
	{
		if(WaitDelay <= TOTALWAITDELAY)
		{
			WaitDelay++;
		}
		else
		{
			WaitDelay = 0;
	  NoPowerUpSeekScan = FALSE;
	  /*Check for Autosonics*/
//	  if(RunningPreset.SWDip.Flags.PowerUpOptions ==  POWERUPSEEKAUTOSONICS)
//	  {
//		 AutoSonicsEnable = TRUE;
//		  }
		  if(MFCycleFlag == TRUE)
			{
				StartMFCycle = TRUE;
				EnableWebIO = TRUE;
			}
	  }
	}
	SetFPGAFrequency();
}

void ScanStartEntry()
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
	Log::WriteEventLog(EVENT_HORNSCAN_START,0,0);
}

void ScanStartExit()
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Scan Start State.                                  */
/*                                                                          */
/****************************************************************************/
{

}

void ScanRunEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Run State.It initializes ScanRun Flag to     */
/* FALSE state .                                                            */
/****************************************************************************/
{
	ScanRun = FALSE;
	ScanAbort = FALSE;
}

void ScanRunAlarmExit()
{
	UINT16 data;
	//FPGA Amplitude is ramp Down
	PWMAmp = 0;
	data = HornScanPointLast;

	//End Mark of scan buffer
	ScanBuffer[HornScanGraphPointCount].Status = data;
	ScanBuffer[HornScanGraphPointCount+1].Status = data;
}

void ScanStopEntry()
/*****************************************************************************/
/* Description:-																				  */
/* This is Entry code for Scan Stop State.It initializes ScanStop flag to    */
/* FALSE and updates number of scan points in FRAM.                          */
/*****************************************************************************/
{
	NumScanPoints = HornScanGraphPointCount;
	ScanStop = FALSE;
}

void ScanCalculateEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Calculate State.It initializes ScanCalculate */
/* and ScanAbort flags to FALSE.                                            */
/****************************************************************************/
{
	ScanCalculate = FALSE;
	ScanAbort = FALSE;
}

void ScanCompleteEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Scan Complete State.It initializes ScanComplete   */
/* flag to FALSE.                                                           */
/****************************************************************************/
{
	ScanComplete = FALSE;
}

void ScanAbortEntry()
/****************************************************************************/
/*Description:- 																				 */
/* This is Entry code for Scan Abort State. It initializes to ScanAbort flag*/
/* to FALSE.                                                                */
/****************************************************************************/
{
	ScanAbortComplete = FALSE;
	//Write Event Log
	if(WebSite::CheckForWebScanAbortCommand()){
		Log::WriteEventLog(EVENT_HORNSCAN_ABORTED,0,0);
	}
	else{
		Log::WriteEventLog(EVENT_HORNSCAN_FAILED,0,0);
		CurrentPreset.DigitalTune = Midband;
		MemoryValue	= 0;
		CurrentPreset.MemoryFreq = MemoryValue;
		SavePreset(0);
	}
	WebSite::ResetScanFlags();
}

void ScanStopExit()
/****************************************************************************/
/*Description:- 																				 */
/* This is Exit code for Scan Stop State.                                   */
/*                                                                          */
/****************************************************************************/
{

}

void ScanCalculateExit()
/****************************************************************************/
/* Description:- 																				 */
/* This is Exit code for Scan Calculate State.                              */
/*                                                                          */
/****************************************************************************/
{

}

void ScanCompleteExit()
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Scan Complete State. It turns the sonics to off    */
/* and updates LOG entry.                                                   */
/****************************************************************************/
{
	ScanInitiate = FALSE;
	ScanStarted = FALSE;
	ScanRun = FALSE;
	SonicsOff();
	SetFPGAFrequency();
	Log::WriteEventLog(EVENT_HORNSCAN_COMPLETED,MainParallelResonanceFreq,0);
	if(ScanComplete == TRUE){
		if((((MainParallelResonanceFreq * 1000) % 150) != 0) && (F_Factor == F30KHZF_FACTOR))
			//to store exact mainparallelfreq in digitaltune at 30Khz
			CurrentPreset.DigitalTune = ((MainParallelResonanceFreq * 1000)/F_Factor) + 1;
		else
			CurrentPreset.DigitalTune = ((MainParallelResonanceFreq * 1000)/F_Factor);
		MemoryValue = 0;
		CurrentPreset.MemoryFreq = MemoryValue;
		RunningPreset.MemoryFreq = MemoryValue;
		SavePreset(0);
	}
	WebSite::ResetScanFlags();
}

void ScanAbortExit()
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

void TestStartEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Test Start State.                                 */
/*                                                                          */
/****************************************************************************/
{
	TestEnable = FALSE;
}

void SeekStartEntry()
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
   //Indicates in seek mode
   UserIO::SetSeekOut(TRUE);
   UserIO::SetSonicsActive(TRUE);
}

void SeekStartExit()
/*****************************************************************************/
/* Description:-																				  */
/*This is Exit code for Seek start State.				                          */
/*                                                                           */
/*****************************************************************************/
{

}

void SeekRampEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Ramp State.It initializes SeekRampFlag and   */
/* AlarmFound flag to FALSE.                                                */
/****************************************************************************/
{
	SeekRampFlag = FALSE;
	AlarmFound = FALSE;
}

void SeekRunEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is entry code for Seek Run State.It initializes SeekRunFlag and     */
/* AlarmFound to FALSE.                                                     */
/****************************************************************************/
{
	SeekRunFlag = FALSE;
	AlarmFound = FALSE;
}

void SeekStopEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Stop State.It initializes SeekStopFlag to    */
/* FALSE.                                                                   */
/****************************************************************************/
{
	SeekStopFlag = FALSE;
}

void SeekStopExit()
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Seek Stop State.It updates DDSFrquency information */
/* to be send to FPGA.                                                      */
/****************************************************************************/
{
	if(OverLoadAlarm || AlarmFound){
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

void WaitNoInputEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Wait no Input State.It updates NoInput flag to    */
/* FALSE value .                                                            */
/****************************************************************************/
{
	NoInput = FALSE;
	TestInputFlag = FALSE;
}

void WaitNoInputPass()
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Wait no Input State.It updates Output Status to be */
/* send to MI Board and DDSFrquency information to be send to FPGA.         */
/****************************************************************************/
{
	//The case where manufacturing cycle will start after powerup. We will ignore the external inputs
	//whether they are on or off
	if(EnableWebIO)
		NoInput = TRUE;
	else if(!(UserIO::CheckForExternalReset() || WebSite::CheckForWebReset() || UserIO::CheckForExternalStart() || UserIO::CheckForExternalTest()
	|| (LCDKeyStatus & KB_TEST) || WebSite::CheckForWebTestCommand()) && !(UserIO::CheckForExternalSeek()) && !(UserIO::CheckForHornScan()) && !(UserIO::CheckForMemoryClear()))
	{
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

void SeekWeldEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Seek Weld State.It initializes SeekWeldFlag flag  */
/* to  FALSE.                                                               */
/****************************************************************************/
{
	SeekWeldFlag = FALSE;
}

void WeldStartEntry()
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

void ManFWeldStartEntry()
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

void WeldStartExit()
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for Weld Start State.        									 */
/*                              															 */
/****************************************************************************/
{

}

void WeldRampEntry()
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

void WeldRunEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Run State.It initializes WeldRunComplete flag*/
/* and AlarmFound Flag to FALSE.                                            */
/****************************************************************************/
{
	//pprintf("%s",__FUNCTION__);
	WeldRunComplete = FALSE;
	AlarmFound = FALSE;
	RampRequired = FALSE;
}

void ManFWeldRunSonicOnEntry()
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

void ManFWeldRunSonicOffEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Manufacturing WeldRunSonicOff State.It initializes*/
/* WeldRunComplete flag ,AlarmFound Flag to FALSE and SonicsOnOffTimer to   */
/* Zero.																							 */
/****************************************************************************/
{
	WeldRunComplete = FALSE;
}

void WeldStopEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop State.It initializes WeldComplete flag  */
/* to FALSE value.                                                          */
/****************************************************************************/
{
	WeldComplete = FALSE;
}

void WeldStopExit()
/****************************************************************************/
/* Description:-																				 */
/* This is normal Exit code for Weld Stop State.It updates DDS frequency    */
/* information to be send to FPGA.                                          */
/****************************************************************************/
{
	SetFPGAFrequency();
	ScanInitiate = FALSE;
	TestInputFlag = FALSE;
	StartInputEnable = FALSE;
}

void ResetEntry()
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

void ResetExit()
/****************************************************************************/
/* Description:-																				 */
/* This is exit code for Reset State.                                       */
/****************************************************************************/
{
	ResetEnable = FALSE;
}

void ResetPass()
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Reset State.It updates DDSFrquency                 */
/* information to be send to FPGA. It also initializes ScanInitiate flag    */
/* to zero. 						                                              */
/****************************************************************************/
{
	FPGAOverloadReset();

	if((fpga_input & OL_MASK) == FALSE)
	{
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
		if(PowerUpCompleted == FALSE)
		{
			// if not reset state machine index to zero.
			Restart();
			PowerUpRequired = TRUE;
		}
		else
		{
			ResetComplete = TRUE;
			PowerUpSeek = FALSE;
			PowerUpScan = FALSE;
			NoPowerUpSeekScan = FALSE;
			if(MFCycleFlag)
			{
				StartMFCycle = TRUE;
			}
		}
	}
}

void WeldErrorEntry()
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
	WriteAlarmLog();
	if(MFCycleFlag){
		StartMFCycle = FALSE;
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
//		pprintf("\n cycle stopped 1");
		if(MFcycleResults.cycleStatus == statCompleted){
		}else
			MFcycleResults.cycleStatus = statStopped;
	}
}

void ManFWeldErrorEntry()
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
//	pprintf("\n cycle stopped 2");
	if(MFcycleResults.cycleStatus == statCompleted){
	}else
		MFcycleResults.cycleStatus = statStopped;
}

void WeldErrorExit()
/****************************************************************************/
/* Description:-																				 */
/* This is normal Exit code for Weld Error state.                           */
/*                                                                          */
/****************************************************************************/
{

}

void WaitStopEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
	//pprintf(" %s ", __FUNCTION__);
	AutoResetEnable = FALSE;
}

void ManFWaitStopEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
	//pprintf(" %s ", __FUNCTION__);
	AutoResetEnable = FALSE;
}

void AutoResetEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for Weld Stop state.                                  */
/*                                                                          */
/****************************************************************************/
{
}

void PreWaitEntry()
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
}

void Test_start()
/****************************************************************************/
/* Description:-																				 */
/* This is Pass code for Test Start State. It initializes TestInputFlag flag*/
/* to TRUE.                                                                 */
/****************************************************************************/
{
	BOOLEAN PresetVerifiedSuccess = TRUE;
	if (PresetVerifiedRequired)
	{
		PresetVerifiedRequired = FALSE;
		PresetVerifiedSuccess = VerifyPreset();
	}
	if(!PresetVerifiedSuccess)
	{
		CheckAndAssignSeekCycleBoundaryValue();
		CheckAndAssignWeldCycleBoundaryValue();
	}
	TestInputFlag = TRUE;
	Log::WriteEventLog(TEST_ACTIVATED, 0, 0);
}

void TopOfListEntry()
/****************************************************************************/
/* Description:-																				 */
/* This is Entry code for NOP State.								                */
/****************************************************************************/
{

}

void TopOfListExit()
/****************************************************************************/
/* Description:-																				 */
/* This is Exit code for NOP State.			    					                */
/****************************************************************************/
{

}

void EndOfListEntrycode()
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
	if(((RunningPreset.AlarmResetReq == FALSE) && !EnableWebIO && (UserIO::CheckForExternalStart())))
	{
		AlarmFlag = FALSE;
		UserIO::SetGenAlarm(FALSE);
		WebSite::ResetWebResetFlag();
		ErrorReason = 0;
		TimedSeekTimerTicks = 0;
		ResetEnable = TRUE;
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

	if(EnableWebIO)
	{
		IsInputNotPresent = TRUE;
	}
	if(!CheckForInputs() && !EnableWebIO)
	{
		IsInputNotPresent = TRUE;
	}
}

CurrentState CheckForCurrentFunction()
/****************************************************************************/
/* Description:-																				 */
/* This function returns Current Index/state of State machine and it is     */
/* called by website.                                                       */
/* Returns Current State of machine.                                        */
/****************************************************************************/
{
	CurrentState state = stateError;
	switch(CurrentIndex)
	{
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
SSTATES Initial[]       = {  {&SystemOK,         JUMP,   JUMP, 	 NONE,    NONE,     	  	     100}};

SSTATES WaitForPowerUp [] = {{&ResetEnable,       SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,    102},
							 {&OverLoadAlarm,	  SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,  102},
							 {&PowerUpSeek,       SKIP,   NORMAL, NONE, 	 SEEK_START_IDX,     102},
                             {&PowerUpScan,       SKIP,   NORMAL, NONE,    SCAN_START_IDX,    103},
                             {&NoPowerUpSeekScan, SKIP,   NORMAL, NONE,    PREWAIT_IDX,       104},
                             {&TimerFlag,         END,    END,    NONE,    NONE,              105}};

SSTATES WaitForInput[]  = {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
							 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
							 {&IsIOConfigured,   SKIP,   NORMAL, NONE,	 PREWAIT_IDX,        106},
							 {&StartMFCycle,		SKIP,   NORMAL, NONE,	 MANF_WELD_START_IDX,106},
							 {&TestEnable,			SKIP,   NORMAL, NONE,  	 TEST_START_IDX,    106},
                             {&StartInputEnable,	SKIP,   NORMAL, NONE,  	 WELD_START_IDX,    106},
                             //{&AutoSonicsEnable, SKIP,   NORMAL, NONE,    WELD_START_IDX,    108},
                             {&ScanEnable,       JUMP,   NORMAL, NONE,    NONE,     	  	     107},
                             {&SeekEnable,			SKIP,   NORMAL, NONE,  	 SEEK_START_IDX,    106},
                             {&TimedSeekEnabled,	SKIP,   NORMAL, NONE,  	 SEEK_START_IDX,    106},
                             {&TimerFlag,        END,    END,    NONE,    NONE,              109}};

SSTATES CheckScanStart[]= {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 SCAN_ABORT_IDX,    102},
									  {&ScanStarted,      JUMP,   NORMAL, NONE,    NONE,     		     111},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              112}};

SSTATES CheckScanRun[]={     {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, OVERLOAD,SCAN_ABORT_IDX,    102},
									  {&ScanAbort,		   SKIP,   NORMAL, NONE,    SCAN_ABORT_IDX,    121},
									  {&ScanRun,          JUMP,   NORMAL, NONE,    NONE,     		     114},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              115}};

SSTATES CheckScanStop[]=  {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 SCAN_ABORT_IDX,    102},
									  {&ScanStop,		   SKIP,   NORMAL, NONE,    SCAN_CALCULATING_IDX,117},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              118}};

SSTATES CheckScanCalculating[]=
                           { {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
                             {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 SCAN_ABORT_IDX,    102},
                             {&ScanCalculate,		SKIP,   NORMAL, NONE,    SCAN_COMPLETED_IDX,120},
							        {&ScanAbort,		   SKIP,   NORMAL, NONE,    SCAN_ABORT_IDX,    121},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              122}};

SSTATES CheckScanCompleted[]=
                           { {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
                             {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 SCAN_ABORT_IDX,    102},
                             {&ScanAbort,		   SKIP,   NORMAL, NONE,    SCAN_ABORT_IDX,    121},
                             {&ScanComplete,		SKIP,   NORMAL, NONE,    PREWAIT_IDX,  	  124},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              125}};

SSTATES CheckScanAbort[]= { {&ResetEnable,       SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&IsIOConfigured,   SKIP,   NORMAL, NONE,	 PREWAIT_IDX,        106},
									 {&ScanAbortComplete,	SKIP,   NORMAL, NONE,    WAITING_IDX,       127},
						          {&TimerFlag,         END,    END,    NONE,    NONE,              128}};

SSTATES CheckTestStart[] = {{&ResetEnable,       SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&TestInputFlag,     SKIP,   NORMAL, NONE,    SEEK_START_IDX,    130},
                            {&TimerFlag,         END,    END,    NONE,    NONE,              131}};

SSTATES CheckSeekStart[]= { {&ResetEnable,       SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&SeekStartFlag, 	   JUMP,   NORMAL, NONE,    NONE,     		     133},
						          {&TimerFlag,         END,    END,    NONE,    NONE,              134}};

SSTATES CheckSeekRamp[]=  { {&ResetEnable,       SKIP,   NORMAL,  NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&SeekRampFlag,	   JUMP,   NORMAL, NONE,    NONE,     		     136},
						          {&AlarmFound, 	      SKIP,   NORMAL, NONE, WELD_ERROR_IDX,    137},
						          {&TimerFlag,         END,    END,    NONE,    NONE,              138}};

SSTATES CheckSeekRun[]=   { {&ResetEnable,       SKIP,   NORMAL,  NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&SeekRunFlag,	      JUMP,   NORMAL, NONE,    NONE,     		     140},
						          {&AlarmFound, 	      SKIP,   NORMAL, NONE, WELD_ERROR_IDX,    141},
						          {&TimerFlag,         END,    END,    NONE,    NONE,              142}};

SSTATES CheckSeekStop[]=  { {&ResetEnable,       SKIP,   NORMAL,  NONE, 	 RESET_ERROR_IDX,   102},
									 {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									 {&SeekStopFlag,	   JUMP,   NORMAL, NONE,    NONE,     	  	     144},
						          {&TestInputFlag, 	   SKIP,   NORMAL, NONE,    SEEK_WELD_IDX,     145},
						          {&TimerFlag,         END,    END,    NONE,    NONE,              146}};

SSTATES WaitForNoInput[]  = {{&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&IsIOConfigured,   SKIP,   NORMAL, NONE,	 PREWAIT_IDX,        106},
									  {&NoInput,          SKIP,   NORMAL, NONE,    WAITING_IDX,  	  148},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              149}};

SSTATES CheckSeekWeld[]= {   {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&SeekWeldFlag,	   SKIP,   NORMAL, NONE,    WELD_RAMP_IDX,     151},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              152}};

SSTATES CheckWeldStart[]= {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&WeldStartFlag,	   JUMP,   NORMAL, NONE,    NONE,     		     154},
									  {&WeldStartEnerFlag,	SKIP,   NORMAL, NONE,    WELD_RAMP_ENER_IDX,154},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              155}};

SSTATES CheckWeldRamp[]=  {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&WeldRampFlag,	   JUMP,   NORMAL, NONE,    NONE,     		     157},
						           {&AlarmFound, 	   SKIP,   NORMAL, NONE,WELD_ERROR_IDX,    158},
						           {&NoInput,          SKIP,   NORMAL, NONE,    WELD_STOP_IDX,     159},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              160}};

SSTATES CheckWeldRun[]=   {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&WeldRunComplete,  JUMP,   NORMAL, NONE,    NONE,     		     162},
						           {&AlarmFound, 	   SKIP,   NORMAL, NONE,WELD_ERROR_IDX,    163},
						           {&RampRequired,	   SKIP,   NORMAL, NONE,    WELD_RAMP_IDX,     151},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              164}};

SSTATES CheckWeldEnerRamp[]= {{&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&WeldRampFlag,	      JUMP,   NORMAL, NONE,    NONE,     		     157},
						           {&AlarmFound, 	   SKIP,   NORMAL, NONE,WELD_ERROR_IDX,    158},
						           {&NoInput,          SKIP,   NORMAL, NONE,    WELD_STOP_IDX,     159},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              160}};

SSTATES CheckWeldEnerRun[]=  {  {&ResetEnable,        SKIP,   NORMAL,  NONE, 	RESET_ERROR_IDX,   102},
								   {&OverLoadAlarm,	  SKIP,   NORMAL,  NONE, 	WELD_ERROR_IDX,    102},
								   {&WeldRunComplete, SKIP,   NORMAL,  NONE,    WELD_STOP_IDX,     		     162},
						           {&AlarmFound, 	  SKIP,   NORMAL,  NONE,	WELD_ERROR_IDX,    163},
						           {&RampRequired,	SKIP,   NORMAL, NONE,    WELD_RAMP_ENER_IDX,	154},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              164}};

SSTATES CheckWeldStop[]=  {  {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 WELD_ERROR_IDX,    102},
									  {&WeldComplete,	   SKIP,   NORMAL, NONE,    WAITING_IDX,       166},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              167}};

SSTATES CheckManFWeldStart[]= {{&ResetEnable,    SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 MANF_WELD_RUNSONICOFF_IDX,    102},
									  {&MFTimeoutEnable,  SKIP,   NORMAL, NONE,    MANF_WELD_STOP_IDX,     159},
									  {&WeldStartFlag,	   JUMP,   NORMAL, NONE,    NONE,     		     154},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              155}};

SSTATES CheckManFWeldRamp[]=  {{&ResetEnable,      SKIP,   NORMAL, NONE,  RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 MANF_WELD_RUNSONICOFF_IDX,    102},
									  {&MFTimeoutEnable,  SKIP,   NORMAL, NONE,    MANF_WELD_STOP_IDX,     159},
									  {&WeldRampFlag,	   JUMP,   NORMAL, NONE,    NONE,     		     157},
						           {&AlarmFound, 	   SKIP,   NORMAL, NONE,	 MANF_WELD_RUNSONICOFF_IDX,    158},
						           {&WeldRunComplete,  SKIP,   NORMAL, NONE,	 MANF_WELD_RUNSONICOFF_IDX,    158},
						           {&NoInput,          SKIP,   NORMAL, NONE,    MANF_WELD_STOP_IDX,     159},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              160}};

SSTATES CheckManFWeldRunSonicOn[]= {
		                       {&ResetEnable,      SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 MANF_WELD_RUNSONICOFF_IDX,    102},
									  {&MFTimeoutEnable,  SKIP,   NORMAL, NONE,    MANF_WELD_STOP_IDX,     159},
									  {&WeldRunComplete,  JUMP,   NORMAL, NONE,    NONE,     		     162},
						           {&AlarmFound, 	   SKIP,   NORMAL, NONE,    MANF_WELD_RUNSONICOFF_IDX,    163},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              164}};


SSTATES CheckManFWeldRunSonicOff[]= {
									  {&MFTimeoutEnable,  JUMP,   NORMAL, NONE,    NONE,     		     162},
									  {&WeldRunComplete,  JUMP,   NORMAL, NONE,    NONE,     		     162},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              164}};

SSTATES CheckManFWeldStop[]= {
									  {&AlarmFound,		   SKIP,   NORMAL, NONE, 	 MANF_WELD_ERROR_IDX,    102},
									  {&OverLoadAlarm,		SKIP,   NORMAL, NONE, 	 MANF_WELD_ERROR_IDX,    102},
									  {&WeldComplete,	   SKIP,   NORMAL, NONE,    WAITING_IDX,       166},
						           {&TimerFlag,        END,    END,    NONE,    NONE,              167}};

SSTATES CheckWeldError[]= {
							 {&ResetEnable,       SKIP,   NORMAL, NONE,    RESET_ERROR_IDX,   102},
							 {&WeldErrorFinish,   SKIP,   NORMAL, NONE,    WAIT_STOP_IDX,     169},
						     {&TimerFlag,         END,    END,    NONE,    NONE,              170}};

SSTATES CheckManFWeldError[]= {
		                     {&ResetEnable,       SKIP,   NORMAL, NONE, 	 RESET_ERROR_IDX,   102},
							 {&WeldErrorFinish,	  JUMP,   NORMAL, NONE,    NONE,     		     157},
						     {&TimerFlag,         END,    END,    NONE,    NONE,              170}};


SSTATES CheckResetError[]={  
 							 {&ResetComplete,	   SKIP,   NORMAL, NONE,    WAITING_IDX,       172},
							 {&PowerUpRequired,   SKIP,   NORMAL, NONE,    POWERUP_IDX,       173},
						     {&TimerFlag,         END,    END,    NONE,    NONE,              174}};

SSTATES CheckWaitStop[]= {   {&ResetEnable,       SKIP,   NORMAL, NONE,    RESET_ERROR_IDX,   102},
							 {&AutoResetEnable,   SKIP,   NORMAL, NONE,    AUTO_RESET_IDX,   102},
							 {&TimerFlag,         END,    END,    NONE,    NONE,              176}};

SSTATES CheckAutoReset[]= {  {&ResetEnable,       SKIP,   NORMAL, NONE,    RESET_ERROR_IDX,   102},
							 {&TimerFlag,         END,    END,    NONE,    NONE,              176}};

SSTATES CheckPreWait[]= {    
							 {&ResetEnable,       SKIP,   NORMAL, NONE,    RESET_ERROR_IDX,   102},
		                     {&IsInputNotPresent, SKIP,   NORMAL, NONE,    WAITING_IDX,       166},
							 {&TimerFlag,         END,    END,    NONE,    NONE,              176}};

SSTATES Nop[]           = {  {&TimerFlag,         END,    END,    NONE,    NONE,     		     177} };


/**************************State Machine for DCP*******************************************************/

/*                   Substate              Pass Code       Entry Code  		JUMP Code  		    ALJUMP Code      	State        */
/*                   Name                                                                                        		          */
/*                   --------              ----------      ----------  		---------  	    	----------       		------------ */
STATE TopOfList=
				          {Initial,     	     Nothing,       TopOfListEntry,    TopOfListExit,     Nothing,      		NOP_IDX           };

STATE PowerUpState=
				          {WaitForPowerUp,	     PowerUpPass,   PowerUpEntry,	   Nothing,           Nothing,      		POWERUP_IDX       };

STATE WaitState=
				          {WaitForInput,	     WaitPass,      WaitEntry,	      Nothing,           Nothing,      		WAITING_IDX       };

STATE ScanStartState=
				          {CheckScanStart,      Scan_Start,    ScanStartEntry,    ScanStartExit,     Nothing,      		SCAN_START_IDX    };

STATE ScanRunState=
			             {CheckScanRun,        Scan_Run,      ScanRunEntry,      Nothing,           ScanRunAlarmExit, 	SCAN_RUN_IDX      };

STATE ScanStopState=
			          	 {CheckScanStop,       Scan_Stop,     ScanStopEntry,     ScanStopExit,      Nothing,      		SCAN_STOP_IDX     };

STATE ScanCalculatingState=
			             {CheckScanCalculating,HS_Resonance,  ScanCalculateEntry,ScanCalculateExit, Nothing,      		SCAN_CALCULATING_IDX };

STATE ScanCompleteState=
			             {CheckScanCompleted,  Scan_Complete, ScanCompleteEntry, ScanCompleteExit,  Nothing,      		SCAN_COMPLETED_IDX };

STATE ScanAbortState=
			             {CheckScanAbort,      Scan_Abort,    ScanAbortEntry,    ScanAbortExit,     Nothing,      		SCAN_ABORT_IDX };

STATE TestStartState=
				          {CheckTestStart,      Test_start,    TestStartEntry,    Nothing,           Nothing,      		TEST_START_IDX    };

STATE SeekStartState=
				         {CheckSeekStart,       Seek_Start,    SeekStartEntry,    SeekStartExit,     Nothing,      		SEEK_START_IDX    };
STATE SeekRampState=
				         {CheckSeekRamp,        Seek_Ramp,     SeekRampEntry,     Nothing,           Nothing,				SEEK_RAMP_IDX     };
STATE SeekRunState=
			            {CheckSeekRun,         Seek_Run,      SeekRunEntry,      Nothing,           Nothing, 				SEEK_RUN_IDX      };
STATE SeekStopState=
				         {CheckSeekStop,        Seek_Stop,     SeekStopEntry,     SeekStopExit,      Nothing,      		SEEK_STOP_IDX     };
STATE WaitNoInputState=
			            {WaitForNoInput,       WaitNoInputPass,WaitNoInputEntry, Nothing,           Nothing,      		WAITNOINPUT_IDX };
STATE SeekWeldState=
			            {CheckSeekWeld,        Seek_Weld,     SeekWeldEntry,     Nothing,    	    Nothing,      		SEEK_WELD_IDX     };
STATE WeldStartState=
				         {CheckWeldStart,       Weld_Start,    WeldStartEntry,    WeldStartExit,     Nothing,      		WELD_START_IDX    };
STATE WeldRampState=
				         {CheckWeldRamp,        Weld_Ramp,     WeldRampEntry,     Nothing,           Nothing, 				WELD_RAMP_IDX     };
STATE WeldRunState=
			            {CheckWeldRun,         Weld_Run,      WeldRunEntry,      Nothing,           Nothing,      		WELD_RUN_IDX      };
STATE WeldStopState=
				         {CheckWeldStop,        Weld_Stop,     WeldStopEntry,     WeldStopExit,      Nothing,      		WELD_STOP_IDX     };
STATE WeldErrorState=
				         {CheckWeldError,       Weld_Error,    WeldErrorEntry,    WeldErrorExit,     Nothing,      		WELD_ERROR_IDX    };
STATE ResetErrorState=
				         {CheckResetError,      ResetPass,     ResetEntry, 	      ResetExit,         Nothing,      		RESET_ERROR_IDX   };
STATE WaitStopState=
				         {CheckWaitStop,        PS_Error,      WaitStopEntry, 	   Nothing,           Nothing,      		WAIT_STOP_IDX     };

STATE ManFWeldStartState=
				         {CheckManFWeldStart,   ManFWeld_Start, ManFWeldStartEntry, WeldStartExit, Nothing,     		   MANF_WELD_START_IDX    };
STATE ManFWeldRampState=
				         {CheckManFWeldRamp,    ManFWeld_Ramp,  WeldRampEntry,      Nothing,        Nothing, 			   MANF_WELD_RAMP_IDX     };
STATE ManFWeldRunSonicOnState=
			     {CheckManFWeldRunSonicOn,     ManFWeldSonicOn_Run,ManFWeldRunSonicOnEntry,Nothing,   Nothing,          MANF_WELD_RUNSONICON_IDX   };
STATE ManFWeldRunSonicOffState=
			    {CheckManFWeldRunSonicOff,     ManFWeldSonicOff_Run,ManFWeldRunSonicOffEntry,Nothing,  Nothing,         MANF_WELD_RUNSONICOFF_IDX  };

STATE ManFWeldStopState=
				         {CheckManFWeldStop,    ManFWeld_Stop,     WeldStopEntry,    WeldStopExit,  Nothing,      	      MANF_WELD_STOP_IDX     };

STATE ManFWeldErrorState=
				         {CheckManFWeldError,   ManFWeld_Error,    ManFWeldErrorEntry,WeldErrorExit,  Nothing,           MANF_WELD_ERROR_IDX    };

STATE ManFWaitStopState=
				         {CheckWaitStop,        ManFPS_Error,      ManFWaitStopEntry, 	 Nothing,        Nothing,           MANF_WAIT_STOP_IDX     };

STATE AutoResetState=
				         {CheckAutoReset,       AutoResetAlarm,    AutoResetEntry, 	   Nothing,           Nothing,      AUTO_RESET_IDX     };

STATE PreReadyState=
				         {CheckPreWait,         PreWait,           PreWaitEntry, 	     Nothing,           Nothing,      PREWAIT_IDX     };

STATE WeldRampEnergyState=
				         {CheckWeldEnerRamp,    Weld_EnerRamp, WeldRampEntry,     Nothing,           Nothing, 				WELD_RAMP_ENER_IDX     };

STATE WeldRunEnergyState=
			            {CheckWeldEnerRun,      Weld_EnerRun,      WeldRunEntry,      Nothing,           Nothing,      		WELD_RUN_ENER_IDX      };

STATE EndOfList=
				        { Nop, 					Nothing, 		  EndOfListEntrycode, Restart,          Nothing,            NOP_IDX           };
