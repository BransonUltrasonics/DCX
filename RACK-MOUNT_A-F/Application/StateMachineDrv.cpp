/*
 $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateMachineDrv.cpp_v   1.0   09 Jun 2015 09:10:48   tayars  $
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
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateMachineDrv.cpp_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.25   06 Nov 2013 06:20:30   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.24   25 Oct 2013 11:59:16   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.23   08 Oct 2013 06:17:30   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.22   21 May 2013 12:25:58   amaurya
 * Code review fixes.
 * 
 *    Rev 1.21   07 May 2013 10:14:06   amaurya
 * Code review fixed.
 * 
 *    Rev 1.20   06 May 2013 09:19:24   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.19   18 Feb 2013 03:10:30   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.18   29 Jan 2013 02:22:58   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.17   11 Jan 2013 02:53:48   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.16   29 Nov 2012 08:47:34   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.15   24 Nov 2012 08:04:58   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.14   09 Nov 2012 00:35:42   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.13   08 Nov 2012 01:20:42   rjamloki
 * Website changes
 * 
 *    Rev 1.12   05 Nov 2012 08:59:20   amaurya
 * State Machine changes
 *
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.

 */

#include "StateMachineDrv.h"
#include "stdio.h"
#include "FUNC.h"
#include "Globals.h"
#include "UserIO.h"
#include "LOG.h"
#include "ModBusRcvTask.h"
#include "preset.h"
#include "NRTModbusTask.h"
#include "StateTable.h"

#define pprintf
//#define printf
extern volatile UINT32 s_ulTime;
/* Current sequence type */
enum WELDSTATES_IDX   WeldState;    /* Current weld state */

/*-------------------------- PRIVATE GLOBAL DATA --------------------------*/
static UINT16  StateMachineFlag = STOPPED;	/* Current StateMachineFlag        */
static STATE   * CurrentPtr;  				/* Current state pointer           */
static SSTATES * SSptr; 					/* Current sub state pointer       */
SINT16 CurrentIndex; 			/* Index to current state pointer */
UINT32 SonicsOffPeriod;         /* Used to handle RF Switching. */

STATE * WCSequence[MAX_STATE + 1]; /* Current weld cycle definition */
/************** Functions Declaration **************/
void SelectWeldSequence(void);

//#define SM_DEBUG

const DebugStateStruct DBGS[] =
{
   {NOP_IDX, 	"NOP_IDX"},  //0
   {POWERUP_IDX, "POWERUP_IDX"},
   {PREREADYSTATE_IDX,  "PREREADYSTATE_IDX"},
   {READYSTATE_IDX, "READYSTATE_IDX"},
   {PRESET_UPDATE_IDX, "PRESET_UPDATE_IDX"},
   {WAITULSTRS_IDX, "WAITULSTRS_IDX"}, //5
   {SONICS_START_IDX, "SONICS START IDX"},
   {ENERGY_BRAKING_IDX, "ENERGY_BRAKING_IDX"},
   {HOLD_OFF_TIME_IDX, "HOLD_OFF_TIME_IDX"},
   {CHECK_LIMIT_IDX, "CHECK_LIMIT_IDX"},
   {AFTER_BURST_IDX, "AFTER_BURST_IDX"},  //10
   {CYCLE_ABORT_IDX, "CYCLE_ABORT_IDX"},
   {SEEKSTATE_IDX, "SEEKSTATE_IDX"},
   {TESTSTATE_IDX, "TESTSTATE_IDX"},
   {SCANSTATE_IDX, "SCANSTATE_IDX"},
   {MANF_CYCLE_IDX, "MANF_CYCLE_IDX"}, //15
   {ERROR_IDX, "ERROR_IDX"},//16
   {MAX_STATE, "MAX_STATE"},
};


/* void PrintStateName(void)
 *
 *  Purpose:
 *    This function is used to print current of state machine.
 *    It is here only for debug purpose and called from input
 *    task whenever required.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void PrintStateName(void)
{
   for(int i = 0; i < MAX_STATE; i++){
      if(DBGS[i].Sidx == CurrentIndex){
         printf("\n %s ", DBGS[i].StateName);
      }
   }
}
#ifdef SM_DEBUG
void PrintStateName(SINT32 Indx)
{
	printf("\n %s ", DBGS[Indx].StateName);
}
#endif
/* void StartStateMachine(void)
 *
 *  Purpose:
 *    This function makes StateMachineFlag TRUE to start State machine.
 *    It is called by SelectWCSequence().
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void StartStateMachine(void)
{
   StateMachineFlag = RUNNING;
}

/* void ResetStateMachine(void)
 *
 *  Purpose:
 *    This function is used to restart StateMachine.
 *    It is called by SelectWCSequence().
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void ResetStateMachine(void)
{
   CurrentPtr = 0;
   CurrentIndex = (SINT16)NOP_IDX;
   StateMachineFlag = STOPPED;
}

/* void SelectWCSequence(void)
 *
 *  Purpose:
 *    This function is used to start state machine.
 *    This function is called by WCTask::Run()
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void SelectWCSequence(void)
{
   ResetStateMachine(); 	/* Turn off the state machine first    */
   SelectWeldSequence();
   StartStateMachine(); 	/* Start the state machine here */
}

