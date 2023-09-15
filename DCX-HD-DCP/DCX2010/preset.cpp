/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/preset.cpp_v   1.27.1.3.1.0.1.2.1.2   Jul 07 2017 13:00:42   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/preset.cpp_v  $
 * 
 *    Rev 1.27.1.3.1.0.1.2.1.2   Jul 07 2017 13:00:42   hasanchez
 * Updated to make the code more readable
 * 
 *    Rev 1.27.1.3.1.0.1.2.1.1   10 May 2016 17:08:38   hasanchez
 * Updated default to of power up function to POWERUPTEST
 * 
 *    Rev 1.27.1.3.1.0.1.2.1.0   17 Mar 2016 16:23:42   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.27.1.3.1.0.1.3   17 Mar 2016 16:07:22   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.27.1.3.1.0.1.2   19 Sep 2014 07:37:34   amaurya
 * Added Initial FPGA-Parameters for 30 kHz units
 * 
 *    Rev 1.27.1.3.1.0.1.1   02 Sep 2014 07:00:12   amaurya
 * Changed for SeekOffset and Digital Tune.
 * 
 *    Rev 1.27.1.3.1.0.1.0   04 Jun 2014 07:45:22   amaurya
 * Added Default values for 30 KH HD
 * 
 *    Rev 1.27.1.3.1.0   18 Mar 2013 08:12:44   amaurya
 * Added F-Limit on Advanced-Preset.html.
 * 
 *    Rev 1.27.1.4   18 Mar 2013 07:26:30   amaurya
 * Added F-Limit on Advanced-Preset.html.
 * 
 *    Rev 1.27.1.3   13 Feb 2013 09:20:08   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.29   15 Jan 2013 04:41:06   amaurya
 * Energy Special.
 * 
 *    Rev 1.27.1.1   30 Mar 2012 06:42:34   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.27.1.0   28 Mar 2012 11:13:40   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.27   12 Dec 2011 11:12:58   tayars
 * Changed minimum parameters for some advanced presets
 * 
 *    Rev 1.26   16 Sep 2011 09:46:06   RatneshA
 * counters get cleared from manufacturing page only and not from coldstart.
 * 
 *    Rev 1.25   16 Sep 2011 08:22:30   RatneshA
 * changed function name
 * 
 *    Rev 1.24   28 Aug 2011 11:42:26   ygupta
 * Ver1.01M
 * 
 *    Rev 1.23   16 Aug 2011 08:26:02   pdwivedi
 * code cleanup
 * 
 *    Rev 1.22   05 Aug 2011 10:33:24   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.21   02 Aug 2011 07:38:44   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.20   22 Jul 2011 16:54:18   tayars
 * Added New Control Loops
 * 
 *    Rev 1.19   18 Jul 2011 10:38:10   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.18   15 Jul 2011 09:47:02   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.17   13 Jun 2011 09:24:20   PDwivedi
 * code commented for 15 and 60K PS
 * 
 *    Rev 1.16   03 Jun 2011 05:46:00   PDwivedi
 * EnableWebIO flag should remain true on restore weld presets from web
 * 
 *    Rev 1.15   02 May 2011 09:27:48   ygupta
 * Manufacturing Bug fixes
 * 
 *    Rev 1.14   27 Apr 2011 10:47:40   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.13   25 Apr 2011 08:44:42   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.12   25 Apr 2011 06:33:50   PDwivedi
 * Restore default should not modify the frquency,power and serial number
 * 
 *    Rev 1.11   21 Apr 2011 11:07:44   PDwivedi
 * modified Ver1.0T for tracker issues
 * 
 *    Rev 1.10   19 Apr 2011 10:47:58   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.9   14 Apr 2011 06:22:34   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.8   04 Apr 2011 11:10:24   PDwivedi
 * Added changes for Manufacturing Web page.
 * 
 *    Rev 1.7   02 Mar 2011 08:25:08   PDwivedi
 * Added default value for AlarmResetReq to 1.
 * 
 *    Rev 1.6   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.5   14 Dec 2010 11:15:12   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.4   11 Dec 2010 05:24:30   PDwivedi
 * Modified SetDefaultValues function to set powerup scan and Memory store at weld end as default options.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:24   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   12 Nov 2010 10:45:50   nhahmed
 * Added ColdStart function and changed  default values.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#include "Globals.h"
#include "string.h"
#include "StateMachineDrv.h"
#include "Typ_Def.h"
#include "FUNC.h"
#include "portable.h"
#include "stdio.h"
#include "LOG.h"
#include "UserIO.h"
#include "DupsTask.h"
#include "CRC.h"
#include "SysConfig.h"
#include "preset.h"
#include "ReceiveCommandHandler.h"

#define IN_RANGE(value)   ((RunningPreset.value >= Preset_Min.value) && (RunningPreset.value <= Preset_Max.value))
#define IN_RANGE_HDINITPARAM(value)   ((RunningPreset.HDParam.value >= Preset_Min.HDParam.value) && (RunningPreset.HDParam.value <= Preset_Max.HDParam.value))

/*---------------------------- Global variables -------------------------------*/
Parameter Preset_Min;
Parameter Preset_Max;
BOOLEAN IOTaskStop = FALSE;

BOOLEAN SavePreset(SINT32 PresetNum)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will save preset specified by PresetNum to FRAM.           */
/*                                                                           */
/*****************************************************************************/
{
   SINT32 SavePresetNo = PresetNum;
   BOOLEAN ReturnValue = FALSE;
   if(((SavePresetNo)>= 0) && ((SavePresetNo)< MAX_WORK_NUMBER))
   {
      SaveCurrentPresetCRC();
   	  //Write the parameters to given memory.
      storedPreset[SavePresetNo] = CurrentPreset;
      PresetVerifiedRequired = TRUE;
      UpdateRPreset = TRUE;
      ReturnValue = TRUE;
      InitParamUpdateRequired = TRUE;
   }
   else
   	ReturnValue = FALSE; //Return failure.

   return ReturnValue;
}

BOOLEAN RecallPreset(SINT32 PresetNum)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will recall preset specified by PresetNum to RunningPreset.*/
/*                                                                           */
/*****************************************************************************/
{
	BOOLEAN ReturnValue = FALSE;

	if(PresetNum < MAX_WORK_NUMBER)
	{
     	StatusSlot[PresetNum] = CurrentDCPStatus;
     	CurrentWorkNumber = PresetNum;
        //Copy preset to current preset.
     	CurrentPreset = storedPreset[CurrentWorkNumber];
      CurrentDCPStatus=StatusSlot[CurrentWorkNumber];
     	
     	CheckCurrentPresetCRC();
     	pprintf("\n preset recalled");
     	//Log::WriteEventLog(EVENT_PRESET_RECALL,0,0);
     	ReturnValue = TRUE;
	}
	else
		ReturnValue = FALSE;

	return ReturnValue;
}

BOOLEAN VerifyPreset()
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will verify current preset values against maximum and      */
/*  minimum preset values.                                                   */
/*                                                                           */
/*****************************************************************************/
{
	BOOLEAN PresetVerifiedFlag;
	//	check for min-max level for all the variables here
//	if(minimum preset value <= preset variable's value <= maximum preset value)
//	set PresetVerifiedFlag = true;
//	else set PresetVerifiedFlag = false;
   if (IN_RANGE(WeldAmplitude) && IN_RANGE(WeldAmplitudeLoopC1) &&
	  IN_RANGE(WeldAmplitudeLoopC2) && IN_RANGE(WeldRampTime) &&
      IN_RANGE(WeldPhaseLimit) && IN_RANGE(WeldPhaseLimitTime) &&
      IN_RANGE(WeldPhaseloopI) && IN_RANGE(WeldPhaseloopCF) &&
      IN_RANGE(SeekAmplitude) && IN_RANGE(SeekAmplitudeLoopC1) &&
      IN_RANGE(SeekAmplitudeLoopC2) && IN_RANGE(SeekPhaseloopI) &&
      IN_RANGE(SeekPhaseloopCF) && IN_RANGE(SeekRampTime) &&
      IN_RANGE(SeekTime)&& IN_RANGE(SeekFrequLimitHigh)&&
      IN_RANGE(SeekFrequLimitLow)&& IN_RANGE(WeldFrequencyLimitHigh)&&
      IN_RANGE(WeldFrequencyLimitLow) && IN_RANGE(SeekFreqOffset) && IN_RANGE_HDINITPARAM(InitPara[HD_MODE]) &&
      IN_RANGE_HDINITPARAM(InitPara[HD_BLIND_TIME_SEEK]) && IN_RANGE_HDINITPARAM(InitPara[HD_BLIND_TIME_WELD]) &&
      IN_RANGE_HDINITPARAM(InitPara[HD_SWITCH_TIME]) && IN_RANGE_HDINITPARAM(InitPara[HD_WELD_AMPLITUDE]) &&
      IN_RANGE_HDINITPARAM(InitPara[HD_PWM_PERCENTAGE]))
   		PresetVerifiedFlag = TRUE;
   else
	   PresetVerifiedFlag = FALSE;

   return PresetVerifiedFlag;
}

