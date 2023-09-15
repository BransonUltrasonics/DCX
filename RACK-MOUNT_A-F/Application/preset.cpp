/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/preset.cpp_v   1.3   03 Feb 2016 00:58:06   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/preset.cpp_v  $
 * 
 *    Rev 1.3   03 Feb 2016 00:58:06   ygupta
 * RecallPreset(), RecallAndVerifyPreset(), SetColdStartPresetValues() updated to hold appropriate value for recalled preset number parameter.
 * 
 *    Rev 1.2   10 Aug 2015 07:12:14   sesharma
 * Added defaults for clear memory before seek, clear memory with reset, set with horn scan and clear memory at power up options.
 * 
 *    Rev 1.1   08 Jul 2015 02:41:56   sesharma
 * Added ClearMemoryFrequency() to clear Stored memory frequency in all Horn Presets at power up.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:52   tayars
 * Initial revision.
 * 
 *    Rev 1.55   24 Mar 2014 14:12:46   rjamloki
 * Setting Memory offset to zero on Setup Weld page restore default

 */

#include "preset.h"
#include "VerifyPreset.h"
#include "FUNC.h"
#include "GLOBALS.h"
#include "SCAN.h"
#include "ProfiBus.h"
#include "ReceiveCommandHandler.h"
#include "DcxManagerHandler.h"
extern UINT32 ActPresetCycleCount;
#define pprintf
#define printf

#define VERIFY_PRESET_RANGE(PresetNum, Psstruct, Param, MinPs, MaxPs) \
((ParameterSet[PresetNum].Psstruct.Param < ParameterSet[MinPs].Psstruct.Param) || \
(ParameterSet[PresetNum].Psstruct.Param > ParameterSet[MaxPs].Psstruct.Param))

#define VERIFY_PRESET_RANGE_VAL(PresetNum, Psstruct, Param, MinVal, MaxVal) \
((ParameterSet[PresetNum].Psstruct.Param < MinVal) || \
(ParameterSet[PresetNum].Psstruct.Param > MaxVal))

#define VERIFY_PRESET_RANGE_HSPS(PresetNum, Hspsstruct, Param, MinPs, MaxPs) \
((HsParameterSet[PresetNum].Hspsstruct.Param < HsParameterSet[MinPs].Hspsstruct.Param) || \
 (HsParameterSet[PresetNum].Hspsstruct.Param > HsParameterSet[MaxPs].Hspsstruct.Param))


DcxPreset RunningPreset;
HSPRESET RunningHSPreset;
DCPPreset DcpParameterSet;
BOOLEAN UpdateRPreset;
SINT32 PresetVerifyError;

extern UINT32 ScanTime, SeekTime, TestTime;
extern BOOLEAN RegUpdateCmdFlag;
extern  DcpWeldResult WeldHistoryData;

HSSTATUS HsStatusSet[MAX_HSSTATUS_SET + 1][MAX_HS_FUNC];
//WSTATUS * WeldStatusSet = new WSTATUS[MAX_STATUS_SET + 1];

WSTATUS WeldStatusSet[MAX_STATUS_SET + 1];
//Array to hold Supply freq dependent preset Parameter's Min Max and Default values.
SINT32 FreqDependentParam[TOTALFREQPARAM][TOTALVALUES];

BOOLEAN SendToDCXM[MAX_TOTAL_PRESETS];

/*  BOOLEAN SavePreset(PRESET_NUMBER PresetNum, HSPRESET_NUMBER HspsNum)
 *
 *  Purpose:
 *	   This function is called to save the current preset into one of the slot in ParameterSet array.
 *	   This function is called by website and fieldbus module.
 *
 *  Entry condition:
 *	   PresetNum:The slot where current preset should be saved. range(preSet_1 to PreSet_TweakMAX)
 *     HspsNum: The slot where cuurent HSPS should be saved in HsParameterSet array.
 *
 *  Exit condition:
 *	   If the preset is saved returns TRUE else returns FALSE.
 */
BOOLEAN SavePreset(PRESET_NUMBER PresetNum, HSPRESET_NUMBER HspsNum)
{
	BOOLEAN RetFlag = FALSE;
	RTCTime Rt;
	UINT16 HsPsInCurrPs = ParameterSet[CurrPreset].InfoParam.HornPresetNumber;
	RTClock::GetCurrentTime(&Rt);
	if((PresetNum < MAX_TOTAL_PRESETS) && (PresetNum > CurrPreset)
	      && (HspsNum < MAX_TOTAL_HSPRESETS) && (HspsNum > CurrHsPreset)){
		ParameterSet[CurrPreset].InfoParam.HornPresetNumber = HspsNum;
		ParameterSet[CurrPreset].InfoParam.ModifiedDate = Rt.Day;
		ParameterSet[CurrPreset].InfoParam.ModifiedHour = Rt.Hour;
		ParameterSet[CurrPreset].InfoParam.ModifiedMinute = Rt.Minute;
		ParameterSet[CurrPreset].InfoParam.ModifiedMonth = Rt.Century_Month;
		ParameterSet[CurrPreset].InfoParam.ModifiedYear = Rt.Years;
		CalculateAndSavePresetCRC(SAVECRC_ALL, CurrPreset , CurrHsPreset, FALSE);
		ParameterSet[PresetNum] = ParameterSet[CurrPreset];
		HsParameterSet[ParameterSet[PresetNum].InfoParam.HornPresetNumber] = HsParameterSet[CurrHsPreset];
		ParameterSet[CurrPreset].InfoParam.HornPresetNumber = HsPsInCurrPs;
		CalculateAndSavePresetCRC(SAVECRC_INFO, CurrPreset);
		RetFlag = TRUE;
	}
//	Sysconfig->PresetChanged = FALSE;
	return RetFlag;
}

/*  BOOLEAN RecallPreset(PRESET_NUMBER PresetNum)
 *
 * 	Purpose:
 *	   This function is called to recall one of the presets in ParameterSet array
 *	   to currentpreset.
 *	   This function is called by website and fieldbus module.
 *
 *   Entry condition:
 *		presetNum- The slot in ParameterSet array from where the preset is to be recalled.range
 *		(Preset_1 to Preset_Default)
 *
 *   Exit condition:
 *		Returns TRUE if the requested Preset is recalled else returns FALSE.
 */
BOOLEAN RecallPreset(PRESET_NUMBER PresetNum)
{
	BOOLEAN RetFlag = FALSE;
	if((PresetNum < MAX_TOTAL_PRESETS) && (PresetNum > CurrPreset)){
		ParameterSet[CurrPreset] = ParameterSet[PresetNum];
		HsParameterSet[CurrHsPreset] = HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber];
		Sysconfig->ActivePreset = PresetNum;
		Sysconfig->RecalledPreset = PresetNum;
		ActPresetCycleCount = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;
		Sysconfig->PresetChanged = FALSE;
		RetFlag = CheckPresetCRC(CurrPreset);
		UpdateRPreset = TRUE;
		RegUpdateCmdFlag = TRUE;
	}
	return RetFlag;
}

/*  BOOLEAN CheckPresetCRC(void)
 *
 *  Purpose:
 *		This function is called from RecallPreset and RecallAndVerifyPreset
 *		function. This function checks the current Preset CRC. If CRC
 *		is not correct the current Preset is assigned the default values.
 *
 *  Entry condition:
 *  PresetNum: The slot in ParameterSet array for which CRC is to be calculated.
 *    None.
 *
 *  Exit condition:
 *    CRC check failed or passed.
 */
BOOLEAN CheckPresetCRC(PRESET_NUMBER PresetNum)
{
	BOOLEAN CrcOK = TRUE;
	UINT32 TempCRC;
//	pprintf("\n preset %d ", PresetNum);
	TempCRC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].ActParam), sizeof(ACTDCXPS));
	if(TempCRC != ParameterSet[PresetNum].CRCAct){
//		pprintf("\nCRCAct");
		CrcOK = FALSE;
	}
	TempCRC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].DcpParam), sizeof(DCPDCXPS));
	if(TempCRC != ParameterSet[PresetNum].CRCDcp){
//      pprintf("\nCRCDcp");
      CrcOK = FALSE;
   }
	TempCRC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].InfoParam), sizeof(INFODCXPS));
	if(TempCRC != ParameterSet[PresetNum].CRCInfo){
      //pprintf("\nCRCInfo");
      CrcOK = FALSE;
   }
	TempCRC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].LimitsParam), sizeof(LIMITSDCXPS));
	if(TempCRC != ParameterSet[PresetNum].CRCLimits){
//      pprintf("\nCRCLimits");
      CrcOK = FALSE;
   }
	TempCRC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].WcParam), sizeof(WCDCXPS));
	if(TempCRC != ParameterSet[PresetNum].CRCWC){
//      pprintf("\nCRCWC");
      CrcOK = FALSE;
   }

	if (PresetNum != CurrPreset) {
		if ((ParameterSet[PresetNum].InfoParam.HornPresetNumber	>= HORNPRESETNUMBER_MIN)
				&& ((ParameterSet[PresetNum].InfoParam.HornPresetNumber	<= HORNPRESETNUMBER_MAX))) {
			TempCRC	= ByteCRC((UINT8 *) (&HsParameterSet[ParameterSet[PresetNum].InfoParam.HornPresetNumber]),
					HSPSCRCDATA_SIZE);
			if (TempCRC != HsParameterSet[ParameterSet[PresetNum].InfoParam.HornPresetNumber].CRChsp) {
//				pprintf("\nCRChsp");
				CrcOK = FALSE;
			}
		}
		else
			CrcOK = FALSE;
	}
	else {
		TempCRC = ByteCRC((UINT8 *) (&HsParameterSet[CurrHsPreset]), HSPSCRCDATA_SIZE);
		if (TempCRC != HsParameterSet[CurrHsPreset].CRChsp) {
//			pprintf("\nCRChsp 2");
			CrcOK = FALSE;
		}
	}
	if (CrcOK == FALSE) {
//		pprintf("\n Set Default values");
		if (PresetNum == CurrPreset)
			SetDefaultValues();
		Log::WriteEventLog(EVENT_PRESETCRC_ERROR, Sysconfig->ActivePreset, PresetNum);
	}
	return CrcOK;
}

/*  BOOLEAN RecallAndVerifyPreset(PRESET_NUMBER PresetNum)
 *
 *  Purpose:
 *		This function is called to recall one of the Presets in ParameterSet array
 *		to current Preset and verify it at the same time.
 *		This function is called by website and fieldbus module.
 *
 *  Entry condition:
 *		PresetNum- The slot in ParameterSet array from where the Preset is to be recalled.range
 *		(Preset_1 to Preset_Default)
 *
 *  Exit condition:
 *		Returns TRUE if the requested Preset is recalled and verified else returns FALSE.
 */
BOOLEAN RecallAndVerifyPreset(PRESET_NUMBER PresetNum)
{
	BOOLEAN RetFlag = FALSE;
	if((PresetNum < MAX_TOTAL_PRESETS) && (PresetNum > CurrPreset)){
		ParameterSet[CurrPreset] = ParameterSet[PresetNum];
		HsParameterSet[CurrHsPreset] = HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber];
		RetFlag = CheckPresetCRC(CurrPreset);
		RunningPreset = ParameterSet[PresetNum];
		RunningHSPreset = HsParameterSet[CurrHsPreset];
		Sysconfig->ActivePreset = PresetNum;
		Sysconfig->RecalledPreset = PresetNum;
		ActPresetCycleCount = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;
		Sysconfig->PresetChanged = FALSE;
		if((RetFlag == TRUE)/* && (VerifyPreset() == TRUE)*/){
		   ParameterSet[CurrPreset].InfoParam.PresetStatus = VERIFIED;
			RunningPreset.InfoParam.PresetStatus = VERIFIED;
			RetFlag = TRUE;
		}
		else{
		   RetFlag = FALSE;
			ParameterSet[CurrPreset].InfoParam.PresetStatus = NOTVERIFIED;
			RunningPreset.InfoParam.PresetStatus = NOTVERIFIED;
		}
		RegUpdateCmdFlag = TRUE;
		CalculateAndSavePresetCRC(SAVECRC_ALL, CurrPreset , CurrHsPreset, FALSE);
		ParameterSet[PresetNum] = ParameterSet[CurrPreset];
		RunningPreset = ParameterSet[PresetNum];
		RunningHSPreset = HsParameterSet[CurrHsPreset];
		PrepareDCPPreset();
	}
	if(RetFlag == FALSE)
	   UpdateRPreset = TRUE;
	return RetFlag;
}