/* void SelectWeldSequence(void)
 *
 *  Purpose:
 *    This function is used to Prepare the Array of all possible
 *    states which will be used to do all weld and horn functions.
 *    WCSequence is filled with the addresses of all the States here
 *    and State machine will use these states based on inputs from
 *    USer I/O, webpages, field buses and LCD.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void SelectWeldSequence(void)
{
   WCSequence[NOP_IDX] = &TopOfList;
   WCSequence[POWERUP_IDX] = &PowerUpState;
   WCSequence[PREREADYSTATE_IDX] = &PreReadyState;
   WCSequence[READYSTATE_IDX] = &ReadyState;
   WCSequence[PRESET_UPDATE_IDX] = &HandleUpdatePreset;
   WCSequence[WAITULSTRS_IDX] = &WaitforULSTRS;
    /*------  Here we turn on some sonics  ------*/
   WCSequence[SONICS_START_IDX] = &SonicOn;
   WCSequence[ENERGY_BRAKING_IDX] = &EnergyBrakeState;
   WCSequence[HOLD_OFF_TIME_IDX] = &HoldOffTimeState;	//either hold time state or off time state
   WCSequence[CHECK_LIMIT_IDX] = &ChkLimitState;
   WCSequence[AFTER_BURST_IDX] = &AfterBurstState;
   WCSequence[CYCLE_ABORT_IDX] = &CycleAbortState;
   WCSequence[ERROR_IDX] = &ErrorState;
   WCSequence[SEEKSTATE_IDX] = &SeekState;
   WCSequence[TESTSTATE_IDX] = &TestState;
   WCSequence[SCANSTATE_IDX] = &ScanState;
   WCSequence[MANF_CYCLE_IDX] = &ManFCycleState;
   WCSequence[MAX_STATE] = &EndOfList;
}

/* void Restart(void)
 *
 *  Purpose:
 *     This function Stops the State machine and Resets the State Machine to initial
 *     state. This is the end state of state machine.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void Restart(void)
{
   CurrentIndex = (SINT16)NOP_IDX;
}

/* void StateMachine(void)
 *
 *  Purpose:
 *    This function is called every WC Interrupt time from
 *    WCTask::CycleRun(void) function. This is the main driver function
 *    for WC State Machine.
 *
 *  Entry condition:
 *  None.
 *
 *  Exit condition:
 *   None.
 */
void StateMachine(void)
{
   BOOLEAN EndFlag = FALSE;
   ACTIONS Cond;
#ifdef SM_DEBUG
   static SINT16 LastIndx = -1;
#endif
   // Check state machine Flag
   if(StateMachineFlag == RUNNING){
      CurrentPtr = WCSequence[CurrentIndex];
      SSptr = CurrentPtr -> ptr; /* Get sub state pointer      */
      if(CurrentPtr -> PassPtr)
    	 CurrentPtr -> PassPtr();   /* Execute this pass code     */
      while(EndFlag == FALSE){
#ifdef SM_DEBUG
		   if(LastIndx != CurrentIndex){
			   LastIndx = CurrentIndex;
			   PrintStateName(CurrentIndex);
		   }
#endif
		 if((*(SSptr -> ValuePtr)) == FALSE)     /* Test the value          */
		    Cond = SSptr -> False;  /* Was FALSE, use FALSE cmd */
		 else
			Cond = SSptr -> True;   /* Was TRUE, use TRUE cmd   */

         switch(Cond){
		    case NORMAL:
			   SSptr++; 			/* Move to Next Sub State   */
			break;
		    case SKIP:
		      if(CurrentPtr -> ExitPtr)
		          CurrentPtr -> ExitPtr(); 			/* Execute exit code       */
			   CurrentIndex = SSptr -> SkipIndex; 		/* Skip to next state      */
			   CurrentPtr = WCSequence[CurrentIndex];
			   if(CurrentPtr -> state != NOP_IDX)
			      WeldState = CurrentPtr -> state; 	/* Update state variable   */
			   if(CurrentPtr->EntryPtr)
			      CurrentPtr->EntryPtr(); /* Execute new entry code                  */

			   EndFlag = TRUE;
            break;

		    default:
			   EndFlag = TRUE;
			break;
         }//switch(Cond)
      }//while(EndFlag == FALSE)
   }//if(StateMachineFlag == RUNNING)
   SonicsOffPeriod++;
}
