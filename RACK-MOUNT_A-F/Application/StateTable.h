/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateTable.h_v   1.0   09 Jun 2015 09:10:48   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateTable.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.11   25 Oct 2013 11:59:16   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.10   30 Aug 2013 15:01:36   amaurya
 * Fixed tracker issue 521,525,526,527,529,531.
 * 
 *    Rev 1.9   21 May 2013 12:26:00   amaurya
 * Code review fixes.
 * 
 *    Rev 1.8   07 May 2013 10:14:36   amaurya
 * Code review fixed.
 * 
 *    Rev 1.7   30 Apr 2013 11:21:40   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.6   29 Jan 2013 02:23:38   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.5   16 Jan 2013 06:29:40   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.4   11 Jan 2013 02:54:30   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.3   14 Dec 2012 05:28:40   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.2   29 Nov 2012 08:48:16   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.1   05 Nov 2012 09:13:42   amaurya
 * State machine changes.
 *
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.

*/
#ifndef STATETABLE_H_
#define STATETABLE_H_

#include "StateMachineDrv.h"
/*-------------------------- External Variables ---------------------------*/
extern STATE TopOfList;
extern STATE PowerUpState;
extern STATE PreReadyState;
extern STATE ReadyState;
extern STATE SonicOn;
extern STATE HandleUpdatePreset;
extern STATE WaitforULSTRS;

/*for time, energy and peak mode*/
extern STATE EnergyBrakeState;
extern STATE HoldOffTimeState;
extern STATE ChkLimitState;
extern STATE AfterBurstState;
extern STATE CycleAbortState;
/*******************************/
extern STATE ErrorState;
extern STATE SeekState;
extern STATE TestState;
extern STATE ScanState;
extern STATE ManFCycleState;
extern STATE EndOfList;

/*for time, energy and peak mode*/


//***********************External Flags**************************/
extern BOOLEAN AlarmCycleAbort, SonicsAbortFlag, WarningFlag;
extern BOOLEAN DCPReadyFlag;
extern BOOLEAN WeldStopFlag, ResetFlag;
extern BOOLEAN CycleAbortFlag;
extern BOOLEAN TimeExpiredFlag, EnergyBrakeFlag, AfterBurstFlag;
extern BOOLEAN DummyFlag;
extern BOOLEAN MFTimeoutEnable;
extern BOOLEAN ScanDone, ScanAborted;
extern BOOLEAN GotoCycleStart , GotoWeldStart, GotToSeek , GoToTEST,
        GoToScan;
extern BOOLEAN AbCompleted;
extern BOOLEAN LoadPresetToDCP;
extern BOOLEAN PowerUpDone;
extern BOOLEAN PresetUpdateDone;
extern BOOLEAN ReadyOK;
extern BOOLEAN ResetDone;
extern BOOLEAN WeldStopFlag, SonicsComplete, WeldComplete;
extern BOOLEAN SeekStarted, SeekDone, SeekAlarmFlag;
extern BOOLEAN TestStarted, TestDone, TestAlarmFlag;
extern BOOLEAN StartTest, StartScan;
extern BOOLEAN SystemOK;
extern BOOLEAN WeldDoneWithAlarm;

//FB
extern BOOLEAN CheckForFBLoadNewPreset;
extern UINT8   FBLoadPresetNumber;
extern BOOLEAN FBWeldFunction , FBHornFunction;
#endif /* STATETABLE_H_ */