void SetMaxMinValues(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the min and max preset values in FRAM .        */
/*                                                                           */
/*****************************************************************************/
{
	Parameter MaxParameters ;
	Parameter MinParameters;
/*******Minimum Preset Values******************************/
	MinParameters.AbAmp = ABAMPMIN;
	MinParameters.AbDelay = ABDELAYMIN;
	MinParameters.AbFlag = FALSE;
	MinParameters.AbTime = ABTIMEMIN;
//	MinParameters.ABSCutoffDist;
//	MinParameters.ABSCutoffFlag;
//	MinParameters.AbsDistCP;
//	MinParameters.AbsLimitFlags;
//	MinParameters.AbsMLR;
//	MinParameters.AbsMLS;
//	MinParameters.AbsPLR;
//	MinParameters.AbsPLS;
	MinParameters.ActClearDist = ACTCLEARDISTMIN;
	MinParameters.ActClearFlag = FALSE;
	MinParameters.WeldAmplitude = AMP1MIN;
	MinParameters.Amplitude2 = AMP2MIN;
	MinParameters.WeldAmplitudeLoopC1 = AMPLOOPC1MIN;
	MinParameters.WeldAmplitudeLoopC2 = AMPLOOPC2MIN;
	MinParameters.WeldAmplitudeLoopC1New = AMPLOOPC1MINNEW;
	MinParameters.WeldAmplitudeLoopC2New = AMPLOOPC2MINNEW;
	MinParameters.AmpStartRamp = AMPSTARTRAMPMIN;
//	MinParameters.AmpStepCondition;
//	MinParameters.AmpStepAbs;
	MinParameters.AmpStepCol = AMPSTEPCOLMIN;
	MinParameters.AmpStepEnergy = AMPSTEPENERMIN;
	MinParameters.AmpStepExt = FALSE;
//	MinParameters.AmpStepForce;
	MinParameters.AmpStepPower = AMPSTEPOWMIN;
	MinParameters.ScrubAmp = SCRUBAMPMIN;
	MinParameters.ColCutoffDist = COLCUTOFFDISTMIN;
	MinParameters.ColCutoffFlag = FALSE;
	MinParameters.ColDistCP = COLDISTMIN;
//	MinParameters.ColLimitFlags;
	MinParameters.ColMLR = OFF;
	MinParameters.ColMLS = COLMLSMIN;
	MinParameters.ColPLR = COLPLRMIN;
	MinParameters.ColPLS = COLPLSMIN;
	MinParameters.ControlLimits = CONTROLIMITMIN;
	MinParameters.CreatedDate = CREATE_DATEMIN;
	MinParameters.CreatedHour = CREATE_HOURMIN;
	MinParameters.CreatedMinute = CREATE_MINUTEMIN;
	MinParameters.CreatedMonth = CREATEMONTHMIN;
	MinParameters.CreatedYear = CREATEYEARMIN;
//	MinParameters.CycleAborts;
//	MinParameters.DownSpeed1;
//	MinParameters.DownSpeed2;
	MinParameters.EBAmp = EBAMPMIN;
	MinParameters.EBFlag = FALSE;
	MinParameters.EBTime = EBATIMEMIN ;
	MinParameters.EnergyCP = ENERGYCPMIN;
	MinParameters.EnergyComp = ENERGYCOMPMIN;
	MinParameters.EnergyCompFlag = FALSE;
	MinParameters.EnergyCutoff = ENERGYCUTOFFMIN;
	MinParameters.EnergyCutoffFlag = FALSE;
//	MinParameters.EnergyLimitFlags;
	MinParameters.EnergyMLR = ENERGYMLRMIN;
	MinParameters.EnergyMLS = ENERGYMLSMIN;
	MinParameters.EnergyPLR = ENERGYPLRMIN;
	MinParameters.EnergyPLS = ENERGYPLSMIN;
//	MinParameters.ExtTrigDelay;
//	MinParameters.FreqLimitFlags;
	if(ePromData.PSfrequency == FREQ_20KHZ)
	{
		MinParameters.FreqMLR = FREQMLR_20K_MIN;
		MinParameters.FreqMLS = FREQMLS_20K_MIN;
		MinParameters.FreqPLR = FREQPLR_20K_MIN;
		MinParameters.FreqPLS = FREQPLS_20K_MIN;
		MinParameters.FreqOffset = FREQOFFSET_20K_MIN;
		MinParameters.SeekFrequLimitHigh = SEEKFREQHIGH20K_MIN;
		MinParameters.SeekFrequLimitLow = SEEKFREQLOW20K_MIN;
		MinParameters.SeekFreqOffset = SEEK_FREQOFFSET20K_MIN;
		MinParameters.WeldFrequencyLimitHigh = WELDFREQHIGH20K_MIN;
		MinParameters.WeldFrequencyLimitLow = WELDFREQLOW20K_MIN;
		MinParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH20K_MINNEW;
		MinParameters.WeldFrequencyLimitLowNew = WELDFREQLOW20K_MINNEW;
		MinParameters.DigitalTune = DIGITALTUNE20K_MIN;			//Digital Tune
		MaxParameters.FreqMLR = FREQMLR_20K_MAX;
		MaxParameters.FreqMLS = FREQMLS_20K_MAX;
		MaxParameters.FreqPLR = FREQPLR_20K_MAX;
		MaxParameters.FreqPLS = FREQPLS_20K_MAX;
		MaxParameters.FreqOffset = FREQOFFSET_20K_MAX;
		MaxParameters.SeekFrequLimitHigh = SEEKFREQHIGH20K_MAX;
		MaxParameters.SeekFrequLimitLow = SEEKFREQLOW20K_MAX;
		MaxParameters.SeekFreqOffset = SEEK_FREQOFFSET20K_MAX;
		MaxParameters.WeldFrequencyLimitHigh = WELDFREQHIGH20K_MAX;
		MaxParameters.WeldFrequencyLimitLow = WELDFREQLOW20K_MAX;
		MaxParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH20K_MAXNEW;
		MaxParameters.WeldFrequencyLimitLowNew = WELDFREQLOW20K_MAXNEW;
		MaxParameters.DigitalTune = DIGITALTUNE20K_MAX;			//Digital Tune
	}
	else if(ePromData.PSfrequency == FREQ_30KHZ)
	{
		MinParameters.FreqMLR = FREQMLR_30K_MIN;
		MinParameters.FreqMLS = FREQMLS_30K_MIN;
		MinParameters.FreqPLR = FREQPLR_30K_MIN;
		MinParameters.FreqPLS = FREQPLS_30K_MIN;
		MinParameters.FreqOffset = FREQOFFSET_30K_MIN;
		MinParameters.SeekFrequLimitHigh = SEEKFREQHIGH30K_MIN;
		MinParameters.SeekFrequLimitLow = SEEKFREQLOW30K_MIN;
		MinParameters.SeekFreqOffset = SEEK_FREQOFFSET30K_MIN;
		MinParameters.WeldFrequencyLimitHigh = WELDFREQHIGH30K_MIN;
		MinParameters.WeldFrequencyLimitLow = WELDFREQLOW30K_MIN;
		MinParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH30K_MINNEW;
		MinParameters.WeldFrequencyLimitLowNew = WELDFREQLOW30K_MINNEW;
		MinParameters.DigitalTune = DIGITALTUNE30K_MIN;			//Digital Tune
		MaxParameters.FreqMLR = FREQMLR_30K_MAX;
		MaxParameters.FreqMLS = FREQMLS_30K_MAX;
		MaxParameters.FreqPLR = FREQPLR_30K_MAX;
		MaxParameters.FreqPLS = FREQPLS_30K_MAX;
		MaxParameters.FreqOffset = FREQOFFSET_30K_MAX;
		MaxParameters.SeekFrequLimitHigh = SEEKFREQHIGH30K_MAX;
		MaxParameters.SeekFrequLimitLow = SEEKFREQLOW30K_MAX;
		MaxParameters.SeekFreqOffset = SEEK_FREQOFFSET30K_MAX;
		MaxParameters.WeldFrequencyLimitHigh = WELDFREQHIGH30K_MAX;
		MaxParameters.WeldFrequencyLimitLow = WELDFREQLOW30K_MAX;
		MaxParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH30K_MAXNEW;
		MaxParameters.WeldFrequencyLimitLowNew = WELDFREQLOW30K_MAXNEW;
		MaxParameters.DigitalTune = DIGITALTUNE30K_MAX;			//Digital Tune
	}
	else if(ePromData.PSfrequency==FREQ_40KHZ)
	{
		MinParameters.FreqMLR = FREQMLR_40K_MIN;
		MinParameters.FreqMLS = FREQMLS_40K_MIN;
		MinParameters.FreqPLR = FREQPLR_40K_MIN;
		MinParameters.FreqPLS = FREQPLS_40K_MIN;
		MinParameters.FreqOffset = FREQOFFSET_40K_MIN;
		MinParameters.SeekFrequLimitHigh = SEEKFREQHIGH40K_MIN;
		MinParameters.SeekFrequLimitLow = SEEKFREQLOW40K_MIN;
		MinParameters.SeekFreqOffset = SEEK_FREQOFFSET40K_MIN;
		MinParameters.WeldFrequencyLimitHigh = WELDFREQHIGH40K_MIN;
		MinParameters.WeldFrequencyLimitLow = WELDFREQLOW40K_MIN;
		MinParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH40K_MINNEW;
		MinParameters.WeldFrequencyLimitLowNew = WELDFREQLOW40K_MINNEW;
		MinParameters.DigitalTune = DIGITALTUNE40K_MIN;		//Digital Tune
		MaxParameters.FreqMLR = FREQMLR_40K_MAX;
		MaxParameters.FreqMLS = FREQMLS_40K_MAX;
		MaxParameters.FreqPLR = FREQPLR_40K_MAX;
		MaxParameters.FreqPLS = FREQPLS_40K_MAX;
		MaxParameters.FreqOffset = FREQOFFSET_40K_MAX;
		MaxParameters.SeekFrequLimitHigh = SEEKFREQHIGH40K_MAX;
		MaxParameters.SeekFrequLimitLow = SEEKFREQLOW40K_MAX;
		MaxParameters.SeekFreqOffset = SEEK_FREQOFFSET40K_MAX;
		MaxParameters.WeldFrequencyLimitHigh = WELDFREQHIGH40K_MAX;
		MaxParameters.WeldFrequencyLimitLow = WELDFREQLOW40K_MAX;
		MaxParameters.WeldFrequencyLimitHighNew = WELDFREQHIGH40K_MAXNEW;
		MaxParameters.WeldFrequencyLimitLowNew = WELDFREQLOW40K_MAXNEW;
		MaxParameters.DigitalTune = DIGITALTUNE40K_MAX;		//Digital Tune
	}
	MinParameters.FreqOffsetFlag = FALSE;
	MinParameters.Force1 = FORCE1MIN;
	MinParameters.Force2 = FORCE2MIN;
//	MinParameters.ForceStartRamp;
//	MinParameters.ForceStepCondition;
//	MinParameters.ForceStepAbs;
	MinParameters.ForceStepCol = FORCESTEPCOLMIN;
	MinParameters.ForceStepEnergy = FORCESTEPENERMIN;
//	MinParameters.ForceStepExt;
	MinParameters.ForceStepAmp = FORCESTEPAMPMIN;
	MinParameters.ForceStepPower = FORCESTEPPOWMIN;
//	MinParameters.GaugePressure;
	MinParameters.GeneralAlarmFlag = FALSE;
	MinParameters.GrdDetectCP = FALSE;
//	MinParameters.HardwareConfig;
//	MinParameters.HoldForce;
//	MinParameters.HoldRorceRamp;
	MinParameters.HoldTime = HOLDTIMEMIN;
//	MinParameters.HoldTimeFlag;
	MinParameters.MaxWeldTime = MAXWELDTIMEMIN;
	MinParameters.MemoryStoreFlag = FALSE;
//	MinParameters.MissingPartMin;
//	MinParameters.MissingPartMax;
//	MinParameters.MissingPartFlag;
//	MinParameters.MissingPartMinFlag;
//	MinParameters.MissingPartMaxFlag;
	MinParameters.ModifiedDate = MODIFY_DATEMIN;
	MinParameters.ModifiedHour = MODIFY_HOURMIN;
	MinParameters.ModifiedMinute = MODIFY_MINUTEMIN;
	MinParameters.ModifiedMonth = MODIFY_MONTHMIN;
	MinParameters.ModifiedYear = MODIFY_YEARMIN;
	MinParameters.PeakPowerCP = PEAKPOWCPMIN ;
	MinParameters.PeakPowerCutoff = PEAKPOWCUTOFFMIN;
//	MinParameters.PeakPowerCutoffFlag;
	MinParameters.WeldPhaseLimit = PHASELIMMIN;
	MinParameters.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(PHASELIMTIMEMIN);
	MinParameters.WeldPhaseloopI = PLOOPMIN;
	MinParameters.WeldPhaseloopPNew = PLOOPPMINNEW;
	MinParameters.FreqWindowSize = 1;
	MinParameters.WeldPhaseloopCF = PLOOPCFMIN;
	MinParameters.WeldPhaseLimitNew = PHASELIMMINNEW;
	MinParameters.WeldPhaseLimitTimeNew = PHASELIMTIMEMINNEW;
	MinParameters.WeldPhaseloopINew = PLOOPMINNEW;
	MinParameters.WeldPhaseloopCFNew = PLOOPCFMINNEW;
	MinParameters.FreqWindowSize = MIN_NUM_PHASE_AVERAGES;
	MinParameters.FrequencyLimitTime = CONV_MILLI_MICROSEC(MIN_FLIMIT_TIME);
//	MinParameters.PMCCycleCount;
//	MinParameters.PMCData[402];
//	MinParameters.PMCEnabledFlag;
//	MinParameters.PMCHighLimit;
//	MinParameters.PMCLowLimit;
//	MinParameters.PMCMinusBandFlag;
//	MinParameters.PMCPlusBandFlag;
//	MinParameters.PMCTimeout;
//	MinParameters.PostWeldSeekFlag;
//	MinParameters.PowerLimitFlags;
	MinParameters.PowerMLR = POWERMLRMIN;
	MinParameters.PowerMLS = POWERMLSMIN;
	MinParameters.PowerPLR = POWERPLRMIN;
	MinParameters.PowerPLS = POWERPLSMIN;
//	MinParameters.PresetID[12];
//	MinParameters.PresetStatus;
//	MinParameters.PreTrigAmp;
//	MinParameters.PreTrigDelay;
//	MinParameters.PreTrigDelayFlag;
//	MinParameters.PreTrigDist;
//	MinParameters.PretrigDistFlag;
//	MinParameters.RejectLimits;
	MinParameters.RejectLimitsFlag = FALSE;
	MinParameters.RejResetRequiredFlag = FALSE;
//	MinParameters.ReturnForceRamp;
	/* DIAG GUI*/
//	MinParameters.SBLEnableMax;
//	MinParameters.SBLEnableMin;
//	MinParameters.SBLFreqFlags;
//	MinParameters.SBLFreqMax;
//	MinParameters.SBLFreqMin;
//	MinParameters.SBLFreqMaxLimit;
//	MinParameters.SBLFreqMinLimit;
	MinParameters.ScrubTime = SCRUBTIMEMIN;
	MinParameters.ScrubTimeFlag = FALSE;
	MinParameters.SeekAmplitude = SEEKAMPPMIN;
	MinParameters.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1MIN;
	MinParameters.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2MIN;

	MinParameters.SeekPhaseloopI = SEEKPHASELOOPMIN;
	MinParameters.SeekPhaseloopCF = SEEKPHASELOOPCFMIN;
	MinParameters.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIMEMIN);
	MinParameters.SeekTime = CONV_MILLI_MICROSEC(SEEKTIMEMIN);
//	MinParameters.SetPressure;
//	MinParameters.SetRTComp;
//	MinParameters.StepForceRamp;
//	MinParameters.SuspectLimits;
//	MinParameters.SuspectLimitsFlag;
//	MinParameters.SusResetRequiredFlag;
	MinParameters.TestAmp = TESTAMPMIN;
	MinParameters.TimedSeekFlag = FALSE;
	MinParameters.TimeOnTrigFlag = FALSE;
	MinParameters.TriggerForce = TRIGGFORCEMIN;
//	MinParameters.TriggerDelay;
//	MinParameters.TriggerDelayFlag;
//	MinParameters.TrsLimitFlags;
//	MinParameters.TrsDistMLR;
//	MinParameters.TrsDistMLS;
//	MinParameters.TrsDistPLR;
//	MinParameters.TrsDistPLS;
//	MinParameters.VQSCycleTimeMax;
//	MinParameters.VQSCycleTimeMin;
//	MinParameters.VQSEnergyMin;
//	MinParameters.VQSEnergyMax;
//	MinParameters.VQSLimit;
//	MinParameters.VQSPowerMin;
//	MinParameters.VQSPowerMax;
//	MinParameters.VQSTotalAbsMax;
//	MinParameters.VQSTotalAbsMin;
//	MinParameters.VQSTotalColMax;
//	MinParameters.VQSTotalColMin;
//	MinParameters.VQSWeldColMax;
//	MinParameters.VQSWeldColMin;
//	MinParameters.VQSWeldForceMax;
//	MinParameters.VQSWeldForceMin;
//	MinParameters.VQSWeldTimeMax;
//	MinParameters.VQSWeldTimeMin;
	MinParameters.WeldCount = WELDCOUNTMIN;
//	MinParameters.WeldForce;
//	MinParameters.WeldForceLimitFlags;
//	MinParameters.WeldForceMLR;
//	MinParameters.WeldForceMLS;
//	MinParameters.WeldForcePLR;
//	MinParameters.WeldForcePLS;
//	MinParameters.WeldForceRamp;
//	MinParameters.WeldMode;
//	MinParameters.WeldFreqHigh;
//	MinParameters.WeldFreqLow;
	MinParameters.WeldRampTime = CONV_MILLI_MICROSEC(AMPSTARTRAMPMIN);
	MinParameters.WeldStatusFlag = FALSE;
	MinParameters.WeldTime = WELDTIMEMIN;
//	MinParameters.WeldTimeLimitFlags;
	MinParameters.WeldTimeMLR = WELDTIMEMLRMIN;
	MinParameters.WeldTimeMLS = WELDTIMEMLSMIN;
	MinParameters.WeldTimePLR = WELDTIMEPLRMIN;
	MinParameters.WeldTimePLS = WELDTIMEPLSMIN;
//	MinParameters.WriteInField1[12];
//	MinParameters.WriteInField2[12];
//	MinParameters.CRC;

/*******Maximum Preset Values******************************/
	MaxParameters.AbAmp = ABAMPMAX;
	MaxParameters.AbDelay = ABDELAYMAX;
	MaxParameters.AbFlag = TRUE;
	MaxParameters.AbTime = ABTIMEMAX;
//	MaxParameters.ABSCutoffDist;
//	MaxParameters.ABSCutoffFlag;
//	MaxParameters.AbsDistCP;
//	MaxParameters.AbsLimitFlags;
//	MaxParameters.AbsMLR;
//	MaxParameters.AbsMLS;
//	MaxParameters.AbsPLR;
//	MaxParameters.AbsPLS;
//	MaxParameters.ActClearDist = ACTCLEARDISTMAX;
	MaxParameters.ActClearFlag = TRUE;
	MaxParameters.WeldAmplitude = AMP1MAX;
	MaxParameters.Amplitude2 = AMP2MAX;
	MaxParameters.WeldAmplitudeLoopC1 = AMPLOOPC1MAX;
	MaxParameters.WeldAmplitudeLoopC2 = AMPLOOPC2MAX;
	MaxParameters.WeldAmplitudeLoopC1New = AMPLOOPC1MAXNEW;
	MaxParameters.WeldAmplitudeLoopC2New = AMPLOOPC2MAXNEW;
	MaxParameters.AmpStartRamp = AMPSTARTRAMPMAX;
//	MaxParameters.AmpStepCondition;
//	MaxParameters.AmpStepAbs;
	MaxParameters.AmpStepCol = AMPSTEPCOLMAX;
//	MaxParameters.AmpStepEnergy = AMPSTEPENERMAX;
	MaxParameters.AmpStepExt = TRUE;
//	MaxParameters.AmpStepForce;
	MaxParameters.AmpStepPower = AMPSTEPOWMAX;
	MaxParameters.ScrubAmp = SCRUBAMPMAX;
	MaxParameters.ColCutoffDist = COLCUTOFFDISTMAX;
	MaxParameters.ColCutoffFlag = TRUE;
	MaxParameters.ColDistCP = COLDISTMAX;
//	MaxParameters.ColLimitFlags;
	MaxParameters.ColMLR = ON;
	MaxParameters.ColMLS = COLMLSMAX;
	MaxParameters.ColPLR = COLPLRMAX;
	MaxParameters.ColPLS = COLPLSMAX;
	MaxParameters.ControlLimits = CONTROLIMITMAX;
	MaxParameters.CreatedDate = CREATE_DATEMAX;
	MaxParameters.CreatedHour = CREATE_HOURMAX;
	MaxParameters.CreatedMinute = CREATE_MINUTEMAX;
	MaxParameters.CreatedMonth = CREATEMONTHMAX;
	MaxParameters.CreatedYear = CREATEYEARMAX;
//	MaxParameters.CycleAborts;
//	MaxParameters.DownSpeed1;
//	MaxParameters.DownSpeed2;
	MaxParameters.EBAmp = EBAMPMAX;
	MaxParameters.EBFlag = TRUE;
	MaxParameters.EBTime = EBATIMEMAX ;
	MaxParameters.EnergyCP = ENERGYCPMAX;
	MaxParameters.EnergyComp = ENERGYCOMPMAX;
	MaxParameters.EnergyCompFlag = TRUE;
	MaxParameters.EnergyCutoff = ENERGYCUTOFFMAX;
	MaxParameters.EnergyCutoffFlag = TRUE;
//	MaxParameters.EnergyLimitFlags;
	MaxParameters.EnergyMLR = ENERGYMLRMAX;
	MaxParameters.EnergyMLS = ENERGYMLSMAX;
	MaxParameters.EnergyPLR = ENERGYPLRMAX;
	MaxParameters.EnergyPLS = ENERGYPLSMAX;
//	MaxParameters.ExtTrigDelay;
//	MaxParameters.FreqLimitFlags;
	MaxParameters.FreqOffsetFlag = TRUE;
//	MaxParameters.Force1 = FORCE1MAX;
//	MaxParameters.Force2 = FORCE2MAX;
//	MaxParameters.ForceStartRamp;
//	MaxParameters.ForceStepCondition;
//	MaxParameters.ForceStepAbs;
	MaxParameters.ForceStepCol = FORCESTEPCOLMAX;
//	MaxParameters.ForceStepEnergy = FORCESTEPENERMAX;
//	MaxParameters.ForceStepExt;
	MaxParameters.ForceStepAmp = FORCESTEPAMPMAX;
	MaxParameters.ForceStepPower = FORCESTEPPOWMAX;
//	MaxParameters.GaugePressure;
	MaxParameters.GeneralAlarmFlag = TRUE;
	MaxParameters.GrdDetectCP = TRUE;
//	MaxParameters.HardwareConfig;
//	MaxParameters.HoldForce;
//	MaxParameters.HoldRorceRamp;
	MaxParameters.HoldTime = HOLDTIMEMAX;
//	MaxParameters.HoldTimeFlag;
	MaxParameters.MaxWeldTime = MAXWELDTIMEMAX;
	MaxParameters.MemoryStoreFlag = TRUE;
//	MaxParameters.MissingPartMin;
//	MaxParameters.MissingPartMax;
//	MaxParameters.MissingPartFlag;
//	MaxParameters.MissingPartMinFlag;
//	MaxParameters.MissingPartMaxFlag;
	MaxParameters.ModifiedDate = MODIFY_DATEMAX;
	MaxParameters.ModifiedHour = MODIFY_HOURMAX;
	MaxParameters.ModifiedMinute = MODIFY_MINUTEMAX;
	MaxParameters.ModifiedMonth = MODIFY_MONTHMAX;
	MaxParameters.ModifiedYear = MODIFY_YEARMAX;
	MaxParameters.PeakPowerCP = PEAKPOWCPMAX ;
	MaxParameters.PeakPowerCutoff = PEAKPOWCUTOFFMAX;
//	MaxParameters.PeakPowerCutoffFlag;
	MaxParameters.WeldPhaseLimit = PHASELIMMAX;
	MaxParameters.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(PHASELIMTIMEMAX);
	MaxParameters.WeldPhaseloopI = PLOOPMAX;
	MaxParameters.WeldPhaseloopPNew = PLOOPPMAXNEW;
	MaxParameters.FreqWindowSize = 1000;
	MaxParameters.WeldPhaseloopCF = PLOOPCFMAX;
	MaxParameters.WeldPhaseLimitNew = PHASELIMMAXNEW;
	MaxParameters.WeldPhaseLimitTimeNew = PHASELIMTIMEMAXNEW;
	MaxParameters.WeldPhaseloopINew = PLOOPMAXNEW;
	MaxParameters.WeldPhaseloopCFNew = PLOOPCFMAXNEW;
	MaxParameters.FreqWindowSize = MAX_NUM_PHASE_AVERAGES;
	MaxParameters.FrequencyLimitTime = CONV_MILLI_MICROSEC(MAX_FLIMIT_TIME);
//	MaxParameters.PMCCycleCount;
//	MaxParameters.PMCData[402];
//	MaxParameters.PMCEnabledFlag;
//	MaxParameters.PMCHighLimit;
//	MaxParameters.PMCLowLimit;
//	MaxParameters.PMCMinusBandFlag;
//	MaxParameters.PMCPlusBandFlag;
//	MaxParameters.PMCTimeout;
//	MaxParameters.PostWeldSeekFlag;
//	MaxParameters.PowerLimitFlags;
	MaxParameters.PowerMLR = POWERMLRMAX;
	MaxParameters.PowerMLS = POWERMLSMAX;
	MaxParameters.PowerPLR = POWERPLRMAX;
	MaxParameters.PowerPLS = POWERPLSMAX;
//	MaxParameters.PresetID[12];
//	MaxParameters.PresetStatus;
//	MaxParameters.PreTrigAmp;
//	MaxParameters.PreTrigDelay;
//	MaxParameters.PreTrigDelayFlag;
//	MaxParameters.PreTrigDist;
//	MaxParameters.PretrigDistFlag;
//	MaxParameters.RejectLimits;
	MaxParameters.RejectLimitsFlag = TRUE;
	MaxParameters.RejResetRequiredFlag = TRUE;
//	MaxParameters.ReturnForceRamp;
	/* DIAG GUI*/
//	MaxParameters.SBLEnableMax;
//	MaxParameters.SBLEnableMin;
//	MaxParameters.SBLFreqFlags;
//	MaxParameters.SBLFreqMax;
//	MaxParameters.SBLFreqMin;
//	MaxParameters.SBLFreqMaxLimit;
//	MaxParameters.SBLFreqMinLimit;
	MaxParameters.ScrubTime = SCRUBTIMEMAX;
	MaxParameters.ScrubTimeFlag = TRUE;
	MaxParameters.SeekAmplitude = SEEKAMPPMAX;
	MaxParameters.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1MAX;
	MaxParameters.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2MAX;
	MaxParameters.SeekPhaseloopI = SEEKPHASELOOPMAX;
	MaxParameters.SeekPhaseloopCF = SEEKPHASELOOPCFMAX;
	MaxParameters.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIMEMAX);
	MaxParameters.SeekTime = CONV_MILLI_MICROSEC(SEEKTIMEMAX);