/*  void SetDefaultCutOffValues(void)
 *
 *  Purpose:
 *     This function is used to set default values for cut-off parameters.This function is used by website module.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultCutOffValues(void)
{
    ParameterSet[CurrPreset].WcParam.CutOffEnabled = ParameterSet[Preset_Default].WcParam.CutOffEnabled;
    ParameterSet[CurrPreset].WcParam.EnergyCutoffFlag = ParameterSet[Preset_Default].WcParam.EnergyCutoffFlag;
    ParameterSet[CurrPreset].WcParam.PeakPowerCutoffFlag = ParameterSet[Preset_Default].WcParam.PeakPowerCutoffFlag;
    ParameterSet[CurrPreset].WcParam.TimeCutOffFlag = ParameterSet[Preset_Default].WcParam.TimeCutOffFlag;
    ParameterSet[CurrPreset].ActParam.CustomAIn1CutoffFlag = ParameterSet[Preset_Default].ActParam.CustomAIn1CutoffFlag;
    ParameterSet[CurrPreset].ActParam.CustomAIn2CutoffFlag = ParameterSet[Preset_Default].ActParam.CustomAIn2CutoffFlag;
    ParameterSet[CurrPreset].WcParam.EnergyCutoff = ParameterSet[Preset_Default].WcParam.EnergyCutoff;
    ParameterSet[CurrPreset].WcParam.PeakPowerCutoff = ParameterSet[Preset_Default].WcParam.PeakPowerCutoff;
    ParameterSet[CurrPreset].WcParam.TimeCutOff = ParameterSet[Preset_Default].WcParam.TimeCutOff;
    ParameterSet[CurrPreset].ActParam.CustomAIn1Cutoff = ParameterSet[Preset_Default].ActParam.CustomAIn1Cutoff;
    ParameterSet[CurrPreset].ActParam.CustomAIn2Cutoff = ParameterSet[Preset_Default].ActParam.CustomAIn2Cutoff;
    ParameterSet[CurrPreset].WcParam.WeldFreqHighCutOffFlag = ParameterSet[Preset_Default].WcParam.WeldFreqHighCutOffFlag;
    ParameterSet[CurrPreset].WcParam.WeldFreqLowCutOffFlag = ParameterSet[Preset_Default].WcParam.WeldFreqLowCutOffFlag;
    ParameterSet[CurrPreset].WcParam.WeldFreqHighCutoff = ParameterSet[Preset_Default].WcParam.WeldFreqHighCutoff;
    ParameterSet[CurrPreset].WcParam.WeldFreqLowCutoff = ParameterSet[Preset_Default].WcParam.WeldFreqLowCutoff;
    CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_ACT | SAVECRC_WC));
}

/*  void SetDefaultValues(DEFAULT_PSCMD DefaulPSCmd)
 *
 *  Purpose:
 *		This function is called to set the default values
 *		in CurrentPreset from the slot in ParameterSet which saves
 *		the default values. Function is called in case of CRC
 *		mismatch and from website module when "Restore Default"
 *		is requested from setup pages showing Preset values.
 *
 *  Entry condition:
 *		DefaultPSCmd: The part of currentPrest where default
 *		values should be restored.
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultValues(DEFAULT_PSCMD DefaulPSCmd, PRESET_NUMBER PresetNum)
{
	UINT16 TempSeekTime,TempSeekRamp,HsPsNum = ParameterSet[PresetNum].InfoParam.HornPresetNumber;
	SINT32 TempSeekFreqOffset;
	if(((DefaulPSCmd & DEFAULT_ALLPS ) == DEFAULT_ALLPS) )//&& (PresetNum == CurrPreset))
		HsPsNum = ParameterSet[Preset_Default].InfoParam.HornPresetNumber;
	//specific to setup-weld webpage
	if((DefaulPSCmd & DEFAULT_WELDPS) == DEFAULT_WELDPS){
		ParameterSet[CurrPreset].WcParam.WeldMode = ParameterSet[Preset_Default].WcParam.WeldMode;
		ParameterSet[CurrPreset].WcParam.WeldTimeCP = ParameterSet[Preset_Default].WcParam.WeldTimeCP;
		ParameterSet[CurrPreset].WcParam.EnergyCP = ParameterSet[Preset_Default].WcParam.EnergyCP;
		ParameterSet[CurrPreset].WcParam.PeakPowerCP = ParameterSet[Preset_Default].WcParam.PeakPowerCP;
		ParameterSet[CurrPreset].WcParam.ScrubTime = ParameterSet[Preset_Default].WcParam.ScrubTime;
		ParameterSet[CurrPreset].WcParam.ABFlag = ParameterSet[Preset_Default].WcParam.ABFlag;
		ParameterSet[CurrPreset].WcParam.ABDelay = ParameterSet[Preset_Default].WcParam.ABDelay;
		ParameterSet[CurrPreset].WcParam.ABTime = ParameterSet[Preset_Default].WcParam.ABTime;
		ParameterSet[CurrPreset].WcParam.FreqOffsetFlag = ParameterSet[Preset_Default].WcParam.FreqOffsetFlag;
		ParameterSet[CurrPreset].WcParam.FreqOffset = ParameterSet[Preset_Default].WcParam.FreqOffset;
		ParameterSet[CurrPreset].DcpParam.MemoryStoreFlag = ParameterSet[Preset_Default].DcpParam.MemoryStoreFlag;
		ParameterSet[CurrPreset].WcParam.EBFlag = ParameterSet[Preset_Default].WcParam.EBFlag;
		ParameterSet[CurrPreset].WcParam.HoldTimeFlag = ParameterSet[Preset_Default].WcParam.HoldTimeFlag;
		ParameterSet[CurrPreset].WcParam.HoldTime = ParameterSet[Preset_Default].WcParam.HoldTime;
		HsParameterSet[CurrHsPreset].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(Midband);
		HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(Midband);
		HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = 0;
		HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP = 0;
		Sysconfig->ClearMemoryBeforeSeek = TRUE;
		Sysconfig->ClearMemoryWithReset = FALSE;
		Sysconfig->SetDigTuneWithHornScan = FALSE;
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_ACT | SAVECRC_WC | SAVECRC_DCP));
	}
	//specific to setup-advanced webpage
	if((DefaulPSCmd & DEFAULT_ADVANCEPS) == DEFAULT_ADVANCEPS){
		ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC1 = ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1;
		ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC2 = ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2;
		ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC1new = ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1new;
		ParameterSet[CurrPreset].DcpParam.WeldAmplitudeLoopC2new = ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2new;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLimit = ParameterSet[Preset_Default].DcpParam.WeldPhaseLimit;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLimitTime = ParameterSet[Preset_Default].DcpParam.WeldPhaseLimitTime;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopI = ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopI;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopCF = ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCF;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopInew = ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopInew;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopCFnew = ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCFnew;
		ParameterSet[CurrPreset].DcpParam.WeldPhaseLoopP = ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopP;
		ParameterSet[CurrPreset].DcpParam.FreqWindowSize = ParameterSet[Preset_Default].DcpParam.FreqWindowSize;
		ParameterSet[CurrPreset].DcpParam.WeldFreqHigh = ParameterSet[Preset_Default].DcpParam.WeldFreqHigh;
		ParameterSet[CurrPreset].DcpParam.WeldFreqLow = ParameterSet[Preset_Default].DcpParam.WeldFreqLow;
		ParameterSet[CurrPreset].DcpParam.UseTrueValues = ParameterSet[Preset_Default].DcpParam.UseTrueValues;
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_DCP));
	}
	//specific to setup-advanced webpage
	if((DefaulPSCmd & DEFAULT_ADVANCEDIAG) == DEFAULT_ADVANCEDIAG){
		TempSeekFreqOffset = HsParameterSet[CurrHsPreset].ParaSeek.FreqOffset;
		TempSeekTime = HsParameterSet[CurrHsPreset].ParaSeek.SeekTime;
		TempSeekRamp = HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime;
		HsParameterSet[CurrHsPreset] = HsParameterSet[HsPreset_Default];
		HsParameterSet[CurrHsPreset].ParaSeek.FreqOffset = TempSeekFreqOffset;
		HsParameterSet[CurrHsPreset].ParaSeek.SeekTime = TempSeekTime;
		HsParameterSet[CurrHsPreset].ParaSeek.SeekRampTime = TempSeekRamp;
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_HSPS));
	}
	//specific to setup-amplitude web page
	if((DefaulPSCmd & DEFAULT_AMPLITUDEPS) == DEFAULT_AMPLITUDEPS){
		ParameterSet[CurrPreset].WcParam.AmpStepCondition = ParameterSet[Preset_Default].WcParam.AmpStepCondition;
		ParameterSet[CurrPreset].WcParam.Amplitude1 = ParameterSet[Preset_Default].WcParam.Amplitude1;
		ParameterSet[CurrPreset].WcParam.Amplitude2 = ParameterSet[Preset_Default].WcParam.Amplitude2;
		ParameterSet[CurrPreset].DcpParam.WeldRampTime = ParameterSet[Preset_Default].DcpParam.WeldRampTime;
		ParameterSet[CurrPreset].WcParam.ScrubAmp = ParameterSet[Preset_Default].WcParam.ScrubAmp;
		ParameterSet[CurrPreset].WcParam.ABAmp = ParameterSet[Preset_Default].WcParam.ABAmp;
		ParameterSet[CurrPreset].WcParam.AmpStepTime = ParameterSet[Preset_Default].WcParam.AmpStepTime;
		ParameterSet[CurrPreset].WcParam.AmpStepEnergy = ParameterSet[Preset_Default].WcParam.AmpStepEnergy;
		ParameterSet[CurrPreset].WcParam.AmpStepPower = ParameterSet[Preset_Default].WcParam.AmpStepPower;
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_DCP | SAVECRC_WC));
	}
	//specific to setup-seek amd powerup web page
	if((DefaulPSCmd & DEAFULT_SEEKPS) == DEAFULT_SEEKPS){
		ParameterSet[CurrPreset].DcpParam.TimedSeekFlag = ParameterSet[Preset_Default].DcpParam.TimedSeekFlag;
		HsParameterSet[CurrPreset].ParaSeek.SeekTime = HsParameterSet[HsPreset_Default].ParaSeek.SeekTime;
		HsParameterSet[CurrPreset].ParaSeek.SeekRampTime = HsParameterSet[HsPreset_Default].ParaSeek.SeekRampTime;
		HsParameterSet[CurrPreset].ParaSeek.FreqOffset = HsParameterSet[HsPreset_Default].ParaSeek.FreqOffset;
		Sysconfig->ClearMemoryAtPowerUp = FALSE;
		CalculateAndSavePresetCRC((SAVECRC_CMD)(SAVECRC_DCP | SAVECRC_HSPS));
	   Sysconfig->PowerupOption = POWERUP_SEEK;
	}

	if((DefaulPSCmd & DEFAULT_ACTPS) == DEFAULT_ACTPS){
		ParameterSet[PresetNum].ActParam = ParameterSet[Preset_Default].ActParam;
		CalculateAndSavePresetCRC(SAVECRC_ACT,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_DCPPS) == DEFAULT_DCPPS){
		ParameterSet[PresetNum].DcpParam = ParameterSet[Preset_Default].DcpParam;
		CalculateAndSavePresetCRC(SAVECRC_DCP,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_INFOPS) == DEFAULT_INFOPS){
		ParameterSet[PresetNum].InfoParam = ParameterSet[Preset_Default].InfoParam;
		ParameterSet[PresetNum].InfoParam.HornPresetNumber = HsPsNum;
		CalculateAndSavePresetCRC(SAVECRC_INFO,PresetNum);
	}
	//limits web page
	if((DefaulPSCmd & DEFAULT_LIMITSPS) == DEFAULT_LIMITSPS){
		ParameterSet[PresetNum].LimitsParam = ParameterSet[Preset_Default].LimitsParam;
		CalculateAndSavePresetCRC(SAVECRC_LIMITS,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_PMCPS) == DEFAULT_PMCPS){
		ParameterSet[PresetNum].PmcParam = ParameterSet[Preset_Default].PmcParam;
		CalculateAndSavePresetCRC(SAVECRC_PMC,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_VQSPS) == DEFAULT_VQSPS){
		ParameterSet[PresetNum].SblvqsParam = ParameterSet[Preset_Default].SblvqsParam;
		CalculateAndSavePresetCRC(SAVECRC_VQS,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_WCPS) == DEFAULT_WCPS){
	    ParameterSet[PresetNum].WcParam = ParameterSet[Preset_Default].WcParam;
	    CalculateAndSavePresetCRC(SAVECRC_WC,PresetNum);
	}
	if((DefaulPSCmd & DEFAULT_HSPS) == DEFAULT_HSPS){
		HsParameterSet[HsPsNum] = HsParameterSet[HsPreset_Default];
		CalculateAndSavePresetCRC(SAVECRC_HSPS, PresetNum, (HSPRESET_NUMBER)HsPsNum);
	}
	Sysconfig->PresetChanged = TRUE;
}

/* void SetColdStartPresetValues(void)
 *
 *  Purpose:
 *		This function is called to assign default values to the slot in ParameterSet reserved
 *		to save default Preset values. This function is called during cold start.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void SetColdStartPresetValues(PSUPDATESPECIAL PresetSpcl)
{
	UINT8 Indx;
	InitFreqDependentParametersMinMaxDefault();
	//Assign default parameter values ParameterSet[Preset_Default]
   ParameterSet[Preset_Default].ActParam.CustomAIn1Cutoff = CUSTOM_INPUT1_DEFAULT;
   ParameterSet[Preset_Default].ActParam.CustomAIn1CutoffFlag = OFF;
   ParameterSet[Preset_Default].ActParam.CustomAIn2Cutoff = CUSTOM_INPUT2_DEFAULT;
   ParameterSet[Preset_Default].ActParam.CustomAIn2CutoffFlag = OFF;
   ParameterSet[Preset_Default].DcpParam.UseTrueValues = OFF;
   
		//PI
	ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopInew  = FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopP  = FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX];
		//standard
	ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopI  = FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX];
	//constants common for PI and standard control loops
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLimit = WELD_PHASELIMIT_DEFAULT;
	ParameterSet[Preset_Default].DcpParam.WeldPhaseLimitTime = WELD_PHASELIMIT_TIME_DEFAULT;
	ParameterSet[Preset_Default].DcpParam.WeldFreqHigh   = FreqDependentParam[WELDFREQHIGH][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldFreqLow    = FreqDependentParam[WELDFREQLOW][DEFAULTVALUEINDEX];

	ParameterSet[Preset_Default].DcpParam.MemoryStoreFlag = ON;   //OFF;
	ParameterSet[Preset_Default].DcpParam.TimedSeekFlag  = OFF;
	ParameterSet[Preset_Default].WcParam.WeldFreqHighCutoff = FreqDependentParam[WELDFREQHIGHCUTOFF][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].WcParam.WeldFreqLowCutoff = FreqDependentParam[WELDFREQLOWCUTOFF][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].DcpParam.WeldRampTime   = WELDRAMPTIME_DEFAULT;
	ParameterSet[Preset_Default].DcpParam.WeldProfileRampTime = WELD_PROFILERAMPTIME_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.CreatedDate = CREATEDATE_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.CreatedHour = CREATEHOUR_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.CreatedMinute = CREATEMINUTE_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.CreatedMonth = CREATEMONTH_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.CreatedYear = CREATEYEAR_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.HardwareConfig = HARDWARECONFIG_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.HornPresetNumber = HORNPRESETNUMBER_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.ModifiedDate = MODIFIEDDATE_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.ModifiedHour = MODIFIEDHOUR_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.ModifiedMinute = MODIFIEDMINUTE_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.ModifiedMonth = MODIFIEDMONTH_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.ModifiedYear = MODIFIEDYEAR_DEFAULT;
	strcpy((SINT8*)ParameterSet[Preset_Default].InfoParam.OperatorID , OPERATORID_DEFAULT);
	ParameterSet[Preset_Default].InfoParam.PowerSupplyFreq = PSFREQ_DEFAULT;
	strcpy((SINT8*)ParameterSet[Preset_Default].InfoParam.PowerSupplySN, PSSERIALNO_DEFAULT);
	ParameterSet[Preset_Default].InfoParam.PowerSupplyWatts = PSPOWER_DEFAULT;
	strcpy((SINT8*)ParameterSet[Preset_Default].InfoParam.PresetID, PRESETID_DEFAULT);
	ParameterSet[Preset_Default].InfoParam.PresetStatus = PRESETSTATUS_DEFAULT;
	ParameterSet[Preset_Default].InfoParam.WeldCount = WELDCOUNT_DEFAULT;

	ParameterSet[Preset_Default].LimitsParam.ControlLimits = OFF;
	ParameterSet[Preset_Default].LimitsParam.EnergyMLR = ENERGYMLR_DEFAULT;
	ParameterSet[Preset_Default].LimitsParam.EnergyPLR = ENERGYPLR_DEFAULT;
	ParameterSet[Preset_Default].LimitsParam.PowerMLR = POWERMLR_DEFAULT;
	ParameterSet[Preset_Default].LimitsParam.PowerPLR = POWERPLR_DEFAULT;
	ParameterSet[Preset_Default].LimitsParam.WeldTimeMLR = WELDTIMEMLR_DEFAULT;
	ParameterSet[Preset_Default].LimitsParam.WeldTimePLR = WELDTIMEPLR_DEFAULT;

	ParameterSet[Preset_Default].WcParam.ABAmp = ABAMP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.ABDelay = ABDELAY_DEFAULT;
	ParameterSet[Preset_Default].WcParam.ABFlag  = OFF;
	ParameterSet[Preset_Default].WcParam.ABTime  = ABTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.Amplitude1 = AMP1_DEFAULT;
	ParameterSet[Preset_Default].WcParam.Amplitude2 = AMP2_DEFAULT;
	ParameterSet[Preset_Default].WcParam.AmpStepCondition = AMPSTEPCONDITION_DEFAULT;
	ParameterSet[Preset_Default].WcParam.AmpStepTime = AMPSTEPTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.AmpStepEnergy = AMPSTEPENERGY_DEFAULT;
	ParameterSet[Preset_Default].WcParam.AmpStepExternal = OFF;
	ParameterSet[Preset_Default].WcParam.AmpStepPower = AMPSTEPPOWER_DEFAULT;
	ParameterSet[Preset_Default].WcParam.CycleAborts = CYCLE_ABORT_OFF;
	ParameterSet[Preset_Default].WcParam.EBAmp = EBAMP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.EBFlag = OFF;
	ParameterSet[Preset_Default].WcParam.EBTime = EBTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.EnergyCP = ENERGYCP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.EnergyComp = ENERGYCOMP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.EnergyCompFlag = OFF;
	ParameterSet[Preset_Default].WcParam.EnergyCutoff = ENERGYCUTOFF_DEFAULT;
	ParameterSet[Preset_Default].WcParam.EnergyCutoffFlag = OFF;
	ParameterSet[Preset_Default].WcParam.ExtTrigDelayFlag = OFF;
	ParameterSet[Preset_Default].WcParam.ExtTrigDelayTime = EXTTRIGDELAYTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.FreqOffset = FreqDependentParam[WELDFREQOFFSET][DEFAULTVALUEINDEX];
	ParameterSet[Preset_Default].WcParam.FreqOffsetFlag = OFF;
	ParameterSet[Preset_Default].WcParam.GeneralAlarmFlag = OFF;
	ParameterSet[Preset_Default].WcParam.GrdDetectFlagCP = OFF;
	ParameterSet[Preset_Default].WcParam.HoldTime = HOLDTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.HoldTimeFlag = ON;
	ParameterSet[Preset_Default].WcParam.MaxWeldTime = MAXWELDTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.PeakPowerCP = PEAKPOWERCP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.PeakPowerCutoff = PEAKPOWERCUTOFF_DEFAULT;
	ParameterSet[Preset_Default].WcParam.PeakPowerCutoffFlag = OFF;
	ParameterSet[Preset_Default].WcParam.PostWeldSeekFlag = OFF;
	ParameterSet[Preset_Default].WcParam.ScrubAmp = SCRUBAMP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.ScrubTime = SCRUBTIME_DEFAULT;
	ParameterSet[Preset_Default].WcParam.ScrubTimeFlag = OFF;
	ParameterSet[Preset_Default].WcParam.TimeCutOff = TIMECUTOFF_DEFAULT;
	ParameterSet[Preset_Default].WcParam.TimeCutOffFlag = OFF;
	ParameterSet[Preset_Default].WcParam.WeldMode = WELDMODE_DEFAULT;
	ParameterSet[Preset_Default].WcParam.WeldTimeCP = WELDTIMECP_DEFAULT;
	ParameterSet[Preset_Default].WcParam.CutOffEnabled = OFF;
	ParameterSet[Preset_Default].WcParam.WeldFreqLowCutOffFlag = OFF;
	ParameterSet[Preset_Default].WcParam.WeldFreqHighCutOffFlag = OFF;
	//Assign default parameter values to HsParameterSet[HsPreset_Default]
	HsParameterSet[HsPreset_Default].ParaSeek.SeekTime = SEEKTIME_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaSeek.SeekAmp = SEEKAMP_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaSeek.SeekRampTime = SEEKRAMPTIME_DEFAULT;

	HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC2 =  FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.FreqOffset = FreqDependentParam[SEEKFREQOFFSET][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.SeekPhaseLoopI  = FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.SeekFreqHigh = FreqDependentParam[SEEKFREQHIGH][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaSeek.SeekFreqLow = FreqDependentParam[SEEKFREQLOW][DEFAULTVALUEINDEX];


	HsParameterSet[HsPreset_Default].ParaTest.Amplitude1 = AMP1_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.AmpStepCondition = AMPSTEPCONDITION_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.AmplitudeProfileTime = AMPSTEPTIME_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.Amplitude2 = AMP2_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.AmplitudeStartRampTime = WELDRAMPTIME_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.AmplitudeProfileRampTime = WELD_PROFILERAMPTIME_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.FreqOffset = FREQOFFSET_DEFAULT;
	HsParameterSet[HsPreset_Default].ParaTest.TimeErrorHigh = TEST_TIMEERROR_HIGH_DEFAULT;

	HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaTest.PhaseLoopI  = FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaTest.WeldFreqHigh = FreqDependentParam[TESTFREQHIGH][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaTest.WeldFreqLow = FreqDependentParam[TESTFREQLOW][DEFAULTVALUEINDEX];

	HsParameterSet[HsPreset_Default].ParaScan.FrequencyStart = FreqDependentParam[SCANFREQUENCYSTART][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaScan.FrequencyStop  = FreqDependentParam[SCANFREQUENCYSTOP][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaScan.FrequencyStep  = FreqDependentParam[SCANFREQUENCYSTEP][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaScan.TimeDelay  = FreqDependentParam[SCANTIMEDELAY][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaScan.MaxAmplitude = FreqDependentParam[SCANMAXAMPLITUDE][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].ParaScan.MaxCurrent = FreqDependentParam[SCANMAXCURRENT][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].DigitalTuneScaledUP = FreqDependentParam[DIGITALTUNE][DEFAULTVALUEINDEX];
	HsParameterSet[HsPreset_Default].MemoryOffsetScaledUP = FreqDependentParam[MEMOFFSET][DEFAULTVALUEINDEX];//TODO:We dint use to do with global StoredFreqAbsArray

	HsParameterSet[HsPreset_Default].ParaScan.TimeErrorHigh = SCANTIMERRORHIGH_DEFAULT;
	CalculateAndSavePresetCRC(SAVECRC_ALL, Preset_Default, HsPreset_Default, FALSE);
	if(PresetSpcl == UPDATENOSPECIALPS){
		ParameterSet[CurrPreset] = RunningPreset = ParameterSet[Preset_Default];
		HsParameterSet[CurrHsPreset] = RunningHSPreset = HsParameterSet[HsPreset_Default];
		Sysconfig->ActivePreset = 0;
		Sysconfig->RecalledPreset = 0;
		ActPresetCycleCount = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;
		Sysconfig->PresetChanged = FALSE;
		Sysconfig->PowerupOption = POWERUP_SEEK;
		Sysconfig->ClearMemoryBeforeSeek = TRUE;
		Sysconfig->ClearMemoryWithReset = FALSE;
		Sysconfig->SetDigTuneWithHornScan = FALSE;
		Sysconfig->ClearMemoryAtPowerUp = FALSE;
		SetMaxMinValues();
		//set defaults in all other presets too
		for(Indx = 1; Indx <= MAXIMUM_USER_PRESETS; Indx++)
			memcpy(&ParameterSet[Indx], &ParameterSet[Preset_Default],  sizeof(DcxPreset));
		for(Indx = 1; Indx <= MAXIMUM_USER_HSPRESETS; Indx++)
			memcpy(&HsParameterSet[Indx], &HsParameterSet[HsPreset_Default],  sizeof(HSPRESET));
		UpdateMinMaxLCDPreset(UPDATEMINMAX);
		SetMinMaxParamForFbTable(UPDATEMINMAX);
		SetHDDefaults();
	}
}

/* void SetMaxMinValues(PSUPDATESPECIAL PresetSpcl)
 *
 *	 Purpose:
 *		This function is called to assign min max values to the slots in ParameterSet reserved
 *		to save min max values. This function is called during cold start from SetColdStartPresetValues() function.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void SetMaxMinValues(PSUPDATESPECIAL PresetSpcl)
{
    PRESET_NUMBER PSMin = Preset_MIN, PSMax = Preset_MAX;
    HSPRESET_NUMBER HSPSMin = HsPreset_MIN, HSPSMax = HsPreset_MAX;
    if(PresetSpcl != UPDATENOSPECIALPS)
    	InitFreqDependentParametersMinMaxDefault();
	if((PresetSpcl & UPDATETWEAKMINPS) == UPDATETWEAKMINPS){
		PSMin = Preset_TweakMIN;
		HSPSMin = HsPreset_TweakMIN;
    }
	if((PresetSpcl & UPDATETWEAKMAXPS) == UPDATETWEAKMAXPS){
		PSMax = Preset_TweakMAX;
		HSPSMax = HsPreset_TweakMAX;
    }
	if((PresetSpcl == UPDATENOSPECIALPS) || ((PresetSpcl & UPDATETWEAKMINPS) == UPDATETWEAKMINPS) ||
			((PresetSpcl & UPDATEMINPS) == UPDATEMINPS)){
	//	printf(" PSMin %d Hspsmin %d", PSMin, HSPSMin);
		ParameterSet[PSMin].ActParam.CustomAIn1Cutoff = CUSTOM_INPUT1_MIN;
		ParameterSet[PSMin].ActParam.CustomAIn1CutoffFlag = OFF;
		ParameterSet[PSMin].ActParam.CustomAIn2Cutoff = CUSTOM_INPUT2_MIN;
		ParameterSet[PSMin].ActParam.CustomAIn2CutoffFlag = OFF;

		ParameterSet[PSMin].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX];
		 //MIN Standard
		ParameterSet[PSMin].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX];
		ParameterSet[PSMin].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX];

		ParameterSet[PSMin].DcpParam.MemoryStoreFlag = OFF;
		ParameterSet[PSMin].DcpParam.WeldPhaseLimit = WELD_PHASELIMIT_MIN;
		ParameterSet[PSMin].DcpParam.WeldPhaseLimitTime = WELD_PHASELIMIT_TIME_MIN;
		ParameterSet[PSMin].DcpParam.UseTrueValues = OFF;
		ParameterSet[PSMin].DcpParam.TimedSeekFlag = OFF;

		ParameterSet[PSMin].DcpParam.WeldFreqHigh = FreqDependentParam[WELDFREQHIGH][MINVALUEINDEX];//WELDFREQHIGH20K_MIN;
		ParameterSet[PSMin].WcParam.WeldFreqHighCutoff = FreqDependentParam[WELDFREQHIGHCUTOFF][MINVALUEINDEX];//WELDFREQHIGH20K_MIN;
		ParameterSet[PSMin].DcpParam.WeldFreqLow = FreqDependentParam[WELDFREQLOW][MINVALUEINDEX];//WELDFREQLOW20K_MIN;
		ParameterSet[PSMin].WcParam.WeldFreqLowCutoff = FreqDependentParam[WELDFREQLOWCUTOFF][MINVALUEINDEX];
		ParameterSet[PSMin].WcParam.FreqOffset = FreqDependentParam[WELDFREQOFFSET][MINVALUEINDEX];//FREQOFFSET20K_MIN;

		ParameterSet[PSMin].DcpParam.WeldRampTime = WELDRAMPTIME_MIN;
		ParameterSet[PSMin].DcpParam.WeldProfileRampTime = WELD_PROFILERAMPTIME_MIN;
		ParameterSet[PSMin].InfoParam.CreatedDate = CREATEDATE_MIN;
		ParameterSet[PSMin].InfoParam.CreatedHour = CREATEHOUR_MIN;
		ParameterSet[PSMin].InfoParam.CreatedMinute = CREATEMINUTE_MIN;
		ParameterSet[PSMin].InfoParam.CreatedMonth = CREATEMONTH_MIN;
		ParameterSet[PSMin].InfoParam.CreatedYear = CREATEYEAR_MIN;
		ParameterSet[PSMin].InfoParam.HardwareConfig = HARDWARECONFIG_MIN;
		ParameterSet[PSMin].InfoParam.HornPresetNumber = HORNPRESETNUMBER_MIN;
		ParameterSet[PSMin].InfoParam.ModifiedDate = MODIFIEDDATE_MIN;
		ParameterSet[PSMin].InfoParam.ModifiedHour = MODIFIEDHOUR_MIN;
		ParameterSet[PSMin].InfoParam.ModifiedMinute = MODIFIEDMINUTE_MIN;
		ParameterSet[PSMin].InfoParam.ModifiedMonth = MODIFIEDMONTH_MIN;
		ParameterSet[PSMin].InfoParam.ModifiedYear = MODIFIEDYEAR_MIN;
		strcpy((SINT8*)ParameterSet[PSMin].InfoParam.OperatorID , OPERATORID_MIN);
		ParameterSet[PSMin].InfoParam.PowerSupplyFreq = PSFREQ_MIN;
		strcpy((SINT8*)ParameterSet[PSMin].InfoParam.PowerSupplySN , PSSERIALNO_MIN);
		ParameterSet[PSMin].InfoParam.PowerSupplyWatts = PSPOWER_MIN;

		if(PSMin == Preset_MIN)
			strcpy((SINT8*)ParameterSet[PSMin].InfoParam.PresetID , PRESETID_MIN);
		else
			strcpy((SINT8*)ParameterSet[PSMin].InfoParam.PresetID , PRESETIDTWEK_MIN);

		ParameterSet[PSMin].InfoParam.PresetStatus = PRESETSTATUS_MIN;
		ParameterSet[PSMin].InfoParam.WeldCount = WELDCOUNT_MIN;
	//limits
		ParameterSet[PSMin].LimitsParam.ControlLimits = OFF;
		ParameterSet[PSMin].LimitsParam.EnergyMLR = ENERGYMLR_MIN;
		ParameterSet[PSMin].LimitsParam.EnergyPLR = ENERGYPLR_MIN;
		ParameterSet[PSMin].LimitsParam.PowerMLR = POWERMLR_MIN;
		ParameterSet[PSMin].LimitsParam.PowerPLR = POWERPLR_MIN;
		ParameterSet[PSMin].LimitsParam.WeldTimeMLR = WELDTIMEMLR_MIN;
		ParameterSet[PSMin].LimitsParam.WeldTimePLR = WELDTIMEPLR_MIN;

		ParameterSet[PSMin].WcParam.ABAmp = ABAMP_MIN;
		ParameterSet[PSMin].WcParam.ABDelay = ABDELAY_MIN;
		ParameterSet[PSMin].WcParam.ABFlag = OFF;
		ParameterSet[PSMin].WcParam.ABTime = ABTIME_MIN;
		ParameterSet[PSMin].WcParam.Amplitude1 = AMP1_MIN;
		ParameterSet[PSMin].WcParam.Amplitude2 = AMP2_MIN;
		ParameterSet[PSMin].WcParam.AmpStepCondition = AMPSTEPCONDITION_MIN;
		ParameterSet[PSMin].WcParam.AmpStepTime = AMPSTEPTIME_MIN;
		ParameterSet[PSMin].WcParam.AmpStepEnergy = AMPSTEPENERGY_MIN;
		ParameterSet[PSMin].WcParam.AmpStepExternal = OFF;
		ParameterSet[PSMin].WcParam.AmpStepPower = AMPSTEPPOWER_MIN;
		ParameterSet[PSMin].WcParam.CycleAborts = CYCLE_ABORT_OFF;
		ParameterSet[PSMin].WcParam.EBAmp = EBAMP_MIN;
		ParameterSet[PSMin].WcParam.EBFlag = OFF;
		ParameterSet[PSMin].WcParam.EBTime = EBTIME_MIN;
		ParameterSet[PSMin].WcParam.EnergyCP = ENERGYCP_MIN;
		ParameterSet[PSMin].WcParam.EnergyComp = ENERGYCOMP_MIN;
		ParameterSet[PSMin].WcParam.EnergyCompFlag = OFF;
		ParameterSet[PSMin].WcParam.EnergyCutoff = ENERGYCUTOFF_MIN;
		ParameterSet[PSMin].WcParam.EnergyCutoffFlag = OFF;
		ParameterSet[PSMin].WcParam.ExtTrigDelayFlag = OFF;
		ParameterSet[PSMin].WcParam.ExtTrigDelayTime = EXTTRIGDELAYTIME_MIN;
		ParameterSet[PSMin].WcParam.FreqOffsetFlag = OFF;
		ParameterSet[PSMin].WcParam.GeneralAlarmFlag = OFF;
		ParameterSet[PSMin].WcParam.GrdDetectFlagCP = OFF;
		ParameterSet[PSMin].WcParam.HoldTime = HOLDTIME_MIN;
		ParameterSet[PSMin].WcParam.HoldTimeFlag = OFF;
		ParameterSet[PSMin].WcParam.MaxWeldTime = MAXWELDTIME_MIN;
		ParameterSet[PSMin].WcParam.PeakPowerCP = PEAKPOWERCP_MIN;
		ParameterSet[PSMin].WcParam.PeakPowerCutoff = PEAKPOWERCUTOFF_MIN;
		ParameterSet[PSMin].WcParam.PeakPowerCutoffFlag = OFF;
		ParameterSet[PSMin].WcParam.PostWeldSeekFlag = OFF;
		ParameterSet[PSMin].WcParam.ScrubAmp = SCRUBAMP_MIN;
		ParameterSet[PSMin].WcParam.ScrubTime = SCRUBTIME_MIN;
		ParameterSet[PSMin].WcParam.ScrubTimeFlag = OFF;
		ParameterSet[PSMin].WcParam.TimeCutOff = TIMECUTOFF_MIN;
		ParameterSet[PSMin].WcParam.TimeCutOffFlag = OFF;
		ParameterSet[PSMin].WcParam.WeldMode = WELDMODE_MIN;
		ParameterSet[PSMin].WcParam.WeldTimeCP = WELDTIMECP_MIN;
		ParameterSet[PSMin].WcParam.CutOffEnabled = OFF;
		ParameterSet[PSMin].WcParam.WeldFreqLowCutOffFlag = OFF;
		ParameterSet[PSMin].WcParam.WeldFreqHighCutOffFlag = OFF;

		//Assign default parameter values to HsParameterSet[HsPreset_Default]
		HsParameterSet[HSPSMin].ParaSeek.SeekTime = SEEKTIME_MIN;
		HsParameterSet[HSPSMin].ParaSeek.SeekAmp = SEEKAMP_MIN;
		HsParameterSet[HSPSMin].ParaSeek.SeekRampTime = SEEKRAMPTIME_MIN;

		HsParameterSet[HSPSMin].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.FreqOffset = FreqDependentParam[SEEKFREQOFFSET][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.SeekFreqHigh = FreqDependentParam[SEEKFREQHIGH][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaSeek.SeekFreqLow = FreqDependentParam[SEEKFREQLOW][MINVALUEINDEX];

		//Test
		HsParameterSet[HSPSMin].ParaTest.Amplitude1 = AMP1_MIN;
		HsParameterSet[HSPSMin].ParaTest.AmpStepCondition = AMPSTEPCONDITION_MIN;
		HsParameterSet[HSPSMin].ParaTest.AmplitudeProfileTime = AMPSTEPTIME_MIN;
		HsParameterSet[HSPSMin].ParaTest.Amplitude2 = AMP2_MIN;
		HsParameterSet[HSPSMin].ParaTest.AmplitudeStartRampTime = WELDRAMPTIME_MIN;
		HsParameterSet[HSPSMin].ParaTest.AmplitudeProfileRampTime = WELD_PROFILERAMPTIME_MIN;
		HsParameterSet[HSPSMin].ParaTest.TimeErrorHigh = TEST_TIMEERROR_HIGH_MIN;

		HsParameterSet[HSPSMin].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.FreqOffset = FreqDependentParam[TESTFREQOFFSET][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.WeldFreqHigh = FreqDependentParam[TESTFREQHIGH][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaTest.WeldFreqLow = FreqDependentParam[TESTFREQLOW][MINVALUEINDEX];

		//scan
		HsParameterSet[HSPSMin].ParaScan.FrequencyStart = FreqDependentParam[SCANFREQUENCYSTART][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.FrequencyStop = FreqDependentParam[SCANFREQUENCYSTOP][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.FrequencyStep = FreqDependentParam[SCANFREQUENCYSTEP][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.TimeDelay = FreqDependentParam[SCANTIMEDELAY][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.MaxAmplitude = FreqDependentParam[SCANMAXAMPLITUDE][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.MaxCurrent = FreqDependentParam[SCANMAXCURRENT][MINVALUEINDEX];
		HsParameterSet[HSPSMin].DigitalTuneScaledUP = FreqDependentParam[DIGITALTUNE][MINVALUEINDEX];
		HsParameterSet[HSPSMin].MemoryOffsetScaledUP = FreqDependentParam[MEMOFFSET][MINVALUEINDEX];
		HsParameterSet[HSPSMin].ParaScan.TimeErrorHigh = SCANTIMERROR_HIGH_MIN;
	}
	//Assign Max parameter values to ParameterSet[Preset_MAX]
	if((PresetSpcl == UPDATENOSPECIALPS) || ((PresetSpcl & UPDATETWEAKMAXPS) == UPDATETWEAKMAXPS) ||
			((PresetSpcl & UPDATEMAXPS) == UPDATEMAXPS)){
	//	printf(" PSMax %d Hspsmax %d", PSMax, HSPSMax);
		ParameterSet[PSMax].ActParam.CustomAIn1Cutoff = CUSTOM_INPUT1_MAX;
		ParameterSet[PSMax].ActParam.CustomAIn1CutoffFlag = ON;
	    ParameterSet[PSMax].ActParam.CustomAIn2Cutoff = CUSTOM_INPUT2_MAX;
	    ParameterSet[PSMax].ActParam.CustomAIn2CutoffFlag = ON;

		ParameterSet[PSMax].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX];
		//standard
		ParameterSet[PSMax].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX];
		ParameterSet[PSMax].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX];

		ParameterSet[PSMax].DcpParam.MemoryStoreFlag = ON;
		ParameterSet[PSMax].DcpParam.WeldPhaseLimit = WELD_PHASELIMIT_MAX;
		ParameterSet[PSMax].DcpParam.WeldPhaseLimitTime = WELD_PHASELIMIT_TIME_MAX;
		ParameterSet[PSMax].DcpParam.UseTrueValues = ON;

		ParameterSet[PSMax].DcpParam.TimedSeekFlag = ON;

		ParameterSet[PSMax].DcpParam.WeldFreqHigh = FreqDependentParam[WELDFREQHIGH][MAXVALUEINDEX];
		ParameterSet[PSMax].WcParam.WeldFreqHighCutoff =FreqDependentParam[WELDFREQHIGHCUTOFF][MAXVALUEINDEX];// WELDFREQHIGH20K_MAX;
		ParameterSet[PSMax].DcpParam.WeldFreqLow = FreqDependentParam[WELDFREQLOW][MAXVALUEINDEX];
		ParameterSet[PSMax].WcParam.WeldFreqLowCutoff = FreqDependentParam[WELDFREQLOWCUTOFF][MAXVALUEINDEX];//WELDFREQLOW20K_MAX;
		ParameterSet[PSMax].WcParam.FreqOffset = FreqDependentParam[WELDFREQOFFSET][MAXVALUEINDEX];

		ParameterSet[PSMax].DcpParam.WeldRampTime = WELDRAMPTIME_MAX;
		ParameterSet[PSMax].DcpParam.WeldProfileRampTime = WELD_PROFILERAMPTIME_MAX;
		ParameterSet[PSMax].InfoParam.CreatedDate = CREATEDATE_MAX;
		ParameterSet[PSMax].InfoParam.CreatedHour = CREATEHOUR_MAX;
		ParameterSet[PSMax].InfoParam.CreatedMinute = CREATEMINUTE_MAX;
		ParameterSet[PSMax].InfoParam.CreatedMonth = CREATEMONTH_MAX;
		ParameterSet[PSMax].InfoParam.CreatedYear = CREATEYEAR_MAX;
		ParameterSet[PSMax].InfoParam.HardwareConfig = HARDWARECONFIG_MAX;
		ParameterSet[PSMax].InfoParam.HornPresetNumber = HORNPRESETNUMBER_MAX;
		ParameterSet[PSMax].InfoParam.ModifiedDate = MODIFIEDDATE_MAX;
		ParameterSet[PSMax].InfoParam.ModifiedHour = MODIFIEDHOUR_MAX;
		ParameterSet[PSMax].InfoParam.ModifiedMinute = MODIFIEDMINUTE_MAX;
		ParameterSet[PSMax].InfoParam.ModifiedMonth = MODIFIEDMONTH_MAX;
		ParameterSet[PSMax].InfoParam.ModifiedYear = MODIFIEDYEAR_MAX;
		strcpy((SINT8*)ParameterSet[PSMax].InfoParam.OperatorID , OPERATORID_MAX);
		ParameterSet[PSMax].InfoParam.PowerSupplyFreq = PSFREQ_MAX;
		strcpy((SINT8*)ParameterSet[PSMax].InfoParam.PowerSupplySN , PSSERIALNO_MAX);
		ParameterSet[PSMax].InfoParam.PowerSupplyWatts = PSPOWER_MAX;
		if(PSMax == Preset_MAX)
			strcpy((SINT8*)ParameterSet[PSMax].InfoParam.PresetID , PRESETID_MAX);
		else
			strcpy((SINT8*)ParameterSet[PSMax].InfoParam.PresetID , PRESETIDTWEK_MAX);
		ParameterSet[PSMax].InfoParam.PresetStatus = PRESETSTATUS_MAX;
		ParameterSet[PSMax].InfoParam.WeldCount = WELDCOUNT_MAX;
	//limits
		ParameterSet[PSMax].LimitsParam.ControlLimits = CONTROL_LIMITS_ENABLEALL;
		ParameterSet[PSMax].LimitsParam.EnergyMLR = ENERGYMLR_MAX;
		ParameterSet[PSMax].LimitsParam.EnergyPLR = ENERGYPLR_MAX;
		ParameterSet[PSMax].LimitsParam.PowerMLR = POWERMLR_MAX;
		ParameterSet[PSMax].LimitsParam.PowerPLR = POWERPLR_MAX;
		ParameterSet[PSMax].LimitsParam.WeldTimeMLR = WELDTIMEMLR_MAX;
		ParameterSet[PSMax].LimitsParam.WeldTimePLR = WELDTIMEPLR_MAX;

		ParameterSet[PSMax].WcParam.ABAmp = ABAMP_MAX;
		ParameterSet[PSMax].WcParam.ABDelay = ABDELAY_MAX;
		ParameterSet[PSMax].WcParam.ABFlag = ON;
		ParameterSet[PSMax].WcParam.ABTime = ABTIME_MAX;
		ParameterSet[PSMax].WcParam.Amplitude1 = AMP1_MAX;
		ParameterSet[PSMax].WcParam.Amplitude2 = AMP2_MAX;
		ParameterSet[PSMax].WcParam.AmpStepCondition = AMPSTEPCONDITION_MAX;
		ParameterSet[PSMax].WcParam.AmpStepTime = AMPSTEPTIME_MAX;
		ParameterSet[PSMax].WcParam.AmpStepEnergy = AMPSTEPENERGY_MAX;
		ParameterSet[PSMax].WcParam.AmpStepExternal = ON;
		ParameterSet[PSMax].WcParam.AmpStepPower = AMPSTEPPOWER_MAX;
		ParameterSet[PSMax].WcParam.CycleAborts = CYCLE_ABORT_ON;
		ParameterSet[PSMax].WcParam.EBAmp = EBAMP_MAX;
		ParameterSet[PSMax].WcParam.EBFlag = ON;
		ParameterSet[PSMax].WcParam.EBTime = EBTIME_MAX;
		ParameterSet[PSMax].WcParam.EnergyCP = ENERGYCP_MAX;
		ParameterSet[PSMax].WcParam.EnergyComp = ENERGYCOMP_MAX;
		ParameterSet[PSMax].WcParam.EnergyCompFlag = ON;
		ParameterSet[PSMax].WcParam.EnergyCutoff = ENERGYCUTOFF_MAX;
		ParameterSet[PSMax].WcParam.EnergyCutoffFlag = ON;
		ParameterSet[PSMax].WcParam.ExtTrigDelayFlag = ON;
		ParameterSet[PSMax].WcParam.ExtTrigDelayTime = EXTTRIGDELAYTIME_MAX;
		ParameterSet[PSMax].WcParam.FreqOffsetFlag = ON;
		ParameterSet[PSMax].WcParam.GeneralAlarmFlag = ON;
		ParameterSet[PSMax].WcParam.GrdDetectFlagCP = ON;
		ParameterSet[PSMax].WcParam.HoldTime = HOLDTIME_MAX;
		ParameterSet[PSMax].WcParam.HoldTimeFlag = ON;
		ParameterSet[PSMax].WcParam.MaxWeldTime = MAXWELDTIME_MAX;
		ParameterSet[PSMax].WcParam.PeakPowerCP = PEAKPOWERCP_MAX;
		ParameterSet[PSMax].WcParam.PeakPowerCutoff = PEAKPOWERCUTOFF_MAX;
		ParameterSet[PSMax].WcParam.PeakPowerCutoffFlag = ON;
		ParameterSet[PSMax].WcParam.PostWeldSeekFlag = ON;
		ParameterSet[PSMax].WcParam.ScrubAmp = SCRUBAMP_MAX;
		ParameterSet[PSMax].WcParam.ScrubTime = SCRUBTIME_MAX;
		ParameterSet[PSMax].WcParam.ScrubTimeFlag = ON;
		ParameterSet[PSMax].WcParam.TimeCutOff = TIMECUTOFF_MAX;
		ParameterSet[PSMax].WcParam.TimeCutOffFlag = ON;
		ParameterSet[PSMax].WcParam.WeldMode = WELDMODE_MAX;
		ParameterSet[PSMax].WcParam.WeldTimeCP = WELDTIMECP_MAX;
		ParameterSet[PSMax].WcParam.CutOffEnabled = ON;
		ParameterSet[PSMax].WcParam.WeldFreqLowCutOffFlag = ON;
		ParameterSet[PSMax].WcParam.WeldFreqHighCutOffFlag = ON;
		///Assign Max parameter values to HSParameterSet[HsPreset_MAX]
		HsParameterSet[HSPSMax].ParaSeek.SeekTime = SEEKTIME_MAX;
		HsParameterSet[HSPSMax].ParaSeek.SeekAmp = SEEKAMP_MAX;
		HsParameterSet[HSPSMax].ParaSeek.SeekRampTime = SEEKRAMPTIME_MAX;

		HsParameterSet[HSPSMax].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.FreqOffset = FreqDependentParam[SEEKFREQOFFSET][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.SeekFreqHigh = FreqDependentParam[SEEKFREQHIGH][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaSeek.SeekFreqLow = FreqDependentParam[SEEKFREQLOW][MAXVALUEINDEX];

		//Test
		HsParameterSet[HSPSMax].ParaTest.Amplitude1 = AMP1_MAX;
		HsParameterSet[HSPSMax].ParaTest.AmpStepCondition = AMPSTEPCONDITION_MAX;
		HsParameterSet[HSPSMax].ParaTest.AmplitudeProfileTime = AMPSTEPTIME_MAX;
		HsParameterSet[HSPSMax].ParaTest.Amplitude2 = AMP2_MAX;
		HsParameterSet[HSPSMax].ParaTest.AmplitudeStartRampTime   = WELDRAMPTIME_MAX;
		HsParameterSet[HSPSMax].ParaTest.AmplitudeProfileRampTime = WELD_PROFILERAMPTIME_MAX;
		HsParameterSet[HSPSMax].ParaTest.TimeErrorHigh = TEST_TIMEERROR_HIGH_MAX;

		HsParameterSet[HSPSMax].ParaTest.FreqOffset = FreqDependentParam[TESTFREQOFFSET][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.WeldFreqHigh = FreqDependentParam[TESTFREQHIGH][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.WeldFreqLow = FreqDependentParam[TESTFREQLOW][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX];;

		//scan
		HsParameterSet[HSPSMax].ParaScan.FrequencyStart = FreqDependentParam[SCANFREQUENCYSTART][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.FrequencyStop = FreqDependentParam[SCANFREQUENCYSTOP][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.FrequencyStep = FreqDependentParam[SCANFREQUENCYSTEP][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.TimeDelay = FreqDependentParam[SCANTIMEDELAY][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.MaxAmplitude = FreqDependentParam[SCANMAXAMPLITUDE][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.MaxCurrent = FreqDependentParam[SCANMAXCURRENT][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].DigitalTuneScaledUP	= FreqDependentParam[DIGITALTUNE][MAXVALUEINDEX];
		HsParameterSet[HSPSMin].MemoryOffsetScaledUP = FreqDependentParam[MEMOFFSET][MAXVALUEINDEX];
		HsParameterSet[HSPSMax].ParaScan.TimeErrorHigh = SCANTIMERROR_HIGH_MAX;
	}
	if(PresetSpcl == UPDATENOSPECIALPS){
		CalculateAndSavePresetCRC(SAVECRC_ALL, Preset_MIN, HsPreset_MIN, FALSE);
		CalculateAndSavePresetCRC(SAVECRC_ALL, Preset_MAX, HsPreset_MAX, FALSE);
		//Update tweak values at cold start
		ParameterSet[Preset_TweakMIN] = ParameterSet[Preset_MIN];
		strcpy((SINT8*)ParameterSet[Preset_TweakMIN].InfoParam.PresetID , "TweakMinPS");
		ParameterSet[Preset_TweakMAX] = ParameterSet[Preset_MAX];
		strcpy((SINT8*)ParameterSet[Preset_TweakMAX].InfoParam.PresetID , "TweakMaxPS");
		HsParameterSet[HsPreset_TweakMIN] = HsParameterSet[HsPreset_MIN];
		HsParameterSet[HsPreset_TweakMAX] = HsParameterSet[HsPreset_MAX];
		CalculateAndSavePresetCRC(SAVECRC_ALL, Preset_TweakMAX, HsPreset_TweakMAX, FALSE);
		CalculateAndSavePresetCRC(SAVECRC_ALL, Preset_TweakMIN, HsPreset_TweakMIN, FALSE);
	}
	if(((PresetSpcl & UPDATETWEAKMINPS) == UPDATETWEAKMINPS) ||	((PresetSpcl & UPDATEMINPS) == UPDATEMINPS)){
		//printf("Crc for %d %d", PSMin, HSPSMin);
		CalculateAndSavePresetCRC(SAVECRC_ALL, PSMin, HSPSMin, FALSE);
	}
	if(((PresetSpcl & UPDATETWEAKMAXPS) == UPDATETWEAKMAXPS) ||	((PresetSpcl & UPDATEMAXPS) == UPDATEMAXPS)){
		//printf("Crc for %d %d", PSMax, HSPSMax);
		CalculateAndSavePresetCRC(SAVECRC_ALL, PSMax, HSPSMax, FALSE);
	}
}

/* void PrepareDCPPreset(void)
 *
 *  Purpose:
 *	    This function is called to prepare the Preset needs to be transferred to DCP over modbus.
 * 	 This function is called by state machine if some weld or horn parameters are changed
 * 	 and need to be transferred to DCP.
 *
 *  Entry condition:
 *     None.
 *
 *  Exit condition:
 *     None.
 */
