/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FUNC.h_v   1.7   06 Jun 2016 14:03:28   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/FUNC.h_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added CheckForInputsSIG() function declaration
 *
 *    Rev 1.7   06 Jun 2016 14:03:28   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.6   01 Sep 2015 02:32:40   amaurya
 * Added declaration for FramCheck().
 * 
 *    Rev 1.5   09 Apr 2015 05:14:02   sesharma
 * SetDefaultWeldPreset() Function decalartion modified.
 * 
 *    Rev 1.4   19 Mar 2015 06:46:16   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   26 Feb 2015 08:25:38   sesharma
 * Impulse Start should be restored to default only when system type is Modified or at Cold start.
 * 
 *    Rev 1.2   27 Nov 2014 09:13:22   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.1   17 Sep 2014 08:49:46   amaurya
 * Removed CheckForMIBoard()
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

#ifndef FUNC_H_
#define FUNC_H_

#include "TYP_DEF.h"

UINT16 ByteCRC(UINT8 *Ptr, UINT32 Length);

/* DupsTask.cpp */
void PowerUpInitilization(void);
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
void InitMemOffsetAtPowerUp();
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
UINT16 HS_Decimate(SINT16 * phase_in, UINT16 count, SINT16 * phase_out);
void HS_Resonance(void);
void HS_4Clusters(void);
UINT16 HS_X4YZero(UINT16 xcap[], SINT16 ycap[], UINT16 vectsize);
void HS_FirFilt(SINT16 vector[], UINT16 vectsize);
BOOLEAN HS_CrossingExists(SINT16 vector[], SINT16 vectsize);
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
void RecallAndVerifyPreset(UINT32 PresetNum);
BOOLEAN VerifyPreset();
void UpdateRunningPreset();
void SetDefaultValues(void);
void ColdStart(void);
void SaveCurrentPresetCRC();
void CheckCurrentPresetCRC();
void SetDefaultScanParameters(void);
void SetMaxMinValues(void);
void UpdateMidBandAndFFactor();
void SetDefaultWeldPreset(void);
void SetDefaultAdvancedPreset(void);
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
BOOLEAN CheckForInputsSIG();
void SetManFDefaultFromWeb();
void Set_ManF_Weld_Status(void);

void SetDefaultEEPROMData();
void ModifyEEPROMdata();
void ReadMACAddressFromEEPRom(UINT8 * MacPtr);
void CheckManFCycleParamCRC(void);
//int GetPowerSupplySpecification();
void SetDHCPServerIPDefaults();
void SetStaticIPDefaults();
void FramCheck(void);
#endif /* FUNC_H_ */