//	MaxParameters.SetPressure;
//	MaxParameters.SetRTComp;
//	MaxParameters.StepForceRamp;
//	MaxParameters.SuspectLimits;
//	MaxParameters.SuspectLimitsFlag;
//	MaxParameters.SusResetRequiredFlag;
	MaxParameters.TestAmp = TESTAMPMAX;
	MaxParameters.TimedSeekFlag = TRUE;
	MaxParameters.TimeOnTrigFlag = TRUE;
//	MaxParameters.TriggerForce = TRIGGFORCEMAX;
//	MaxParameters.TriggerDelay;
//	MaxParameters.TriggerDelayFlag;
//	MaxParameters.TrsLimitFlags;
//	MaxParameters.TrsDistMLR;
//	MaxParameters.TrsDistMLS;
//	MaxParameters.TrsDistPLR;
//	MaxParameters.TrsDistPLS;
//	MaxParameters.VQSCycleTimeMax;
//	MaxParameters.VQSCycleTimeMin;
//	MaxParameters.VQSEnergyMin;
//	MaxParameters.VQSEnergyMax;
//	MaxParameters.VQSLimit;
//	MaxParameters.VQSPowerMin;
//	MaxParameters.VQSPowerMax;
//	MaxParameters.VQSTotalAbsMax;
//	MaxParameters.VQSTotalAbsMin;
//	MaxParameters.VQSTotalColMax;
//	MaxParameters.VQSTotalColMin;
//	MaxParameters.VQSWeldColMax;
//	MaxParameters.VQSWeldColMin;
//	MaxParameters.VQSWeldForceMax;
//	MaxParameters.VQSWeldForceMin;
//	MaxParameters.VQSWeldTimeMax;
//	MaxParameters.VQSWeldTimeMin;
	MaxParameters.WeldCount = WELDCOUNTMAX;
