/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfile/VerifyPreset.cpp_v   1.19   13 Mar 2014 15:55:12   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfile/VerifyPreset.cpp_v  $
 * 
 *    Rev 1.19   13 Mar 2014 15:55:12   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.18   08 Oct 2013 06:17:30   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.17   04 Sep 2013 13:11:44   rjamloki
 * Removed Test Offset
 * 
 *    Rev 1.16   24 Aug 2013 17:14:14   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.15   31 May 2013 10:37:20   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.14   21 May 2013 12:26:02   amaurya
 * Code review fixes.
 * 
 *    Rev 1.12   23 Apr 2013 10:24:48   ygupta
 * Fixed issue of System Test page on website.
 * 
 *    Rev 1.11   22 Apr 2013 11:33:04   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.10   15 Mar 2013 10:53:50   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.9   11 Mar 2013 02:30:06   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.8   18 Feb 2013 03:12:02   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.7   13 Dec 2012 00:13:38   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.6   24 Nov 2012 08:06:10   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   29 Oct 2012 02:57:02   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.4   26 Oct 2012 02:41:18   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   10 Sep 2012 03:13:40   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.2   05 Jul 2012 04:48:08   ygupta
 * Cleanup after review summary on database and BSP.

*/

#include "VerifyPreset.h"
#include "preset.h"
#include "GLOBALS.h"

//Macro to check that preset parameter is with in range
#define IN_RANGE(Psstruct , Param , MinPs, MaxPs) \
((RunningPreset.Psstruct.Param >= ParameterSet[MinPs].Psstruct.Param) && \
(RunningPreset.Psstruct.Param <= ParameterSet[MaxPs].Psstruct.Param))

#define IN_RANGE_HSPS(Hspsstruct, Param, MinPs, MaxPs) \
((RunningHSPreset.Hspsstruct.Param >= HsParameterSet[MinPs].Hspsstruct.Param) && \
(RunningHSPreset.Hspsstruct.Param  <= HsParameterSet[MaxPs].Hspsstruct.Param))

#define NOT_IN_RANGE(Psstruct , Param , MinPs, MaxPs) \
((RunningPreset.Psstruct.Param < ParameterSet[MinPs].Psstruct.Param) || \
(RunningPreset.Psstruct.Param > ParameterSet[MaxPs].Psstruct.Param))

#define NOT_IN_RANGE_HSPS(Hspsstruct, Param, MinPs, MaxPs) \
((RunningHSPreset.Hspsstruct.Param < HsParameterSet[MinPs].Hspsstruct.Param) || \
 (RunningHSPreset.Hspsstruct.Param  > HsParameterSet[MaxPs].Hspsstruct.Param))

#define NOT_IN_RANGE_VAL(Psstruct , Param , MinVal, MaxVal) \
((RunningPreset.Psstruct.Param < MinVal) || \
(RunningPreset.Psstruct.Param > MaxVal))

/*
 * 	 Purpose:
 *		This function is called to check the boundary condition of RunningPreset
 *
 *
 *   Entry condition:
 *		ChangePresetStatus: Whether or not to set the preset status after successful verification
 *   Exit condition:
 *
 */