void PrepareDCPWeldParam(void)
{
   if(RunningPreset.DcpParam.UseTrueValues == ON){
      DcpParameterSet.WeldParam.WeldAmplitudeLoopC1 = RunningPreset.DcpParam.WeldAmplitudeLoopC1new;
      DcpParameterSet.WeldParam.WeldAmplitudeLoopC2 = RunningPreset.DcpParam.WeldAmplitudeLoopC2new;
      DcpParameterSet.WeldParam.WeldPhaseLoopI = RunningPreset.DcpParam.WeldPhaseLoopInew;
      DcpParameterSet.WeldParam.WeldPhaseLoopCF = RunningPreset.DcpParam.WeldPhaseLoopCFnew;
   }
   else{
      DcpParameterSet.WeldParam.WeldAmplitudeLoopC1 = RunningPreset.DcpParam.WeldAmplitudeLoopC1;
      DcpParameterSet.WeldParam.WeldAmplitudeLoopC2 = RunningPreset.DcpParam.WeldAmplitudeLoopC2;
      DcpParameterSet.WeldParam.WeldPhaseLoopI = RunningPreset.DcpParam.WeldPhaseLoopI;
      DcpParameterSet.WeldParam.WeldPhaseLoopCF = RunningPreset.DcpParam.WeldPhaseLoopCF;
   }
   DcpParameterSet.WeldParam.WeldPhaseLimit = RunningPreset.DcpParam.WeldPhaseLimit;
   DcpParameterSet.WeldParam.WeldPhaseLimitTime = RunningPreset.DcpParam.WeldPhaseLimitTime;
   DcpParameterSet.WeldParam.WeldPhaseLoopP = RunningPreset.DcpParam.WeldPhaseLoopP;
   DcpParameterSet.WeldParam.FreqWindowSize = RunningPreset.DcpParam.FreqWindowSize;
   DcpParameterSet.WeldParam.WeldFreqHigh = RunningPreset.DcpParam.WeldFreqHigh;
   DcpParameterSet.WeldParam.WeldFreqLow = RunningPreset.DcpParam.WeldFreqLow;
   DcpParameterSet.WeldParam.WeldRampTime = RunningPreset.DcpParam.WeldRampTime;
   DcpParameterSet.WeldParam.WeldProfileRampTime = RunningPreset.DcpParam.WeldProfileRampTime;
   DcpParameterSet.WeldParam.MemoryStoreFlag = RunningPreset.DcpParam.MemoryStoreFlag;
   DcpParameterSet.WeldParam.UseTrueValues = RunningPreset.DcpParam.UseTrueValues;
   DcpParameterSet.WeldParam.TimedSeekFlag = RunningPreset.DcpParam.TimedSeekFlag;
}

