/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SCAN.cpp_v   1.6   06 Jun 2016 15:24:30   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SCAN.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added SIG frequency start stop parameters
 *
 *    Rev 1.6   06 Jun 2016 15:24:30   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.5   04 Jun 2015 07:49:30   sesharma
 * Type casting for Data1 & Data2 fields of event log changed to SINT32 when writing an event. Branson coding standard formatter applied.
 * 
 *    Rev 1.4   19 Mar 2015 07:01:06   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   13 Jan 2015 06:01:58   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.2   07 Nov 2014 16:20:26   ewomack
 * Remove AlarmFlag.
 * 
 *    Rev 1.1   17 Sep 2014 08:59:54   amaurya
 * Code Review General comments changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#include "GLOBALS.h"
#include "FUNC.h"
#include "StateMachineDrv.h"
#include "stdio.h"
#include "UserIO.h"
#include "CRC.h"
#include "string.h"
#include "SysConfig.h"
#include "SCAN.h"
#include "Log.h"
#define SCANIN_RANGE(value)   ((ParaScan.value >= ScanParamMin.value) && (ParaScan.value <= ScanParamMax.value))

/*-------------------------- Global  Variables ----------------------------*/
SINT32 SignValue;
static SINT32 ScanTimer = 0;
ScanParameter ScanParamMin;
ScanParameter ScanParamMax;
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 *HornScanRawFrequency;
extern SINT16 *HornScanRawPhase;
extern SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
extern SINT16 HornScanRawTestPhase[DCP_HORNSCANMAXPOINTS];
extern RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
UINT8 PercentScan;
/*--------------------------------------------- ---------------------------*/

void InitializeScanbuffer(void)
/****************************************************************************/
/* InitializeScanbuffer(void)                                               */
/* Description:-                                                            */
/* Function Initializes ScanBuffer.											          */
/* 																			                */
/****************************************************************************/
{
   Reset_Scanbuffer();
}

void Reset_Scanbuffer(void)
/******************************************************************************/
/* Reset_Scanbuffer(void)							                    	            */
/* Description:-                                                              */
/* In this function ,ScanBuffer is initialiazed.  							         */
/******************************************************************************/
{
   UINT16 data = HornScanPointNotValid;
   HornScanGraphPointCount = -1;

   ScanBuffer[HornScanGraphPointCount + 1].Status = data;

   //  Clear the old scan phase
   for (SINT32 i = 0; i <= DCP_HORNSCANMAXPOINTS; i++)
      HornScanRawPhase[i] = SCANRAWPHASE_DEFAULT;

   //  Clear the old scan Resosance frequency
   for (SINT32 i = 0; i < DCP_NUMRESONANCES; i++) {
      ResonanceFreqs[i] = 0;
      ResonanceFreqType[i] = FREQ_NONE;
   }
   ScanTimer = 0;
}

BOOLEAN IncScanBufferIndex(void)
/******************************************************************************/
/* IncScanBufferIndex(void)												               */
/* Description:-                                                              */
/* In this function ,ScanBuffer index is increased until it becomes equal 	   */
/* to maximum after that scan is stopped.									            */
/* returns 0 if end of scan buffer is reached else 1 if not.*/
/******************************************************************************/
{
   UINT16 data = HornScanPointValid;
   BOOLEAN LastScanPointNotReached = FALSE;
   if (HornScanGraphPointCount >= (SIZE_SCANBUFFER - 2)) {
      ScanBuffer[HornScanGraphPointCount - 1].Status = data;
      data = HornScanPointLast;
      //End Mark of scanbuffer
      ScanBuffer[HornScanGraphPointCount].Status = data;
      LastScanPointNotReached = FALSE;
   }
   else {
      // Increment horn scan buffer index
      HornScanGraphPointCount++;
      LastScanPointNotReached = TRUE;
   }
   return LastScanPointNotReached;
}