//	MaxParameters.WeldForce;
//	MaxParameters.WeldForceLimitFlags;
//	MaxParameters.WeldForceMLR;
//	MaxParameters.WeldForceMLS;
//	MaxParameters.WeldForcePLR;
//	MaxParameters.WeldForcePLS;
//	MaxParameters.WeldForceRamp;
//	MaxParameters.WeldMode;
//	MaxParameters.WeldFreqHigh;
//	MaxParameters.WeldFreqLow;
	MaxParameters.WeldRampTime = CONV_MILLI_MICROSEC(AMPSTARTRAMPMAX);
	MaxParameters.WeldStatusFlag = TRUE;
	MaxParameters.WeldTime = WELDTIMEMAX;
//	MaxParameters.WeldTimeLimitFlags;
	MaxParameters.WeldTimeMLR = WELDTIMEMLRMAX;
	MaxParameters.WeldTimeMLS = WELDTIMEMLSMAX;
	MaxParameters.WeldTimePLR = WELDTIMEPLRMAX;
	MaxParameters.WeldTimePLS = WELDTIMEPLSMAX;
//	MaxParameters.WriteInField1[12];
//	MaxParameters.WriteInField2[12];
//	MaxParameters.DEFAULTCRC;

	//Min HD
	for(int indx = 0 ;indx < SIZE_PREINITPARAMS; indx++)
	{
		MinParameters.HDParam.PreInitPara[indx] = 0;
	}

	MinParameters.HDParam.InitPara[HD_MODE] 			= HD_MODE_MIN;
	MinParameters.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= HD_BLIND_TIME_SEEK_MIN;
	MinParameters.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= HD_BLIND_TIME_WELD_MIN;
	MinParameters.HDParam.InitPara[HD_SWITCH_TIME] 		= HD_SWITCH_TIME_MIN;
	MinParameters.HDParam.InitPara[HD_WELD_AMPLITUDE] 	= HD_WELD_AMPLITUDE_MIN;
	MinParameters.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= HD_PWM_PERCENTAGE_MIN;

	for(int indx = 6  ;indx < SIZE_INITPARAMS; indx++)
	{
		MinParameters.HDParam.InitPara[indx]= 0;
	}

	//Max HD
	for(int indx = 0 ;indx < SIZE_PREINITPARAMS; indx++)
	{
		MaxParameters.HDParam.PreInitPara[indx]= indx + ePromData.PSpower * 100 * ePromData.PSfrequency * 1000000;
	}

	MaxParameters.HDParam.InitPara[HD_MODE] = HD_MODE_MAX;
	MaxParameters.HDParam.InitPara[HD_BLIND_TIME_SEEK] = HD_BLIND_TIME_SEEK_MAX;
	MaxParameters.HDParam.InitPara[HD_BLIND_TIME_WELD] = HD_BLIND_TIME_WELD_MAX;
	MaxParameters.HDParam.InitPara[HD_SWITCH_TIME] = HD_SWITCH_TIME_MAX;
	MaxParameters.HDParam.InitPara[HD_WELD_AMPLITUDE] = HD_WELD_AMPLITUDE_MAX;
	MaxParameters.HDParam.InitPara[HD_PWM_PERCENTAGE] = HD_PWM_PERCENTAGE_MAX;

	for(int indx = 6  ;indx < SIZE_INITPARAMS; indx++)
	{
		MaxParameters.HDParam.InitPara[indx]= indx + 12341111 * 100 ;
	}

	memcpy(MinParameters.PresetID, "MINIMUM    ",PRESETID_LEN);
    memcpy(&Preset_Min, &MinParameters,sizeof(Parameter));

    memcpy(MaxParameters.PresetID, "MAXIMUM    ",PRESETID_LEN);
    memcpy(&Preset_Max, &MaxParameters,sizeof(Parameter));

}

