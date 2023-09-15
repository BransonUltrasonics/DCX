/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SCAN.cpp_v   1.25.1.1   Jul 07 2017 12:32:54   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SCAN.cpp_v  $
 * 
 *    Rev 1.25.1.1   Jul 07 2017 12:32:54   hasanchez
 * Update to make the code more readable
 * 
 *    Rev 1.25.1.0   17 Mar 2016 16:23:44   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.26   17 Mar 2016 16:07:22   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.25   02 Sep 2014 07:00:40   amaurya
 * Changed for Digital Tune.
 * 
 *    Rev 1.24   13 Feb 2013 09:20:44   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.20.1.2   09 Nov 2012 01:21:00   amaurya
 * Fixed Bar Graph wrap issue.
 * 
 *    Rev 1.20.1.1   09 Oct 2012 09:59:02   ygupta
 * Fixed Scaling Issue.
 * 
 *    Rev 1.20.1.0   09 Oct 2012 07:07:48   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.20   30 Mar 2012 06:42:50   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.19   16 Sep 2011 08:25:32   RatneshA
 * corrected code formatting
 * 
 *    Rev 1.18   16 Aug 2011 08:27:02   pdwivedi
 * code cleanup
 * 
 *    Rev 1.17   02 Aug 2011 07:39:10   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.16   18 Jul 2011 10:38:24   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.15   15 Jul 2011 09:47:52   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.14   21 Jun 2011 11:07:32   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.13   13 Jun 2011 09:18:48   PDwivedi
 * code commented for 15KHZ PS
 * 
 *    Rev 1.12   03 Jun 2011 05:49:06   PDwivedi
 * Default frequency step for 40KHZ Ps is doubled.
 * 
 *    Rev 1.11   11 May 2011 08:42:12   ygupta
 * sonic out fix during seek,scan and TEST
 * 
 *    Rev 1.10   02 May 2011 09:28:14   ygupta
 * Manufacturing Bug fixes
 * 
 *    Rev 1.9   25 Apr 2011 08:44:56   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.8   14 Apr 2011 06:23:12   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.7   04 Apr 2011 11:02:26   PDwivedi
 * Scan will be aborted if the SCAN input is removed..
 * 
 *    Rev 1.6   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.5   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.4   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   15 Nov 2010 08:47:12   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

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
#include "preset.h"

#define SCANIN_RANGE(value)   ((ParaScan.value >= ScanParamMin.value) && (ParaScan.value <= ScanParamMax.value))

