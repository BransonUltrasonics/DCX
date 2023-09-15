/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FUNC.h_v   1.11   02 Jul 2014 09:15:40   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/FUNC.h_v  $
 * 
 *    Rev 1.11   02 Jul 2014 09:15:40   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.10   15 Jan 2013 04:39:58   amaurya
 * Energy Special.
 * 
 *    Rev 1.8.2.9   16 Sep 2011 09:46:06   RatneshA
 * counters get cleared from manufacturing page only and not from coldstart.
 * 
 *    Rev 1.8.2.8   16 Sep 2011 08:22:26   RatneshA
 * changed function name
 * 
 *    Rev 1.8.2.7   29 Aug 2011 11:37:48   ygupta
 * Ver1.01P. Implemented Shut Down Process.
 * 
 *    Rev 1.8.2.6   16 Aug 2011 08:23:10   pdwivedi
 * code cleanup
 * 
 *    Rev 1.8.2.5   05 Aug 2011 10:31:58   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.8.2.4   02 Aug 2011 07:37:16   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.8.2.3   22 Jul 2011 16:53:44   tayars
 * Added New Control Loops
 * 
 *    Rev 1.8.2.2   15 Jul 2011 09:45:14   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.8.2.1   13 Jun 2011 09:27:20   PDwivedi
 * Chnages due to frequency and power level combination.
 * 
 *    Rev 1.8.2.0   02 May 2011 09:27:08   ygupta
 * Manufacturing Bug fixes
 * 
 *    Rev 1.8   19 Apr 2011 10:47:04   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   04 Apr 2011 10:58:32   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.6   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.5   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:22   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:12   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:40:10   nhahmed
 * Added ColdStart and CheckforMI functions.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

******************************************************************************/

#ifndef FUNC_H_
#define FUNC_H_

#include "TYP_DEF.h"

UINT16 ByteCRC(UINT8 *Ptr, UINT32 Length);

/* DupsTask.cpp */
void PowerUpInitilization(void) ;
void Power_On_Copy(void);
void PowerOnCounterCheck(void);
void ResetCycleCounter(void);
void IncrementCycleCounter(void);
UINT32 ReadCycleCounter(void);
void ResetErrorCounter(void);
void ResetManFErrorCounter(void);
void IncrementErrorCounter(void);
UINT32 ReadErrorCounter(void);
UINT32 ReadManFErrorCounter(void);
void IncrementMinuteCounter(void);
void ResetHourCounter(void);
void IncrementHourCounter(void);
void WriteHourCounter(UINT32 HourCounter);
UINT32 ReadHourCounter(void);
UINT32 ReadPowerOnSecCounter(void);
UINT32 ReadSonicOnSecCounter(void);
void IncrementPowerOnSecCounter(void);
void IncrementSonicOnSecCounter();
void ResetPowerOnSecCounter(void);
void ResetSonicOnSecCounter(void);
void ResetCounters();
int CheckForMIBoard();
void InitMemFreqAndTuneFreqAtPowerUp();
void CheckDCPCounterCRC();
void SetFPGAFrequency();
SINT32 ConvertFreqtoDCPformat(SINT32 freqIn);
void VerifyPresetAtPowerUp();
BOOLEAN CheckForVersionChange();
void CheckHWConfiguration();
void ReadFPGAVersion();
void CheckForFPGAVersionChange();

/* seek.c */
void Seek_Start(void);
void Seek_Ramp(void);
void Seek_Run(void);
void Seek_Stop(void);
void Fix_DDSLimits(void);
void Clear_Seek_Status(void);
void Set_Seek_Status(void);
void Set_Seek_Status_Error(void);
void CheckAndAssignSeekCycleBoundaryValue();

/* weld.c */
void Cycle_Run(void);				//ab v191, 977us->1ms, dazu
void Weld_Error(void);
void ManFWeld_Error(void);
void ManFPS_Error(void);
void PS_Error(void);
SINT16 Control(SINT32 Flag);
void InitializeWeldbuffer(void);
void Reset_WeldBuffer(void);
void IncWeldbufferIndex(void);
void Weld_Start(void);
void Weld_Ramp(void);
void Weld_Run(void);
void Weld_Stop(void);
void Seek_Weld(void);
void Clear_Weld_Status(void);
void Set_Weld_Status(void);
void Set_Weld_Status_Error(void);
SINT32 ReadWeldBufferLog(SINT32 nLogs);
void AmplitudeControl(void);
void AmplitudeControlTrue(void);
void FrequencyControl(SINT32 Flag);
void FrequencyControlTrue(SINT32 Flag);
void CheckAndAssignWeldCycleBoundaryValue();
void Weld_EnerRamp(void);
void Weld_EnerRun(void);
/* scan.c */
void Scan_Start(void);
void Scan_Run(void);
void Scan_Stop(void);
void InitializeScanbuffer(void);
void Reset_Scanbuffer(void);
BOOLEAN IncScanBufferIndex(void);
void Detect_Resonance(void);
void CheckScanParamCRC();
void SaveScanParamCRC();
void SetMinMaxScanParameter();
BOOLEAN VerifyScanParam();
void CheckAndAssignScanCycleBoundaryValue();
/* ScanResDetect.c */
UINT16 HS_Decimate( SINT16 * phase_in, UINT16 count,SINT16 * phase_out);
void HS_Resonance(void);
void HS_4Clusters(void);
UINT16 HS_X4YZero(UINT16 xcap[], SINT16 ycap[],UINT16 vectsize);
void HS_FirFilt( SINT16 vector[], UINT16 vectsize);
BOOLEAN HS_CrossingExists (SINT16 vector[], SINT16 vectsize);
void Scan_Abort();
void Scan_Complete();

/*StateMachineDrv.c*/
void StartSonics();
void FPGAOverloadReset();
void SonicsOff();
void StateMachine();

/*****preset.c*********/
BOOLEAN SavePreset(SINT32 PresetNum);
BOOLEAN RecallPreset(SINT32 PresetNum);
void RecallAndVerifyPreset (UINT32 PresetNum);
BOOLEAN VerifyPreset();
void UpdateRunningPreset();
void SetDefaultValues(void);
void ColdStart(void);
void SaveCurrentPresetCRC();
void CheckCurrentPresetCRC();
void SetDefaultScanParameters(void);
void SetMaxMinValues(void);
void UpdateMidBandAndFFactor();
void SetDefaultWeldPreset(BOOLEAN advancedPreset);
extern void MSTickFunc();

//system setup
void SetMinMaxCycleParam();
void SetDefaultCycleParam();
void IncrementManFErrorCycleCounter(void);
void ManFWeld_Start(void);
void ManFWeld_Ramp(void);
void ManFWeldSonicOn_Run(void);
void ManFWeldSonicOff_Run(void);
void ManFWeld_Stop(void);
void WriteAlarmLog();
void CheckForPowerDown();
BOOLEAN CheckForInputs();
void SetManFDefaultFromWeb();
void Set_ManF_Weld_Status(void);

void SetDefaultEEPROMData();
void ModifyEEPROMdata();
void ReadMACAddressFromEEPRom(UINT8 * macPtr);
void CheckManFCycleParamCRC(void);
//int GetPowerSupplySpecification();
void SetDHCPServerIPDefaults();
void SetStaticIPDefaults();
#endif /* FUNC_H_ */