void PrepareDCPPreset(BOOLEAN MFState)
{
   DcpParameterSet.HsParam = RunningHSPreset;
   PrepareDCPWeldParam();
   DcpParameterSet.PeakPwrNeeded = 0;
   DcpParameterSet.PeakPwrCutoff = 0;
   DcpParameterSet.EnergyNeeded = 0;
   DcpParameterSet.EnergyCutoff = 0;
   DcpParameterSet.TimeNeeded = 0;
   DcpParameterSet.TimeCutoff = 0;
   DcpParameterSet.FreqLowCutoff = 0;
   DcpParameterSet.FreqHighCutoff = 0;
   DcpParameterSet.Amplitude1 = 0;
   DcpParameterSet.Amplitude2 = 0;
   DcpParameterSet.StepAtTime = 0;
   DcpParameterSet.StepAtEnergy = 0;
   DcpParameterSet.StepAtPower = 0;
   DcpParameterSet.ExtraFlags = 0;

   if(MFState == FALSE){
      if(RunningPreset.WcParam.CutOffEnabled == TRUE){
         if(RunningPreset.WcParam.TimeCutOffFlag == TRUE)
           DcpParameterSet.TimeCutoff = RunningPreset.WcParam.TimeCutOff;
         if(RunningPreset.WcParam.EnergyCutoffFlag == TRUE)
           DcpParameterSet.EnergyCutoff = RunningPreset.WcParam.EnergyCutoff;
         if(RunningPreset.WcParam.PeakPowerCutoffFlag == TRUE)
           DcpParameterSet.PeakPwrCutoff = RunningPreset.WcParam.PeakPowerCutoff;
         if(RunningPreset.WcParam.WeldFreqLowCutOffFlag == TRUE)
          DcpParameterSet.FreqLowCutoff  = RunningPreset.WcParam.WeldFreqLowCutoff;
         if(RunningPreset.WcParam.WeldFreqHighCutOffFlag == TRUE)
          DcpParameterSet.FreqHighCutoff = RunningPreset.WcParam.WeldFreqHighCutoff;

      }

      switch(RunningPreset.WcParam.WeldMode){
         case ContinuousMode:
//            if((RunningPreset.WcParam.CutOffEnabled == TRUE) && (RunningPreset.WcParam.TimeCutOffFlag == TRUE))
//                DcpParameterSet.TimeCutoff  = MAX_TIME_CUTOFF;
         break;
         case TimeMode:
             DcpParameterSet.TimeNeeded = RunningPreset.WcParam.WeldTimeCP;
             //DcpParameterSet.TimeCutoff = 0;
         break;
         case EnergyMode:
            DcpParameterSet.EnergyNeeded = RunningPreset.WcParam.EnergyCP;
            if((RunningPreset.WcParam.CutOffEnabled == FALSE) || (RunningPreset.WcParam.TimeCutOffFlag == FALSE))
               DcpParameterSet.TimeCutoff  = MAX_TIME_CUTOFF;
            //DcpParameterSet.EnergyCutoff = 0;
         break;
         case PeakpowerMode:
            DcpParameterSet.PeakPwrNeeded  = RunningPreset.WcParam.PeakPowerCP;
            if((RunningPreset.WcParam.CutOffEnabled == FALSE) || (RunningPreset.WcParam.TimeCutOffFlag == FALSE))
               DcpParameterSet.TimeCutoff  = MAX_TIME_CUTOFF;
            //DcpParameterSet.PeakPwrCutoff = 0;
         break;
         case GroundDetectMode:
             if((RunningPreset.WcParam.CutOffEnabled == FALSE) || (RunningPreset.WcParam.TimeCutOffFlag == FALSE))
                    DcpParameterSet.TimeCutoff  = MAX_TIME_CUTOFF;
         break;
         default: break;
      }
   }
   DcpParameterSet.StepAtTime = 0;
   DcpParameterSet.StepAtEnergy = 0;
   DcpParameterSet.StepAtPower = 0;
   DcpParameterSet.ExtraFlags = 0;

   if(MFState == FALSE){
      if(RunningPreset.WcParam.EBFlag == FALSE)
         DcpParameterSet.ExtraFlags |= STOPWELD_AT_PRIMARYPARAMETER_REACH;

      switch ((AMPSTEP_COND)RunningPreset.WcParam.AmpStepCondition){
         case AMPSTEP_COND_TIME:
            DcpParameterSet.StepAtTime = RunningPreset.WcParam.AmpStepTime;
         break;
         case AMPSTEP_COND_ENERGY:
            DcpParameterSet.StepAtEnergy = RunningPreset.WcParam.AmpStepEnergy;
         break;
         case AMPSTEP_COND_POWER:
            DcpParameterSet.StepAtPower = RunningPreset.WcParam.AmpStepPower;
         break;
         default:
            DcpParameterSet.StepAtTime = 0;
            DcpParameterSet.StepAtEnergy = 0;
            DcpParameterSet.StepAtPower = 0;
         break;
      }
      DcpParameterSet.Amplitude1 = RunningPreset.WcParam.Amplitude1;	//AMP1_DEFAULT;
      DcpParameterSet.Amplitude2 = RunningPreset.WcParam.Amplitude2;	//AMP2_DEFAULT;
   }
   else{
      DcpParameterSet.Amplitude1 = CurrentCycleParam->Amplitude; //AMP1_DEFAULT;
      DcpParameterSet.TimeNeeded = CurrentCycleParam->SonicsOnTimePerWeldCycle;
      DcpParameterSet.TimeCutoff = 0;
      DcpParameterSet.EnergyCutoff = 0;
      DcpParameterSet.PeakPwrCutoff = 0;
   }
   DcpParameterSet.WeldPresetCrc = ByteCRC((UINT8 *)&DcpParameterSet,
         sizeof(DcpParameterSet) - sizeof(DcpParameterSet.WeldPresetCrc) - sizeof(HSPRESET));
}


