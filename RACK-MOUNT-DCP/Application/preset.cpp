/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/preset.cpp_v   1.22   Jan 11 2017 15:25:58   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/preset.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added additional SIG default values.
 *
 *    Rev 1.22   Jan 11 2017 15:25:58   EGUAJARDO
 * Added new SIG default values of power limits.
 * 
 *    Rev 1.21   25 Jul 2016 12:25:14   GButron
 * RACKMOUNT_BASIC changed to DCX_S
 * 
 *    Rev 1.20   06 Jun 2016 15:14:40   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.19   30 Sep 2015 09:03:04   RJamloki
 * Changed default value of Clear Memory at Power up to TRUE
 * 
 *    Rev 1.18   21 Sep 2015 06:46:06   RJamloki
 * Added InRange check for Time maintained mode in VerifyPreset().
 * 
 *    Rev 1.17   01 Sep 2015 02:35:54   amaurya
 * Added defaults values for ClrMemAtPwrUp and SetDigTuneWithScan.
 * 
 *    Rev 1.16   25 Jun 2015 07:06:16   amaurya
 * Added default Amplitude for SIG special.
 * 
 *    Rev 1.15   04 Jun 2015 07:48:32   sesharma
 * Clear Memory Before seek flag added to preset.Branson coding standard formatter applied.
 * 
 *    Rev 1.14   09 Apr 2015 05:14:58   sesharma
 * All references to Impulse Start removed.
 * 
 *    Rev 1.13   19 Mar 2015 06:57:34   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.12   26 Feb 2015 08:27:44   sesharma
 * Impulse Start should be restored to default only when system type is Modified or at Cold Start.
 * 
 *    Rev 1.11   20 Feb 2015 16:20:02   ewomack
 * Set Impulse Start to default value from SetDefaultWeldPreset function
 * 
 *    Rev 1.10   22 Jan 2015 08:34:38   ewomack
 * Weld Mode and Time are set to defaults properly on cold start for DC-Std.
 * 
 *    Rev 1.9   22 Jan 2015 05:00:14   sesharma
 * Bug Fixing for default weld mode setting when cold start is done.
 * 
 *    Rev 1.8   15 Jan 2015 01:53:42   sesharma
 * Latching Alarm is Enabled by Default & bug fixing for restore defaults from LCD.
 * 
 *    Rev 1.7   13 Jan 2015 06:01:16   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.6   27 Nov 2014 09:14:18   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.5   07 Nov 2014 16:13:40   ewomack
 * Refactoring
 * 
 *    Rev 1.4   28 Oct 2014 16:55:18   ewomack
 * Make default PS frequency 20kHz when setting default preset values
 * 
 *    Rev 1.3   01 Oct 2014 10:07:34   amaurya
 * Removed unused array from SetDefaultValue function
 * 
 *    Rev 1.2   17 Sep 2014 08:41:22   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.1   03 Sep 2014 07:38:24   amaurya
 * Added defines for ClearMemAtRst in CurrentPreset.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

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
#include "SysConfig.h"
#include "preset.h"
#include "ReceiveCommandHandler.h"

#define IN_RANGE(value)   ((RunningPreset.value >= Preset_Min.value) && (RunningPreset.value <= Preset_Max.value))

/*---------------------------- Global variables -------------------------------*/
Parameter Preset_Min;
Parameter Preset_Max;
BOOLEAN IOTaskStop = FALSE;
Parameter MaxParameters;
Parameter MinParameters;

