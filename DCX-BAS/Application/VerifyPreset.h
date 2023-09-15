/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfile/VerifyPreset.h_v   1.13   02 Sep 2016 11:53:34   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/workfile/VerifyPreset.h_v  $
 * 
 *    Rev 1.13   02 Sep 2016 11:53:34   EGUAJARDO
 * Added ParaTestAmpCondition to the HSPresetElementIDs.
 * 
 *    Rev 1.12   04 Mar 2014 07:49:26   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.11   24 Aug 2013 17:14:14   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.10   21 May 2013 12:26:02   amaurya
 * Code review fixes.
 * 
 *    Rev 1.9   06 May 2013 09:20:54   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.8   22 Apr 2013 11:33:16   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.7   15 Mar 2013 10:54:02   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.6   11 Mar 2013 02:30:20   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.5   18 Feb 2013 03:12:14   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   29 Jan 2013 02:24:32   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.3   24 Nov 2012 08:06:28   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.2   29 Oct 2012 02:57:16   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.1   05 Jul 2012 04:48:20   ygupta
 * Cleanup after review summary on database and BSP.
 */
#ifndef VERIFYPRESET_H_
#define VERIFYPRESET_H_

#include "CyGOS.h"
#include "portable.h"
#include "DEF_DEF.H"

enum PresetElementIDs
{
	ActParamCustomAIn1Cutoff = 1,
	ActParamCustomAIn1CutoffFlag,
   ActParamCustomAIn2Cutoff,
   ActParamCustomAIn2CutoffFlag,
	DcpParamWeldAmplitudeLoopC1,
	DcpParamWeldAmplitudeLoopC2,
   DcpParamWeldAmplitudeLoopC1new,
   DcpParamWeldAmplitudeLoopC2new,
	DcpParamMemoryStoreFlag,  //9
	DcpParamWeldPhaseLimit,
	DcpParamWeldPhaseLimitTime,
	DcpParamWeldPhaseLoopI,
	DcpParamWeldPhaseLoopCF,
   DcpParamWeldPhaseLoopInew,
   DcpParamWeldPhaseLoopCFnew,
	DcpParamUseTrueValues,    //16
	DcpParamWeldPhaseLoopP,
	DcpParamFreqWindowSize,
	DcpParamWeldFreqHigh,
	DcpParamWeldFreqLow,
	DcpParamTimedSeekFlag,
	DcpParamWeldRampTime,  //22
	DcpParamWeldProfileRampTime,//23
	//info
	InfoParamCreatedDate,
	InfoParamCreatedHour,
	InfoParamCreatedMinute,
	InfoParamCreatedMonth, //27
	InfoParamCreatedYear,
	InfoParamHardwareConfig,
	InfoParamHornPresetNumber,
	InfoParamModifedDate,
	InfoParamModifiedHour, //32
	InfoParamModifiedMinute,
	InfoParamModifiedMonth,
	InfoParamModifiedYear,
	InfoParamOperatorID,
	InfoParamPowerSupplyFreq, //37
	InfoParamPowerSupplyWatts,
	InfoParamPresetStatus,
	InfoParamWeldCount,
	//limit
	LimitsParamControlLimits,
	LimitsParamEnergyMLR,  //42
	LimitsParamEnergyPLR,
//	LimitsParamFreqMLR,
//	LimitsParamFreqPLR,
	LimitsParamPowerMLR,
	LimitsParamPowerPLR,   //45
	LimitsParamWeldTimeMLR,
	LimitsParamWeldTimePLR,
	//wc
	WcParamABAmp,
	WcParamABDelay,
	WcParamABFlag,   //50
	WcParamABTime,
	WcParamAmplitude1,
	WcParamAmplitude2,
	WcParamAmpStepCondition,
	WcParamAmpStepTime, //55
	WcParamAmpStepEnergy,
	WcParamAmpStepExternal,
	WcParamAmpStepPower,
	WcParamCycleAborts,
	WcParamEBAmp, //60
	WcParamEBFlag,
	WcParamEBTime,
	WcParamEnergyCP,
	WcParamEnergyComp,
	WcParamEnergyCompFlag, //65
	WcParamEnergyCutoff,
	WcParamEnergyCutoffFlag,
	WcParamExtTrigDelayFlag,
	WcParamExtTrigDelayTime,
	WcParamFreqOffset,  //70
	WcParamFreqOffsetFlag,
	WcParamGeneralAlarmFlag,
	WcParamGrdDetectFlagCP,
	WcParamHoldTime,
	WcParamHoldTimeFlag,   //75
	WcParamMaxWeldTime,
	WcParamPeakPowerCP,
	WcParamPeakPowerCutoff,
	WcParamPeakPowerCutoffFlag,
	WcParamPostWeldSeekFlag,  //80

	WcParamScrubAmp = 85, //85
	WcParamScrubTime,
	WcParamScrubTimeFlag,
	WcParamTimeCutOff,
	WcParamTimeCutOffFlag,
	WcParamWeldMode, //90
	WcParamWeldTimeCP,
	WcParamCutOffEnabled,
	WcParamWeldFreqHighCutOffFlag,
	WcParamWeldFreqLowCutOffFlag,
	WcParamWeldFreqHighCutoff = 96,
	WcParamWeldFreqLowCutoff,//97
};

enum HSPresetElementIDs
{
	//hs seek
	ParaSeekSeekTime = WcParamWeldFreqLowCutoff + 1,//98
	ParaSeekSeekAmp,
	ParaSeekSeekRampTime,
	ParaSeekSeekAmpLoopC1,
	ParaSeekSeekAmpLoopC2,
	ParaSeekFreqOffset,
	ParaSeekSeekPhaseLoopI,
	ParaSeekSeekPhaseLoopCF,//105
	ParaSeekSeekFreqHigh,
	ParaSeekSeekFreqLow,
	//hs test
	ParaTestAmplitude1,
	ParaTestAmpStepCondition,
	ParaTestAmplitudeProfileTime,
	ParaTestAmplitude2,//111
	ParaTestAmplitudeStartRampTime,
	ParaTestAmplitudeProfileRampTime,
	ParaTestAmplitudeLoopC1,
	ParaTestAmplitudeLoopC2,//115
	ParaTestFreqOffset,
	ParaTestPhaseLoopI,
	ParaTestPhaseLoopCF,//118
	ParaTestWeldFreqHigh,
	ParaTestWeldFreqLow,
	ParaTestTimeErrorHigh,
	ParaTestAmpCondition,
	//hs scan
	ParaScanFrequencyStart,//122
	ParaScanFrequencyStop,
	ParaScanFrequencyStep,
	ParaScanTimeDelay,//125
	ParaScanMaxAmplitude,
	ParaScanMaxCurrent,
	ParaScanTimeErrorHigh,//128
	DigitalTune,

};

BOOLEAN VerifyPreset(BOOLEAN ChangePresetStatus = FALSE);

#endif /* VERIFYPRESET_H_ */