/*  void UpdateRunningPreset(void)
 *
 *  Purpose:
 *	   This function is called by state machine to update the RunningPreset from
 *	   currentPreset before weld if the UpdateRPreset global flag is TRUE.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void UpdateRunningPreset(void)
{
   RunningPreset = ParameterSet[CurrPreset];
   RunningHSPreset = HsParameterSet[CurrHsPreset];
   PrepareDCPPreset();
   SendPresetToDCXM = TRUE;
   UpdateRPreset = FALSE;
}

/*	void CalculateAndSavePresetCRC(SAVECRC_CMD CrcCmd , PRESET_NUMBER PresetNum, HSPRESET_NUMBER HsPresetNum)
 *
 *  Purpose:
 *	  This function is called save and calculate the current Preset CRC.
 *	  This function is called from SetDefaultValues() function,
 *	  Website module, Fieldbus module whenever Preset parameter
 *	  is modified.
 *
 *  Entry condition:
 *	   CrcCmd- parameter denotes the part of Preset structure for which CRC should be calculated.
 *	   PresetNum- Preset number in ParaMeterSet array For which CRC needs to be calculated. Default value is CurrPreset
 *	   HsPresetNum- horn Preset number  HsParameterSet array for which CRC needs to be calculated.Default is hscurrentPreset
 *
 *  Exit condition:
 *	   None.
 */
void CalculateAndSavePresetCRC(SAVECRC_CMD CrcCmd , PRESET_NUMBER PresetNum, HSPRESET_NUMBER HsPresetNum, BOOLEAN CheckForPresetChange)
{
	BOOLEAN PresetChanged = FALSE;
	if((CheckForPresetChange == TRUE) && ((CrcCmd & SAVECRC_INFO) != SAVECRC_INFO)){
		ParameterSet[PresetNum].InfoParam.PresetStatus = NOTVERIFIED;
		CrcCmd = (SAVECRC_CMD)(CrcCmd | SAVECRC_INFO);
	}
	if((CrcCmd & SAVECRC_ACT) == SAVECRC_ACT){
		ParameterSet[PresetNum].CRCAct = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].ActParam), sizeof(ACTDCXPS));
		PresetChanged = TRUE;
	}

	if((CrcCmd & SAVECRC_DCP) == SAVECRC_DCP){
		ParameterSet[PresetNum].CRCDcp = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].DcpParam), sizeof(DCPDCXPS));
		PresetChanged = TRUE;
	}

	if((CrcCmd & SAVECRC_INFO) == SAVECRC_INFO){
		ParameterSet[PresetNum].CRCInfo = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].InfoParam), sizeof(INFODCXPS));
	}

	if((CrcCmd & SAVECRC_LIMITS) == SAVECRC_LIMITS){
		ParameterSet[PresetNum].CRCLimits = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].LimitsParam), sizeof(LIMITSDCXPS));
		PresetChanged = TRUE;
	}

	if((CrcCmd & SAVECRC_PMC) == SAVECRC_PMC)
		ParameterSet[PresetNum].CRCPmc = 0;

	if((CrcCmd & SAVECRC_VQS) == SAVECRC_VQS)
		ParameterSet[PresetNum].CRCSblVqs = 0;

	if((CrcCmd & SAVECRC_WC) == SAVECRC_WC){
		ParameterSet[PresetNum].CRCWC = ByteCRC((UINT8 *)(&ParameterSet[PresetNum].WcParam), sizeof(WCDCXPS));
		PresetChanged = TRUE;
	}

	if((CrcCmd & SAVECRC_HSPS) == SAVECRC_HSPS){
		HsParameterSet[HsPresetNum].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPresetNum]), HSPSCRCDATA_SIZE);
		PresetChanged = TRUE;
	}
	if((PresetChanged == TRUE)&&(PresetNum == CurrPreset))
	{
		Sysconfig->PresetChanged = TRUE;
	}
	SendToDCXM[PresetNum] = TRUE;
}

/*  void AutoNamePreset(void)
 *
 *  Purpose:
 *		This function is called to generate a name based on the weld mode
 *		selected in CurrentPreset before saving the Preset from website
 *		"Save and Recall" setup page.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
SINT8 * AutoNamePreset(void)
{
   static SINT8 AutoPresetId[MAX_PRESET_NAME_LEN + 1];
   AutoPresetId[0] = 0;
   SINT8 FormatStrTime[20];
   SINT32 MsVal = 0;

   switch (ParameterSet[CurrPreset].WcParam.WeldMode){
      case TimeMode:
    	  strcpy(FormatStrTime, "Tm = %d.%dS");
    	  MsVal = ParameterSet[CurrPreset].WcParam.WeldTimeCP % 1000;
    	  if((MsVal > 0) && (MsVal < 10))
    		  strcpy(FormatStrTime, "Tm = %d.00%dS");
    	  else if((MsVal > 9) && (MsVal < 100)){
    		  if((MsVal % 10) == 0)
				  MsVal = MsVal / 10;
    		  strcpy(FormatStrTime, "Tm = %d.0%dS");
    	  }
    	  else if((MsVal > 99)  && (MsVal < 1000)){
    		  if((MsVal % 100) == 0)
    			  MsVal = MsVal / 100;
    		  else if((MsVal % 10) == 0)
    			  MsVal = MsVal / 10;
    	  }
         sprintf(AutoPresetId, FormatStrTime,
        		 (ParameterSet[CurrPreset].WcParam.WeldTimeCP / 1000), MsVal);
         break;
      case EnergyMode:
    	  sprintf(AutoPresetId, "En = %ld.%ldj",ParameterSet[CurrPreset].WcParam.EnergyCP / 10,ParameterSet[CurrPreset].WcParam.EnergyCP % 10);
         break;
      case PeakpowerMode:
    	 sprintf(AutoPresetId, "PP = %d%%", (ParameterSet[CurrPreset].WcParam.PeakPowerCP / 10));
         break;
      case GroundDetectMode:
    	  strcpy(FormatStrTime, "GD = %d.%dS");
    	  MsVal = (ParameterSet[CurrPreset].WcParam.ScrubTime % 1000);
    	  if((MsVal > 0) && (MsVal < 10))
    		  strcpy(FormatStrTime, "GD = %d.00%dS");
    	   else if((MsVal > 9) && (MsVal < 100)){
    		   if((MsVal % 10) == 0)
    		     	MsVal = MsVal / 10;
    		   strcpy(FormatStrTime, "GD = %d.0%dS");
		   }
    	   else if((MsVal > 99)  && (MsVal < 1000)){
    		   if((MsVal % 100) == 0)
    			   MsVal = MsVal / 100;
    		   else if((MsVal % 10) == 0)
    			   MsVal = MsVal / 10;
    	   }
    	  sprintf(AutoPresetId, FormatStrTime ,(ParameterSet[CurrPreset].WcParam.ScrubTime / 1000), MsVal);
         break;
      case ContinuousMode:
         strcpy(AutoPresetId, "Continuous");
         break;
      default:
         strcpy(AutoPresetId, "Bad Mode");
         break;
   }
   return AutoPresetId;
}


/*  void SaveWeldStatusSet(void)
 *
 *  Purpose:
 *    This function is called by state machine after a weld cycle
 *    is completed to save results to status set to be send to field bus
 *    interface for diagnostic purpose.
 *
 *   Entry condition: None.
 *
 *   Exit condition: None.
 */
void SaveWeldStatusSet(void)
{
   WeldStatusSet[Sysconfig->ActivePreset].DcxPs = RunningPreset;
   //Copy Weld result to status set at index Sysconfig->ActivePreset in WeldStatusSet array
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.CycleCounter = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCDate.RTDate.Year = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Year;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCDate.RTDate.Month = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Month;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCDate.RTDate.Day = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Day;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCTime.RTTime.Hour = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Hour;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCTime.RTTime.Minute = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Minute;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RTCTime.RTTime.Second = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].TS.Second;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.WeldTime = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldTime;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.WeldEnergy = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldEnergy;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.PeakPower = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].PeakPower;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.CustomAIn1 = UserIO::GetCustomAIn1();
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.Amplitude_1 = RunningPreset.WcParam.Amplitude1;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.Amplitude_2 = RunningPreset.WcParam.Amplitude2;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.PresetNumber = Sysconfig->ActivePreset;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.WeldStartFrequency = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldStartFrequency;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.WeldStopFrequency = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldStopFrequency;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.FrequencyChange = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].FrequencyChange;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.ErrorPsv = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorPwm;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.ErrorReason = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorReason;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.ErrorCodeFB = FBStat.Error;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.HoldTime = RunningPreset.WcParam.HoldTime;
   if(AvgPowerCount)
	   WeldStatusSet[Sysconfig->ActivePreset].Wstat.AvgPower = (AvgPower/AvgPowerCount);
   else{
	   WeldStatusSet[Sysconfig->ActivePreset].Wstat.AvgPower = 0;
   }
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.AvgAmplitude1 = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude1;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.AvgAmplitude2 = WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude2;

   WeldStatusSet[Sysconfig->ActivePreset].Wstat.RecalledResonanceFrequencyAbs = RtReceiveData.FreqOut ;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.StoredFrequencyAbs = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut)) /FREQ_SCALING_FACTOR;
   if((RunningPreset.DcpParam.MemoryStoreFlag == TRUE) && (RtReceiveData.ErrorReason == 0))
      WeldStatusSet[Sysconfig->ActivePreset].Wstat.ResFreqOK = OK;
   else
      WeldStatusSet[Sysconfig->ActivePreset].Wstat.ResFreqOK = NOTOK;

   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndAmplitudeSet = WeldBuffer[WeldBufferIndex].NominalAmplitude;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndAmplitude = WeldBuffer[WeldBufferIndex].ActualAmplitude;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndPower = WeldBuffer[WeldBufferIndex].Power;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndCurrent = WeldBuffer[WeldBufferIndex].Current;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndPhase = (WeldBuffer[WeldBufferIndex].Phase * 180) / MAXDCPADCRAWVALUE;
   WeldStatusSet[Sysconfig->ActivePreset].Wstat.EndTemperature = RtReceiveData.Temperature;
   memcpy(WeldStatusSet[Sysconfig->ActivePreset].Wstat.FBAlarms , FBAlarms, sizeof(FBAlarms));
   WeldStatusSet[0] = WeldStatusSet[Sysconfig->ActivePreset];

   if(LastSSCopied == FALSE)
      SSOverRunIndication |= ((UINT32)(BIT(StatusSetNumber) | BIT(0)));
   else
      SSOverRunIndication = 0;

   StatusSetNumber = Sysconfig->ActivePreset;
   LastSSCopied = FALSE;
}

/* void SaveHSStatusSet(HornFunction HSFunc)
 *
 * Purpose:
 *    This function is called by state machine after a Horn function
 *    (seek, scan or TEST)is completed to save results to
 *    Horn specific status set to be send to field bus
 *    interface for diagnostic purpose.
 *
 * Entry condition:
 *    HSFunc: Horn Scan Function i.e. SCAN/SEEK/TEST
 *
 * Exit condition:
 *    None.
 */
void SaveHSStatusSet(HornFunction HSFunc)
{
   RTCTime TM = {0,0,0,0,0,0,0};
   UINT16 HsPsNum = ParameterSet[Sysconfig->ActivePreset].InfoParam.HornPresetNumber;
   //Copy horn result to HsStatusSet at index
   RTClock::GetCurrentTime(&TM);	//Get the current time from RTC

   switch (HSFunc) {
   case HFSCAN:
      HsStatusSet[HsPsNum][HFSCAN].HsPreset = HsParameterSet[HsPsNum];
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.Time = CONVMICRO_MILLI(ScanTime);
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.ScanStartFrequency = ScanBuffer[0].Frequency;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.ScanStopFrequency = ScanBuffer[NumScanPoints].Frequency;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndAmplitude = ScanBuffer[NumScanPoints].Amplitude;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndPwm = ScanBuffer[NumScanPoints].PWMValue;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndPower = ScanBuffer[NumScanPoints].Power;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndCurrent = ScanBuffer[NumScanPoints].Current;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndPhase = ScanBuffer[NumScanPoints].Phase;
      HsStatusSet[HsPsNum][HFSCAN].HsStatus.ScanStatus.EndTemperature = RtReceiveData.Temperature;
      HsStatusSet[HsPsNum][HFSCAN].ErrorCodeFB = FBStat.Error;
      memcpy(HsStatusSet[HsPsNum][HFSCAN].FBAlarms, FBAlarms, sizeof(FBAlarms));
      HsStatusSet[HsPsNum][HFSCAN].RTCDate.RTDate.Day = TM.Day;
      HsStatusSet[HsPsNum][HFSCAN].RTCDate.RTDate.Year = TM.Years;
      HsStatusSet[HsPsNum][HFSCAN].RTCDate.RTDate.Month = TM.Century_Month;
      HsStatusSet[HsPsNum][HFSCAN].RTCTime.RTTime.Hour = TM.Hour;
      HsStatusSet[HsPsNum][HFSCAN].RTCTime.RTTime.Minute = TM.Minute;
      HsStatusSet[HsPsNum][HFSCAN].RTCTime.RTTime.Second = TM.Second;
      break;
   case HFSEEK:
      HsStatusSet[HsPsNum][HFSEEK].HsPreset = HsParameterSet[HsPsNum];
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.Time = SeekTime;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.AvgAmplitude = AvgAmplitude;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.RecalledMidBandFreqAbs = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) / FREQ_SCALING_FACTOR);
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.SeekStartFrequencyAbs = SeekData.SeekStartFrequencyAbs;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.SeekStopFrequencyAbs = SeekData.SeekStopFrequencyAbs;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.SeekMemoryFrequencyAbs = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut) / FREQ_SCALING_FACTOR);
      if(SeekAlarmFlag == TRUE)
    	  HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.ResFreqOK = NOTOK;
      else
    	  HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.ResFreqOK = OK;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndSetAmplitude = WeldBuffer[WeldBufferIndex].NominalAmplitude;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndAmplitude = WeldBuffer[WeldBufferIndex].ActualAmplitude;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndPwm = WeldBuffer[WeldBufferIndex].PWMValue;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndPower = WeldBuffer[WeldBufferIndex].Power;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndCurrent = WeldBuffer[WeldBufferIndex].Current;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndPhase = (WeldBuffer[WeldBufferIndex].Phase * 180) / MAXDCPADCRAWVALUE;
      HsStatusSet[HsPsNum][HFSEEK].HsStatus.SeekStatus.EndTemperature = RtReceiveData.Temperature;
      HsStatusSet[HsPsNum][HFSEEK].ErrorCodeFB = FBStat.Error;
      memcpy(HsStatusSet[HsPsNum][HFSEEK].FBAlarms, FBAlarms, sizeof(FBAlarms));
      HsStatusSet[HsPsNum][HFSEEK].RTCDate.RTDate.Day = TM.Day;
      HsStatusSet[HsPsNum][HFSEEK].RTCDate.RTDate.Year = TM.Years;
      HsStatusSet[HsPsNum][HFSEEK].RTCDate.RTDate.Month = TM.Century_Month;
      HsStatusSet[HsPsNum][HFSEEK].RTCTime.RTTime.Hour = TM.Hour;
      HsStatusSet[HsPsNum][HFSEEK].RTCTime.RTTime.Minute = TM.Minute;
      HsStatusSet[HsPsNum][HFSEEK].RTCTime.RTTime.Second = TM.Second;
      break;
   case HFTEST:
      HsStatusSet[HsPsNum][HFTEST].HsPreset = HsParameterSet[HsPsNum];
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.Time = TestTime;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.AvgAmplitude1 = WeldHistoryData.AvgAmplitude1;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.AvgAmplitude2 = WeldHistoryData.AvgAmplitude2;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.RecalledResFreqAbs
            = (SCALE_DOWN_FOR_N_BITS(RtReceiveData.MemOut)) / FREQ_SCALING_FACTOR;
      if (MemoryReset == TRUE)
         HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.ResFreqOK = NOTOK;
      else
         HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.ResFreqOK = OK;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.TestStartFrequencyAbs = WeldHistoryData.WeldStartFrequency;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.TestStopFrequencyAbs = WeldHistoryData.WeldStopFrequency;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndAmplitudeSet = WeldBuffer[WeldBufferIndex].NominalAmplitude;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndAmplitude = WeldBuffer[WeldBufferIndex].ActualAmplitude;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndPwm = WeldBuffer[WeldBufferIndex].PWMValue;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndPower = WeldBuffer[WeldBufferIndex].Power;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndCurrent = WeldBuffer[WeldBufferIndex].Current;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndPhase = (WeldBuffer[WeldBufferIndex].Phase * 180)/ MAXDCPADCRAWVALUE;
      HsStatusSet[HsPsNum][HFTEST].HsStatus.TestStatus.EndTemperature = RtReceiveData.Temperature;
      HsStatusSet[HsPsNum][HFTEST].ErrorCodeFB = FBStat.Error;
      memcpy(HsStatusSet[HsPsNum][HFTEST].FBAlarms, FBAlarms, sizeof(FBAlarms));
      HsStatusSet[HsPsNum][HFTEST].RTCDate.RTDate.Day = TM.Day;
      HsStatusSet[HsPsNum][HFTEST].RTCDate.RTDate.Year = TM.Years;
      HsStatusSet[HsPsNum][HFTEST].RTCDate.RTDate.Month = TM.Century_Month;
      HsStatusSet[HsPsNum][HFTEST].RTCTime.RTTime.Hour = TM.Hour;
      HsStatusSet[HsPsNum][HFTEST].RTCTime.RTTime.Minute = TM.Minute;
      HsStatusSet[HsPsNum][HFTEST].RTCTime.RTTime.Second = TM.Second;
      break;
   default:
      break;
   }

   if (LastHSSSCopied == FALSE)
      HSSSOverRunIndication[HSStatusSetNumber] |= (UINT8)BIT(SFCT);
   else if((HSStatusSetNumber < MAXIMUM_USER_HSPRESETS) && (HsPsNum == HSStatusSetNumber))
      HSSSOverRunIndication[HSStatusSetNumber] = 0;

   HSStatusSetNumber = HsPsNum;
   SFCT = HSFunc;
   LastSFCTInHSStatus[HsPsNum] = HSFunc;
   LastHSSSCopied = FALSE;
}

