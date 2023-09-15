/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateFunctions.h_v   1.17   29 Jul 2015 07:54:48   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/StateFunctions.h_v  $
 * 
 *    Rev 1.17   29 Jul 2015 07:54:48   RJamloki
 * Added ClearMemoryFrequency() function declaration
 * 
 *    Rev 1.16   04 Mar 2014 07:49:24   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.15   06 Nov 2013 06:20:30   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.14   25 Oct 2013 11:59:14   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.13   08 Jul 2013 08:55:14   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.12   24 May 2013 10:16:28   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.11   21 May 2013 12:25:58   amaurya
 * Code review fixes.
 * 
 *    Rev 1.10   07 May 2013 10:13:54   amaurya
 * Code review fixed.
 * 
 *    Rev 1.9   06 May 2013 09:19:10   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.8   30 Apr 2013 11:20:58   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.7   22 Apr 2013 11:32:16   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.6   10 Apr 2013 10:32:24   ygupta
 * Added and fixed EQ Alarm generation
 * 
 *    Rev 1.5   08 Apr 2013 08:25:30   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.4   29 Jan 2013 02:22:46   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.3   11 Jan 2013 02:53:34   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.2   03 Jan 2013 07:19:14   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.1   05 Nov 2012 09:13:12   amaurya
 * State machine changes.
 * 
 *    Rev 1.0   11 Oct 2012 17:13:06   tayars
 * Initial revision.

*/

#ifndef STATEFUNCTIONS_H_
#define STATEFUNCTIONS_H_
#include "DEF_DEF.H"

#define UPPERLIMIT_TIMEOUT        4000000    /* Max time in us between CycleStart & ULS    */
#define TRIGGER_TIMEOUT           10000000   /* Max time in us between ULS & trigger */
#define STARTSIGNAL_TIMEOUT		  4000000     /* Max time in us before EQ1 is raised after cycle if start signal active*/
#define SONICDELAY_TIMEOUTUS      10000000
#define MAX_RESETTIME		5000//in us. for at least this duration WC should keep sending reset to DCP on error reset

#define ABRAMPDOWNTIME   10
#define SCANTIMEOUT      60000000
#define POWERUPTIMEOUT   7000000//in us
#define CHECKEQ02_TIMEOUT 500000//in us
#define ABAMPLITUDE 5 //5%
#define ENERGYBRAKETIME_REDUCTION 3000//in us //Needed to compensate state machine switching time and  WC DCP communication delay

#define FIND_ALARM_TYPE(V)  (V & 0x000000FF)
#define FIND_ALARM_ID(V)    ((V & 0xFFFFFF00) >> 8)

enum SonicStopStates{
   NO_SONICS = 0,
   WELD_COMPLETE,
   SONICS_COMPLETE,
};

//*****************Function Prototypes************************/
void Restart(void);

void PowerUpEntry(void);
void PowerUpPass(void);
void PowerUpALExit(void);

void PreReadyEntry(void);
void PreReadyPassAct(void);
void PreReadyPassNoAct(void);
void PreReadyExit(void);

void ClearAlarmOutputs(void);
void SonicsEntryAct(void);
void SonicsEntryNoAct(void);
void SonicsPassAct(void);
void SonicsPassNoAct(void);

void ReadyEntry(void);
void ReadyPassACT(void);
void ReadyPassNoACT(void);
void ReadyExit(void);

void UpdatePresetEntry(void);
void UpdatePresetPassAct(void);
void UpdatePresetPassNoAct(void);
void UpdatePresetExit(void);

void WaitforULSTRSEntry(void);
void WaitforULSTRSPass(void);

void SonicsEntry(void);
void SonicsPassAct(void);
void SonicsPassNoAct(void);
void SonicsExit(void);

void EnergyBrakeEntry(void);
void EnergyBrakePassAct(void);
void EnergyBrakePassNoAct(void);
void EnergyBrakeExit(void);

void HoldOffTimeEntry(void);
void HoldOffTimePassAct(void);
void HoldOffTimePassNoAct(void);
void HoldOffTimeExit(void);

void CheckLimitsEntry(void);
void CheckLimitsPass(void);
void CheckLimitsExit(void);

void CycleAbortEntry(void);
void CycleAbortPass(void);
void CycleAbortExit(void);

void AfterBurstEntry(void);
void AfterBurstPass(void);
void AfterBurstExit(void);

void ErrorEntry(void);
void ErrorPass(void);
void ErrorExit(void);

void SeekEntry(void);
void SeekPass(void);
void SeekExit(void);

void TestEntry(void);
void TestPass(void);
void TestExit(void);

void ScanEntry(void);
void ScanPass(void);
void ScanExit(void);

void MfgCycleStartEntry(void);
void MfgCycleRunPass(void);
void MfgCycleExit(void);

void CheckForPresetChange(void);
void CheckForStepping(void);
BOOLEAN CheckForNoInput(void);
BOOLEAN CheckMemClearReset(void);
void CheckforRFSwitching(void);
RFSwitchingStatus SelectRFRelayFB(UINT16 StackNumber);
RFSwitchingStatus SelectRFRelayIO(UINT16 StackNumber);
void HandleRFSwitching(void);
void CheckForCustomCutoffSetup();
void ClearMemoryFrequency(void);
#endif /* STATEFUNCTIONS_H_ */