BOOLEAN SavePreset(SINT32 PresetNum)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will save preset specified by PresetNum to FRAM.           */
/*                                                                           */
/*****************************************************************************/
{
   SINT32 SavePresetNo = PresetNum;
   BOOLEAN ReturnValue = FALSE;
   if (((SavePresetNo) >= 0) && ((SavePresetNo) < MAX_WORK_NUMBER)) {
      SaveCurrentPresetCRC();
      //Write the parameters to given memory.
      storedPreset[SavePresetNo] = CurrentPreset;
      PresetVerifiedRequired = TRUE;
      UpdateRPreset = TRUE;
      ReturnValue = TRUE;
   }
   else
      ReturnValue = FALSE;   //Return failure.

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

   if (PresetNum < MAX_WORK_NUMBER) {
      StatusSlot[PresetNum] = CurrentDCPStatus;
      CurrentWorkNumber = PresetNum;
      //Copy preset to current preset.
      CurrentPreset = storedPreset[CurrentWorkNumber];
      CurrentDCPStatus = StatusSlot[CurrentWorkNumber];

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
   if (IN_RANGE(WeldAmplitude) && IN_RANGE(WeldAmplitudeLoopC1) &&
   IN_RANGE(WeldAmplitudeLoopC2) && IN_RANGE(WeldRampTime) &&
   IN_RANGE(WeldPhaseLimit) && IN_RANGE(WeldPhaseLimitTime) &&
   IN_RANGE(WeldPhaseloopI) && IN_RANGE(WeldPhaseloopCF) &&
   IN_RANGE(SeekAmplitude) && IN_RANGE(SeekAmplitudeLoopC1) &&
   IN_RANGE(SeekAmplitudeLoopC2) && IN_RANGE(SeekPhaseloopI) &&
   IN_RANGE(SeekPhaseloopCF) && IN_RANGE(SeekRampTime) &&
   IN_RANGE(SeekTime)&& IN_RANGE(SeekFrequLimitHigh)&&
   IN_RANGE(SeekFrequLimitLow)&& IN_RANGE(WeldFrequencyLimitHigh)&&
   IN_RANGE(WeldFrequencyLimitLow) && IN_RANGE(SeekFreqOffset) &&
   (IN_RANGE(WeldMode) || CurrentPreset.WeldMode == TIMEMAINTAINEDMODE) && IN_RANGE(EnergyCP)
   && IN_RANGE(WeldTime) && IN_RANGE(PeakPowerCP) && IN_RANGE(ScrubTime))
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
   /*******Minimum Preset Values******************************/
   MinParameters.AbAmp = ABAMPMIN;
   MinParameters.AbDelay = ABDELAYMIN;
   MinParameters.AbFlag = FALSE;
   MinParameters.AbTime = ABTIMEMIN;
   MinParameters.WeldAmplitude = AMP1MIN;
   MinParameters.Amplitude2 = AMP2MIN;
   MinParameters.WeldAmplitudeLoopC1 = AMPLOOPC1MIN;
   MinParameters.WeldAmplitudeLoopC2 = AMPLOOPC2MIN;
   MinParameters.WeldAmplitudeLoopC1New = AMPLOOPC1MINNEW;
   MinParameters.WeldAmplitudeLoopC2New = AMPLOOPC2MINNEW;
   MinParameters.AmpStartRamp = AMPSTARTRAMPMIN;
   MinParameters.AmpStepCol = AMPSTEPCOLMIN;
   MinParameters.AmpStepEnergy = AMPSTEPENERMIN;
   MinParameters.AmpStepExt = FALSE;
   MinParameters.AmpStepPower = AMPSTEPOWMIN;
   MinParameters.ScrubAmp = SCRUBAMPMIN;
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
   MinParameters.CutOffs = 0;
   MinParameters.EBAmp = EBAMPMIN;
   MinParameters.EBFlag = FALSE;
   MinParameters.EBTime = EBATIMEMIN;
   MinParameters.EnergyCP = ENERGYCPMIN;
   MinParameters.EnergyComp = ENERGYCOMPMIN;
   MinParameters.EnergyCompFlag = FALSE;
   MinParameters.EnergyCutoff = ENERGYCUTOFFMIN;
   MinParameters.EnergyCutoffFlag = FALSE;
   MinParameters.EnergyMLR = ENERGYMLRMIN;
   MinParameters.EnergyMLS = ENERGYMLSMIN;
   MinParameters.EnergyPLR = ENERGYPLRMIN;
   MinParameters.EnergyPLS = ENERGYPLSMIN;
   if (EPromData.PSfrequency == FREQ_20KHZ) {
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
   else if (EPromData.PSfrequency == FREQ_30KHZ) {
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
   else if (EPromData.PSfrequency == FREQ_40KHZ) {
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
   MinParameters.GeneralAlarmFlag = FALSE;
   MinParameters.GrdDetectCP = FALSE;
   MinParameters.HoldTime = HOLDTIMEMIN;
   MinParameters.MaxWeldTime = MAXWELDTIMEMIN;
   MinParameters.MemoryStoreFlag = FALSE;

   MinParameters.ModifiedDate = MODIFY_DATEMIN;
   MinParameters.ModifiedHour = MODIFY_HOURMIN;
   MinParameters.ModifiedMinute = MODIFY_MINUTEMIN;
   MinParameters.ModifiedMonth = MODIFY_MONTHMIN;
   MinParameters.ModifiedYear = MODIFY_YEARMIN;
   MinParameters.PeakPowerCP = PEAKPOWCPMIN;
   MinParameters.PeakPowerCutoff = PEAKPOWCUTOFFMIN;
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
   MinParameters.PowerMLR = POWERMLRMIN;
   MinParameters.PowerMLS = POWERMLSMIN;
   MinParameters.PowerPLR = POWERPLRMIN;
   MinParameters.PowerPLS = POWERPLSMIN;
   MinParameters.ScrubTime = SCRUBTIMEMIN;
   MinParameters.ScrubTimeFlag = FALSE;
   MinParameters.SeekAmplitude = SEEKAMPPMIN;
   MinParameters.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1MIN;
   MinParameters.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2MIN;
   MinParameters.SeekPhaseloopI = SEEKPHASELOOPMIN;
   MinParameters.SeekPhaseloopCF = SEEKPHASELOOPCFMIN;
   MinParameters.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIMEMIN);
   MinParameters.SeekTime = CONV_MILLI_MICROSEC(SEEKTIMEMIN);
   MinParameters.TestAmp = TESTAMPMIN;
   MinParameters.TimedSeekFlag = FALSE;
   MinParameters.WeldCount = WELDCOUNTMIN;
   MinParameters.WeldMode = CONTINUOUSMODE;
   MinParameters.WeldRampTime = CONV_MILLI_MICROSEC(AMPSTARTRAMPMIN);
   MinParameters.WeldStatusFlag = FALSE;
   MinParameters.WeldTime = WELDTIMEMIN;
   MinParameters.WeldTimeMLR = WELDTIMEMLRMIN;
   MinParameters.WeldTimeMLS = WELDTIMEMLSMIN;
   MinParameters.WeldTimePLR = WELDTIMEPLRMIN;
   MinParameters.WeldTimePLS = WELDTIMEPLSMIN;
   /*******Maximum Preset Values******************************/
   MaxParameters.AbAmp = ABAMPMAX;
   MaxParameters.AbDelay = ABDELAYMAX;
   MaxParameters.AbFlag = TRUE;
   MaxParameters.AbTime = ABTIMEMAX;
   MaxParameters.WeldAmplitude = AMP1MAX;
   MaxParameters.Amplitude2 = AMP2MAX;
   MaxParameters.WeldAmplitudeLoopC1 = AMPLOOPC1MAX;
   MaxParameters.WeldAmplitudeLoopC2 = AMPLOOPC2MAX;
   MaxParameters.WeldAmplitudeLoopC1New = AMPLOOPC1MAXNEW;
   MaxParameters.WeldAmplitudeLoopC2New = AMPLOOPC2MAXNEW;
   MaxParameters.AmpStartRamp = AMPSTARTRAMPMAX;
   MaxParameters.AmpStepCol = AMPSTEPCOLMAX;
   MaxParameters.AmpStepExt = TRUE;
   MaxParameters.AmpStepPower = AMPSTEPOWMAX;
   MaxParameters.ScrubAmp = SCRUBAMPMAX;
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
   MaxParameters.CutOffs = 1;
   MaxParameters.EBAmp = EBAMPMAX;
   MaxParameters.EBFlag = TRUE;
   MaxParameters.EBTime = EBATIMEMAX;
   MaxParameters.EnergyCP = ENERGYCPMAX;
   MaxParameters.EnergyComp = ENERGYCOMPMAX;
   MaxParameters.EnergyCompFlag = TRUE;
   MaxParameters.EnergyCutoff = ENERGYCUTOFFMAX;
   MaxParameters.EnergyCutoffFlag = TRUE;
   MaxParameters.EnergyMLR = ENERGYMLRMAX;
   MaxParameters.EnergyMLS = ENERGYMLSMAX;
   MaxParameters.EnergyPLR = ENERGYPLRMAX;
   MaxParameters.EnergyPLS = ENERGYPLSMAX;
   MaxParameters.FreqOffsetFlag = TRUE;
   MaxParameters.GeneralAlarmFlag = TRUE;
   MaxParameters.GrdDetectCP = TRUE;
   MaxParameters.HoldTime = HOLDTIMEMAX;
   MaxParameters.MaxWeldTime = MAXWELDTIMEMAX;
   MaxParameters.MemoryStoreFlag = TRUE;
   MaxParameters.ModifiedDate = MODIFY_DATEMAX;
   MaxParameters.ModifiedHour = MODIFY_HOURMAX;
   MaxParameters.ModifiedMinute = MODIFY_MINUTEMAX;
   MaxParameters.ModifiedMonth = MODIFY_MONTHMAX;
   MaxParameters.ModifiedYear = MODIFY_YEARMAX;
   MaxParameters.PeakPowerCP = PEAKPOWCPMAX;
   MaxParameters.PeakPowerCutoff = PEAKPOWCUTOFFMAX;
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
   MaxParameters.PowerMLR = POWERMLRMAX;
   MaxParameters.PowerMLS = POWERMLSMAX;
   MaxParameters.PowerPLR = POWERPLRMAX;
   MaxParameters.PowerPLS = POWERPLSMAX;
   MaxParameters.ScrubTime = SCRUBTIMEMAX;
   MaxParameters.ScrubTimeFlag = TRUE;
   MaxParameters.SeekAmplitude = SEEKAMPPMAX;
   MaxParameters.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1MAX;
   MaxParameters.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2MAX;
   MaxParameters.SeekPhaseloopI = SEEKPHASELOOPMAX;
   MaxParameters.SeekPhaseloopCF = SEEKPHASELOOPCFMAX;
   MaxParameters.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIMEMAX);
   MaxParameters.SeekTime = CONV_MILLI_MICROSEC(SEEKTIMEMAX);
   MaxParameters.TestAmp = TESTAMPMAX;
   MaxParameters.TimedSeekFlag = TRUE;
   MaxParameters.WeldCount = WELDCOUNTMAX;
   switch (BIT(EPromData.Systemtype)) {
   case DCX_S:
   case RACKMOUNT_S:
      MaxParameters.WeldMode = CONTINUOUSMODE;					//Added new
      break;
   case RACKMOUNT_STD:
      MaxParameters.WeldMode = ENERGYMODE;				//Changed as preset verification failed when in Time Maintained mode.
      break;
   default:
      break;
   }
   MaxParameters.WeldRampTime = CONV_MILLI_MICROSEC(AMPSTARTRAMPMAX);
   MaxParameters.WeldStatusFlag = TRUE;
   MaxParameters.WeldTime = WELDTIMEMAX;
   MaxParameters.WeldTimeMLR = WELDTIMEMLRMAX;
   MaxParameters.WeldTimeMLS = WELDTIMEMLSMAX;
   MaxParameters.WeldTimePLR = WELDTIMEPLRMAX;
   MaxParameters.WeldTimePLS = WELDTIMEPLSMAX;
   memcpy(MinParameters.PresetID, "MINIMUM    ", PRESETID_LEN);
   memcpy(&Preset_Min, &MinParameters, sizeof(Parameter));

   memcpy(MaxParameters.PresetID, "MAXIMUM    ", PRESETID_LEN);
   memcpy(&Preset_Max, &MaxParameters, sizeof(Parameter));

}

void SetDefaultValues(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the current preset with default values when    */
/*  a cold start is executed.                                                */
/*                                                                           */
/*****************************************************************************/
{
   CurrentPreset.AbAmp = ABAMP_DEFAULT;
   CurrentPreset.AbDelay = ABDELAY_DEFAULT;
   CurrentPreset.AbFlag = FALSE;
   CurrentPreset.AbTime = ABTIME_DEFAULT;
   //CurrentPreset.WeldAmplitude = AMP1_DEFAULT;
   CurrentPreset.Amplitude2 = AMP2_DEFAULT;
   CurrentPreset.SystemType = EPromData.Systemtype;
   switch (EPromData.PSfrequency) {
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
   default:
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
   }
   CurrentPreset.AmpStartRamp = AMPSTARTRAMP_DEFAULT;
   CurrentPreset.ScrubAmp = SCRUBAMP_DEFAULT;
   CurrentPreset.ColMLR = OFF;
   CurrentPreset.CycleAborts = CYCLE_ABORT_OFF;
   CurrentPreset.CutOffs = 0;
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
   CurrentPreset.FreqLimitFlags = FREQ_LIMIT_OFF;
   CurrentPreset.FreqMLR = OFF;
   CurrentPreset.FreqMLS = OFF;
   CurrentPreset.FreqPLR = OFF;
   CurrentPreset.FreqPLS = OFF;
   CurrentPreset.FreqOffset = FREQ_OFFSET_DEFAULT;
   CurrentPreset.FreqOffsetFlag = FALSE;
   CurrentPreset.GeneralAlarmFlag = FALSE;
   CurrentPreset.GrdDetectCP = FALSE;
   CurrentPreset.HoldTimeFlag = FALSE;
   CurrentPreset.MaxWeldTime = MAXWELDTIME_DEFAULT;
   CurrentPreset.MemoryStoreFlag = FALSE;
   CurrentPreset.MemoryFreq = MEMFREQ_DEFAULT;
   CurrentPreset.PeakPowerCP = (UINT8) PEAKPOWER_DEFAULT;
   CurrentPreset.PeakPowerCutoff = PEAKPOWCUTOFF_DEFAULT;
   CurrentPreset.PeakPowerCutoffFlag = FALSE;
   CurrentPreset.PowerMLR = OFF;
   CurrentPreset.PowerMLS = OFF;
   CurrentPreset.PowerPLR = OFF;
   CurrentPreset.PowerPLS = OFF;
   memcpy(CurrentPreset.PresetID, "COLD START", 12);
   CurrentPreset.PresetStatus = NOTVERIFIED;
   CurrentPreset.ScrubTime = SCRUBTIME_DEFAULT;
   CurrentPreset.SeekAmplitude = SEEKAMPP_DEFAULT;
   CurrentPreset.SeekAmplitudeLoopC1 = SEEKAMPLOOPC1_DEFAULT;
   CurrentPreset.SeekAmplitudeLoopC2 = SEEKAMPLOOPC2_DEFAULT;
   CurrentPreset.SeekFrequLimitHigh = SEEKFREQHIGH_DEFAULT;
   CurrentPreset.SeekFrequLimitLow = SEEKFREQLOW_DEFAULT;
   CurrentPreset.SeekFreqOffset = SEEK_FREQOFFSET_DEFAULT;
   CurrentPreset.SeekPhaseloopCF = SEEKPHASELOOPCF_DEFAULT;
   CurrentPreset.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIME_DEFAULT);
   CurrentPreset.SeekTime = CONV_MILLI_MICROSEC(SEEKTIME_DEFAULT);
   CurrentPreset.SIG = DEFSIG;
   CurrentPreset.TestAmp = TESTAMP_DEFAULT;
   CurrentPreset.TimedSeekFlag = FALSE;
   CurrentPreset.WeldCount = WELDCOUNT_DEFAULT;
   if (BIT(EPromData.Systemtype) == RACKMOUNT_STD) {
      CurrentPreset.WeldMode = TIMEMODE;
      CurrentPreset.SWDip.Flags.PowerLimitOn = ON;
      CurrentPreset.SWDip.Flags.PlusLimitCheck = ON;
      CurrentPreset.SWDip.Flags.MinusLimitCheck = ON;
      CurrentPreset.WeldTime = TIMEMODE_DEFAULT; // Time
   }
   else {
      CurrentPreset.WeldMode = CONTINUOUSMODE;
      CurrentPreset.WeldTime = WELDTIME_DEFAULT;
   }
   CurrentPreset.WeldFrequencyLimitHigh = WELD_FREQLIMITHIGH;
   CurrentPreset.WeldFrequencyLimitLow = WELD_FREQLIMITLOW;
   CurrentPreset.WeldFrequencyLimitHighNew = WELD_FREQLIMITHIGHNEW;
   CurrentPreset.WeldFrequencyLimitLowNew = WELD_FREQLIMITLOWNEW;
   CurrentPreset.WeldRampTime = CONV_MILLI_MICROSEC(WELDRAMPTIME_DEFAULT);
   CurrentPreset.WeldStatusFlag = FALSE;
   CurrentPreset.WeldPhaseLimit = PHASELIM_DEFAULT;
   CurrentPreset.WeldPhaseLimitTime = CONV_MILLI_MICROSEC(PHASELIMTIME_DEFAULT);
   CurrentPreset.WeldPhaseLimitNew = PHASELIM_DEFAULTNEW;
   CurrentPreset.WeldPhaseLimitTimeNew = PHASELIMTIME_DEFAULTNEW;
   CurrentPreset.WeldTimeMLR = OFF;
   CurrentPreset.WeldTimeMLS = OFF;
   CurrentPreset.WeldTimePLR = OFF;
   CurrentPreset.WeldTimePLS = OFF;
   CurrentPreset.FreqWindowSize = 100;
   CurrentPreset.SWDip.SoftwareFlag = 1;
   CurrentPreset.SWDip.Flags.EndOfWeldStore = ON;
   CurrentPreset.AlarmResetReq = TRUE;

   CurrentPreset.Jumper2 = JP3_12AMP;
   //CurrentPreset.SWDip.Flags.ClearMemAtRst = OFF;
   CurrentPreset.SWDip.Flags.ClrMemBfrSeek = ON;
   //SIG Special feature is available only for DC-STD. But once its enabled, it remains enabled even after changing system type.
   if((RACKMOUNT_STD == BIT(EPromData.Systemtype)) && (EPromData.SIGSpecial)) {
      CurrentPreset.WeldAmplitude = AMP1_DEFAULT_SIG;   //weld preset Amp
      CurrentPreset.SWDip.Flags.ClearMemAtRst = ON;
      CurrentPreset.PowerWindowLimitLow = DEFPOWERLIMITLOWSIG; //PLL
      CurrentPreset.PowerWindowLimitHigh = DEFPOWERLIMITHIGHSIG; //PHL
      CurrentPreset.SWDip.Flags.SetDigTuneWithScan = ON;
      CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPSCAN;
   }
   else{
      CurrentPreset.WeldAmplitude = AMP1_DEFAULT;   //weld preset
      CurrentPreset.SWDip.Flags.ClearMemAtRst = OFF;
      CurrentPreset.PowerWindowLimitLow = DEFPOWERLIMITLOW;
      CurrentPreset.PowerWindowLimitHigh = DEFPOWERLIMITHIGH;
      CurrentPreset.SWDip.Flags.SetDigTuneWithScan = OFF;
      CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPSEEK;
   }
   CurrentPreset.SWDip.Flags.ClrMemAtPwrUp = TRUE;
   printf("\n Default Preset parameters initialized ");
}

void SetMinPresets(void)
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the current preset with MinPreset values.      */
/*                                                                           */
/*****************************************************************************/
{
   //	Copy the MinPreset into the Current Preset
   memcpy(&CurrentPreset, &Preset_Min, sizeof(CurrentPreset));
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
   memcpy(&CurrentPreset, &Preset_Max, sizeof(CurrentPreset));
   SavePreset(0);
}

void UpdateRunningPreset()
/*****************************************************************************/
/*  Description:-                                                            */
/*  This function will update the running preset from current Preset.        */
/*                                                                           */
/*****************************************************************************/
{
   memcpy(&RunningPreset, &CurrentPreset, sizeof(RunningPreset));
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
   SetMaxMinValues();
   SetDefaultValues();
   for (UINT8 j = 0; j < MAX_WORK_NUMBER; j++) {
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
   RegUpdateCmdFlag = true;
   StartStateMachine();
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
   UINT32 CRCValue = ByteCRC((UINT8*) &CurrentPreset,
         sizeof(CurrentPreset) - sizeof(CurrentPreset.CRC));
   if (CurrentPreset.CRC != CRCValue) {
      Log::WriteEventLog(CRC_ERROR, (SINT32) CurrentPreset.CRC,
            (SINT32) CRCValue, 1, true);
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
   CurrentPreset.CRC = ByteCRC((UINT8 *) &CurrentPreset,
         sizeof(CurrentPreset) - sizeof(CurrentPreset.CRC));
}

void SetDefaultWeldPreset(void)
{
	CurrentPreset.SWDip.SoftwareFlag = 1; //It should be updated 1st before updating any flag.
   //SIG Special feature is available only for DC-STD. But once its enabled, it remains enabled even after changing system type.
   if((RACKMOUNT_STD == BIT(EPromData.Systemtype)) && (EPromData.SIGSpecial)) {
      CurrentPreset.WeldAmplitude = AMP1_DEFAULT_SIG;   //weld preset Amp
      CurrentPreset.SWDip.Flags.ClearMemAtRst = ON;
      CurrentPreset.PowerWindowLimitLow = DEFPOWERLIMITLOWSIG; // PLL
      CurrentPreset.PowerWindowLimitHigh = DEFPOWERLIMITHIGHSIG; //PHL
      CurrentPreset.SWDip.Flags.SetDigTuneWithScan = ON;
      CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPSCAN;
   }
   else{
      CurrentPreset.WeldAmplitude = AMP1_DEFAULT;   //weld preset
      CurrentPreset.SWDip.Flags.ClearMemAtRst = OFF;
      CurrentPreset.PowerWindowLimitLow = DEFPOWERLIMITLOW;
      CurrentPreset.PowerWindowLimitHigh = DEFPOWERLIMITHIGH;
      CurrentPreset.SWDip.Flags.SetDigTuneWithScan = OFF;
      CurrentPreset.SWDip.Flags.PowerUpOptions = POWERUPSEEK;
   }

   CurrentPreset.SWDip.Flags.EndOfWeldStore = ON;
   CurrentPreset.SWDip.Flags.ExtAmplitude = OFF;
   CurrentPreset.SWDip.Flags.TimedSeek = OFF;
   CurrentPreset.SWDip.Flags.ExtFrequency = OFF;
   CurrentPreset.SWDip.Flags.ClrMemBfrSeek = ON;
   CurrentPreset.SWDip.Flags.ClrMemAtPwrUp = TRUE;
   CurrentPreset.CutOffs = NOTUSED;
   if (BIT(EPromData.Systemtype) == RACKMOUNT_STD) {
      CurrentPreset.WeldMode = TIMEMODE;
      CurrentPreset.WeldTime = TIMEMODE_DEFAULT;
   }
   else
      CurrentPreset.WeldMode = CONTINUOUSMODE;

   CurrentPreset.EnergyCP = ENERGYCP_DEFAULT;
   CurrentPreset.PeakPowerCP = PEAKPOWER_DEFAULT;
   CurrentPreset.ScrubTime = NOTUSED;
   CurrentPreset.SystemType = EPromData.Systemtype;
   CurrentPreset.FreqOffset = FREQ_OFFSET_DEFAULT;   //weld preset
   CurrentPreset.WeldRampTime = CONV_MILLI_MICROSEC(WELDRAMPTIME_DEFAULT);   //weld preset
   CurrentPreset.SeekRampTime = CONV_MILLI_MICROSEC(SEEKRAMPTIME_DEFAULT);
   CurrentPreset.SeekTime = CONV_MILLI_MICROSEC(SEEKTIME_DEFAULT);
   CurrentPreset.SeekFreqOffset = SEEK_FREQOFFSET_DEFAULT;
   CurrentPreset.AlarmResetReq = TRUE;
   CurrentPreset.MemoryFreq = OFF;
   CurrentPreset.SWDip.Flags.PowerLimitOn = ON;
   CurrentPreset.SWDip.Flags.PlusLimitCheck = ON;
   CurrentPreset.SWDip.Flags.MinusLimitCheck = ON;
   CurrentPreset.Limits = ((CurrentPreset.SWDip.Flags.PowerLimitOn << 0)
         | (CurrentPreset.SWDip.Flags.MinusLimitCheck << 1)
         | (CurrentPreset.SWDip.Flags.PlusLimitCheck << 2));
   CurrentPreset.Jumper2 = JP3_12AMP;
   MemoryValue = 0;
   if (BIT(EPromData.Systemtype) == DCX_S) {
      CurrentPreset.Jumper2 = JP3_12AMP;   // To ask what should be default value
      UserIO::AssignAnalogOutputFunction(AOUT2, AMPL_OUT, TRUE);
   }
   switch (EPromData.PSfrequency) {
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
   SavePreset(0);
   RegUpdateCmdFlag = TRUE;
}

void SetDefaultAdvancedPreset(void)
{
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
   default:
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
   SetDefaultScanParameters();
   SavePreset(0);
   RegUpdateCmdFlag = TRUE;
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
   for (UINT8 j = 0; j < MAX_WORK_NUMBER; j++) {
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
   RegUpdateCmdFlag = TRUE;
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
   memset(&MFcycleResults, 0, sizeof(MFcycleResults));
   CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
         sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
}