/*  void SetMinMaxCycleParam(void)
 *
 *  Purpose:
 *    This function is added for website compilation.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None.
 */
void SetMinMaxCycleParam(void)
{
	MinCycleParam.TotalCycleTime = MIN_CYCLE_TIME;
	MinCycleParam.SonicsOnTimePerWeldCycle = MIN_ONIME_PERCYCLE;
	MinCycleParam.SonicsOffTimePerWeldCycle = MIN_OFFIME_PERCYCLE;
	MinCycleParam.Amplitude = ParameterSet[Preset_MIN].WcParam.Amplitude1;
	MinCycleParam.StopOnAlarm = FALSE;
	MinCycleParam.MfCycling = FALSE;

	MaxCycleParam.TotalCycleTime = MAX_CYCLE_TIME;
	MaxCycleParam.SonicsOnTimePerWeldCycle = MAX_ONIME_PERCYCLE;
	MaxCycleParam.SonicsOffTimePerWeldCycle = MAX_OFFIME_PERCYCLE;
	MaxCycleParam.Amplitude = ParameterSet[Preset_MAX].WcParam.Amplitude1;
	MaxCycleParam.StopOnAlarm = TRUE;
	MaxCycleParam.MfCycling = FALSE;
}

/*  void SetDefaultCycleParam(void)
 *
 *  Purpose:
 *    This function sets up Default values of manufacturing Cycle parameters.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultCycleParam(void)
{
	*CurrentCycleParam = MaxCycleParam;
	CurrentCycleParam->StopOnAlarm = TRUE;
	memset(MFcycleResults, 0 , sizeof(CycleResults));
	CurrentCycleParam->CRC = ByteCRC((UINT8*)CurrentCycleParam, sizeof(CycleParam) - sizeof(CurrentCycleParam->CRC));
}

/*  void SetDefaultSecretValues(void)
 *
 *  Purpose:
 *    This function is added for website compilation.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultSecretValues(void)
{
   ParameterSet[CurrPreset].WcParam.EBTime = EBTIME_DEFAULT;
   ParameterSet[CurrPreset].WcParam.EBAmp = EBAMP_DEFAULT;
   Sysconfig->DcpSpeedControl = SPEEDCONTROL_STANDARD;
   CalculateAndSavePresetCRC(SAVECRC_WC);
}

void UpdatePresetsOnLoopSpeedSwitching(void)
{
  SINT32 Indx;
  //1- assign default values of loop constants according to current loop selection.
  InitFreqAndLoopspeedDependantLoopConstantsMinMaxDefault();
  //2- Second restore min, max, tweak min, tweak max and default values according to new loop selection.
  ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopI  = FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldPhaseLoopI = FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX];


  ParameterSet[Preset_Default].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopInew  = FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldPhaseLoopInew = FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopP  = FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldPhaseLoopP = FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX];

  ParameterSet[Preset_Default].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX];
  ParameterSet[Preset_MIN].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX];
  ParameterSet[Preset_MAX].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX];
  ParameterSet[Preset_TweakMIN].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX];
  ParameterSet[Preset_TweakMAX].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaTest.PhaseLoopI  = FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaTest.PhaseLoopI = FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaSeek.SeekAmpLoopC2 =  FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaSeek.SeekAmpLoopC2 = FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaSeek.SeekPhaseLoopI  = FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaSeek.SeekPhaseLoopI = FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX];

  HsParameterSet[HsPreset_Default].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX];
  HsParameterSet[HsPreset_MIN].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX];
  HsParameterSet[HsPreset_MAX].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX];
  HsParameterSet[HsPreset_TweakMIN].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX];
  HsParameterSet[HsPreset_TweakMAX].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX];
  //3- Update current preset and other 32 presets with default loop constants according to new loop selection
  for(Indx = 0; Indx <= MAXIMUM_USER_PRESETS; Indx++){
	  ParameterSet[Indx].DcpParam.WeldAmplitudeLoopC1 = FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	  ParameterSet[Indx].DcpParam.WeldAmplitudeLoopC2 = FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.WeldPhaseLoopI  = FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.WeldPhaseLoopCF = FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.WeldAmplitudeLoopC1new = FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.WeldAmplitudeLoopC2new = FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	  ParameterSet[Indx].DcpParam.WeldPhaseLoopInew  = FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.WeldPhaseLoopP  = FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX];
      ParameterSet[Indx].DcpParam.FreqWindowSize = FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX];
	  ParameterSet[Indx].DcpParam.WeldPhaseLoopCFnew = FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX];
  }
  //4- Update current Horn preset and other 16 horn presets with default loop constants according to new loop selection
  for(Indx = 0; Indx <= MAXIMUM_USER_HSPRESETS; Indx++)
  {
	  HsParameterSet[Indx].ParaTest.AmplitudeLoopC1 = FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
	  HsParameterSet[Indx].ParaTest.AmplitudeLoopC2 = FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
	  HsParameterSet[Indx].ParaTest.PhaseLoopI  = FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX];
	  HsParameterSet[Indx].ParaTest.PhaseLoopCF = FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX];
      HsParameterSet[Indx].ParaSeek.SeekAmpLoopC1 = FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX];
      HsParameterSet[Indx].ParaSeek.SeekAmpLoopC2 =  FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX];
      HsParameterSet[Indx].ParaSeek.SeekPhaseLoopI  = FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX];
      HsParameterSet[Indx].ParaSeek.SeekPhaseLoopCF = FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX];
  }

  //5- Update Preset CRCs
  for(Indx = 0; Indx < MAX_TOTAL_PRESETS; Indx++)
	  CalculateAndSavePresetCRC(SAVECRC_ALL, (PRESET_NUMBER)Indx);

  //6- Update Horn Preset CRCs
  for(Indx = 0; Indx < MAX_TOTAL_HSPRESETS; Indx++)
	  HsParameterSet[Indx].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[Indx]), HSPSCRCDATA_SIZE);
}

/*  void SetDefaultRFswitch(void)
 *
 *  Purpose:
 *    This function is added for website compilation.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None.
 */
void SetDefaultRFswitch(void)
{
   Sysconfig->RFSwitch.RFSwitchingType = NORFSWITCHING;
   Sysconfig->RFSwitch.RFFeedback = NOFEEDBACK;
   Sysconfig->RFSwitch.RFOption = DIRECTSWITCH;
   Sysconfig->RFSwitch.FeedBackTimeOut = FEEDBACKTIMEOUT;
   Sysconfig->RFSwitch.RFSwitchDelay = RFSWITCHDELAY;
   Sysconfig->RFSwitch.RelaySwitchTime = RELAYSWITCHTIME;
   RegUpdateCmdFlag = TRUE;
}


/* void SetDefaultAdvRnd(void)
 *
 * Purpose:
 *   This function is added for website compilation.
 *
 * Entry condition:
 *   None
 *
 * Exit condition:
 *   None.
 */
void SetDefaultAdvRnd(void)
{
   AdvanceConfigData->Advrnd.DCPTimerRate = DCP_TIMERINTERRUPTUS_DEFAULT;
   AdvanceConfigData->Advrnd.DCPStateMachineTime = DCP_STATEMACHINETIMEUS_DEFAULT;
   AdvanceConfigData->Advrnd.DCPControlLoopTime = DCP_CONTROLLOOPTIMEUS_DEFAULT;
   AdvanceConfigData->Advrnd.DCPModBusTime = DCP_MODBUSCOMMTIMEUS_DEFAULT;
   AdvanceConfigData->Advrnd.DCPADCResolution = DCP_ADCRESOLUTION_11BIT;
   AdvanceConfigData->Advrnd.DCPBlindTime = DCP_BLINDTIMEUS_DEFAULT;
   AdvanceConfigData->Advrnd.Gp2Value1 = WD_DEFAULT;
   AdvanceConfigData->Advrnd.Gp2Value2 = 0;
   AdvanceConfigData->Advrnd.Gp2Value3 = 0;
   AdvanceConfigData->Advrnd.Gp2Value4 = 0;
   AdvanceConfigData->Advrnd.Gp2Value5 = 0;
   AdvanceConfigData->Advrnd.Gp2Value6 = 0;
   AdvanceConfigData->Advrnd.RevDigitalOP = TRUE;
   AdvanceConfigData->Advrnd.Flag2 = 0;//Recall Preset Event enable/disable. Default disable
   AdvanceConfigData->Advrnd.Flag3 = FALSE;//enable/disable Debounce for ethernet link status. Default Disable
   AdvanceConfigData->Advrnd.Flag4 = 0;
   AdvanceConfigData->Advrnd.Flag5 = 0;
   AdvanceConfigData->Advrnd.Flag6 = 0;
}

/*  void SetDefaultAlarms(void)
 *
 * Purpose:
 *   This function is used to set Defaults for alarms page.
 *
 * Entry condition:
 *   None.
 *
 * Exit condition:
 *   None.
 */
void SetDefaultAlarms(void)
{
   Alarmconfig->AlarmAction[OVERLOADS] = DEFOVERLOADS;
   Alarmconfig->AlarmAction[CUTOFFS] = DEFCUTOFFS;
   Alarmconfig->AlarmAction[SETUP] = DEFSETUP;
   Alarmconfig->AlarmAction[CYCLE_MODIFIED] = DEFCYCLE_MODIFIED;
   Alarmconfig->AlarmAction[WARNINGS] = DEFWARNINGS;
   Alarmconfig->AlarmAction[PROCESS_LIMITS] = DEFPROCESS_LIMITS;
   Alarmconfig->AlarmAction[EQUIPMENT_FAILURES] = DEFEQUIPMENT_FAILURES;
   Alarmconfig->AlarmAction[NOCYCLE] = DEFNOCYCLE;
   Alarmconfig->AlarmAction[COMM_FAILURES] = DEFCOMM_FAILURES;
   Alarmconfig->AlarmAction[TEMP] = DEFTEMP;
   Alarmconfig->AlarmAction[HARDWARE_FAILURE] = DEFHARDWARE_FAILURE;
   Alarmconfig->AlarmAction[NON_CYCLE_OVERLOADS] = DEFNON_CYCLE_OVERLOADS;
   RegUpdateCmdFlag = TRUE;
}

/*  BOOLEAN VerifyPresetFromWebPage(PRESET_NUMBER PresetNum)
 *
 *  Purpose:
 *    This function is called to verify one of the Presets in ParameterSet array.
 *    This function is called by website module.
 *
 *  Entry condition:
 *    PresetNum- The slot in ParameterSet array from where the Preset is to be recalled.Range
 *    (Preset_1 to Preset_Default)
 *    HsPresetNum- The slot in HSParameterSet array from where the HsPreset is to be recalled.Range
 *    (HsPreset_1 to HSPreset_Default)
 *
 *  Exit condition:
 *    Returns TRUE if the requested Preset is verified else returns FALSE.
 */
BOOLEAN VerifyPresetFromWebPage(PRESET_NUMBER PresetNum, HSPRESET_NUMBER HsPresetNum)
{
   BOOLEAN RetFlag = TRUE;
   SINT32 MinPSNum, MaxPSNum, MinHSPSNum, MaxHSPSNum;
   SINT32 RangeError = 0;
   if(Sysconfig->TweakRangeEnabled){
      MinPSNum = Preset_TweakMIN;
      MaxPSNum = Preset_TweakMAX;
      MinHSPSNum = HsPreset_TweakMIN;
      MaxHSPSNum = HsPreset_TweakMAX;
   }
   else{
      MinPSNum = Preset_MIN;
      MaxPSNum = Preset_MAX;
      MinHSPSNum = HsPreset_MIN;
      MaxHSPSNum = HsPreset_MAX;
   }
   //ActParam
   if(VERIFY_PRESET_RANGE(PresetNum, ActParam, CustomAIn1Cutoff, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn1Cutoff;
   if(VERIFY_PRESET_RANGE(PresetNum, ActParam, CustomAIn1CutoffFlag, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn1CutoffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, ActParam, CustomAIn2Cutoff, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn2Cutoff;
   if(VERIFY_PRESET_RANGE(PresetNum, ActParam, CustomAIn2CutoffFlag, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn2CutoffFlag;
   //DcpParam
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldAmplitudeLoopC1, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC1;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldAmplitudeLoopC2, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC2;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldAmplitudeLoopC1new, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC1new;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldAmplitudeLoopC2new, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC2new;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, MemoryStoreFlag, MinPSNum, MaxPSNum))
      RangeError = DcpParamMemoryStoreFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLimit, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLimit;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLimitTime, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLimitTime;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLoopI, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopI;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLoopCF, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopCF;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLoopInew, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopInew;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLoopCFnew, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopCFnew;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, UseTrueValues, MinPSNum, MaxPSNum))
      RangeError = DcpParamUseTrueValues;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldPhaseLoopP, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopP;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, FreqWindowSize, MinPSNum, MaxPSNum))
      RangeError = DcpParamFreqWindowSize;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldFreqHigh, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldFreqHigh;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldFreqLow, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldFreqLow;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, TimedSeekFlag, MinPSNum, MaxPSNum))
      RangeError = DcpParamTimedSeekFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, DcpParam, WeldRampTime, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldRampTime;
   if(VERIFY_PRESET_RANGE(PresetNum,DcpParam, WeldProfileRampTime, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldProfileRampTime;

   //InfoParam
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, CreatedDate, CREATEDATE_MIN, CREATEDATE_MAX))
      RangeError = InfoParamCreatedDate;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, CreatedHour, CREATEHOUR_MIN, CREATEHOUR_MAX))
      RangeError = InfoParamCreatedHour;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, CreatedMinute, CREATEMINUTE_MIN, CREATEMINUTE_MAX))
      RangeError = InfoParamCreatedMinute;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, CreatedMonth, CREATEMONTH_MIN, CREATEMONTH_MAX))
      RangeError = InfoParamCreatedMonth;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, CreatedYear, CREATEYEAR_MIN, CREATEYEAR_MAX))
      RangeError = InfoParamCreatedYear;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, HardwareConfig, HARDWARECONFIG_MIN, HARDWARECONFIG_MAX))
      RangeError = InfoParamHardwareConfig;

   if(VERIFY_PRESET_RANGE(PresetNum, InfoParam, HornPresetNumber, MinPSNum, MaxPSNum))
      RangeError = InfoParamHornPresetNumber;

   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, ModifiedDate, MODIFIEDDATE_MIN, MODIFIEDDATE_MAX))
      RangeError = InfoParamModifedDate;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, ModifiedHour, MODIFIEDHOUR_MIN, MODIFIEDHOUR_MAX))
      RangeError = InfoParamModifiedHour;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, ModifiedMinute, MODIFIEDMINUTE_MIN, MODIFIEDMINUTE_MAX))
      RangeError = InfoParamModifiedMinute;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, ModifiedMonth, MODIFIEDMONTH_MIN, MODIFIEDMONTH_MAX))
      RangeError = InfoParamModifiedMonth;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, ModifiedYear, MODIFIEDYEAR_MIN, MODIFIEDYEAR_MAX))
      RangeError = InfoParamModifiedYear;
   if(VERIFY_PRESET_RANGE(PresetNum, InfoParam, PowerSupplyFreq, MinPSNum, MaxPSNum))
      RangeError = InfoParamPowerSupplyFreq;
   if(VERIFY_PRESET_RANGE(PresetNum, InfoParam, PowerSupplyWatts, MinPSNum, MaxPSNum))
      RangeError = InfoParamPowerSupplyWatts;
   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, PresetStatus, NOTVERIFIED, VERIFIED))
      RangeError = InfoParamPresetStatus;
