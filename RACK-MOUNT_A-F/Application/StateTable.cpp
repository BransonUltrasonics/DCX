/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateTable.cpp_v   1.0   09 Jun 2015 09:10:48   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateTable.cpp_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.23   06 Nov 2013 06:20:32   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.22   25 Oct 2013 11:59:16   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.21   03 Sep 2013 11:14:08   amaurya
 * Tracker fixes and Synchronization fixes for User Outputs.
 * 
 *    Rev 1.20   30 Aug 2013 15:01:36   amaurya
 * Fixed tracker issue 521,525,526,527,529,531.
 * 
 *    Rev 1.19   14 Jun 2013 11:21:10   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.18   11 Jun 2013 09:51:48   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.17   22 May 2013 10:41:38   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 * 
 *    Rev 1.16   21 May 2013 12:26:00   amaurya
 * Code review fixes.
 * 
 *    Rev 1.15   06 May 2013 09:19:54   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.14   30 Apr 2013 11:21:22   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.13   11 Mar 2013 02:29:04   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.12   29 Jan 2013 02:23:26   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.11   22 Jan 2013 10:38:34   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.10   16 Jan 2013 06:32:00   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.9   11 Jan 2013 02:54:16   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.8   03 Jan 2013 07:41:24   ygupta
 * Binary Files
 * 
 *    Rev 1.7   03 Jan 2013 07:19:28   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.6   14 Dec 2012 05:28:26   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.5   13 Dec 2012 00:12:38   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.4   29 Nov 2012 08:48:02   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.3   24 Nov 2012 08:05:10   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.2   09 Nov 2012 00:36:10   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.1   05 Nov 2012 09:13:26   amaurya
 * State machine changes.
 * 
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.
 */

#include "StateTable.h"
#include "StateFunctions.h"
#include "portable.h"
#include "cpudefs.h"
#include "GLOBALS.h"

//***************************Local Variable pointers********************************/