/*-------------------------- Global  Variables ----------------------------*/
int SignValue;
static int ScanTimer = 0;
ScanParameter ScanParamMin;
ScanParameter ScanParamMax;
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 *HornScanRawFrequency;
extern SINT16 *HornScanRawPhase;
extern SINT32  ResonanceFreqs[DCP_NUMRESONANCES];
extern SINT16 HornScanRawTestPhase[DCP_HORNSCANMAXPOINTS];
extern RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
extern BOOLEAN AlarmFlag;
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

   ScanBuffer[HornScanGraphPointCount+1].Status = data;

   //  Clear the old scan phase
   for(int i=0; i<= DCP_HORNSCANMAXPOINTS; i++)
      HornScanRawPhase[i] = SCANRAWPHASE_DEFAULT;

   //  Clear the old scan Resosance frequency
   for(int i=0; i < DCP_NUMRESONANCES; i++) {
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
   if (HornScanGraphPointCount >= (SIZE_SCANBUFFER-2))
   {
      ScanBuffer[HornScanGraphPointCount-1].Status = data;
      data = HornScanPointLast;
      //End Mark of scanbuffer
      ScanBuffer[HornScanGraphPointCount].Status = data;
      LastScanPointNotReached = FALSE;
   }
   else
   {
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
	if (PresetVerifiedRequired)
	{
		PresetVerifiedRequired = FALSE;
		PresetVerifiedSuccess = VerifyScanParam();
	}
	if(!PresetVerifiedSuccess)
	{
		pprintf("\n preset verified failed, assigning boundary values");
		CheckAndAssignScanCycleBoundaryValue();
	}
	static UINT32 count;
	DDSFrequency = ParaScan.FrequencyStart;
	FrequencyValue = 0;

	if(InitParamUpdateRequired)
	  	   UpdateFPGAInitParameters();

	FPGA->st.stWUsOff.i32Para[HD_MODE ] = 0;
	fpga_CtrlHdX = FPGA_HDCTRL_SCAN;
	/* detect direction of scan */
	if ((ParaScan.FrequencyStop - ParaScan.FrequencyStart) > 0)
		SignValue = 1;
	else
		SignValue = -1;

	if (!count)
	{
		//Increase FPGA output amplitude
		PWMAmp += 0x100 ;//around 0.39% of 0xFFFF
		count = ParaScan.TimeDelay;
	}
	else if(count >= DCPTIME)
	{
		count-=DCPTIME;
	}
	else
		count = 0;

	// Check if feedback current or amplitude reaches Max. current/amplitude in preset
	if ((CurrentInput > ((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR)) || (ActualAmplitude > ((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR)))
	{
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
   if (!PhaseSign)
   	  PhaseInput = -PhaseInput;
   if ((ScanTimer > CONV_MILLI_MICROSEC(SCANTIMEOUT)) || (WebSite::CheckForWebScanAbortCommand() && !PowerUpScan)||(!UserIO::CheckForHornScan() && !EnableWebIO && !PowerUpScan))
   {
		pprintf("\n Scan Timeout");
		ScanTimer = 0;
		ScanAbort = TRUE;
		ScanInitiate = FALSE;
		UserIO::SetSeekOut(FALSE);
		UserIO::SetSonicsActive(FALSE);
   }
   ScanTimer += DCPTIME;
   if (!delay)
   {
      //Check if feedback Current or amplitude is greater than their Preset values
      if ((CurrentInput > ((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR)) || (ActualAmplitude > ((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR)))
      {
         //Decrease FPGA output amplitude
         PWMAmp -= 0x100;
         delay = ParaScan.TimeDelay;
      }
      //Check if feedback Current and amplitude is less than half of Preset values
      else if ((CurrentInput < (((ParaScan.MaxCurrent * ADC11BITVALUE) / SCALING_FACTOR) / 2))
               && (ActualAmplitude < (((ParaScan.MaxAmplitude * ADC11BITVALUE) / SCALING_FACTOR) / 2)))
      {
         //Increase FPGA output amplitude
         PWMAmp += 0x100;
         delay = ParaScan.TimeDelay;
      }
      Detect_Resonance();
   }
   else if(delay >= DCPTIME)
   {
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
   if (CountDelay > ParaScan.TimeDelay)
	{
      //Check direction of scan
      if (SignValue > 0)
      {
         // Check boundary condition
    	  //FPGA is returning (value - 1).
         if (DDSFrequencyFpga < (ParaScan.FrequencyStop + 10))
         {
         	printf(".");
         	DDSFrequency += ParaScan.FrequencyStep;
         }
         else
         {
            ScanRun = TRUE;
            ScanTimer = 0;
         }
      }
      else
      {
         // Check boundary condition
         if (DDSFrequencyFpga > (ParaScan.FrequencyStop - 10))
         {
         	DDSFrequency -= ParaScan.FrequencyStep;
         }
         else
         {
            ScanRun = TRUE;
            ScanTimer = 0;
         }
      }
      // Check Scan Buffer Index against maximum value
		if (IncScanBufferIndex())
		{
         //Update Scan Buffer
			if (HornScanGraphPointCount != 0)
			{
				data = 0x00;
				ScanBuffer[HornScanGraphPointCount-1].Status = data;
			}
			data = 0x02;
			ScanBuffer[HornScanGraphPointCount].Status = data;

			freq = DDSFrequencyFpga/SCANFREQFACTOR;//((DDSFrequencyFpga * F_Factor)/1000);

			ScanBuffer[HornScanGraphPointCount].Frequency = freq;
			HornScanRawFrequency[HornScanGraphPointCount] =	freq;
			ScanBuffer[HornScanGraphPointCount].PWMValue = PWMAmp;
			ScanBuffer[HornScanGraphPointCount].Amplitude = ActualAmplitude;
			ScanBuffer[HornScanGraphPointCount].Current = CurrentInput;
			ScanBuffer[HornScanGraphPointCount].Power = PowerInput;
			HornScanRawPhase[HornScanGraphPointCount] = PhaseInputFpga;
			HornScanRawTestPhase [HornScanGraphPointCount] = PhaseInputFpga;
				//Update positive Phasevalue in ScanBuffer
			ScanBuffer[HornScanGraphPointCount].Phase = PhaseInputFpga;
		}
		CountDelay = 0;
	}
	else
		CountDelay+=DCPTIME;
	if(ParaScan.FrequencyStop < ParaScan.FrequencyStart)
		PercentScan = (ParaScan.FrequencyStart - DDSFrequency) * 100 / (ParaScan.FrequencyStart - ParaScan.FrequencyStop);
	else
		PercentScan = (DDSFrequency - ParaScan.FrequencyStart) * 100 / (ParaScan.FrequencyStop - ParaScan.FrequencyStart);

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
   ScanBuffer[HornScanGraphPointCount+1].Status = data;

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
	ParaScan.FrequencyStart = STARTFREQ20K;
	ParaScan.FrequencyStop = STOPFREQ20K;
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
	switch (ePromData.PSfrequency)
	{
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
	switch(ePromData.PSfrequency)
	{
      case FREQ_20KHZ:
         ScanParamMin.FrequencyStart = SCANFREQ20KSTART_MIN;
         ScanParamMin.FrequencyStop  = SCANFREQ20KEND_MIN;
         ScanParamMax.FrequencyStart = SCANFREQ20KSTART_MAX;
         ScanParamMax.FrequencyStop  = SCANFREQ20KEND_MAX;
         ScanParamMin.FrequencyStep = SCANFREQ20KSTEP_MIN;
         ScanParamMax.FrequencyStep = SCANFREQ20KSTEP_MAX;
      break;
      case FREQ_30KHZ:
         ScanParamMin.FrequencyStart = SCANFREQ30KSTART_MIN;
         ScanParamMin.FrequencyStop  = SCANFREQ30KEND_MIN;
         ScanParamMax.FrequencyStart = SCANFREQ30KSTART_MAX;
         ScanParamMax.FrequencyStop  = SCANFREQ30KEND_MAX;
         ScanParamMin.FrequencyStep = SCANFREQ30KSTEP_MIN;
         ScanParamMax.FrequencyStep = SCANFREQ30KSTEP_MAX;
      break;
      case FREQ_40KHZ:
         ScanParamMin.FrequencyStart = SCANFREQ40KSTART_MIN;
         ScanParamMin.FrequencyStop  = SCANFREQ40KEND_MIN;
         ScanParamMax.FrequencyStart = SCANFREQ40KSTART_MAX;
         ScanParamMax.FrequencyStop  = SCANFREQ40KEND_MAX;
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
	if(ParaScan.FrequencyStart < ScanParamMin.FrequencyStart)
		ParaScan.FrequencyStart = ScanParamMin.FrequencyStart;
	if(ParaScan.FrequencyStart > ScanParamMax.FrequencyStart)
		ParaScan.FrequencyStart = ScanParamMax.FrequencyStart;

	if(ParaScan.FrequencyStop < ScanParamMin.FrequencyStop)
		ParaScan.FrequencyStop = ScanParamMin.FrequencyStop;
	if(ParaScan.FrequencyStop > ScanParamMax.FrequencyStop)
		ParaScan.FrequencyStop = ScanParamMax.FrequencyStop;

	if(ParaScan.FrequencyStep < ScanParamMin.FrequencyStep)
		ParaScan.FrequencyStep = ScanParamMin.FrequencyStep;
	if(ParaScan.FrequencyStep > ScanParamMax.FrequencyStep)
		ParaScan.FrequencyStep = ScanParamMax.FrequencyStep;

	if(ParaScan.TimeDelay < ScanParamMin.TimeDelay)
		ParaScan.TimeDelay = ScanParamMin.TimeDelay;
	if(ParaScan.TimeDelay > ScanParamMax.TimeDelay)
		ParaScan.TimeDelay = ScanParamMax.TimeDelay;

	if(ParaScan.MaxAmplitude < ScanParamMin.MaxAmplitude)
		ParaScan.MaxAmplitude = ScanParamMin.MaxAmplitude;
	if(ParaScan.MaxAmplitude > ScanParamMax.MaxAmplitude)
		ParaScan.MaxAmplitude = ScanParamMax.MaxAmplitude;

	if(ParaScan.MaxCurrent < ScanParamMin.MaxCurrent)
		ParaScan.MaxCurrent = ScanParamMin.MaxCurrent;
	if(ParaScan.MaxCurrent > ScanParamMax.MaxCurrent)
		ParaScan.MaxCurrent = ScanParamMax.MaxCurrent;

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
	 UINT32 CRCValue = ByteCRC((UINT8*)&ParaScan, sizeof(ParaScan) - sizeof(ParaScan.CRC));
	 if(ParaScan.CRC != CRCValue)
	 {
		 Log::WriteEventLog(CRC_ERROR, (UINT16)ParaScan.CRC, (UINT16)CRCValue, 2, true);
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
	ParaScan.CRC = ByteCRC((UINT8*)&ParaScan, sizeof(ParaScan) - sizeof(ParaScan.CRC));
	PresetVerifiedRequired = TRUE;
}