//   if(VERIFY_PRESET_RANGE_VAL(PresetNum, InfoParam, WeldCount, WELDCOUNT_MIN, WELDCOUNT_MAX))//This is required
//      RangeError = InfoParamWeldCount;

   //LimitsParam
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, ControlLimits, MinPSNum, MaxPSNum))
      RangeError = LimitsParamControlLimits;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, EnergyMLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamEnergyMLR;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, EnergyPLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamEnergyPLR;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, PowerMLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamPowerMLR;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, PowerPLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamPowerPLR;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, WeldTimeMLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamWeldTimeMLR;
   if(VERIFY_PRESET_RANGE(PresetNum, LimitsParam, WeldTimePLR, MinPSNum, MaxPSNum))
      RangeError = LimitsParamWeldTimePLR;
   //wc
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ABAmp, MinPSNum, MaxPSNum))
      RangeError = WcParamABAmp;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ABDelay, MinPSNum, MaxPSNum))
      RangeError = WcParamABDelay;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ABFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamABFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ABTime, MinPSNum, MaxPSNum))
      RangeError = WcParamABTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, Amplitude1, MinPSNum, MaxPSNum))
      RangeError = WcParamAmplitude1;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, Amplitude2, MinPSNum, MaxPSNum))
      RangeError = WcParamAmplitude2;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, AmpStepCondition, MinPSNum, MaxPSNum))
      RangeError = WcParamAmpStepCondition;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, AmpStepTime, MinPSNum, MaxPSNum))
      RangeError = WcParamAmpStepTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, AmpStepEnergy, MinPSNum, MaxPSNum))
      RangeError = WcParamAmpStepEnergy;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, AmpStepExternal, MinPSNum, MaxPSNum))
      RangeError = WcParamAmpStepExternal;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, AmpStepPower, MinPSNum, MaxPSNum))
      RangeError = WcParamAmpStepPower;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, CycleAborts, MinPSNum, MaxPSNum))
      RangeError = WcParamCycleAborts;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EBAmp, MinPSNum, MaxPSNum))
      RangeError = WcParamEBAmp;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EBFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamEBFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EBTime, MinPSNum, MaxPSNum))
      RangeError = WcParamEBTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EnergyCP, MinPSNum, MaxPSNum))
      RangeError = WcParamEnergyCP;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EnergyComp, MinPSNum, MaxPSNum))
      RangeError = WcParamEnergyComp;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EnergyCompFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamEnergyCompFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EnergyCutoff, MinPSNum, MaxPSNum))
      RangeError = WcParamEnergyCutoff;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, EnergyCutoffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamEnergyCutoffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ExtTrigDelayFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamExtTrigDelayFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ExtTrigDelayTime, MinPSNum, MaxPSNum))
      RangeError = WcParamExtTrigDelayTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, FreqOffset, MinPSNum, MaxPSNum))
      RangeError = WcParamFreqOffset;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, FreqOffsetFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamFreqOffsetFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, GeneralAlarmFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamGeneralAlarmFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, GrdDetectFlagCP, MinPSNum, MaxPSNum))
      RangeError = WcParamGrdDetectFlagCP;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, HoldTime, MinPSNum, MaxPSNum))
      RangeError = WcParamHoldTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, HoldTimeFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamHoldTimeFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, MaxWeldTime, MinPSNum, MaxPSNum))
      RangeError = WcParamMaxWeldTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, PeakPowerCP, MinPSNum, MaxPSNum))
      RangeError = WcParamPeakPowerCP;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, PeakPowerCutoff, MinPSNum, MaxPSNum))
      RangeError = WcParamPeakPowerCutoff;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, PeakPowerCutoffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamPeakPowerCutoffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, PostWeldSeekFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamPostWeldSeekFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ScrubAmp, MinPSNum, MaxPSNum))
      RangeError = WcParamScrubAmp;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ScrubTime, MinPSNum, MaxPSNum))
      RangeError = WcParamScrubTime;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, ScrubTimeFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamScrubTimeFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, TimeCutOff, MinPSNum, MaxPSNum))
      RangeError = WcParamTimeCutOff;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, TimeCutOffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamTimeCutOffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldMode, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldMode;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldTimeCP, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldTimeCP;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, CutOffEnabled, MinPSNum, MaxPSNum))
      RangeError = WcParamCutOffEnabled;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldFreqHighCutOffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqHighCutOffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldFreqLowCutOffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqLowCutOffFlag;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldFreqHighCutoff, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqHighCutoff;
   if(VERIFY_PRESET_RANGE(PresetNum, WcParam, WeldFreqLowCutoff , MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqLowCutoff;
   if (HsPresetNum >= HORNPRESETNUMBER_MIN && HsPresetNum <= HORNPRESETNUMBER_MAX){
      //HS seek
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekTime, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekTime;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekAmp, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekAmp;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekRampTime, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekRampTime;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekAmpLoopC1, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekAmpLoopC1;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekAmpLoopC2, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekAmpLoopC2;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, FreqOffset, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekFreqOffset;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekPhaseLoopI, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekPhaseLoopI;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekPhaseLoopCF, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekPhaseLoopCF;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekFreqHigh, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekFreqHigh;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaSeek, SeekFreqLow, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaSeekSeekFreqLow;
      //HS test
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, Amplitude1, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitude1;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmpStepCondition, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmpStepCondition;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmplitudeProfileTime, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitudeProfileTime;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, Amplitude2, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitude2;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmplitudeStartRampTime, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitudeStartRampTime;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmplitudeProfileRampTime, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitudeProfileRampTime;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmplitudeLoopC1, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitudeLoopC1;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, AmplitudeLoopC2, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestAmplitudeLoopC2;
//      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, FreqOffset, MinHSPSNum, MaxHSPSNum))
//         RangeError = ParaTestFreqOffset;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, PhaseLoopI, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestPhaseLoopI;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, PhaseLoopCF, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestPhaseLoopCF;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, WeldFreqHigh, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestWeldFreqHigh;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, WeldFreqLow, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestWeldFreqLow;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaTest, TimeErrorHigh, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaTestTimeErrorHigh;
      //HS scan
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, FrequencyStart, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanFrequencyStart;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, FrequencyStop, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanFrequencyStop;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, FrequencyStep, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanFrequencyStep;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, TimeDelay, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanTimeDelay;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, MaxAmplitude, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanMaxAmplitude;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, MaxCurrent, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanMaxCurrent;
      if(VERIFY_PRESET_RANGE_HSPS(HsPresetNum, ParaScan, TimeErrorHigh, MinHSPSNum, MaxHSPSNum))
         RangeError = ParaScanTimeErrorHigh;
   }
   else
      RangeError = InfoParamHornPresetNumber;

   if(RangeError > 0){
      ParameterSet[PresetNum].InfoParam.PresetStatus = NOTVERIFIED;
      //Set a global variable for alarm purpose
//      printf("\n Preset Verification Failed for Parameter %d", RangeError);
      RetFlag = FALSE;
   }
   else
      ParameterSet[PresetNum].InfoParam.PresetStatus = VERIFIED;
   CalculateAndSavePresetCRC((SAVECRC_CMD)SAVECRC_INFO, (PRESET_NUMBER)PresetNum);
   return RetFlag;
}

void InitFreqAndLoopspeedDependantLoopConstantsMinMaxDefault()
{
	switch (MacchipData.Freq){
		case FREQ_20KHZ:
			if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM){
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_DEF20K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_DEF20K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SEEK_PHASE_LOOP_STD_DEF20K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_DEF20K;


				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF20K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF20K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_DEF20K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = PI_PF_PHASE_CONTROL_LOOP_DEF20K;

				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF20K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF20K;
			}
			else//Apply S and V defaults
			{
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF20K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF20K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_DEF20K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_DEF20K;


				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF20K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF20K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_DEF20K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_DEF20K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_PI_PF_PHASE_CONTROL_LOOP_DEF20K;

				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF20K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF20K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF20K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF20K;
			}
			break;
		case FREQ_30KHZ:
			if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM){
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_DEF30K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_DEF30K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SEEK_PHASE_LOOP_STD_DEF30K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_DEF30K;

				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF30K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF30K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_DEF30K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = PI_PF_PHASE_CONTROL_LOOP_DEF30K;

				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF30K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF30K;
			}
			else//Apply S and V defaults
			{
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF30K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF30K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_DEF30K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_DEF30K;

				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF30K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF30K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_DEF30K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_DEF30K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_PI_PF_PHASE_CONTROL_LOOP_DEF30K;

				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF30K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF30K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF30K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF30K;
			}
			break;
		case FREQ_40KHZ:
			if(Sysconfig->DcpSpeedControl == SPEEDCONTROL_CUSTOM){
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C1_STD_DEF40K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SEEK_AMPLITUDE_LOOP_C2_STD_DEF40K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SEEK_PHASE_LOOP_STD_DEF40K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SEEK_PHASE_LOOP_CF_STD_DEF40K;

				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF40K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF40K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = PI_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = PI_PHASE_INTEGRAL_GAIN_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = PI_PHASE_PROPORTIONAL_GAIN_DEF40K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = PI_FREQUENCY_WINDOW_SIZE_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = PI_PF_PHASE_CONTROL_LOOP_DEF40K;


				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = STD_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_DEF40K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = STD_PHASE_LOOP_CF_DEF40K;
			}
			else//Apply S and V defaults
			{
				//Advanced Seek param
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C1_STD_DEF40K;

				FreqDependentParam[SEEKAMPLITUDELOOPC2][MINVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MIN;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_MAX;
				FreqDependentParam[SEEKAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_SEEK_AMPLITUDE_LOOP_C2_STD_DEF40K;

				FreqDependentParam[SEEKPHASELOOPI][MINVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPI][MAXVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPI][DEFAULTVALUEINDEX] = SV_SEEK_PHASE_LOOP_STD_DEF40K;

				FreqDependentParam[SEEKPHASELOOPCF][MINVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MIN;
				FreqDependentParam[SEEKPHASELOOPCF][MAXVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_MAX;
				FreqDependentParam[SEEKPHASELOOPCF][DEFAULTVALUEINDEX]= SV_SEEK_PHASE_LOOP_CF_STD_DEF40K;

				//Advanced Weld param
				FreqDependentParam[WELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[WELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[WELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[WELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[WELDPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[WELDPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[WELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF40K;

				FreqDependentParam[WELDPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[WELDPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[WELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF40K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MINVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[PIWELDAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_PI_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPI][MINVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPI][MAXVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPI][DEFAULTVALUEINDEX] = SV_PI_PHASE_INTEGRAL_GAIN_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPP][MINVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MIN;
				FreqDependentParam[PIWELDPHASELOOPP][MAXVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_MAX;
				FreqDependentParam[PIWELDPHASELOOPP][DEFAULTVALUEINDEX] = SV_PI_PHASE_PROPORTIONAL_GAIN_DEF40K;

				FreqDependentParam[PIFREQWINDOWSIZE][MINVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MIN;
				FreqDependentParam[PIFREQWINDOWSIZE][MAXVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_MAX;
				FreqDependentParam[PIFREQWINDOWSIZE][DEFAULTVALUEINDEX] = SV_PI_FREQUENCY_WINDOW_SIZE_DEF40K;

				FreqDependentParam[PIWELDPHASELOOPCF][MINVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MIN;
				FreqDependentParam[PIWELDPHASELOOPCF][MAXVALUEINDEX] = SV_PF_PHASE_CONTROL_LOOP_MAX;
				FreqDependentParam[PIWELDPHASELOOPCF][DEFAULTVALUEINDEX] = SV_PI_PF_PHASE_CONTROL_LOOP_DEF40K;

				//Advanced TEST param
				FreqDependentParam[TESTAMPLITUDELOOPC1][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC1][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC1][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C1_DEF40K;

				FreqDependentParam[TESTAMPLITUDELOOPC2][MINVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MIN;
				FreqDependentParam[TESTAMPLITUDELOOPC2][MAXVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_MAX;
				FreqDependentParam[TESTAMPLITUDELOOPC2][DEFAULTVALUEINDEX] = SV_STD_AMPLITUDE_LOOP_C2_DEF40K;

				FreqDependentParam[TESTPHASELOOPI][MINVALUEINDEX] = SV_STD_PHASE_LOOP_MIN;
				FreqDependentParam[TESTPHASELOOPI][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_MAX;
				FreqDependentParam[TESTPHASELOOPI][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_DEF40K;

				FreqDependentParam[TESTPHASELOOPCF][MINVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MIN;
				FreqDependentParam[TESTPHASELOOPCF][MAXVALUEINDEX] = SV_STD_PHASE_LOOP_CF_MAX;
				FreqDependentParam[TESTPHASELOOPCF][DEFAULTVALUEINDEX] = SV_STD_PHASE_LOOP_CF_DEF40K;
			}
			break;
	}

}

void InitFreqDependentParametersMinMaxDefault()
{
	switch (MacchipData.Freq){
		case FREQ_20KHZ:
			//Weld CutOffs
			FreqDependentParam[WELDFREQLOWCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFLOW20KHZ_MIN;
			FreqDependentParam[WELDFREQLOWCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFLOW20KHZ_MAX;
			FreqDependentParam[WELDFREQLOWCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFLOW20KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGHCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFHIGH20KHZ_MIN;
			FreqDependentParam[WELDFREQHIGHCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFHIGH20KHZ_MAX;
			FreqDependentParam[WELDFREQHIGHCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFHIGH20KHZ_DEFAULT;
			//Weld Freq Overload Limits
			FreqDependentParam[WELDFREQLOW][MINVALUEINDEX] = WELD_FREQLOW20KHZ_MIN;
			FreqDependentParam[WELDFREQLOW][MAXVALUEINDEX] = WELD_FREQLOW20KHZ_MAX;
			FreqDependentParam[WELDFREQLOW][DEFAULTVALUEINDEX] = WELD_FREQLOW20KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGH][MINVALUEINDEX] = WELD_FREQHIGH20KHZ_MIN;
			FreqDependentParam[WELDFREQHIGH][MAXVALUEINDEX] = WELD_FREQHIGH20KHZ_MAX;
			FreqDependentParam[WELDFREQHIGH][DEFAULTVALUEINDEX] = WELD_FREQHIGH20KHZ_DEFAULT;
			//Weld Freq Offset
			FreqDependentParam[WELDFREQOFFSET][MINVALUEINDEX] = FREQOFFSET20K_MIN;
			FreqDependentParam[WELDFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET20K_MAX;
			FreqDependentParam[WELDFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;
			//Seek
			FreqDependentParam[SEEKFREQLOW][MINVALUEINDEX] = SEEKFREQLOW20K_MIN;
			FreqDependentParam[SEEKFREQLOW][MAXVALUEINDEX] = SEEKFREQLOW20K_MAX;
			FreqDependentParam[SEEKFREQLOW][DEFAULTVALUEINDEX] = SEEKFREQLOW20K_DEFAULT;

			FreqDependentParam[SEEKFREQHIGH][MINVALUEINDEX] = SEEKFREQHIGH20K_MIN;
			FreqDependentParam[SEEKFREQHIGH][MAXVALUEINDEX] = SEEKFREQHIGH20K_MAX;
			FreqDependentParam[SEEKFREQHIGH][DEFAULTVALUEINDEX] = SEEKFREQHIGH20K_DEFAULT;

			FreqDependentParam[SEEKFREQOFFSET][MINVALUEINDEX] = FREQOFFSET20K_MIN;
			FreqDependentParam[SEEKFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET20K_MAX;
			FreqDependentParam[SEEKFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;


			//Test
			FreqDependentParam[TESTFREQLOW][MINVALUEINDEX] = TESTFREQLOW20KHZ_MIN;
			FreqDependentParam[TESTFREQLOW][MAXVALUEINDEX] = TESTFREQLOW20KHZ_MAX;
			FreqDependentParam[TESTFREQLOW][DEFAULTVALUEINDEX] = TESTFREQLOW20KHZ_DEFAULT;

			FreqDependentParam[TESTFREQHIGH][MINVALUEINDEX] = TESTFREQHIGH20KHZ_MIN;
			FreqDependentParam[TESTFREQHIGH][MAXVALUEINDEX] = TESTFREQHIGH20KHZ_MAX;
			FreqDependentParam[TESTFREQHIGH][DEFAULTVALUEINDEX] = TESTFREQHIGH20KHZ_DEFAULT;

			FreqDependentParam[TESTFREQOFFSET][MINVALUEINDEX] = TESTFREQOFFSET20KHZ_MIN;
			FreqDependentParam[TESTFREQOFFSET][MAXVALUEINDEX] = TESTFREQOFFSET20KHZ_MAX;
			FreqDependentParam[TESTFREQOFFSET][DEFAULTVALUEINDEX] = TESTFREQOFFSET20KHZ_DEFAULT;

			//SCAN
			FreqDependentParam[SCANFREQUENCYSTART][MINVALUEINDEX] = SCANFREQ20KSTART_MIN;
			FreqDependentParam[SCANFREQUENCYSTART][MAXVALUEINDEX] = SCANFREQ20KSTART_MAX;
			FreqDependentParam[SCANFREQUENCYSTART][DEFAULTVALUEINDEX] = STARTFREQ20K;

			FreqDependentParam[SCANFREQUENCYSTOP][MINVALUEINDEX] = SCANFREQ20KEND_MIN;
			FreqDependentParam[SCANFREQUENCYSTOP][MAXVALUEINDEX] = SCANFREQ20KEND_MAX;
			FreqDependentParam[SCANFREQUENCYSTOP][DEFAULTVALUEINDEX]  = STOPFREQ20K;

			FreqDependentParam[SCANFREQUENCYSTEP][MINVALUEINDEX] = SCANFREQ20KSTEP_MIN;
			FreqDependentParam[SCANFREQUENCYSTEP][MAXVALUEINDEX] = SCANFREQ20KSTEP_MAX;
			FreqDependentParam[SCANFREQUENCYSTEP][DEFAULTVALUEINDEX]  = FREQSTEP20K;

			FreqDependentParam[SCANTIMEDELAY][MINVALUEINDEX]  = SCANTIMEDELAY_MIN;
			FreqDependentParam[SCANTIMEDELAY][MAXVALUEINDEX]  = SCANTIMEDELAY_MAX;
			FreqDependentParam[SCANTIMEDELAY][DEFAULTVALUEINDEX]  = TIMEDELAY20K;

			FreqDependentParam[SCANMAXAMPLITUDE][MINVALUEINDEX] = SCANMAXAMP_MIN;
			FreqDependentParam[SCANMAXAMPLITUDE][MAXVALUEINDEX] = SCANMAXAMP_MAX;
			FreqDependentParam[SCANMAXAMPLITUDE][DEFAULTVALUEINDEX] = MAXAMPLITUDE20K;

			FreqDependentParam[SCANMAXCURRENT][MINVALUEINDEX] = SCANMAXCURRENT_MIN;
			FreqDependentParam[SCANMAXCURRENT][MAXVALUEINDEX] = SCANMAXCURRENT_MAX;
			FreqDependentParam[SCANMAXCURRENT][DEFAULTVALUEINDEX] = MAXCURRENT20K;
			FreqDependentParam[DIGITALTUNE][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE20K_MIN);
			FreqDependentParam[DIGITALTUNE][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE20K_MAX);
			FreqDependentParam[DIGITALTUNE][DEFAULTVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE20K_DEF);

			FreqDependentParam[MEMOFFSET][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET20K_MIN);
			FreqDependentParam[MEMOFFSET][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET20K_MAX);
			FreqDependentParam[MEMOFFSET][DEFAULTVALUEINDEX] = 0;
			break;
		case FREQ_30KHZ:
			//Weld CutOffs
			FreqDependentParam[WELDFREQLOWCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFLOW30KHZ_MIN;
			FreqDependentParam[WELDFREQLOWCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFLOW30KHZ_MAX;
			FreqDependentParam[WELDFREQLOWCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFLOW30KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGHCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFHIGH30KHZ_MIN;
			FreqDependentParam[WELDFREQHIGHCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFHIGH30KHZ_MAX;
			FreqDependentParam[WELDFREQHIGHCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFHIGH30KHZ_DEFAULT;
			//Weld Freq Overload Limits
			FreqDependentParam[WELDFREQLOW][MINVALUEINDEX] = WELD_FREQLOW30KHZ_MIN;
			FreqDependentParam[WELDFREQLOW][MAXVALUEINDEX] = WELD_FREQLOW30KHZ_MAX;
			FreqDependentParam[WELDFREQLOW][DEFAULTVALUEINDEX] = WELD_FREQLOW30KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGH][MINVALUEINDEX] = WELD_FREQHIGH30KHZ_MIN;
			FreqDependentParam[WELDFREQHIGH][MAXVALUEINDEX] = WELD_FREQHIGH30KHZ_MAX;
			FreqDependentParam[WELDFREQHIGH][DEFAULTVALUEINDEX] = WELD_FREQHIGH30KHZ_DEFAULT;
			//Weld Freq Offset
			FreqDependentParam[WELDFREQOFFSET][MINVALUEINDEX] = FREQOFFSET30K_MIN;
			FreqDependentParam[WELDFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET30K_MAX;
			FreqDependentParam[WELDFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;

			//Seek
			FreqDependentParam[SEEKFREQLOW][MINVALUEINDEX] = SEEKFREQLOW30K_MIN;
			FreqDependentParam[SEEKFREQLOW][MAXVALUEINDEX] = SEEKFREQLOW30K_MAX;
			FreqDependentParam[SEEKFREQLOW][DEFAULTVALUEINDEX] = SEEKFREQLOW30K_DEFAULT;

			FreqDependentParam[SEEKFREQHIGH][MINVALUEINDEX] = SEEKFREQHIGH30K_MIN;
			FreqDependentParam[SEEKFREQHIGH][MAXVALUEINDEX] = SEEKFREQHIGH30K_MAX;
			FreqDependentParam[SEEKFREQHIGH][DEFAULTVALUEINDEX] = SEEKFREQHIGH30K_DEFAULT;

			FreqDependentParam[SEEKFREQOFFSET][MINVALUEINDEX] = FREQOFFSET30K_MIN;
			FreqDependentParam[SEEKFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET30K_MAX;
			FreqDependentParam[SEEKFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;


			//Test
			FreqDependentParam[TESTFREQLOW][MINVALUEINDEX] = TESTFREQLOW30KHZ_MIN;
			FreqDependentParam[TESTFREQLOW][MAXVALUEINDEX] = TESTFREQLOW30KHZ_MAX;
			FreqDependentParam[TESTFREQLOW][DEFAULTVALUEINDEX] = TESTFREQLOW30KHZ_DEFAULT;

			FreqDependentParam[TESTFREQHIGH][MINVALUEINDEX] = TESTFREQHIGH30KHZ_MIN;
			FreqDependentParam[TESTFREQHIGH][MAXVALUEINDEX] = TESTFREQHIGH30KHZ_MAX;
			FreqDependentParam[TESTFREQHIGH][DEFAULTVALUEINDEX] = TESTFREQHIGH30KHZ_DEFAULT;

			FreqDependentParam[TESTFREQOFFSET][MINVALUEINDEX] = TESTFREQOFFSET30KHZ_MIN;
			FreqDependentParam[TESTFREQOFFSET][MAXVALUEINDEX] = TESTFREQOFFSET30KHZ_MAX;
			FreqDependentParam[TESTFREQOFFSET][DEFAULTVALUEINDEX] = TESTFREQOFFSET30KHZ_DEFAULT;

			//SCAN
			FreqDependentParam[SCANFREQUENCYSTART][MINVALUEINDEX] = SCANFREQ30KSTART_MIN;
			FreqDependentParam[SCANFREQUENCYSTART][MAXVALUEINDEX] = SCANFREQ30KSTART_MAX;
			FreqDependentParam[SCANFREQUENCYSTART][DEFAULTVALUEINDEX] = STARTFREQ30K;

			FreqDependentParam[SCANFREQUENCYSTOP][MINVALUEINDEX] = SCANFREQ30KEND_MIN;
			FreqDependentParam[SCANFREQUENCYSTOP][MAXVALUEINDEX] = SCANFREQ30KEND_MAX;
			FreqDependentParam[SCANFREQUENCYSTOP][DEFAULTVALUEINDEX]  = STOPFREQ30K;

			FreqDependentParam[SCANFREQUENCYSTEP][MINVALUEINDEX] = SCANFREQ30KSTEP_MIN;
			FreqDependentParam[SCANFREQUENCYSTEP][MAXVALUEINDEX] = SCANFREQ30KSTEP_MAX;
			FreqDependentParam[SCANFREQUENCYSTEP][DEFAULTVALUEINDEX]  = FREQSTEP30K;

			FreqDependentParam[SCANTIMEDELAY][MINVALUEINDEX]  = SCANTIMEDELAY_MIN;
			FreqDependentParam[SCANTIMEDELAY][MAXVALUEINDEX]  = SCANTIMEDELAY_MAX;
			FreqDependentParam[SCANTIMEDELAY][DEFAULTVALUEINDEX]  = TIMEDELAY30K;

			FreqDependentParam[SCANMAXAMPLITUDE][MINVALUEINDEX] = SCANMAXAMP_MIN;
			FreqDependentParam[SCANMAXAMPLITUDE][MAXVALUEINDEX] = SCANMAXAMP_MAX;
			FreqDependentParam[SCANMAXAMPLITUDE][DEFAULTVALUEINDEX] = MAXAMPLITUDE30K;

			FreqDependentParam[SCANMAXCURRENT][MINVALUEINDEX] = SCANMAXCURRENT_MIN;
			FreqDependentParam[SCANMAXCURRENT][MAXVALUEINDEX] = SCANMAXCURRENT_MAX;
			FreqDependentParam[SCANMAXCURRENT][DEFAULTVALUEINDEX] = MAXCURRENT30K;
			FreqDependentParam[DIGITALTUNE][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE30K_MIN);
			FreqDependentParam[DIGITALTUNE][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE30K_MAX);
			FreqDependentParam[DIGITALTUNE][DEFAULTVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE30K_DEF);

			FreqDependentParam[MEMOFFSET][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET30K_MIN);
			FreqDependentParam[MEMOFFSET][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET30K_MAX);
			FreqDependentParam[MEMOFFSET][DEFAULTVALUEINDEX] = 0;
			break;
		case FREQ_40KHZ:
			//Weld CutOffs
			FreqDependentParam[WELDFREQLOWCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFLOW40KHZ_MIN;
			FreqDependentParam[WELDFREQLOWCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFLOW40KHZ_MAX;
			FreqDependentParam[WELDFREQLOWCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFLOW40KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGHCUTOFF][MINVALUEINDEX] = WELD_FREQCUTOFFHIGH40KHZ_MIN;
			FreqDependentParam[WELDFREQHIGHCUTOFF][MAXVALUEINDEX] = WELD_FREQCUTOFFHIGH40KHZ_MAX;
			FreqDependentParam[WELDFREQHIGHCUTOFF][DEFAULTVALUEINDEX] = WELD_FREQCUTOFFHIGH40KHZ_DEFAULT;
			//Weld Freq Overload Limits
			FreqDependentParam[WELDFREQLOW][MINVALUEINDEX] = WELD_FREQLOW40KHZ_MIN;
			FreqDependentParam[WELDFREQLOW][MAXVALUEINDEX] = WELD_FREQLOW40KHZ_MAX;
			FreqDependentParam[WELDFREQLOW][DEFAULTVALUEINDEX] = WELD_FREQLOW40KHZ_DEFAULT;

			FreqDependentParam[WELDFREQHIGH][MINVALUEINDEX] = WELD_FREQHIGH40KHZ_MIN;
			FreqDependentParam[WELDFREQHIGH][MAXVALUEINDEX] = WELD_FREQHIGH40KHZ_MAX;
			FreqDependentParam[WELDFREQHIGH][DEFAULTVALUEINDEX] = WELD_FREQHIGH40KHZ_DEFAULT;
			//Weld Freq Offset
			FreqDependentParam[WELDFREQOFFSET][MINVALUEINDEX] = FREQOFFSET40K_MIN;
			FreqDependentParam[WELDFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET40K_MAX;
			FreqDependentParam[WELDFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;

			//Seek
			FreqDependentParam[SEEKFREQLOW][MINVALUEINDEX] = SEEKFREQLOW40K_MIN;
			FreqDependentParam[SEEKFREQLOW][MAXVALUEINDEX] = SEEKFREQLOW40K_MAX;
			FreqDependentParam[SEEKFREQLOW][DEFAULTVALUEINDEX] = SEEKFREQLOW40K_DEFAULT;

			FreqDependentParam[SEEKFREQHIGH][MINVALUEINDEX] = SEEKFREQHIGH40K_MIN;
			FreqDependentParam[SEEKFREQHIGH][MAXVALUEINDEX] = SEEKFREQHIGH40K_MAX;
			FreqDependentParam[SEEKFREQHIGH][DEFAULTVALUEINDEX] = SEEKFREQHIGH40K_DEFAULT;

			FreqDependentParam[SEEKFREQOFFSET][MINVALUEINDEX] = FREQOFFSET40K_MIN;
			FreqDependentParam[SEEKFREQOFFSET][MAXVALUEINDEX] = FREQOFFSET40K_MAX;
			FreqDependentParam[SEEKFREQOFFSET][DEFAULTVALUEINDEX] = FREQOFFSET_DEFAULT;
			//Test
			FreqDependentParam[TESTFREQLOW][MINVALUEINDEX] = TESTFREQLOW40KHZ_MIN;
			FreqDependentParam[TESTFREQLOW][MAXVALUEINDEX] = TESTFREQLOW40KHZ_MAX;
			FreqDependentParam[TESTFREQLOW][DEFAULTVALUEINDEX] = TESTFREQLOW40KHZ_DEFAULT;

			FreqDependentParam[TESTFREQHIGH][MINVALUEINDEX] = TESTFREQHIGH40KHZ_MIN;
			FreqDependentParam[TESTFREQHIGH][MAXVALUEINDEX] = TESTFREQHIGH40KHZ_MAX;
			FreqDependentParam[TESTFREQHIGH][DEFAULTVALUEINDEX] = TESTFREQHIGH40KHZ_DEFAULT;

			FreqDependentParam[TESTFREQOFFSET][MINVALUEINDEX] = TESTFREQOFFSET40KHZ_MIN;
			FreqDependentParam[TESTFREQOFFSET][MAXVALUEINDEX] = TESTFREQOFFSET40KHZ_MAX;
			FreqDependentParam[TESTFREQOFFSET][DEFAULTVALUEINDEX] = TESTFREQOFFSET40KHZ_DEFAULT;
			//SCAN
			FreqDependentParam[SCANFREQUENCYSTART][MINVALUEINDEX] = SCANFREQ40KSTART_MIN;
			FreqDependentParam[SCANFREQUENCYSTART][MAXVALUEINDEX] = SCANFREQ40KSTART_MAX;
			FreqDependentParam[SCANFREQUENCYSTART][DEFAULTVALUEINDEX] = STARTFREQ40K;

			FreqDependentParam[SCANFREQUENCYSTOP][MINVALUEINDEX] = SCANFREQ40KEND_MIN;
			FreqDependentParam[SCANFREQUENCYSTOP][MAXVALUEINDEX] = SCANFREQ40KEND_MAX;
			FreqDependentParam[SCANFREQUENCYSTOP][DEFAULTVALUEINDEX]  = STOPFREQ40K;

			FreqDependentParam[SCANFREQUENCYSTEP][MINVALUEINDEX] = SCANFREQ40KSTEP_MIN;
			FreqDependentParam[SCANFREQUENCYSTEP][MAXVALUEINDEX] = SCANFREQ40KSTEP_MAX;
			FreqDependentParam[SCANFREQUENCYSTEP][DEFAULTVALUEINDEX]  = FREQSTEP40K;

			FreqDependentParam[SCANTIMEDELAY][MINVALUEINDEX]  = SCANTIMEDELAY_MIN;
			FreqDependentParam[SCANTIMEDELAY][MAXVALUEINDEX]  = SCANTIMEDELAY_MAX;
			FreqDependentParam[SCANTIMEDELAY][DEFAULTVALUEINDEX]  = TIMEDELAY40K;

			FreqDependentParam[SCANMAXAMPLITUDE][MINVALUEINDEX] = SCANMAXAMP_MIN;
			FreqDependentParam[SCANMAXAMPLITUDE][MAXVALUEINDEX] = SCANMAXAMP_MAX;
			FreqDependentParam[SCANMAXAMPLITUDE][DEFAULTVALUEINDEX] = MAXAMPLITUDE40K;

			FreqDependentParam[SCANMAXCURRENT][MINVALUEINDEX] = SCANMAXCURRENT_MIN;
			FreqDependentParam[SCANMAXCURRENT][MAXVALUEINDEX] = SCANMAXCURRENT_MAX;
			FreqDependentParam[SCANMAXCURRENT][DEFAULTVALUEINDEX] = MAXCURRENT40K;

			FreqDependentParam[DIGITALTUNE][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE40K_MIN);
			FreqDependentParam[DIGITALTUNE][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE40K_MAX);
			FreqDependentParam[DIGITALTUNE][DEFAULTVALUEINDEX] = SCALE_UP_FOR_N_BITS(DIGITALTUNE40K_DEF);

			FreqDependentParam[MEMOFFSET][MINVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET40K_MIN);
			FreqDependentParam[MEMOFFSET][MAXVALUEINDEX] = SCALE_UP_FOR_N_BITS(FREQOFFSET40K_MAX);
			FreqDependentParam[MEMOFFSET][DEFAULTVALUEINDEX] = 0;

		break;
	}
	InitFreqAndLoopspeedDependantLoopConstantsMinMaxDefault();
}

/* void SetHDDefaults()
 *
 * Purpose:
 *   To set HD defaults. HD Param not used currentl but can be set/get from web page.
 *
 * Entry condition:
 *   None
 *
 * Exit condition:
 *   None.
 */
void SetHDDefaults()
{
	SINT32 Indx;
	for(Indx = 0; Indx < SIZE_INITPARAMS; Indx++){
	  ParameterSet[CurrPreset].HDVal.InitPara[Indx] = 0;
	  ParameterSet[Preset_MIN].HDVal.InitPara[Indx] = 0;
	  ParameterSet[Preset_MAX].HDVal.InitPara[Indx] = INT_MAX;
	  ParameterSet[Preset_TweakMIN].HDVal.InitPara[Indx] = 0;
	  ParameterSet[Preset_TweakMAX].HDVal.InitPara[Indx] = INT_MAX;
	}
	for(Indx = 0; Indx < SIZE_PREINITPARAMS; Indx++){
		ParameterSet[CurrPreset].HDVal.PreInitPara[Indx] = 0;
		ParameterSet[Preset_MIN].HDVal.PreInitPara[Indx] = 0;
		ParameterSet[Preset_MAX].HDVal.PreInitPara[Indx] = INT_MAX;
		ParameterSet[Preset_TweakMIN].HDVal.PreInitPara[Indx] = 0;
		ParameterSet[Preset_TweakMAX].HDVal.PreInitPara[Indx] = INT_MAX;
	}
	ParameterSet[CurrPreset].HDVal.LoopControl = Standard;
	ParameterSet[Preset_MAX].HDVal.LoopControl = HD;
	ParameterSet[Preset_TweakMIN].HDVal.LoopControl = Standard;
	ParameterSet[Preset_TweakMAX].HDVal.LoopControl = HD;
}

/* void ClearMemoryFrequencyAtPowerup(void)
 *
 * Purpose:
 *    This function is called once at powerup from PowerUpInitialization() to clear the memory
 *    frequencies stored in all horn presets.
 *
 * Entry condition:
 *    PowerUp
 *
 * Exit condition:
 *    All memory frequency gets cleared.
 */
void ClearMemoryFrequencyAtPowerup(void)
{
   UINT8 Indx = 0;
   for (Indx = 0; Indx < MAX_TOTAL_HSPRESETS; Indx++) 
      HsParameterSet[Indx].MemoryOffsetScaledUP = 0;    
}