void SetDefaultValues(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the current preset with default values when    */
/*  a cold start is executed.                                                */
/*                                                                           */
/*****************************************************************************/
{
	UINT16 DEFAULTFIELD[WRITEINFIELD_LEN];
	CurrentPreset.AbAmp = ABAMP_DEFAULT;
	CurrentPreset.AbDelay = ABDELAY_DEFAULT;
	CurrentPreset.AbFlag = FALSE;
	CurrentPreset.AbTime = ABTIME_DEFAULT;
//	RunningPreset.ABSCutoffDist;
//	RunningPreset.ABSCutoffFlag;
//	RunningPreset.AbsDistCP;
//	RunningPreset.AbsLimitFlags;
//	RunningPreset.AbsMLR;
//	RunningPreset.AbsMLS;
//	RunningPreset.AbsPLR;
//	RunningPreset.AbsPLS;
	CurrentPreset.ActClearDist = OFF;
	CurrentPreset.ActClearFlag = FALSE;
	CurrentPreset.WeldAmplitude = AMP1_DEFAULT;
	CurrentPreset.Amplitude2 = AMP2_DEFAULT;
	switch(ePromData.PSfrequency)
	{
		case FREQ_20KHZ:
			CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_20KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_20KDEFAULT;
			CurrentPreset.WeldPhaseloopI = PLOOP_20KDEFAULT;
			CurrentPreset.WeldPhaseloopCF = PLOOPCF_20KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_20KDEFAULTNEW;
			CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopINew = PLOOP_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopPNew = PLOOPP_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_20KDEFAULTNEW;
			CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_20KDEFAULT;
			CurrentPreset.DigitalTune = DIGITALTUNE20K_DEF;
		break;
		case FREQ_30KHZ:
			CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_30KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_30KDEFAULT;
			CurrentPreset.WeldPhaseloopI = PLOOP_30KDEFAULT;
			CurrentPreset.WeldPhaseloopCF = PLOOPCF_30KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_30KDEFAULTNEW;
			CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_30KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopINew = PLOOP_30KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopPNew = PLOOPP_30KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_30KDEFAULTNEW;
			CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_30KDEFAULT;
			CurrentPreset.DigitalTune = DIGITALTUNE30K_DEF;
		break;
		case FREQ_40KHZ:
			CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_40KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_40KDEFAULT;
			CurrentPreset.WeldPhaseloopI = PLOOP_40KDEFAULT;
		    CurrentPreset.WeldPhaseloopCF = PLOOPCF_40KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_40KDEFAULTNEW;
			CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_40KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopINew = PLOOP_40KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopPNew = PLOOPP_40KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_40KDEFAULTNEW;
		    CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_40KDEFAULT;
		    CurrentPreset.DigitalTune = DIGITALTUNE40K_DEF;
		break;
	}
	CurrentPreset.AmpStartRamp = AMPSTARTRAMP_DEFAULT;
	CurrentPreset.AmpStepCondition = AMPSTEP_COND_OFF;
//	CurrentPreset.AmpStepAbs;
//	CurrentPreset.AmpStepCol=AMPSTEPCOLMIN;
//	CurrentPreset.AmpStepEnergy=AMPSTEPENERMIN;
//	CurrentPreset.AmpStepExt = TRUE;
//	CurrentPreset.AmpStepForce;
//	CurrentPreset.AmpStepPower=AMPSTEPOWMIN;
	CurrentPreset.ScrubAmp = SCRUBAMP_DEFAULT;
	CurrentPreset.ColCutoffDist = COLCUTOFFDIST_DEFAULT;
	CurrentPreset.ColCutoffFlag = FALSE;
	CurrentPreset.ColDistCP = OFF;
	CurrentPreset.ColLimitFlags = COL_LIMITS_OFF;
	CurrentPreset.ColMLR = OFF;
//	CurrentPreset.ColMLS=COLMLSMIN;
//	CurrentPreset.ColPLR=COLPLRMIN;
//	CurrentPreset.ColPLS=COLPLSMIN;
//	CurrentPreset.ControlLimits=CONTROLIMITMIN;
//	CurrentPreset.CreatedDate=CREATE_DATEMIN;
//	CurrentPreset.CreatedHour=CREATE_HOURMIN;
//	CurrentPreset.CreatedMinute=CREATE_MINUTEMIN;
//	CurrentPreset.CreatedMonth=CREATEMONTHMIN;
//	CurrentPreset.CreatedYear=CREATEYEARMIN;
	CurrentPreset.CycleAborts = CYCLE_ABORT_OFF;
//	CurrentPreset.DownSpeed1;
//	CurrentPreset.DownSpeed2;
	CurrentPreset.EBAmp = EBAMP_DEFAULT;
	CurrentPreset.EBFlag = FALSE;
	CurrentPreset.EBTime = EBATIME_DEFAULT;
	CurrentPreset.EnergyCP = ENERGYCP_DEFAULT;
	CurrentPreset.EnergyComp = ENERGYCOMP_DEFAULT;
	CurrentPreset.EnergyCompFlag = FALSE;
	CurrentPreset.EnergyCutoff = ENERGYCUTOFF_DEFAULT;
	CurrentPreset.EnergyCutoffFlag = FALSE;
	CurrentPreset.EnergyLimitFlags = ENERGY_LIMIT_OFF;
	CurrentPreset.EnergyMLR = OFF;
	CurrentPreset.EnergyMLS = OFF;
	CurrentPreset.EnergyPLR = OFF;
	CurrentPreset.EnergyPLS = OFF;
//	CurrentPreset.ExtTrigDelay;
	CurrentPreset.FreqLimitFlags = FREQ_LIMIT_OFF;
	CurrentPreset.FreqMLR = OFF;
	CurrentPreset.FreqMLS = OFF;
	CurrentPreset.FreqPLR = OFF;
	CurrentPreset.FreqPLS = OFF;
	CurrentPreset.FreqOffset = FREQ_OFFSET_DEFAULT;
	CurrentPreset.FreqOffsetFlag = FALSE;
	CurrentPreset.Force1 = FORCE1_DEFAULT;
	CurrentPreset.Force2 = FORCE2_DEFAULT;
//	CurrentPreset.ForceStartRamp;
	CurrentPreset.ForceStepCondition = FORCESTEP_COND_OFF;
//	CurrentPreset.ForceStepAbs;
//	CurrentPreset.ForceStepCol=FORCESTEPCOLMIN;
//	CurrentPreset.ForceStepEnergy=FORCESTEPENERMIN;
//	CurrentPreset.ForceStepExt;
//	CurrentPreset.ForceStepAmp=FORCESTEPAMPMIN;
//	CurrentPreset.ForceStepPower=FORCESTEPPOWMIN;
//	CurrentPreset.GaugePressure;
	CurrentPreset.GeneralAlarmFlag = FALSE;
	CurrentPreset.GrdDetectCP = FALSE;
//	CurrentPreset.HardwareConfig;
//	CurrentPreset.HoldForce;
//	CurrentPreset.HoldRorceRamp;
//	CurrentPreset.HoldTime=HOLDTIMEMIN;
	CurrentPreset.HoldTimeFlag = FALSE;
	CurrentPreset.MaxWeldTime = MAXWELDTIME_DEFAULT;
	CurrentPreset.MemoryStoreFlag = FALSE;
	CurrentPreset.MemoryFreq = MEMFREQ_DEFAULT;
//	CurrentPreset.MissingPartMin;
//	CurrentPreset.MissingPartMax;
//	CurrentPreset.MissingPartFlag;
//	CurrentPreset.MissingPartMinFlag;
//	CurrentPreset.MissingPartMaxFlag;
//	CurrentPreset.ModifiedDate=MODIFY_DATEMIN;
//	CurrentPreset.ModifiedHour=MODIFY_HOURMIN;
//	CurrentPreset.ModifiedMinute=MODIFY_MINUTEMIN;
//	CurrentPreset.ModifiedMonth=MODIFY_MONTHMIN;
//	CurrentPreset.ModifiedYear=MODIFY_YEARMIN;
	CurrentPreset.PeakPowerCP = PEAKPOWCP_DEFAULT ;
	CurrentPreset.PeakPowerCutoff = PEAKPOWCUTOFF_DEFAULT;
	CurrentPreset.PeakPowerCutoffFlag = FALSE;
//	CurrentPreset.PhaseLimit=PHASELIMMIN;
//	CurrentPreset.PhaseLimitTime=PHASELIMTIMEMIN;
//	CurrentPreset.PMCCycleCount;
//	CurrentPreset.PMCData[402];
//	CurrentPreset.PMCEnabledFlag;
//	CurrentPreset.PMCHighLimit;
//	CurrentPreset.PMCLowLimit;
//	CurrentPreset.PMCMinusBandFlag;
//	CurrentPreset.PMCPlusBandFlag;
//	CurrentPreset.PMCTimeout;
//	CurrentPreset.PostWeldSeekFlag;
	CurrentPreset.PowerLimitFlags = POWER_LIMIT_OFF;
	CurrentPreset.PowerMLR = OFF;
	CurrentPreset.PowerMLS = OFF;
	CurrentPreset.PowerPLR = OFF;
	CurrentPreset.PowerPLS = OFF;
	memcpy(CurrentPreset.PresetID,"COLD START",12);
	CurrentPreset.PresetStatus = NOTVERIFIED;
//	CurrentPreset.PreTrigAmp;
//	CurrentPreset.PreTrigDelay;
//	CurrentPreset.PreTrigDelayFlag;
//	CurrentPreset.PreTrigDist;
//	CurrentPreset.PretrigDistFlag;
	CurrentPreset.RejectLimits = REJECT_LIMIT_OFF;
	CurrentPreset.RejectLimitsFlag = FALSE;
	CurrentPreset.RejResetRequiredFlag = FALSE;
//	CurrentPreset.ReturnForceRamp;
	/* DIAG GUI*/
//	CurrentPreset.SBLEnableMax;
//	CurrentPreset.SBLEnableMin;
//	CurrentPreset.SBLFreqFlags;
//	CurrentPreset.SBLFreqMax;
//	CurrentPreset.SBLFreqMin;
//	CurrentPreset.SBLFreqMaxLimit;
//	CurrentPreset.SBLFreqMinLimit;
	CurrentPreset.ScrubTime = SCRUBTIME_DEFAULT;
	CurrentPreset.ScrubTimeFlag = FALSE;
	CurrentPreset.SeekAmplitude = SEEKAMPP_DEFAULT;
	CurrentPreset.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1_DEFAULT;
	CurrentPreset.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2_DEFAULT;
	CurrentPreset.SeekFrequLimitHigh = SEEKFREQHIGH_DEFAULT;
	CurrentPreset.SeekFrequLimitLow = SEEKFREQLOW_DEFAULT;
	CurrentPreset.SeekFreqOffset = SEEK_FREQOFFSET_DEFAULT;

	CurrentPreset.SeekPhaseloopCF = SEEKPHASELOOPCF_DEFAULT;
	CurrentPreset.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIME_DEFAULT);
	CurrentPreset.SeekTime = CONV_MILLI_MICROSEC(SEEKTIME_DEFAULT);
//	CurrentPreset.SetPressure;
//	CurrentPreset.SetRTComp;
//	CurrentPreset.StepForceRamp;
//	CurrentPreset.SuspectLimits;
//	CurrentPreset.SuspectLimitsFlag;
//	CurrentPreset.SusResetRequiredFlag;
	CurrentPreset.TestAmp = TESTAMP_DEFAULT;
	CurrentPreset.TimedSeekFlag = FALSE;
	CurrentPreset.TimeOnTrigFlag = FALSE;
	CurrentPreset.TriggerForce = TRIGGFORCE_DEFAULT;
//	CurrentPreset.TriggerDelay;
//	CurrentPreset.TriggerDelayFlag;
	CurrentPreset.TrsLimitFlags = TRS_LIMIT_OFF;
//	CurrentPreset.TuneFreq = TUNEFREQ_DEFAULT;
//	CurrentPreset.TrsDistMLR;
//	CurrentPreset.TrsDistMLS;
//	CurrentPreset.TrsDistPLR;
//	CurrentPreset.TrsDistPLS;
//	CurrentPreset.VQSCycleTimeMax;
//	CurrentPreset.VQSCycleTimeMin;
//	CurrentPreset.VQSEnergyMin;
//	CurrentPreset.VQSEnergyMax;
//	CurrentPreset.VQSLimit;
//	CurrentPreset.VQSPowerMin;
//	CurrentPreset.VQSPowerMax;
//	CurrentPreset.VQSTotalAbsMax;
//	CurrentPreset.VQSTotalAbsMin;
//	CurrentPreset.VQSTotalColMax;
//	CurrentPreset.VQSTotalColMin;
//	CurrentPreset.VQSWeldColMax;
//	CurrentPreset.VQSWeldColMin;
//	CurrentPreset.VQSWeldForceMax;
//	CurrentPreset.VQSWeldForceMin;
//	CurrentPreset.VQSWeldTimeMax;
//	CurrentPreset.VQSWeldTimeMin;
	CurrentPreset.WeldCount = WELDCOUNT_DEFAULT;
//	CurrentPreset.WeldForce;
//	CurrentPreset.WeldForceLimitFlags;
//	CurrentPreset.WeldForceMLR;
//	CurrentPreset.WeldForceMLS;
//	CurrentPreset.WeldForcePLR;
//	CurrentPreset.WeldForcePLS;
//	CurrentPreset.WeldForceRamp;
	CurrentPreset.WeldMode = CONTINUOUSMODE;
	SetDefaultHDInitParam();
	SetDefaultPreInitHDParams();
	CurrentPreset.WeldFrequencyLimitHigh = WELD_FREQLIMITHIGH;
	CurrentPreset.WeldFrequencyLimitLow = WELD_FREQLIMITLOW;
	CurrentPreset.WeldFrequencyLimitHighNew = WELD_FREQLIMITHIGHNEW;
	CurrentPreset.WeldFrequencyLimitLowNew = WELD_FREQLIMITLOWNEW;
	CurrentPreset.WeldRampTime = CONV_MILLI_MICROSEC(WELDRAMPTIME_DEFAULT);
	CurrentPreset.WeldStatusFlag = FALSE;

	//1000   2047 //original DUPS code
	//2000   4096 //corresponding to current maximum raw value
	//Now In terms of scaled up milli volts it should be
	//((2000 * 2500)/4096) * ADC_MULT_FACTOR
	CurrentPreset.WeldPhaseLimit = PHASELIM_DEFAULT;
	CurrentPreset.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(PHASELIMTIME_DEFAULT);
	CurrentPreset.WeldPhaseLimitNew = PHASELIM_DEFAULTNEW;
	CurrentPreset.WeldPhaseLimitTimeNew = PHASELIMTIME_DEFAULTNEW;
	CurrentPreset.WeldTime = WELDTIME_DEFAULT;
	CurrentPreset.WeldTimeLimitFlags = WELDTIME_LIMIT_OFF;
	CurrentPreset.WeldTimeMLR = OFF;
	CurrentPreset.WeldTimeMLS = OFF;
	CurrentPreset.WeldTimePLR = OFF;
	CurrentPreset.WeldTimePLS = OFF;
	CurrentPreset.FreqWindowSize = 100;
	memcpy(CurrentPreset.WriteInField1,DEFAULTFIELD,WRITEINFIELD_LEN);
	memcpy(CurrentPreset.WriteInField1,DEFAULTFIELD,WRITEINFIELD_LEN);

	CurrentPreset.SWDip.SoftwareFlag = 1;
	CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPTEST;
	CurrentPreset.SWDip.Flags.EndOfWeldStore = ON;
	CurrentPreset.AlarmResetReq = FALSE;
	pprintf("\n Default Preset parameters initialized ");
}

