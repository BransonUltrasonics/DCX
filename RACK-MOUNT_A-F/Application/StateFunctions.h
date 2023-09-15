/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateFunctions.h_v   1.1   10 Aug 2015 09:41:26   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/StateFunctions.h_v  $
 * 
 *    Rev 1.1   10 Aug 2015 09:41:26   RJamloki
 * Added ClearMemoryFrequency() function declaration
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.16   04 Mar 2014 07:49:24   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
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