void Scan_Start(void)
/******************************************************************************/
/* scan_start(void)															               */
/* Description:-                                                              */
/* In this function ,A scan through the whole band is started. Initially, only*/
/* the frequency initialized the amplitude is automatically regulated.  	   */
/******************************************************************************/
{
   BOOLEAN PresetVerifiedSuccess = TRUE;
   if (PresetVerifiedRequired) {
      PresetVerifiedRequired = FALSE;
      PresetVerifiedSuccess = VerifyScanParam();
   }
   if (!PresetVerifiedSuccess) {
      pprintf("\n preset verified failed, assigning boundary values");
      CheckAndAssignScanCycleBoundaryValue();
   }
   static UINT32 count;
   DDSFrequency = ParaScan.FrequencyStart;

   FrequencyValue = 0;
   /* detect direction of scan */
   if ((ParaScan.FrequencyStop - ParaScan.FrequencyStart) > 0)
      SignValue = 1;
   else
      SignValue = -1;

   if (!count) {
      //Increase FPGA output amplitude
      PWMAmp++;
      count = ParaScan.TimeDelay;
   }
   else if (count >= DCPTIME) {
      count -= DCPTIME;
   }
   else
      count = 0;

   // Check if feedback current or amplitude reaches Max. current/amplitude in preset
   if ((CurrentInput > ((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR))
         || (ActualAmplitude
               > ((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR))) {
      ScanStarted = TRUE;
   }

   StartSonics();
} /* scan_start() */

void Scan_Run(void)
/******************************************************************************/
/* scan_run(void)															                  */
/* Description:-                                                              */
/* The resonance frequency is calculated in whole Band.	  					      */
/*   										                                             */
/******************************************************************************/
{
   static UINT32 delay = 0;

   if ((ScanTimer > CONV_MILLI_MICROSEC(SCANTIMEOUT))
         || (WebSite::CheckForWebScanAbortCommand() && !PowerUpScan)
         || (!UserIO::CheckForHornScan() && !EnableWebIO && !PowerUpScan)) {
      pprintf("\n Scan Timeout");
      ScanTimer = 0;
      ScanAbort = TRUE;
      ScanInitiate = FALSE;
      UserIO::SetSeekOut(FALSE);
      UserIO::SetSonicsActive(FALSE);
   }
   ScanTimer += DCPTIME;
   if (!delay) {
      //Check if feedback Current or amplitude is greater than their Preset values
      if ((CurrentInput
            > ((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR))
            || (ActualAmplitude
                  > ((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR))) {
         //Decrease FPGA output amplitude
         PWMAmp--;
         delay = ParaScan.TimeDelay;
      }
      //Check if feedback Current and amplitude is less than half of Preset values
      else if ((CurrentInput
            < (((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR) / 2))
            && (ActualAmplitude
                  < (((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR)
                        / 2))) {
         //Increase FPGA output amplitude
         PWMAmp++;
         delay = ParaScan.TimeDelay;
      }
      Detect_Resonance();
   }
   else if (delay >= DCPTIME) {
      //Decrease DelayCounter
      delay -= DCPTIME;
   }
   else
      delay = 0;

   StartSonics();
}/* scan(.. */

void Detect_Resonance(void)
/******************************************************************************/
/* detect_resonance(void)													               */
/* Description:-                                                              */
/* Here current,phase and Amplitude values are stored in ScanBuffer (FRAM)    */
/* at each frequency . If frequency violate boundary condition ,then 		   */
/* Scan is stopped .                                                          */
/******************************************************************************/
{
   static SINT32 CountDelay = 0;
   SINT16 data;
   SINT32 freq;
   //Check if CountDelay is greater than Preset variable TimeDelay
   if (CountDelay > ParaScan.TimeDelay) {
      //Check direction of scan
      if (SignValue > 0) {
         // Check boundary condition
         if (DDSFrequency < ParaScan.FrequencyStop) {
            printf(".");
            DDSFrequency += ParaScan.FrequencyStep;
         }
         else {
            ScanRun = TRUE;
            ScanTimer = 0;
         }
      }
      else {
         // Check boundary condition
         if (DDSFrequency > ParaScan.FrequencyStop) {
            DDSFrequency -= ParaScan.FrequencyStep;
         }
         else {
            ScanRun = TRUE;
            ScanTimer = 0;
         }
      }
      // Check Scan Buffer Index against maximum value
      if (IncScanBufferIndex()) {
         //Update Scan Buffer
         if (HornScanGraphPointCount != 0) {
            data = 0x00;
            ScanBuffer[HornScanGraphPointCount - 1].Status = data;
         }
         data = 0x02;
         ScanBuffer[HornScanGraphPointCount].Status = data;

         freq = DDSFrequency / SCANFREQFACTOR;

         ScanBuffer[HornScanGraphPointCount].Frequency = freq;
         HornScanRawFrequency[HornScanGraphPointCount] = freq;
         ScanBuffer[HornScanGraphPointCount].PWMValue = PWMAmp;
         ScanBuffer[HornScanGraphPointCount].Amplitude = ActualAmplitude;
         ScanBuffer[HornScanGraphPointCount].Current = CurrentInput;
         ScanBuffer[HornScanGraphPointCount].Power = PowerInput;
         if (!PhaseSign) {
            data = -PhaseInput;
            HornScanRawPhase[HornScanGraphPointCount] = data;
            HornScanRawTestPhase[HornScanGraphPointCount] = data;
            //Update negative Phasevalue in ScanBuffer
            ScanBuffer[HornScanGraphPointCount].Phase = data;
         }
         else {
            HornScanRawPhase[HornScanGraphPointCount] = PhaseInput;
            HornScanRawTestPhase[HornScanGraphPointCount] = PhaseInput;
            //Update positive Phasevalue in ScanBuffer
            ScanBuffer[HornScanGraphPointCount].Phase = PhaseInput;
         }
      }
      CountDelay = 0;
   }
   else
      CountDelay += DCPTIME;
   if (ParaScan.FrequencyStop < ParaScan.FrequencyStart)
      PercentScan = (ParaScan.FrequencyStart - DDSFrequency) * 100
            / (ParaScan.FrequencyStart - ParaScan.FrequencyStop);
   else
      PercentScan = (DDSFrequency - ParaScan.FrequencyStart) * 100
            / (ParaScan.FrequencyStop - ParaScan.FrequencyStart);
}

void Scan_Stop(void)
/******************************************************************************/
/* scan_stop(void)										    				               */
/* Description:-                                                              */
/* Here the amplitude is driven to 0 and WeldStatus is changed to WAITING.    */
/******************************************************************************/
{
   UINT16 data = HornScanPointLast;

   //FPGA Amplitude is ramp Down
   PWMAmp = 0;
   pprintf("\n scan_stop");

   //End Mark of scan buffer
   ScanBuffer[HornScanGraphPointCount].Status = data;
   ScanBuffer[HornScanGraphPointCount + 1].Status = data;

   // OutputStatus = NO_OUT;
   UserIO::SetSeekOut(FALSE);
   UserIO::SetSonicsActive(FALSE);

   SonicsOff();

   SetFPGAFrequency();
   ScanStop = TRUE;
   ScanInitiate = FALSE;

}/* scan_stop() */

void Set20KScanParameters(void)
/******************************************************************************/
/* Set20KScanParameters(void)										                     */
/* Description:-                                                              */
/* Here the default scan parameters for 20 Khz specification are initialized. */
/******************************************************************************/
{
	if((RACKMOUNT_STD == BIT(EPromData.Systemtype)) && (EPromData.SIGSpecial)) {
		ParaScan.FrequencyStart = STARTFREQ20KSIG;
	   ParaScan.FrequencyStop = STOPFREQ20KSIG;
	}
	else {
		ParaScan.FrequencyStart = STARTFREQ20K;
	   ParaScan.FrequencyStop = STOPFREQ20K;
	}
   ParaScan.FrequencyStep = FREQSTEP20K;
   ParaScan.TimeDelay = CONV_MILLI_MICROSEC(TIMEDELAY20K);
   ParaScan.MaxAmplitude = MAXAMPLITUDE20K;
   ParaScan.MaxCurrent = MAXCURRENT20K;
}

void Set30KScanParameters(void)
/******************************************************************************/
/* Set30KScanParameters(void)										                     */
/* Description:-                                                              */
/* Here the default scan parameters for 30 Khz specification are initialized. */
/******************************************************************************/
{
   ParaScan.FrequencyStart = STARTFREQ30K;
   ParaScan.FrequencyStop = STOPFREQ30K;
   ParaScan.FrequencyStep = FREQSTEP30K;
   ParaScan.TimeDelay = CONV_MILLI_MICROSEC(TIMEDELAY30K);
   ParaScan.MaxAmplitude = MAXAMPLITUDE30K;
   ParaScan.MaxCurrent = MAXCURRENT30K;
}

void Set40KScanParameters(void)
/******************************************************************************/
/* Set40KScanParameters(void)										                     */
/* Description:-                                                              */
/* Here the default scan parameters for 40 Khz specification are initialized. */
/******************************************************************************/
{
   ParaScan.FrequencyStart = STARTFREQ40K;
   ParaScan.FrequencyStop = STOPFREQ40K;
   ParaScan.FrequencyStep = FREQSTEP40K;
   ParaScan.TimeDelay = CONV_MILLI_MICROSEC(TIMEDELAY40K);
   ParaScan.MaxAmplitude = MAXAMPLITUDE40K;
   ParaScan.MaxCurrent = MAXCURRENT40K;
}

void SetDefaultScanParameters(void)
/******************************************************************************/
/* SetDefaultScanParameters(void)										               */
/* Description:-                                                              */
/* This function will initializes default scan parameter in current scan param*/
/******************************************************************************/
{
   pprintf("\n Set Scan parameter default");
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
      Set20KScanParameters();
      break;
   case FREQ_30KHZ:
      Set30KScanParameters();
      break;
   case FREQ_40KHZ:
      Set40KScanParameters();
      break;
   }
   SaveScanParamCRC();
}

void SetMinMaxScanParameter()
/******************************************************************************/
/* SetMinMaxScanParameter(void)					   		                        */
/* Description:-                                                              */
/* This function will initializes maximum and minimum scan parameter.         */
/******************************************************************************/
{
   ////////////////ScanParam/////////////////////////
   switch (EPromData.PSfrequency) {
   case FREQ_20KHZ:
      ScanParamMin.FrequencyStart = SCANFREQ20KSTART_MIN;
      ScanParamMin.FrequencyStop = SCANFREQ20KEND_MIN;
      ScanParamMax.FrequencyStart = SCANFREQ20KSTART_MAX;
      ScanParamMax.FrequencyStop = SCANFREQ20KEND_MAX;
      ScanParamMin.FrequencyStep = SCANFREQ20KSTEP_MIN;
      ScanParamMax.FrequencyStep = SCANFREQ20KSTEP_MAX;
      break;
   case FREQ_30KHZ:
      ScanParamMin.FrequencyStart = SCANFREQ30KSTART_MIN;
      ScanParamMin.FrequencyStop = SCANFREQ30KEND_MIN;
      ScanParamMax.FrequencyStart = SCANFREQ30KSTART_MAX;
      ScanParamMax.FrequencyStop = SCANFREQ30KEND_MAX;
      ScanParamMin.FrequencyStep = SCANFREQ30KSTEP_MIN;
      ScanParamMax.FrequencyStep = SCANFREQ30KSTEP_MAX;
      break;
   case FREQ_40KHZ:
      ScanParamMin.FrequencyStart = SCANFREQ40KSTART_MIN;
      ScanParamMin.FrequencyStop = SCANFREQ40KEND_MIN;
      ScanParamMax.FrequencyStart = SCANFREQ40KSTART_MAX;
      ScanParamMax.FrequencyStop = SCANFREQ40KEND_MAX;
      ScanParamMin.FrequencyStep = SCANFREQ40KSTEP_MIN;
      ScanParamMax.FrequencyStep = SCANFREQ40KSTEP_MAX;
      break;
   }
   ScanParamMin.TimeDelay = CONV_MILLI_MICROSEC(SCANTIMEDELAY_MIN);
   ScanParamMin.MaxAmplitude = SCANMAXAMP_MIN;
   ScanParamMin.MaxCurrent = SCANMAXCURRENT_MIN;

   ScanParamMax.TimeDelay = CONV_MILLI_MICROSEC(SCANTIMEDELAY_MAX);
   ScanParamMax.MaxAmplitude = SCANMAXAMP_MAX;
   ScanParamMax.MaxCurrent = SCANMAXCURRENT_MAX;
}

BOOLEAN VerifyScanParam()
/******************************************************************************/
/* VerifyScanParam(void)					   		                              */
/* Description:-                                                              */
/*  This function will verify Scan preset values against maximum and          */
/*  minimum preset values.                                                    */
/******************************************************************************/
{
   BOOLEAN ScanPresetVerifiedFlag;
   //	check for min-max level for all the variables here
   //	if(minimum preset value <= preset variable's value <= maximum preset value)
   //	set PresetVerifiedFlag = true;
   //	else set PresetVerifiedFlag = false;
   if (SCANIN_RANGE(FrequencyStart) && SCANIN_RANGE(FrequencyStop) &&
   SCANIN_RANGE(FrequencyStep) && SCANIN_RANGE(TimeDelay) &&
   SCANIN_RANGE(MaxAmplitude) && SCANIN_RANGE(MaxCurrent))
      ScanPresetVerifiedFlag = TRUE;
   else
      ScanPresetVerifiedFlag = FALSE;

   return ScanPresetVerifiedFlag;
}

void CheckAndAssignScanCycleBoundaryValue()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for Checking ParaScan preset against Minimum*/
/* and Maximum values,if boundary conditions are violated, then Minimum and */
/* maximum values are assigned.                                             */
/****************************************************************************/
{
   if (ParaScan.FrequencyStart < ScanParamMin.FrequencyStart)
      ParaScan.FrequencyStart = ScanParamMin.FrequencyStart;
   if (ParaScan.FrequencyStart > ScanParamMax.FrequencyStart)
      ParaScan.FrequencyStart = ScanParamMax.FrequencyStart;

   if (ParaScan.FrequencyStop < ScanParamMin.FrequencyStop)
      ParaScan.FrequencyStop = ScanParamMin.FrequencyStop;
   if (ParaScan.FrequencyStop > ScanParamMax.FrequencyStop)
      ParaScan.FrequencyStop = ScanParamMax.FrequencyStop;

   if (ParaScan.FrequencyStep < ScanParamMin.FrequencyStep)
      ParaScan.FrequencyStep = ScanParamMin.FrequencyStep;
   if (ParaScan.FrequencyStep > ScanParamMax.FrequencyStep)
      ParaScan.FrequencyStep = ScanParamMax.FrequencyStep;

   if (ParaScan.TimeDelay < ScanParamMin.TimeDelay) ParaScan.TimeDelay =
         ScanParamMin.TimeDelay;
   if (ParaScan.TimeDelay > ScanParamMax.TimeDelay) ParaScan.TimeDelay =
         ScanParamMax.TimeDelay;

   if (ParaScan.MaxAmplitude < ScanParamMin.MaxAmplitude)
      ParaScan.MaxAmplitude = ScanParamMin.MaxAmplitude;
   if (ParaScan.MaxAmplitude > ScanParamMax.MaxAmplitude)
      ParaScan.MaxAmplitude = ScanParamMax.MaxAmplitude;

   if (ParaScan.MaxCurrent < ScanParamMin.MaxCurrent) ParaScan.MaxCurrent =
         ScanParamMin.MaxCurrent;
   if (ParaScan.MaxCurrent > ScanParamMax.MaxCurrent) ParaScan.MaxCurrent =
         ScanParamMax.MaxCurrent;

   SaveScanParamCRC();
}

void CheckScanParamCRC()
/******************************************************************************/
/* CheckScanParamCRC(void)										                        */
/* Description:-                                                              */
/* This function will check calculated CRC with Stored CRC, if not equal,     */
/* default values is assignment. 															*/
/******************************************************************************/
{
   UINT32 CRCValue = ByteCRC((UINT8*) &ParaScan,
         sizeof(ParaScan) - sizeof(ParaScan.CRC));
   if (ParaScan.CRC != CRCValue) {
      Log::WriteEventLog(CRC_ERROR, (SINT32) ParaScan.CRC, (SINT32) CRCValue, 2,
            true);
      //Set default value in ParanScan
      SetDefaultScanParameters();
      SaveScanParamCRC();
   }
}

void SaveScanParamCRC()
/******************************************************************************/
/* SaveScanParamCRC(void)										                        */
/* Description:-                                                              */
/* This function saves CRC in ParaScan structure.								      */
/******************************************************************************/
{
   ParaScan.CRC = ByteCRC((UINT8*) &ParaScan,
         sizeof(ParaScan) - sizeof(ParaScan.CRC));
   PresetVerifiedRequired = TRUE;
}