void SetMinPresets(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the current preset with MinPreset values.      */
/*                                                                           */
/*****************************************************************************/
{
	//	Copy the MinPreset into the Current Preset
	memcpy(&CurrentPreset, &Preset_Min, sizeof(CurrentPreset) );
	SavePreset(0);
}

void SetMaxPresets(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the current preset with MaxPreset values.      */
/*                                                                           */
/*****************************************************************************/
{
	//	Copy the MaxPreset into the Current Preset
	memcpy(&CurrentPreset, &Preset_Max, sizeof(CurrentPreset) );
	SavePreset(0);
}

void UpdateRunningPreset()
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the running preset from current Preset.        */
/*                                                                           */
/*****************************************************************************/
{
   memcpy(&RunningPreset, &CurrentPreset, sizeof(RunningPreset) );
}

void ColdStart(void)
/****************************************************************************/
/* Description:-                                                            */
/* This function sets up the factory defaults. 										 */
/****************************************************************************/
{
	IOTaskStop = TRUE;
	StopStateMachine();
	memcpy(&ParmName, "DEFAULT 20KHz\n", MAXPARAMNAME_LEN);
	CurrentWorkNumber = 0;
	SetDefaultValues();
	SetMaxMinValues();
	for (int j = 0; j < MAX_WORK_NUMBER; j++)
	{
		SavePreset(j);
	}
	SetMinMaxScanParameter();
	//ResetCounters();
	//Set default value in ParanScan
	SetDefaultScanParameters();
	SaveScanParamCRC();
	UserIO::ResetUserIO();
	Log::ResetLog();
	SetDHCPServerIPDefaults();
	SetStaticIPDefaults();
	SetDefaultCycleParam();
	regUpdateCmdFlag = true;
	StartStateMachine ();
	IOTaskStop = FALSE;

}

void CheckCurrentPresetCRC()
/****************************************************************************/
/* Description:-                                                            */
/* This function calculates current preset CRC and compares with stored		 */
/* CRC ,if not correct SetDefaultValues function is called.		   			 */
/*                                                                          */
/****************************************************************************/
{
	UINT32 CRCValue = ByteCRC((UINT8*)&CurrentPreset, sizeof(CurrentPreset) - sizeof(CurrentPreset.CRC));
	if(CurrentPreset.CRC != CRCValue) 
	{
		Log::WriteEventLog(CRC_ERROR, (UINT16)CurrentPreset.CRC, (UINT16)CRCValue, 1, true);
		SetDefaultValues();
		SavePreset(0);
	}
}

void SaveCurrentPresetCRC()
/****************************************************************************/
/* Description:-                                                            */
/* This function saves current preset CRC in current preset structure.   	 */
/****************************************************************************/
{
	CurrentPreset.CRC = ByteCRC((UINT8 *)&CurrentPreset, sizeof(CurrentPreset) - sizeof(CurrentPreset.CRC));
}

void SetDefaultWeldPreset(BOOLEAN advancedPreset)
{
	if(advancedPreset)
	{
		switch(ePromData.PSfrequency)
      {
         case FREQ_20KHZ:
            CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_20KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_20KDEFAULT;
            CurrentPreset.WeldPhaseloopI = PLOOP_20KDEFAULT;
            CurrentPreset.WeldPhaseloopCF = PLOOPCF_20KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_20KDEFAULTNEW;
            CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_20KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopINew = PLOOP_20KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopPNew = PLOOPP_20KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_20KDEFAULTNEW;
            CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_20KDEFAULT;
         break;
         case FREQ_30KHZ:
            CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_30KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_30KDEFAULT;
            CurrentPreset.WeldPhaseloopI = PLOOP_30KDEFAULT;
            CurrentPreset.WeldPhaseloopCF = PLOOPCF_30KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_30KDEFAULTNEW;
            CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_30KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopINew = PLOOP_30KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopPNew = PLOOPP_30KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_30KDEFAULTNEW;
            CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_30KDEFAULT;
         break;
         case FREQ_40KHZ:
            CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_40KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_40KDEFAULT;
            CurrentPreset.WeldPhaseloopI = PLOOP_40KDEFAULT;
            CurrentPreset.WeldPhaseloopCF = PLOOPCF_40KDEFAULT;
            CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_40KDEFAULTNEW;
            CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_40KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopINew = PLOOP_40KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopPNew = PLOOPP_40KDEFAULTNEW;
            CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_40KDEFAULTNEW;
            CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_40KDEFAULT;
         break;
         default: // 20K Default
			CurrentPreset.WeldAmplitudeLoopC1 = AMPLOOPC1_20KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC2 = AMPLOOPC2_20KDEFAULT;
			CurrentPreset.WeldPhaseloopI = PLOOP_20KDEFAULT;
			CurrentPreset.WeldPhaseloopCF = PLOOPCF_20KDEFAULT;
			CurrentPreset.WeldAmplitudeLoopC1New = AMPLOOPC1_20KDEFAULTNEW;
			CurrentPreset.WeldAmplitudeLoopC2New = AMPLOOPC2_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopINew = PLOOP_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopPNew = PLOOPP_20KDEFAULTNEW;
			CurrentPreset.WeldPhaseloopCFNew = PLOOPCF_20KDEFAULTNEW;
			CurrentPreset.SeekPhaseloopI = SEEKPHASELOOP_20KDEFAULT;
        break;
      }

		CurrentPreset.FreqWindowSize = 100;
		CurrentPreset.WeldFrequencyLimitHigh = WELD_FREQLIMITHIGH;
		CurrentPreset.WeldFrequencyLimitLow = WELD_FREQLIMITLOW;
		CurrentPreset.WeldPhaseLimit = PHASELIM_DEFAULT;
		CurrentPreset.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(PHASELIMTIME_DEFAULT);
		CurrentPreset.WeldFrequencyLimitHighNew = WELD_FREQLIMITHIGHNEW;
		CurrentPreset.WeldFrequencyLimitLowNew = WELD_FREQLIMITLOWNEW;
		CurrentPreset.WeldPhaseLimitNew = PHASELIM_DEFAULTNEW;
		CurrentPreset.WeldPhaseLimitTimeNew = PHASELIMTIME_DEFAULTNEW;
		CurrentPreset.SeekAmplitude = SEEKAMPP_DEFAULT;
		CurrentPreset.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1_DEFAULT;
		CurrentPreset.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2_DEFAULT;
		CurrentPreset.SeekFrequLimitHigh = SEEKFREQHIGH_DEFAULT;
		CurrentPreset.SeekFrequLimitLow = SEEKFREQLOW_DEFAULT;
		CurrentPreset.SeekPhaseloopCF = SEEKPHASELOOPCF_DEFAULT;
		CurrentPreset.FrequencyLimitTime = CONV_MILLI_MICROSEC(FLIMIT_TIMEDEFAULT); //F-LIMIT DEFAUT
		SetDefaultScanParameters();
	}
	else
	{
		CurrentPreset.WeldAmplitude = CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE] = AMP1_DEFAULT;//weld preset
		CurrentPreset.SWDip.SoftwareFlag = 1;
		CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPTEST;
		CurrentPreset.SWDip.Flags.EndOfWeldStore = ON;
		CurrentPreset.FreqOffset = FREQ_OFFSET_DEFAULT;//weld preset
		CurrentPreset.WeldRampTime = CONV_MILLI_MICROSEC(WELDRAMPTIME_DEFAULT);//weld preset
		CurrentPreset.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIME_DEFAULT);
		CurrentPreset.SeekTime = CONV_MILLI_MICROSEC(SEEKTIME_DEFAULT);
		CurrentPreset.SeekFreqOffset = SEEK_FREQOFFSET_DEFAULT;
		CurrentPreset.AlarmResetReq = FALSE;
		CurrentPreset.MemoryFreq = 0;
		MemoryValue = 0;
		switch(ePromData.PSfrequency){
			case FREQ_20KHZ:
				CurrentPreset.DigitalTune = DIGITALTUNE20K_DEF;
			break;
			case FREQ_30KHZ:
				CurrentPreset.DigitalTune = DIGITALTUNE30K_DEF;
			break;
			case FREQ_40KHZ:
				CurrentPreset.DigitalTune = DIGITALTUNE40K_DEF;
			break;
			default:
				CurrentPreset.DigitalTune = DIGITALTUNE20K_DEF;
			break;
		}
		InitParamUpdateRequired = TRUE;
	}
	SavePreset(0);
	regUpdateCmdFlag = TRUE;
}