/*************************   Sub State of State Machine for DCP   *************************/
/*            Variable                   TRUE        FALSE       Skip      */
/*            pointer                    Action      Action      Index     */
/*            ----------                 -------     -------     ------    */
SSTATES Initial[] = {
             {&SystemOK,                 SKIP,       NORMAL,     POWERUP_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES PowerUp[] = {
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&DCXFunc.SeekStart,    	  SKIP,       NORMAL,     SEEKSTATE_IDX},
             {&DCXFunc.ScanStart,    	  SKIP,       NORMAL,     SCANSTATE_IDX},
             {&PowerUpDone,              SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CheckPreReady[] = {
             {&ResetFlag,                SKIP,       NORMAL,     ERROR_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&PowerUpDone,              NORMAL,     SKIP,       POWERUP_IDX},
             {&ReadyOK,                  SKIP,       NORMAL,     READYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES Ready[] = {
			 {&DCPReadyFlag,                NORMAL,     SKIP,       ERROR_IDX},
			 {&ResetFlag,                   SKIP,       NORMAL,     PREREADYSTATE_IDX},
			 {&DigitalInputConfigured,		SKIP, 		NORMAL,		PREREADYSTATE_IDX},
             {&CycleAbortFlag,           SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&LoadPresetToDCP,          SKIP,       NORMAL,     PRESET_UPDATE_IDX},
             {&DCXFunc.CycleStart,		  SKIP,       NORMAL,     WAITULSTRS_IDX},
             {&DCXFunc.WeldStart, 		  SKIP,       NORMAL,     SONICS_START_IDX},
             {&DCXFunc.SeekStart,        SKIP,       NORMAL,     SEEKSTATE_IDX},
             {&DCXFunc.TestStart,        SKIP,       NORMAL,     TESTSTATE_IDX},
             {&DCXFunc.ScanStart,        SKIP,       NORMAL,     SCANSTATE_IDX},
             {&DCXFunc.MFCycleStart,     SKIP,       NORMAL,     MANF_CYCLE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CheckForUpdatePreset[] = {
             {&CycleAbortFlag,           SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&GotoWeldStart,            SKIP,       NORMAL,     SONICS_START_IDX},
             {&GotoCycleStart,           SKIP,       NORMAL,     WAITULSTRS_IDX},
             {&GotToSeek,                SKIP,       NORMAL,     SEEKSTATE_IDX},
             {&GoToTEST,                 SKIP,       NORMAL,     TESTSTATE_IDX},
             {&GoToScan,                 SKIP,       NORMAL,     SCANSTATE_IDX},
             {&DCXFunc.MFCycleStart,     SKIP,       NORMAL,     MANF_CYCLE_IDX},
             {&PresetUpdateDone ,        SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};


SSTATES WaitforULSandTRS[] = {
             {&CycleAbortFlag,           SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&GotoCycleStart,           SKIP,       NORMAL,     SONICS_START_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CheckForSonics[] = {
             {&PowerupCommunicationDone, NORMAL,     SKIP,       ERROR_IDX},
             {&CycleAbortFlag,           SKIP,       NORMAL,     CYCLE_ABORT_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     HOLD_OFF_TIME_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     CYCLE_ABORT_IDX},
             {&EnergyBrakeFlag,          SKIP,       NORMAL,     ENERGY_BRAKING_IDX},
             {&SonicsComplete,           SKIP,       NORMAL,     HOLD_OFF_TIME_IDX},
             {&WeldComplete,             SKIP,       NORMAL,     CHECK_LIMIT_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CheckForEB[] = {
             {&CycleAbortFlag,           SKIP,       NORMAL,     CYCLE_ABORT_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     HOLD_OFF_TIME_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&SonicsComplete,           SKIP,       NORMAL,     HOLD_OFF_TIME_IDX},
             {&WeldComplete,             SKIP,       NORMAL,     CHECK_LIMIT_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CheckForHT[] = {
             {&CycleAbortFlag,           SKIP,       NORMAL,     CYCLE_ABORT_IDX},
  //           {&SonicsAbortFlag,          SKIP,       NORMAL,     CHECK_LIMIT_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&TimeExpiredFlag,          SKIP,       NORMAL,     CHECK_LIMIT_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES ChkLimitSt[] = {
			 {&WarningFlag,				 SKIP,       NORMAL,     AFTER_BURST_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&AfterBurstFlag,           SKIP,       NORMAL,     AFTER_BURST_IDX},
             {&SonicsAbortFlag,          SKIP,       NORMAL,     ERROR_IDX},
             {&WeldStopFlag,             SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES AfterBurst[] = {
            {&PowerupCommunicationDone,  NORMAL,     SKIP,       ERROR_IDX},
            {&CycleAbortFlag,            SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&AlarmCycleAbort,          SKIP,       NORMAL,     ERROR_IDX},
             {&SonicsAbortFlag,			 SKIP,       NORMAL,     ERROR_IDX},
             {&AbCompleted,       		  SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES CycleAbortSt[] = {
             {&PowerupCommunicationDone, NORMAL,     SKIP,       ERROR_IDX},
             {&WeldStopFlag,             SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&WeldDoneWithAlarm,        SKIP,       NORMAL,     ERROR_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES ErrorSt[] = {
             {&ResetDone,                SKIP,       NORMAL,     PREREADYSTATE_IDX},
             {&DummyFlag,                END,        END,        NOP_IDX}};

SSTATES SeekSt[] = {
             {&PowerupCommunicationDone, NORMAL,    SKIP,        ERROR_IDX},
             //{&CycleAbortFlag,           SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&SeekAlarmFlag,            SKIP,      NORMAL,      ERROR_IDX},
             {&SeekDone,                 SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&DummyFlag,                END,       END,         NOP_IDX}};

SSTATES TestSt[] = {
             {&PowerupCommunicationDone, NORMAL,    SKIP,        ERROR_IDX},
             //{&CycleAbortFlag,           SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&TestAlarmFlag,            SKIP,      NORMAL,      ERROR_IDX},
             {&TestDone,                 SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&DummyFlag,                END,       END,         NOP_IDX}};

SSTATES ScanSt[] = {
             {&PowerupCommunicationDone, NORMAL,    SKIP,        ERROR_IDX},
             //{&CycleAbortFlag,           SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&SonicsAbortFlag,          SKIP,      NORMAL,      ERROR_IDX},
             {&ScanDone,                 SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&ScanAborted,              SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&DummyFlag,                END,       END,         NOP_IDX}};

SSTATES CheckManFCycle[] = {
             {&SonicsAbortFlag,          SKIP,      NORMAL,      ERROR_IDX},
             {&AlarmCycleAbort,          SKIP,      NORMAL,      ERROR_IDX},
             {&MFTimeoutEnable,          SKIP,      NORMAL,      PREREADYSTATE_IDX},
             {&DummyFlag,                END,       END,         NOP_IDX}};

SSTATES Nop[] =
            {{&DummyFlag,                END,       END,         NOP_IDX}};


/**************************State Machine for WC*******************************************************/
/*     Substate            Pass Code                     Entry Code  	    Exits Code  		   State        */
/*     Name                                                                             		          */
/*     --------            ----------                    ----------  	    ---------  	   	------------ */
STATE TopOfList =
       {Initial,              NULL,                      NULL,                NULL,             NOP_IDX };

STATE PowerUpState =
       {PowerUp,              PowerUpPass,               PowerUpEntry,        NULL,             POWERUP_IDX };

STATE PreReadyState =
       {CheckPreReady,        PreReadyPassFuncPtr,       PreReadyEntry,       PreReadyExit,             PREREADYSTATE_IDX };

STATE ReadyState =
       {Ready,                RdyPassFuncPtr,            ReadyEntry,          ReadyExit,        READYSTATE_IDX };

STATE HandleUpdatePreset =
       {CheckForUpdatePreset, UpdatePresetPassFuncPtr,   UpdatePresetEntry,   UpdatePresetExit, PRESET_UPDATE_IDX };

STATE WaitforULSTRS =
       {WaitforULSandTRS,     WaitforULSTRSPass,         WaitforULSTRSEntry,  NULL,             WAITULSTRS_IDX };

STATE SonicOn =
       {CheckForSonics,       SonicsPassFuncPtr,         SonicsEntryFuncPtr,  SonicsExit,       SONICS_START_IDX };

STATE EnergyBrakeState =
       {CheckForEB,           EnergyBrakePassFuncPtr,    EnergyBrakeEntry,    EnergyBrakeExit,  ENERGY_BRAKING_IDX };

STATE HoldOffTimeState =
       {CheckForHT,           HoldOffTimePassFuncPtr,    HoldOffTimeEntry,    HoldOffTimeExit,  HOLD_OFF_TIME_IDX };

STATE ChkLimitState =
       {ChkLimitSt,           CheckLimitsPass,           CheckLimitsEntry,    CheckLimitsExit,  CHECK_LIMIT_IDX };

STATE AfterBurstState =
       {AfterBurst,           AfterBurstPass,            AfterBurstEntry,     AfterBurstExit,    AFTER_BURST_IDX };

STATE CycleAbortState =
       {CycleAbortSt,          CycleAbortPass,             CycleAbortEntry,      CycleAbortExit,    CYCLE_ABORT_IDX };

STATE ErrorState =
       {ErrorSt,              ErrorPass,                 ErrorEntry,          ErrorExit,        ERROR_IDX };

STATE SeekState =
       {SeekSt,               SeekPass,                  SeekEntry,           SeekExit,         SEEKSTATE_IDX };

STATE TestState =
       {TestSt,               TestPass,                  TestEntry,           TestExit,         TESTSTATE_IDX };

STATE ScanState =
       {ScanSt,               ScanPass,                  ScanEntry,           ScanExit,         SCANSTATE_IDX };

STATE ManFCycleState =
       {CheckManFCycle,       MfgCycleRunPass,           MfgCycleStartEntry,  MfgCycleExit,     MANF_CYCLE_IDX };

STATE EndOfList =
       {Nop, 			         NULL, 		               NULL,                Restart,          NOP_IDX  };