BOOLEAN VerifyPreset(BOOLEAN ChangePresetStatus)
{
	//, from stafuncyion
	BOOLEAN RetFlag = TRUE;
	SINT32 MinPSNum, MaxPSNum, MinHSPSNum, MaxHSPSNum,
	PresetVerifyError = 0;
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
	//Act
	if(NOT_IN_RANGE(ActParam, CustomAIn1Cutoff, MinPSNum, MaxPSNum))
		RangeError = ActParamCustomAIn1Cutoff;
	if(NOT_IN_RANGE(ActParam, CustomAIn1CutoffFlag, MinPSNum, MaxPSNum))
		RangeError = ActParamCustomAIn1CutoffFlag;
   if(NOT_IN_RANGE(ActParam, CustomAIn2Cutoff, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn2Cutoff;
   if(NOT_IN_RANGE(ActParam, CustomAIn2CutoffFlag, MinPSNum, MaxPSNum))
      RangeError = ActParamCustomAIn2CutoffFlag;
	//dcp
	if(NOT_IN_RANGE(DcpParam, WeldAmplitudeLoopC1, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldAmplitudeLoopC1;
	if(NOT_IN_RANGE(DcpParam, WeldAmplitudeLoopC2, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldAmplitudeLoopC2;
   if(NOT_IN_RANGE(DcpParam, WeldAmplitudeLoopC1new, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC1new;
   if(NOT_IN_RANGE(DcpParam, WeldAmplitudeLoopC2new, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldAmplitudeLoopC2new;
	if(NOT_IN_RANGE(DcpParam, MemoryStoreFlag, MinPSNum, MaxPSNum))
		RangeError = DcpParamMemoryStoreFlag;
	if(NOT_IN_RANGE(DcpParam, WeldPhaseLimit, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldPhaseLimit;
	if(NOT_IN_RANGE(DcpParam, WeldPhaseLimitTime, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldPhaseLimitTime;
	if(NOT_IN_RANGE(DcpParam, WeldPhaseLoopI, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldPhaseLoopI;
	if(NOT_IN_RANGE(DcpParam, WeldPhaseLoopCF, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldPhaseLoopCF;
   if(NOT_IN_RANGE(DcpParam, WeldPhaseLoopInew, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopInew;
   if(NOT_IN_RANGE(DcpParam, WeldPhaseLoopCFnew, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldPhaseLoopCFnew;
	if(NOT_IN_RANGE(DcpParam, UseTrueValues, MinPSNum, MaxPSNum))
		RangeError = DcpParamUseTrueValues;
	if(NOT_IN_RANGE(DcpParam, WeldPhaseLoopP, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldPhaseLoopP;
	if(NOT_IN_RANGE(DcpParam, FreqWindowSize, MinPSNum, MaxPSNum))
		RangeError = DcpParamFreqWindowSize;
   if(NOT_IN_RANGE(DcpParam, WeldFreqHigh, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldFreqHigh;
   if(NOT_IN_RANGE(DcpParam, WeldFreqLow, MinPSNum, MaxPSNum))
      RangeError = DcpParamWeldFreqLow;
	if(NOT_IN_RANGE(DcpParam, TimedSeekFlag, MinPSNum, MaxPSNum))
		RangeError = DcpParamTimedSeekFlag;
	if(NOT_IN_RANGE(DcpParam, WeldRampTime, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldRampTime;
	if(NOT_IN_RANGE(DcpParam, WeldProfileRampTime, MinPSNum, MaxPSNum))
		RangeError = DcpParamWeldProfileRampTime;
	//info
	if(NOT_IN_RANGE_VAL(InfoParam, CreatedDate, CREATEDATE_MIN, CREATEDATE_MAX))
		RangeError = InfoParamCreatedDate;
	if(NOT_IN_RANGE_VAL(InfoParam, CreatedHour, CREATEHOUR_MIN, CREATEHOUR_MAX))
		RangeError = InfoParamCreatedHour;
	if(NOT_IN_RANGE_VAL(InfoParam, CreatedMinute, CREATEMINUTE_MIN, CREATEMINUTE_MAX))
		RangeError = InfoParamCreatedMinute;
	if(NOT_IN_RANGE_VAL(InfoParam, CreatedMonth, CREATEMONTH_MIN, CREATEMONTH_MAX))
		RangeError = InfoParamCreatedMonth;
	if(NOT_IN_RANGE_VAL(InfoParam, CreatedYear, CREATEYEAR_MIN, CREATEYEAR_MAX))
		RangeError = InfoParamCreatedYear;
	if(NOT_IN_RANGE_VAL(InfoParam, HardwareConfig, HARDWARECONFIG_MIN, HARDWARECONFIG_MAX))
		RangeError = InfoParamHardwareConfig;

	if(NOT_IN_RANGE(InfoParam, HornPresetNumber, MinPSNum, MaxPSNum))
		RangeError = InfoParamHornPresetNumber;

	if(NOT_IN_RANGE_VAL(InfoParam, ModifiedDate, MODIFIEDDATE_MIN, MODIFIEDDATE_MAX))
		RangeError = InfoParamModifedDate;
	if(NOT_IN_RANGE_VAL(InfoParam, ModifiedHour, MODIFIEDHOUR_MIN, MODIFIEDHOUR_MAX))
		RangeError = InfoParamModifiedHour;
	if(NOT_IN_RANGE_VAL(InfoParam, ModifiedMinute, MODIFIEDMINUTE_MIN, MODIFIEDMINUTE_MAX))
		RangeError = InfoParamModifiedMinute;
	if(NOT_IN_RANGE_VAL(InfoParam, ModifiedMonth, MODIFIEDMONTH_MIN, MODIFIEDMONTH_MAX))
		RangeError = InfoParamModifiedMonth;
	if(NOT_IN_RANGE_VAL(InfoParam, ModifiedYear, MODIFIEDYEAR_MIN, MODIFIEDYEAR_MAX))
		RangeError = InfoParamModifiedYear;
	if(NOT_IN_RANGE(InfoParam, PowerSupplyFreq, MinPSNum, MaxPSNum))
		RangeError = InfoParamPowerSupplyFreq;
	if(NOT_IN_RANGE(InfoParam, PowerSupplyWatts, MinPSNum, MaxPSNum))
		RangeError = InfoParamPowerSupplyWatts;
	if(NOT_IN_RANGE_VAL(InfoParam, PresetStatus, NOTVERIFIED, VERIFIED))
		RangeError = InfoParamPresetStatus;
//	if(NOT_IN_RANGE_VAL(InfoParam, WeldCount, WELDCOUNT_MIN, WELDCOUNT_MAX)) //This is required
//		RangeError = InfoParamWeldCount;
	//limit
	if(NOT_IN_RANGE(LimitsParam, ControlLimits, MinPSNum, MaxPSNum))
		RangeError = LimitsParamControlLimits;
	if(NOT_IN_RANGE(LimitsParam, EnergyMLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamEnergyMLR;
	if(NOT_IN_RANGE(LimitsParam, EnergyPLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamEnergyPLR;
	if(NOT_IN_RANGE(LimitsParam, PowerMLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamPowerMLR;
	if(NOT_IN_RANGE(LimitsParam, PowerPLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamPowerPLR;
//	if(NOT_IN_RANGE(LimitsParam, FreqMLR, MinPSNum, MaxPSNum))
//		RangeError = LimitsParamFreqMLR;
//	if(NOT_IN_RANGE(LimitsParam, FreqPLR, MinPSNum, MaxPSNum))
//		RangeError = LimitsParamFreqPLR;
	if(NOT_IN_RANGE(LimitsParam, WeldTimeMLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamWeldTimeMLR;
	if(NOT_IN_RANGE(LimitsParam, WeldTimePLR, MinPSNum, MaxPSNum))
		RangeError = LimitsParamWeldTimePLR;
	//wc
	if(NOT_IN_RANGE(WcParam, ABAmp, MinPSNum, MaxPSNum))
		RangeError = WcParamABAmp;
	if(NOT_IN_RANGE(WcParam, ABDelay, MinPSNum, MaxPSNum))
		RangeError = WcParamABDelay;
	if(NOT_IN_RANGE(WcParam, ABFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamABFlag;
	if(NOT_IN_RANGE(WcParam, ABTime, MinPSNum, MaxPSNum))
		RangeError = WcParamABTime;
	if(NOT_IN_RANGE(WcParam, Amplitude1, MinPSNum, MaxPSNum))
		RangeError = WcParamAmplitude1;
	if(NOT_IN_RANGE(WcParam, Amplitude2, MinPSNum, MaxPSNum))
		RangeError = WcParamAmplitude2;
	if(NOT_IN_RANGE(WcParam, AmpStepCondition, MinPSNum, MaxPSNum))
		RangeError = WcParamAmpStepCondition;
	if(NOT_IN_RANGE(WcParam, AmpStepTime, MinPSNum, MaxPSNum))
		RangeError = WcParamAmpStepTime;
	if(NOT_IN_RANGE(WcParam, AmpStepEnergy, MinPSNum, MaxPSNum))
		RangeError = WcParamAmpStepEnergy;
	if(NOT_IN_RANGE(WcParam, AmpStepExternal, MinPSNum, MaxPSNum))
		RangeError = WcParamAmpStepExternal;
	if(NOT_IN_RANGE(WcParam, AmpStepPower, MinPSNum, MaxPSNum))
		RangeError = WcParamAmpStepPower;
	if(NOT_IN_RANGE(WcParam, CycleAborts, MinPSNum, MaxPSNum))
		RangeError = WcParamCycleAborts;
	if(NOT_IN_RANGE(WcParam, EBAmp, MinPSNum, MaxPSNum))
		RangeError = WcParamEBAmp;
	if(NOT_IN_RANGE(WcParam, EBFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamEBFlag;
	if(NOT_IN_RANGE(WcParam, EBTime, MinPSNum, MaxPSNum))
		RangeError = WcParamEBTime;
	if(NOT_IN_RANGE(WcParam, EnergyCP, MinPSNum, MaxPSNum))
		RangeError = WcParamEnergyCP;
	if(NOT_IN_RANGE(WcParam, EnergyComp, MinPSNum, MaxPSNum))
		RangeError = WcParamEnergyComp;
	if(NOT_IN_RANGE(WcParam, EnergyCompFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamEnergyCompFlag;
	if(NOT_IN_RANGE(WcParam, EnergyCutoff, MinPSNum, MaxPSNum))
		RangeError = WcParamEnergyCutoff;
	if(NOT_IN_RANGE(WcParam, EnergyCutoffFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamEnergyCutoffFlag;
	if(NOT_IN_RANGE(WcParam, ExtTrigDelayFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamExtTrigDelayFlag;
	if(NOT_IN_RANGE(WcParam, ExtTrigDelayTime, MinPSNum, MaxPSNum))
		RangeError = WcParamExtTrigDelayTime;
	if(NOT_IN_RANGE(WcParam, FreqOffset, MinPSNum, MaxPSNum))
		RangeError = WcParamFreqOffset;
	if(NOT_IN_RANGE(WcParam, FreqOffsetFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamFreqOffsetFlag;
	if(NOT_IN_RANGE(WcParam, GeneralAlarmFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamGeneralAlarmFlag;
	if(NOT_IN_RANGE(WcParam, GrdDetectFlagCP, MinPSNum, MaxPSNum))
		RangeError = WcParamGrdDetectFlagCP;
	if(NOT_IN_RANGE(WcParam, HoldTime, MinPSNum, MaxPSNum))
		RangeError = WcParamHoldTime;
	if(NOT_IN_RANGE(WcParam, HoldTimeFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamHoldTimeFlag;
	if(NOT_IN_RANGE(WcParam, MaxWeldTime, MinPSNum, MaxPSNum))
		RangeError = WcParamMaxWeldTime;
	if(NOT_IN_RANGE(WcParam, PeakPowerCP, MinPSNum, MaxPSNum))
		RangeError = WcParamPeakPowerCP;
	if(NOT_IN_RANGE(WcParam, PeakPowerCutoff, MinPSNum, MaxPSNum))
		RangeError = WcParamPeakPowerCutoff;
	if(NOT_IN_RANGE(WcParam, PeakPowerCutoffFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamPeakPowerCutoffFlag;
	if(NOT_IN_RANGE(WcParam, PostWeldSeekFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamPostWeldSeekFlag;
	if(NOT_IN_RANGE(WcParam, ScrubAmp, MinPSNum, MaxPSNum))
		RangeError = WcParamScrubAmp;
	if(NOT_IN_RANGE(WcParam, ScrubTime, MinPSNum, MaxPSNum))
		RangeError = WcParamScrubTime;
	if(NOT_IN_RANGE(WcParam, ScrubTimeFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamScrubTimeFlag;
	if(NOT_IN_RANGE(WcParam, TimeCutOff, MinPSNum, MaxPSNum))
		RangeError = WcParamTimeCutOff;
	if(NOT_IN_RANGE(WcParam, TimeCutOffFlag, MinPSNum, MaxPSNum))
		RangeError = WcParamTimeCutOffFlag;
	if(NOT_IN_RANGE(WcParam, WeldMode, MinPSNum, MaxPSNum))
		RangeError = WcParamWeldMode;
	if(NOT_IN_RANGE(WcParam, WeldTimeCP, MinPSNum, MaxPSNum))
		RangeError = WcParamWeldTimeCP;
	if(NOT_IN_RANGE(WcParam, CutOffEnabled, MinPSNum, MaxPSNum))
		RangeError = WcParamCutOffEnabled;
   if(NOT_IN_RANGE(WcParam, WeldFreqHighCutOffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqHighCutOffFlag;
   if(NOT_IN_RANGE(WcParam, WeldFreqLowCutOffFlag, MinPSNum, MaxPSNum))
      RangeError = WcParamWeldFreqLowCutOffFlag;
	if(NOT_IN_RANGE(WcParam, WeldFreqHighCutoff, MinPSNum, MaxPSNum))
	      RangeError = WcParamWeldFreqHighCutoff;
	if(NOT_IN_RANGE(WcParam, WeldFreqLowCutoff , MinPSNum, MaxPSNum))
	      RangeError = WcParamWeldFreqLowCutoff;
	//hs seek
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekTime, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekTime;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekAmp, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekAmp;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekRampTime, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekRampTime;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekAmpLoopC1, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekAmpLoopC1;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekAmpLoopC2, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekAmpLoopC2;
	if(NOT_IN_RANGE_HSPS(ParaSeek, FreqOffset, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekFreqOffset;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekPhaseLoopI, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekPhaseLoopI;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekPhaseLoopCF, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekPhaseLoopCF;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekFreqHigh, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekFreqHigh;
	if(NOT_IN_RANGE_HSPS(ParaSeek, SeekFreqLow, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaSeekSeekFreqLow;
	//hs test
	if(NOT_IN_RANGE_HSPS(ParaTest, Amplitude1, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitude1;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmpStepCondition, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmpStepCondition;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmplitudeProfileTime, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitudeProfileTime;
	if(NOT_IN_RANGE_HSPS(ParaTest, Amplitude2, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitude2;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmplitudeStartRampTime, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitudeStartRampTime;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmplitudeProfileRampTime, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitudeProfileRampTime;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmplitudeLoopC1, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitudeLoopC1;
	if(NOT_IN_RANGE_HSPS(ParaTest, AmplitudeLoopC2, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestAmplitudeLoopC2;
//	if(NOT_IN_RANGE_HSPS(ParaTest, FreqOffset, MinHSPSNum, MaxHSPSNum))
//		RangeError = ParaTestFreqOffset;
	if(NOT_IN_RANGE_HSPS(ParaTest, PhaseLoopI, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestPhaseLoopI;
	if(NOT_IN_RANGE_HSPS(ParaTest, PhaseLoopCF, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestPhaseLoopCF;
	if(NOT_IN_RANGE_HSPS(ParaTest, WeldFreqHigh, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestWeldFreqHigh;
	if(NOT_IN_RANGE_HSPS(ParaTest, WeldFreqLow, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestWeldFreqLow;
	if(NOT_IN_RANGE_HSPS(ParaTest, TimeErrorHigh, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaTestTimeErrorHigh;
	//hs scan
	if(NOT_IN_RANGE_HSPS(ParaScan, FrequencyStart, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanFrequencyStart;
	if(NOT_IN_RANGE_HSPS(ParaScan, FrequencyStop, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanFrequencyStop;
	if(NOT_IN_RANGE_HSPS(ParaScan, FrequencyStep, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanFrequencyStep;
	if(NOT_IN_RANGE_HSPS(ParaScan, TimeDelay, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanTimeDelay;
	if(NOT_IN_RANGE_HSPS(ParaScan, MaxAmplitude, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanMaxAmplitude;
	if(NOT_IN_RANGE_HSPS(ParaScan, MaxCurrent, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanMaxCurrent;
	if(NOT_IN_RANGE_HSPS(ParaScan, TimeErrorHigh, MinHSPSNum, MaxHSPSNum))
		RangeError = ParaScanTimeErrorHigh;

	if(RangeError > 0){
		//Set a global variable for alarm purpose
//		printf("\n preset verification failed for parameter %d", RangeError);
		PresetVerifyError = RangeError;
		Log::WriteEventLog(EVENT_PSVERIFY_FAIL, PresetVerifyError);
		RetFlag = FALSE;
		Sysconfig->PresetChanged = TRUE;
	}
	else{
		ParameterSet[CurrPreset].InfoParam.PresetStatus = VERIFIED;
		RunningPreset.InfoParam.PresetStatus = VERIFIED;
		CalculateAndSavePresetCRC(SAVECRC_INFO);
		if((Sysconfig->PresetChanged == FALSE) && (ChangePresetStatus == TRUE)){
			ParameterSet[Sysconfig->ActivePreset] = ParameterSet[CurrPreset];
			RunningPreset = ParameterSet[Sysconfig->ActivePreset];
			RunningHSPreset = HsParameterSet[CurrHsPreset];
		}
	}
	return RetFlag;
}