//DUMMY-Values:
//2Digits-Frequecy-Type
//4Digits-Power-Type
//2Digits-Parameter Index 09..00
//e.g. d20400005 (20KHz, 4000W, Idx 5)
void SetDefaultPreInitHDParams()
{
	for(int indx = 0 ;indx < SIZE_PREINITPARAMS; indx++)
	{
		CurrentPreset.HDParam.PreInitPara[indx]= indx + ePromData.PSpower * 100 * ePromData.PSfrequency * 1000000;
	}
	if(ePromData.PSfrequency == FREQ_20KHZ)
	{
		CurrentPreset.HDParam.PreInitPara[0] = 48;
		CurrentPreset.HDParam.PreInitPara[1] = 1388;
		CurrentPreset.HDParam.PreInitPara[2] = 1136;
		CurrentPreset.HDParam.PreInitPara[3] = 950;
	}
	else if(ePromData.PSfrequency == FREQ_30KHZ)
	{
		CurrentPreset.HDParam.PreInitPara[0] = 48;
		CurrentPreset.HDParam.PreInitPara[1] = 925;
		CurrentPreset.HDParam.PreInitPara[2] = 757;
		CurrentPreset.HDParam.PreInitPara[3] = 1024;
	}
	else if(ePromData.PSfrequency == FREQ_40KHZ)
	{
		CurrentPreset.HDParam.PreInitPara[0] = 48;
		CurrentPreset.HDParam.PreInitPara[1] = 694;
		CurrentPreset.HDParam.PreInitPara[2] = 568;
		CurrentPreset.HDParam.PreInitPara[3] = 1024;
	}

	SavePreset(0);
}


void SetDefaultHDInitParam()
{

	CurrentPreset.HDParam.InitPara[HD_MODE] 			= MODE_DEFAULT;

	//Parameter 9 to 14 for frequencies other than
	//20 Khz
	//for(int indx = 8  ;indx < SIZE_INITPARAMS - 1; indx++){           this will be overwrited on the follow switch case.
	//	CurrentPreset.HDParam.InitPara[indx]= indx + 12341111 * 100 ;
	//}
	switch(ePromData.PSfrequency){
		case FREQ_20KHZ:

			CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

			CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]=	HD_LT_FREQ_CTRL2_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_NOT_USED1] 	= 	HD_NOT_USED1_20K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_NOT_USED2] 	= 	HD_NOT_USED2_20K_DEFAULT;
			break;
		case FREQ_30KHZ:
			if (ePromData.PSpower==P4500W)
			{
				CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_30K_45KW_DEFAULT;

				CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_30K_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_30K_45KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_30K_45KW_DEFAULT;
			}
			else
			{
				if (ePromData.PSpower==P3000W)
				{
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

					CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_30K_30KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_30K_30KW_DEFAULT;

				}
				else
				{
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

					CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_30K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_30K_DEFAULT;
				}

			}


			break;
		case FREQ_40KHZ:

			if (ePromData.PSpower==P2400W)
			{
				CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

				CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_40K_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_40K_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_40K_24KW_DEFAULT;
				CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_40K_24KW_DEFAULT;

			}
			else
			{
				if (ePromData.PSpower==P1600W)
				{
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

					CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_40K_16KW_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_40K_16KW_DEFAULT;
				}
				else
				{
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK] 	= BLIND_TIME_SEEK_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD] 	= BLIND_TIME_WELD_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE]	= CurrentPreset.WeldAmplitude = WELD_AMPLITUDE_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] 	= PWM_PERCENTAGE_DEFAULT;

					CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME] 	= 	HD_SWITCH_TIME_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_HW_DELAY] 	= 	HD_HW_DELAY_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_FSYNC_DPLL] 	= 	HD_FSYNC_DPLL_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL1]= 	HD_LT_FREQ_CTRL1_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL2]= 	HD_LT_FREQ_CTRL2_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL3]= 	HD_LT_FREQ_CTRL3_40K_DEFAULT;
					CurrentPreset.HDParam.InitPara[HD_LT_FREQ_CTRL4]= 	HD_LT_FREQ_CTRL4_40K_DEFAULT;
				}
			}
			CurrentPreset.HDParam.InitPara[HD_NOT_USED1] 	= 	HD_NOT_USED1_40K_DEFAULT;
			CurrentPreset.HDParam.InitPara[HD_NOT_USED2]	= 	HD_NOT_USED2_40K_DEFAULT;

			break;
	}
	CurrentPreset.HDParam.InitPara[HD_HC_TEST] = HD_HC_TEST_DEFAULT;
	InitParamUpdateRequired = TRUE;
	regAmplitudeUpdateFlag = TRUE;
	SavePreset(0);
}

void UpdateFrequencyPowerSpecificHDParam()
{
	for(int indx = 0 ;indx < SIZE_PREINITPARAMS; indx++)
	{
		CurrentPreset.HDParam.PreInitPara[indx]= indx + ePromData.PSpower * 100 * ePromData.PSfrequency * 1000000;
	}
	if(ePromData.PSfrequency == FREQ_20KHZ)
	{
		CurrentPreset.HDParam.PreInitPara[0] = 48;
		CurrentPreset.HDParam.PreInitPara[1] = 1388;
		CurrentPreset.HDParam.PreInitPara[2] = 1136;
		CurrentPreset.HDParam.PreInitPara[3] = 950;
	}

	if(ePromData.PSfrequency == FREQ_20KHZ)
	{
		CurrentPreset.HDParam.InitPara[8] = 800;
		CurrentPreset.HDParam.InitPara[9] = 1000;
		CurrentPreset.HDParam.InitPara[10] = 200;
		CurrentPreset.HDParam.InitPara[11] = 5;
		CurrentPreset.HDParam.InitPara[12] = 0;
		CurrentPreset.HDParam.InitPara[13] = 0;
	}
	InitParamUpdateRequired = TRUE;
	SavePreset(0);
	regUpdateCmdFlag = TRUE;
}

void SetManFDefaultFromWeb()
/****************************************************************************/
/* Description:-                                                            */
/* This function sets up the factory defaults from WEB.  						 */
/****************************************************************************/
{
	pprintf("\n Restore default from Web Site");
	StartMFCycle = MFCycleFlag = CurrentCycleParam.MfTest = FALSE;
	memcpy(&ParmName, "DEFAULT 20KHz\n", MAXPARAMNAME_LEN);
	CurrentWorkNumber = 0;
	SetDefaultValues();
	SetMaxMinValues();
	for (int j = 0; j < MAX_WORK_NUMBER; j++)
	{
		SavePreset(j);
	}

	SetMinMaxScanParameter();
	//Set default value in ParanScan
	SetDefaultScanParameters();
	SaveScanParamCRC();

	ResetCounters();
	SetMinMaxCycleParam();
	SetDefaultCycleParam();
	ResetManFErrorCounter();
	MemoryReset = TRUE;
	Log::ResetLog();
	SetDHCPServerIPDefaults();
	SetStaticIPDefaults();
	regUpdateCmdFlag = TRUE;
}

void SetMinMaxCycleParam()
/****************************************************************************/
/* Description:-                                                            */
/* This function sets up minimum and maximum parameters of manufacturing Cycle.*/
/****************************************************************************/
{
	MinCycleParam.totalCycleTime = MIN_CYCLE_TIME;
	MinCycleParam.sonicsOnTimePerWeldCycle = MIN_ONIME_PERCYCLE;
	MinCycleParam.sonicsOffTimePerWeldCycle = MIN_OFFIME_PERCYCLE;
	MinCycleParam.Amplitude = Preset_Min.WeldAmplitude;
	MinCycleParam.stopOnAlarm = FALSE;
	MinCycleParam.MfTest = FALSE;

	MaxCycleParam.totalCycleTime = MAX_CYCLE_TIME;
	MaxCycleParam.sonicsOnTimePerWeldCycle = MAX_ONIME_PERCYCLE;
	MaxCycleParam.sonicsOffTimePerWeldCycle = MAX_OFFIME_PERCYCLE;
	MaxCycleParam.Amplitude = Preset_Max.WeldAmplitude;
	MaxCycleParam.stopOnAlarm = TRUE;
	MaxCycleParam.MfTest = FALSE;
}

void SetDefaultCycleParam()
/****************************************************************************/
/* Description:-                                                            */
/* This function sets up Default values of manufacturing Cycle parameters.             */
/****************************************************************************/
{
	CurrentCycleParam = MaxCycleParam;
	CurrentCycleParam.stopOnAlarm = FALSE;
	memset(&MFcycleResults, 0 , sizeof(MFcycleResults));
	CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
}


