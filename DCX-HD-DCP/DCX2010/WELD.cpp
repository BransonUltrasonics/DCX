/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WELD.cpp_v   1.13.1.6.1.10.1.8.1.9   Jul 03 2018 09:51:48   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/WELD.cpp_v  $
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.9   Jul 03 2018 09:51:48   EGUAJARDO
 * Renamed local variables.
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.8   May 28 2018 23:58:26   EGUAJARDO
 * Fixed ramp time to 80ms at stack adjustment function and update PWM only if amplitude result is greater or equal than 10 percent.
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.7   Apr 26 2018 15:58:38   EGUAJARDO
 * Fixed ramp time issue.
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.6   Apr 23 2018 09:23:42   hasanchez
 * Miscelanius changes related to the control of amplitude frequecy,  and bug fixes.
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.5   Dec 29 2017 13:47:50   hasanchez
 * Added fix to the issue related to the Weld Ramp
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.4   Jul 07 2017 12:30:24   hasanchez
 * Update to make the code more readable
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.3   Apr 05 2017 13:47:52   hasanchez
 * Updated the variable name WeldErrorFinish to WeldErrorFinishFlag
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.2   Mar 29 2017 09:20:36   hasanchez
 * Added functionality to delay the ready signal if resistor limit has been reached. 
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.1   Feb 09 2017 13:45:58   hasanchez
 * Added calculations to Manager Basic.
 * 
 *    Rev 1.13.1.6.1.10.1.8.1.0   17 Mar 2016 16:23:58   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.13.1.6.1.10.1.9   17 Mar 2016 16:07:34   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.13.1.6.1.10.1.8   05 Nov 2015 03:10:20   sesharma
 * Cleared 'AccumulatedEnergy' parameter in void Weld_Start().
 * 
 *    Rev 1.13.1.6.1.10.1.7   02 Sep 2014 07:03:16   amaurya
 * Added Amplitude Ramping.
 * 
 *    Rev 1.13.1.6.1.10.1.6   01 Apr 2014 07:50:28   rjamloki
 * Added Reset_WebWeldBuffer(), added changes to InitializeWeldbuffer(), IncWeldbufferIndex(),Weld_Start(), Control() to include changes for graph and F-Limiter
 * 
 *    Rev 1.13.1.6.1.10.1.5   13 Feb 2013 09:23:24   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.16   15 Jan 2013 04:44:34   amaurya
 * Energy Special.
 * 
 *    Rev 1.13.1.6.1.10.1.0.1.0   23 Aug 2012 11:34:44   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.13.1.6.1.10.1.0   30 Mar 2012 06:43:42   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.13.1.6.1.10   12 Dec 2011 11:16:18   tayars
 * Removed 10x for original dups control loops
 * 
 *    Rev 1.13.1.6.1.9   22 Nov 2011 11:41:26   Builder
 * Added the 10x multiplier in the frequency control loops.
 * 
 *    Rev 1.13.1.6.1.8   21 Oct 2011 11:05:40   Builder
 * Release V1.2 with Cable detect and inconsistency fixes
 * 
 *    Rev 1.13.1.6.1.7   16 Sep 2011 08:20:42   RatneshA
 * commented autosonics.
 * 
 *    Rev 1.13.1.6.1.6   01 Sep 2011 13:06:24   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.13.1.6.1.5   28 Aug 2011 11:42:28   ygupta
 * Ver1.01M
 * 
 *    Rev 1.13.1.6.1.4   16 Aug 2011 08:31:10   pdwivedi
 * code cleanup
 * 
 *    Rev 1.13.1.6.1.3   05 Aug 2011 10:36:14   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.13.1.6.1.2   02 Aug 2011 10:54:30   pdwivedi
 * Added changes for Memory management and removing warnings.
 * 
 *    Rev 1.13.1.6.1.1   02 Aug 2011 07:40:34   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.13.1.6.1.0   25 Jul 2011 11:24:08   tayars
 * Added New Control Loops
 * 
 *    Rev 1.13.1.7   22 Jul 2011 16:54:26   tayars
 * Added New Control Loops
 * 
 *    Rev 1.13.1.6   15 Jul 2011 09:50:04   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.13.1.5   21 Jun 2011 11:08:22   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.13.1.4   11 May 2011 08:41:18   ygupta
 * sonic out fix during seek,scan and TEST
 * 
 *    Rev 1.13.1.3   09 May 2011 08:38:34   ygupta
 * Manufacturing counting bug fixed
 * 
 *    Rev 1.13.1.0   27 Apr 2011 10:48:54   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.13   21 Apr 2011 11:08:50   PDwivedi
 * modified Ver1.0T for tracker issues
 * 
 *    Rev 1.12   20 Apr 2011 11:39:24   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.11   19 Apr 2011 10:50:40   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.10   04 Apr 2011 10:58:16   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.9   14 Jan 2011 07:55:46   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.8   14 Dec 2010 11:15:16   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.7   10 Dec 2010 09:24:18   ygupta
 * Modified WeldRun to use Amplitude values from LCD during current Weld cycle.
 * 
 *    Rev 1.6   09 Dec 2010 07:54:28   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.5   06 Dec 2010 09:56:56   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:42   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:28   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:52:52   nhahmed
 * Added Support for TEST through LCD.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

#include "GLOBALS.h"
#include "FUNC.h"
#include "StateMachineDrv.h"
#include "stdio.h"
#include "UserIO.h"
#include "ReceiveCommandHandler.h"
#include "LOG.h"
#include "CRC.h"
#include "preset.h"

//#define pprintf
//#define printf
/*
----------------------------- DEFINES ---------------------------------------
*/
#define SGN(x) ((x>=0)?(x):(-x))   
#define CURRENT_INPUT_GAIN		10
/*--------------------------------------------- ---------------------------*/

/*-------------------------- Global  Variables ----------------------------*/
UINT32 TempPeakPower;
UINT32 AccumulatedEnergy; /* the power is integrated to energy */
UINT32 Energy; /* the power is integrated to energy */


SINT32 RampTime, WeldAmplitudeLimit, TempWeldAmplitudeLimit;
SINT32 PhaseValueI = 0;
SINT32 PhaseValueP = 0;
SINT32 PhaseValues[MAX_NUM_PHASE_AVERAGES];
SINT32 IntegralValues[MAX_NUM_PHASE_AVERAGES];
UINT16 numFunctionCalls;
UINT32 EnergyRequired; /* This energy need to be achieved */
UINT32 RampStepTimer;
BOOLEAN RampDown = false;
BOOLEAN WeldCollectPoints;
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 LCDKeyStatus;
extern BOOLEAN AlarmFlag;
extern Parameter Preset_Min ;
extern Parameter Preset_Max ;
extern SINT32 SeekStartFreqOffset;
/*--------------------------------------------- ---------------------------*/

/* Declaration */
void CalculateRampStep(void);

void Reset_WeldBuffer(void)
/****************************************************************************/
/* Reset_WeldBuffer(void)                                                   */
/* Description:-                                                            */
/* Function resets WeldBuffer.							    				          */
/* 																			                */
/****************************************************************************/
{
   WeldBufferIndex = -1;
   WeldBufferTime = 0;
}
void Reset_WebWeldBuffer(void)
/****************************************************************************/
/* Reset_WebWeldBuffer(void)                                                   */
/* Description:-                                                            */
/* Function resets WeldBuffer.							    				          */
/* 																			                */
/****************************************************************************/
{
	WebWeldBufferTime = 0;
}
void InitializeWeldbuffer(void)
/****************************************************************************/
/* InitializeWeldbuffer(void)                                               */
/* Description:-                                                            */
/* Function Initializes WeldBuffer.											          */
/* 																			                */
/****************************************************************************/
{
	Reset_WebWeldBuffer();
	Reset_WeldBuffer();
}

void IncWeldbufferIndex(void)
/******************************************************************************/
/* IncWeldbufferIndex(void)												               */
/* Description:-                                                            	*/
/* In this function ,WeldBuffer index is increased until it becomes equal 	   */
/* to maximum, then it again becomes equal to zero and starts increasing.      */
/******************************************************************************/
{
	WeldBufferIndex = (WeldBufferIndex + 1) % SIZE_WELDBUFFER;
	WebWeldBufferIndex = (WebWeldBufferIndex + 1) % WEBWELD_GRAPHPTS;
}

void Weld_Start(void)
/******************************************************************************/
/* Weld_Start(void)															               */
/* Description:-                                                            	*/
/* This function is called before preparing the Weld.The parameters for the   */
/* loop are initialized from preset structure.The Power and Energy are   	   */
/* initialized to zero.														               */
/******************************************************************************/
{
	BOOLEAN PresetVerifiedSuccess = TRUE;
	if (PresetVerifiedRequired)
	{
		PresetVerifiedRequired = FALSE;
		PresetVerifiedSuccess = VerifyPreset();
	}
	if(!PresetVerifiedSuccess)
	{
//		pprintf("\n preset verified failed, assigning boundary values");
		CheckAndAssignWeldCycleBoundaryValue();
	}
   Clear_Weld_Status();
   Reset_WeldBuffer();
	Reset_WebWeldBuffer();
   FrequencycountLow = 0;
   FrequencycountHigh = 0;

   if(InitParamUpdateRequired)
  	   UpdateFPGAInitParameters();

   ddsMode =  RunningPreset.HDParam.InitPara[HD_MODE ];

   IntegralValue = MemoryValue;
   IntegralFactor =  MemoryValue << 8;
   FreqOffsetInputAtWeldStart = FreqOffsetInput;
   FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;

   DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
   WeldStartFrequency = DDSFrequency;
   PhaseI = RunningPreset.WeldPhaseloopI;
   AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1<<16)/AMPLOOP_C1FACTOR;
   AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2<<16)/AMPLOOP_C2FACTOR;
   CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;

   //******* New Test Values *********************************************
   PhaseINew = (RunningPreset.WeldPhaseloopINew << 16) / FREQLOOP_KI;
   PhasePNew = (RunningPreset.WeldPhaseloopPNew << 16) / FREQLOOP_KP;
   AmplitudePropGainNew = (RunningPreset.WeldAmplitudeLoopC1New<<16)/AMPLOOP_C1FACTORNEW;
   AmplitudeIntGainNew = (RunningPreset.WeldAmplitudeLoopC2New<<16)/AMPLOOP_C2FACTORNEW;
   CurrentPowerFactorNew = RunningPreset.WeldPhaseloopCFNew;
   numFunctionCalls = 1;				// so that we can start a new running average in frequency control true
   FinalAmplitude = 0;
   PWMLastValue = 0;
   ADILastValue = 0;
   LastAmpDifference = 0;
   pwmAmpValueAmpLoopOff = 0;

   RampTime = RunningPreset.WeldRampTime;
   if (RampTime==0)
   {
      RampTime= CONV_MILLI_MICROSEC(1); /* LowerLimit=1	*/
   }
   RampStepTimer = 0;

	if(AMPLITUDELOOPON)	{
		/* (Check if external amplitude option is enabled) */
	   if (RunningPreset.SWDip.Flags.ExtAmplitude)
		   WeldAmplitudeLimit = ExtAmplitudeInput;
	   else
		   WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
	}
	else{
		if (RunningPreset.SWDip.Flags.ExtAmplitude)
		{
			//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
			// == ExtAmp * PWMPara * 83926/262144
			WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput*(((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;

		}
		else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384
			WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;
		}
	}

   TempWeldAmplitudeLimit = WeldAmplitudeLimit;
   RampDown = FALSE;
   FPGA->st.stWUsOff.i32Para[HD_MODE ] = ddsMode;
   fpga_CtrlHdX = FPGA_HDCTRL_WELD;

   Power = 0;
   PowerNumber = 0;
   WeldAmpAverage = 0;
   WeldAmpAverageNumber = 0;
   AccumulatedEnergy = 0;
   Energy = 0;
   TempPeakPower = 0;

   WeldTimer = 0; /* (Timer for weld set) */  

   TempSuppressTime = 0;
   BlindTime = RunningPreset.HDParam.InitPara[HD_BLIND_TIME_WELD ];
   
   
   //Check if mode is energy mode which will be set if energy input is selected
   //on user input with a valid input voltage.
   if(RunningPreset.WeldMode == ENERGYMODE)
   {
   	EnergyRequired = EnergyNeeded;
   	WeldStartEnerFlag = TRUE;
   }
   else
   WeldStartFlag = TRUE;
}/* Weld_Start() */

void Weld_Ramp(void)
/******************************************************************************/
/* Weld_Ramp(void)														                  */
/* Description:-                                                            	*/
/* Once the parameters have been registered .The ramp is started based upon   */
/* WeldTimer.In each run, the nominal amplitude is calculated and regulated.  */
/* The Weld Ramp state is exited if	there is an weld error or run input 	   */
/* is no longer available.													               */
/******************************************************************************/
{
	UINT32 TempRampStepTimer = 0;
	UINT32 TempRampTimeinMilliseconds = 0;

   StartSonics();
	CycleStarted = TRUE;
   /* Ramp in 12bit ADC steps  */
   if(AMPLITUDELOOPON)
   {
	  if(RampDown == TRUE){
		   RampStepTimer -= DCPTIME;
		   FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
	      // Check if final amplitude is reached
	      if(FinalAmplitude <= WeldAmplitudeLimit){
	         WeldRampFlag = TRUE;
	         FinalAmplitude = WeldAmplitudeLimit;
	      }
	   }
	   else{
		   RampStepTimer += DCPTIME;
		   FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
	      // Check if final amplitude is reached
	      if(FinalAmplitude >= WeldAmplitudeLimit){
	         WeldRampFlag = TRUE;
	         FinalAmplitude = WeldAmplitudeLimit;
	      }
	   }
   }
   else//HD Amp control
   {

	   TempRampTimeinMilliseconds= CONVMICRO_MILLI(RampTime);
	  if(RampDown == TRUE){
		  RampStepTimer -= DCPTIME;
		  TempRampStepTimer = CONVMICRO_MILLI(RampStepTimer);


		  pwmAmpValueAmpLoopOff = ((TempRampStepTimer* ((UINT32)MAX_PWM_AMP_HD*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]/100))/TempRampTimeinMilliseconds);

		  // Check if final amplitude is reached
		  if(pwmAmpValueAmpLoopOff < WeldAmplitudeLimit)
		  {
			  WeldRampFlag = TRUE;
			  pwmAmpValueAmpLoopOff = (UINT16)WeldAmplitudeLimit;
		  }
	  }
	  else
	  {
		  RampStepTimer += DCPTIME;
		  TempRampStepTimer = CONVMICRO_MILLI(RampStepTimer);

		  pwmAmpValueAmpLoopOff = ((TempRampStepTimer * ((UINT32)MAX_PWM_AMP_HD*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]/100))/TempRampTimeinMilliseconds);

		  // Check if final amplitude is reached

		  if(pwmAmpValueAmpLoopOff > WeldAmplitudeLimit)
		  {
				WeldRampFlag = TRUE;
				pwmAmpValueAmpLoopOff = (UINT16)WeldAmplitudeLimit;

		  }
	  }
	  PWMAmp = pwmAmpValueAmpLoopOff;
   }

   WeldError = Control(WELDFUNCTION);



  /* (Loop for frequency and amplitude) */
   if (WeldError)
   {
      //Indicates there is currently at least one alarm.
      UserIO::SetGenAlarm(TRUE);
      AlarmFound = TRUE;
   }
   else
   {
		 WeldTimer+= DCPTIME; /* (Timer increment) */
		 Energy += PowerInput;
		 WeldEnergyResult += PowerInput;
	   //Transition check
		if (!(UserIO::CheckForExternalTest())
				&& !(UserIO::CheckForExternalStart()) && !(LCDKeyStatus & KB_TEST) /*&& !(AutoSonicsEnable)*/
				&& !(WebSite::CheckForWebTestCommand())
				&& !PWMAmpAdjustFlag
		)
		{
		  NoInput = TRUE;
		  WeldStopFrequency = DDSFrequencyFpga;
		  WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

		  // Check if store at weldend option is enabled
		  if (RunningPreset.SWDip.Flags.EndOfWeldStore)
		  {
			/*store at weldend*/
			MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
			CurrentPreset.MemoryFreq = MemoryValue;
			SavePreset(0);
		  }
		}
		else{
			 /********* Transition Check ******************/
			if(AMPLITUDELOOPON)
			{
				/* (Check if external amplitude option is enabled) */
			   if ((RunningPreset.SWDip.Flags.ExtAmplitude == TRUE) && WebSite::CheckForWebTestCommand() == FALSE)
				   WeldAmplitudeLimit = ExtAmplitudeInput;
			   else
				   WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
			}
			else{
				if ((RunningPreset.SWDip.Flags.ExtAmplitude) && WebSite::CheckForWebTestCommand() == FALSE)
				{
					//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
					// == ExtAmp * PWMPara * 83926/262144
					WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput*(((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;

				}
				else{
					//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
					//== WeldAmp * PWMPara * 107372/16384
					WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;
				}
			}
			if(TempWeldAmplitudeLimit != WeldAmplitudeLimit){
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		 }
   }
}/* Weld_Ramp() */

void Weld_EnerRamp(void)
/******************************************************************************/
/* Weld_EnerRamp(void)														                  */
/* Description:-                                                            	*/
/* Once the parameters have been registered .The ramp is started based upon   */
/* WeldTimer.In each run, the nominal amplitude is calculated and regulated.  */
/* The Weld Ramp state is exited if	there is an weld error or run input 	   */
/* is no longer available or energy required is reached.													               */
/******************************************************************************/
{
	UINT32 TempRampStepTimer = 0;
	UINT32 TempRampTimeinMilliseconds = 0;
   StartSonics();
   /* Ramp in 12bit ADC steps  */
   if(AMPLITUDELOOPON)
   {
	  if(RampDown == TRUE){
		   RampStepTimer -= DCPTIME;
		   FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		  // Check if final amplitude is reached
		  if(FinalAmplitude <= WeldAmplitudeLimit){
			 WeldRampFlag = TRUE;
			 FinalAmplitude = WeldAmplitudeLimit;
		  }
	   }
	   else{
		   RampStepTimer += DCPTIME;
		   FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		  // Check if final amplitude is reached
		  if(FinalAmplitude >= WeldAmplitudeLimit){
			 WeldRampFlag = TRUE;
			 FinalAmplitude = WeldAmplitudeLimit;
		  }
	   }
   }
   else//HD Amp control
   {
	   TempRampTimeinMilliseconds= CONVMICRO_MILLI(RampTime);
	  if(RampDown == TRUE){
		  RampStepTimer -= DCPTIME;

		  TempRampStepTimer = CONVMICRO_MILLI(RampStepTimer);

		  pwmAmpValueAmpLoopOff = ((TempRampStepTimer * ((UINT32)MAX_PWM_AMP_HD*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]/100))/TempRampTimeinMilliseconds);



		  // Check if final amplitude is reached
		  if(pwmAmpValueAmpLoopOff < WeldAmplitudeLimit)
		  {
			  WeldRampFlag = TRUE;
			  pwmAmpValueAmpLoopOff = (UINT16)WeldAmplitudeLimit;
		  }
	  }
	  else{
		  RampStepTimer += DCPTIME;



		  TempRampStepTimer = CONVMICRO_MILLI(RampStepTimer);

		  pwmAmpValueAmpLoopOff = ((TempRampStepTimer * ((UINT32)MAX_PWM_AMP_HD*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]/100))/TempRampTimeinMilliseconds);


		  // Check if final amplitude is reached
		  if(pwmAmpValueAmpLoopOff > WeldAmplitudeLimit)
		  {
			  WeldRampFlag = TRUE;
			  pwmAmpValueAmpLoopOff = (UINT16)WeldAmplitudeLimit;
		  }
	  }
	  PWMAmp = pwmAmpValueAmpLoopOff;
   }

   WeldError = Control(WELDFUNCTION);


   /* (Loop for frequency and amplitude) */
   if (WeldError)
   {
      //Indicates there is currently at least one alarm.
      UserIO::SetGenAlarm(TRUE);
      AlarmFound = TRUE;
   }
   else
   {
	   WeldTimer+= DCPTIME; /* (Timer increment) */
	   AccumulatedEnergy += PowerInput;
	   WeldEnergyResult += PowerInput;
	   /* Stop sonics, energy value reached*/
		if(AccumulatedEnergy >= EnergyRequired)
		{
			NoInput = TRUE;
			IsIOConfigured = TRUE;
			WeldStopFrequency = DDSFrequencyFpga;
			WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

			if (RunningPreset.SWDip.Flags.EndOfWeldStore)
			{
				/*store at weldend*/
				MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;

				SavePreset(0);
			}
		}
		   //Transition check
		else if (!(UserIO::CheckForExternalTest())
					&& !(UserIO::CheckForExternalStart()) && !(LCDKeyStatus & KB_TEST) /*&& !(AutoSonicsEnable)*/
					&& !(WebSite::CheckForWebTestCommand())
					&& !PWMAmpAdjustFlag
		)
		 {
			  NoInput = TRUE;
			  WeldStopFrequency = DDSFrequencyFpga;
			  WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;
			  // Check if store at weldend option is enabled
			  if (RunningPreset.SWDip.Flags.EndOfWeldStore)
			  {
				/*store at weldend*/
				MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
			 }
		 }
		else{
			 /********* Transition Check ******************/
			if(AMPLITUDELOOPON)	{
				/* (Check if external amplitude option is enabled) */
			   if (RunningPreset.SWDip.Flags.ExtAmplitude)
				   WeldAmplitudeLimit = ExtAmplitudeInput;
			   else
				   WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
			}
			else{
				if (RunningPreset.SWDip.Flags.ExtAmplitude)
				{
					//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
					// == ExtAmp * PWMPara * 83926/262144
					WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput*(((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;

				}
				else{
					//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
					//== WeldAmp * PWMPara * 107372/16384
					WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;
				}
			}
			if(TempWeldAmplitudeLimit != WeldAmplitudeLimit){
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
   }
}/* Weld_Ramp() */

void Weld_Run(void)
/*******************************************************************************/
/* Weld_Run(void)															                   */
/* Description:-                                                            	 */
/* Here amplitude has reached constant value.The Loop for frequency and  	    */
/* amplitude are run through.The welding cycle is stopped if there is an       */
/* weld error or Run Input is no longer available. The memory is stored at     */
/* weld end if SW Dip switch is enabled.             						          */
/*******************************************************************************/
{
	StartSonics();
	if(AMPLITUDELOOPON)
	{
	   if (RunningPreset.SWDip.Flags.ExtAmplitude)
		  WeldAmplitudeLimit = ExtAmplitudeInput;
	   else
		  WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
	}
	else{
		/* (Check if external amplitude option is enabled) */
		if (RunningPreset.SWDip.Flags.ExtAmplitude)
		{
			//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
			// == ExtAmp * PWMPara * 83926/262144
			WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput* (((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;//(((((ExtAmplitudeInput*SCALING_FACTOR)/ADC11BITVALUE)*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ])/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
		}
		else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384
			WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;//(((RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*RunningPreset.WeldAmplitude)/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
		}
		//PWMAmp = pwmAmpValueAmpLoopOff;
	}

   WeldError = Control(WELDFUNCTION);
	/* (Loop for frequency and amplitude)*/
	if (WeldError)
	{
		AlarmFound = TRUE;
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
	}
	else
	{
		/********* Transition Check ******************/
		// Check if Run input is still active.
		if (!(UserIO::CheckForExternalTest())
				&& !(UserIO::CheckForExternalStart()) && !(LCDKeyStatus & KB_TEST)/*&& !(AutoSonicsEnable)*/
				&& !(WebSite::CheckForWebTestCommand())
				&& !PWMAmpAdjustFlag )
		{
			WeldRunComplete = TRUE;
			WeldStopFrequency = DDSFrequencyFpga;
			WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;
			if (RunningPreset.SWDip.Flags.EndOfWeldStore)
			{
				/*store at weldend*/
				MemoryValue = WeldMemoryFrequency;
				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
			}
		}
		else
		{
			Energy += PowerInput;
			WeldEnergyResult += PowerInput;

			// Calculate Peak power
			if (PowerInput > TempPeakPower)
			  TempPeakPower = PowerInput;
			Power += PowerInput;
			WRPower += PowerInput;
			PowerNumber++;
			WRPowerNumber++;

			WeldAmpAverage += ActualAmplitude;
			WeldAmpAverageNumber++;
			WeldTimer+= DCPTIME;
			if(TempWeldAmplitudeLimit != WeldAmplitudeLimit){
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
	}
}/* Weld_Run() */

void Weld_EnerRun(void)
/*******************************************************************************/
/* Weld_EnerRun(void)															*/
/* Description:-                                                            	*/
/* Here amplitude has reached constant value.The Loop for frequency and  	    */
/* amplitude are run through.The welding cycle is stopped if there is an        */
/* weld error or Run Input is no longer available or energy required is reached.*/
/* The memory is stored at weld end if SW Dip switch is enabled.                */
/********************************************************************************/
{
	StartSonics();
	if(AMPLITUDELOOPON)
	{
	   if (RunningPreset.SWDip.Flags.ExtAmplitude)
		  WeldAmplitudeLimit = ExtAmplitudeInput;
	   else
		  WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
	}
	else{
		/* (Check if external amplitude option is enabled) */
		if (RunningPreset.SWDip.Flags.ExtAmplitude)
		{
			//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
			// == ExtAmp * PWMPara * 83926/262144
			WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput* (((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;//(((((ExtAmplitudeInput*SCALING_FACTOR)/ADC11BITVALUE)*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ])/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
		}
		else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384
			WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;//(((RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*RunningPreset.WeldAmplitude)/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
		}
		//PWMAmp = pwmAmpValueAmpLoopOff;
	}
	/* (Loop for frequency and amplitude)*/
    WeldError = Control(WELDFUNCTION);

	if (WeldError)
	{
		AlarmFound = TRUE;
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
	}
	else
	{
		/********* Transition Check ******************/
		// Check if Run input is still active.
		if (!(UserIO::CheckForExternalTest())
				&& !(UserIO::CheckForExternalStart()) && !(LCDKeyStatus & KB_TEST)/*&& !(AutoSonicsEnable)*/
				&& !(WebSite::CheckForWebTestCommand())
				&& !PWMAmpAdjustFlag)
		{
			WeldRunComplete = TRUE;

			WeldStopFrequency = DDSFrequencyFpga;
			WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

			if (RunningPreset.SWDip.Flags.EndOfWeldStore)
			{
				/*store at weldend*/
				MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;

				SavePreset(0);
			}
		}
		else{
			AccumulatedEnergy += PowerInput;
			WeldEnergyResult += PowerInput;

			/* Stop sonics, energy value reached*/
			if(AccumulatedEnergy >= EnergyRequired)
			{
				WeldRunComplete = TRUE;
				IsIOConfigured = TRUE;
				WeldStopFrequency = DDSFrequencyFpga;
				WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

				if (RunningPreset.SWDip.Flags.EndOfWeldStore)
				{
					/*store at weldend*/
					MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
					CurrentPreset.MemoryFreq = MemoryValue;
					SavePreset(0);
				}
			}			// Calculate Peak power
			if (PowerInput > TempPeakPower)
			  TempPeakPower = PowerInput;
			Power += PowerInput;
			WRPower += PowerInput;
			PowerNumber++;
			WRPowerNumber++;

			WeldAmpAverage += ActualAmplitude;
			WeldAmpAverageNumber++;
			WeldTimer+= DCPTIME;
			if(TempWeldAmplitudeLimit != WeldAmplitudeLimit){
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
	}
}/* Weld_Run() */


void Weld_Stop(void)
/******************************************************************************/
/* Weld_Stop(void)															               */
/* Description:-                                                            	*/
/* The PWM amplitude is reset to zero. The Average power and amplitude is     */
/* calculated  and WeldStatus is changed to waiting.                          */
/******************************************************************************/
{
	// Indicates that ultrasonics are off for weld or test mode.
	UserIO::SetSonicsActive(FALSE);

    SonicsOff();

	//FPGA Amplitude is ramp Down
	PWMAmp = 0;

	// Calculate Weld Energy
	AccumulatedEnergy += PowerInput;
	Energy += PowerInput;
   WeldEnergy = Energy;
   WeldEnergyResult += PowerInput;
	// Calculate Weld Power
	if (PowerNumber!=0)
	{
		WeldPower = Power/PowerNumber; //Averaging about constant phase
	}

	// Calculate Average amplitude
	if (WeldAmpAverageNumber!=0)
	{
		WeldAmplitudeAvr = (WeldAmpAverage/WeldAmpAverageNumber)+10; //Averaging about constant phase
	}

	// Update weld timer
	WeldTime = CONVMICRO_MILLI(WeldTimer);

	// Set weld status
	Set_Weld_Status();

	if (PWMAdjustONFlag == TRUE )
	{
		RunningPreset.WeldAmplitude = RunningPreset.HDParam.InitPara[HD_WELD_AMPLITUDE ]=TempAmp;
		RunningPreset.HDParam.InitPara[0] = TempMode;
		TempAmp=0;
		TempMode=0;

		if((WeldBuffer[APROP_TIME_TO_READ_PWM].PWMValue >= HD_PWM_PERCENTAGE_MIN))
		{
			CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE]=  WeldBuffer[APROP_TIME_TO_READ_PWM].PWMValue;
		}
		SavePreset(0);
		IsIOConfigured=FALSE; ///Added 03/01
	}

   WeldComplete = TRUE;
}

void Seek_Weld(void)
/******************************************************************************/
/* Seek_Weld(void)															               */
/* Description:-                                                            	*/
/* The TEST input was set. A seek is performed, followed by a run function.   */
/* This state handles the transition from seek to weld ramp state.	     	   */
/******************************************************************************/
{
	fpga_CtrlHdX = FPGA_HDCTRL_TEST_WELDRAMP;
	 TempSuppressTime = 0;
	 BlindTime = RunningPreset.HDParam.InitPara[HD_BLIND_TIME_WELD ];

	StartSonics();
	UserIO::SetSonicsActive(TRUE);//We dont need it really
	// Clear weld status
	Clear_Weld_Status();
	FreqOffsetInputAtWeldStart = SeekStartFreqOffset;

	FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;
	DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;

	WeldStartFrequency = DDSFrequency;

	// PID constants are intialized from preset
	PhaseI = RunningPreset.WeldPhaseloopI;
	AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1<<16)/AMPLOOP_C1FACTOR;
	AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2<<16)
			/AMPLOOP_C2FACTOR;
	CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;

	if (PWMAdjustONFlag == TRUE)
	{
		RampTime = CONV_MILLI_MICROSEC(WELDRAMPTIME_DEFAULT);
	}
	else
		RampTime = RunningPreset.WeldRampTime; ////////////commented

   if (RampTime==0)
   	{
   	  RampTime= CONV_MILLI_MICROSEC(1); /* LowerLimit=1	*/
   	}
	// Intialized global variables
	Energy = 0;
	Power = 0;
	WRPower=0;
	TempPeakPower = 0;
	PowerNumber = 0;
	WRPowerNumber =0;
	WeldAmpAverage = 0;
	WeldAmpAverageNumber = 0;
	RampStepTimer = ((ActualAmplitude * RampTime) / ADC11BITVALUE) + DCPTIME;
	if(AMPLITUDELOOPON)	{
	/* (Check if external amplitude option is enabled) */
	   if (RunningPreset.SWDip.Flags.ExtAmplitude)
		   WeldAmplitudeLimit = ExtAmplitudeInput;
	   else
		   WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
	}
	else{
		if (RunningPreset.SWDip.Flags.ExtAmplitude)
		{
			//calc: ExtAmplitudeInput*PWMPara*0xFFFF/(100*0x7FF) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(2048/2047))/((100*128/100)(2047*2048/2047))
			// == ExtAmp * PWMPara * 83926/262144
			WeldAmplitudeLimit = (UINT32)ExtAmplitudeInput*(((UINT32)((UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*83926))>>18) ;

		}
		else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384
			WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;
		}
	}
	RampDown = FALSE;
	FinalAmplitude = ActualAmplitude;
    SeekWeldFlag = TRUE;
}/* Seek_Weld() */

void Clear_Weld_Status(void)
/******************************************************************************/
/* Clear_Weld_Status(void)	     											               */
/* Description:-                                                            	*/
/* Here CurrentDCPStatus structure is initialized to zero.	 					   */
/******************************************************************************/
{
	WeldStartFrequency = 0;
	WeldStopFrequency = 0;
	WeldTime = 0;
	WeldEnergy = 0;
	WeldPower = 0;
	TempPeakPower = 0;
	WeldAmplitudeAvr = 0;
	ErrorState = 0;
	ErrorReason = NO_ERROR;

	CurrentDCPStatus.WeldStartFrequency = 0;
	CurrentDCPStatus.WeldStopFrequency = 0;
	CurrentDCPStatus.WeldTime = 0;
	CurrentDCPStatus.WeldEnergy = 0;
	CurrentDCPStatus.WeldPower = 0;
	CurrentDCPStatus.PeakPower = 0;
	CurrentDCPStatus.WeldAmplitude = 0;

	CurrentDCPStatus.ErrorReason = NO_ERROR;
	CurrentDCPStatus.ErrorFrequency = 0;
	CurrentDCPStatus.ErrorPower = 0;
	CurrentDCPStatus.ErrorCurrent = 0;
	CurrentDCPStatus.ErrorAmplitude = 0;
	CurrentDCPStatus.ErrorPsv = 0;
	CurrentDCPStatus.ErrorTemperature = 0;
	CurrentDCPStatus.ErrorTime = 0;
}

void Set_Weld_Status(void)
/******************************************************************************/
/* Set_Weld_Status(void)													               */
/* Description:-                                                            	*/
/* Here CurrentDCPStatus structure is initialized with end values of weld     */
/* cycle and weld cycle counter is incremented.    								   */
/******************************************************************************/
{
	CurrentDCPStatus.WeldStartFrequency = WeldStartFrequency;
	CurrentDCPStatus.WeldStopFrequency = WeldStopFrequency;
	CurrentDCPStatus.WeldMemoryFrequency = WeldMemoryFrequency;
	CurrentDCPStatus.WeldTime = WeldTime;
	CurrentDCPStatus.WeldEnergy = WeldEnergy;
	CurrentDCPStatus.WeldPower = WeldPower;
	CurrentDCPStatus.WeldAmplitude = WeldAmplitudeAvr;
	CurrentDCPStatus.PeakPower = TempPeakPower;

	CycleCounterIncrement++;
//	pprintf(
//			"\nWR:\n WSF =\t%d,\n WStF =\t%d,\n WMF =\t%d,"
//				"\n WT =\t%d ms \n WE =\t%d \n WP =\t%d \n WAmAvr =\t%d \n"
//				"\n ErR =\t%d \n ErFr =\t%d,\n PP=\t%d",
//			WeldStartFrequency, WeldStopFrequency, WeldMemoryFrequency,
//			WeldTime, WeldEnergy, WeldPower, WeldAmplitudeAvr, ErrorReason,
//			CurrentDCPStatus.ErrorFrequency, TempPeakPower);
}

void Set_Weld_Status_Error(void)
/******************************************************************************/
/* Set_Weld_Status_Error(void)												            */
/* Description:-                                                            	*/
/* Here CurrentDCPStatus structure is initialized with end values of weld     */
/* cycle in case of weld error.The weld cycle and error counter is incremented*/
/******************************************************************************/
{
   BOOLEAN ValidFlag = FALSE;
   UINT16 FreqValue;
	SINT16 PowerValue, CurrentValue, IstAmplitudeVlue, PWMValuetemp;

   //indicate invalid data in weldbuffer
  WeldBuffer[WeldBufferIndex].logValid = ValidFlag;

   CurrentDCPStatus.WeldStartFrequency = WeldStartFrequency;
   CurrentDCPStatus.WeldStopFrequency = WeldStopFrequency;
   CurrentDCPStatus.WeldMemoryFrequency = WeldMemoryFrequency;
   CurrentDCPStatus.WeldTime = WeldTime;
   CurrentDCPStatus.WeldEnergy = WeldEnergy;
   CurrentDCPStatus.WeldPower = WeldPower;
   CurrentDCPStatus.WeldAmplitude = WeldAmplitudeAvr;
   CurrentDCPStatus.PeakPower = TempPeakPower;
   CurrentDCPStatus.ErrorReason = ErrorReason;

   // Set Error Frequency
   FreqValue = WeldBuffer[WeldBufferIndex].Frequency;

   CurrentDCPStatus.ErrorFrequency = (((UINT32)FreqValue)*1000)/ F_Factor;

    // Set Error Power
   PowerValue = WeldBuffer[WeldBufferIndex].Power;

   CurrentDCPStatus.ErrorPower=PowerValue;

   // Set Error Current
   CurrentValue = WeldBuffer[WeldBufferIndex].Current;

   CurrentDCPStatus.ErrorCurrent = CurrentValue;

   //Set Actual amplitude
   IstAmplitudeVlue = WeldBuffer[WeldBufferIndex].ActualAmplitude;

   CurrentDCPStatus.ErrorAmplitude=IstAmplitudeVlue;

   // Set Error PWM amplitude
   PWMValuetemp = WeldBuffer[WeldBufferIndex].PWMValue;

   CurrentDCPStatus.ErrorPsv = PWMValuetemp;

   CurrentDCPStatus.ErrorTemperature = InputTemperature;
   CurrentDCPStatus.ErrorTime = WeldTimer;

 	//To avoid multiple log entries in the case power supply goes off
 	//before DCP board we are checking that there should not be more than
 	//1 overloadBit high at the same time.
 	UINT8 skipLog = 0;
 	UINT8 indx = 0;
 	for(indx = 1; indx < 4; indx++)
 	{
 		if(ErrorReason & (1 << indx))
 			skipLog++;
 	}
 	if(skipLog <= 1)
 	{
		//Increment cycle and error counter
		//CycleCounterIncrement++;
   if(MFCycleFlag)
   	 ManFErrorCycleCounterInc++;
   else
   	ErrorCounterIncrement++;
 	 }
// 	pprintf(
// 				"\nWR:\n WSF =\t%d,\n WStF =\t%d,\n WMF =\t%d,"
// 					"\n WT =\t%d ms \n WE =\t%d \n WP =\t%d \n WAmAvr =\t%d \n"
// 					"\n ErR =\t%d \n ErFr =\t%d,\n PP=\t%d",
// 				WeldStartFrequency, WeldStopFrequency, WeldMemoryFrequency,
// 				WeldTime, WeldEnergy, WeldPower, WeldAmplitudeAvr, ErrorReason,
// 				CurrentDCPStatus.ErrorFrequency, TempPeakPower);
}

void Set_ManFWeld_Status_Error(void)
/******************************************************************************/
/* Set_Weld_Status_Error(void)												            */
/* Description:-                                                            	*/
/* Here CurrentDCPStatus structure is initialized with end values of weld     */
/* cycle in case of weld error.The weld cycle and error counter is incremented*/
/******************************************************************************/
{
   BOOLEAN ValidFlag = FALSE;
   UINT16 FreqValue;
	SINT16 PowerValue, CurrentValue, IstAmplitudeVlue, PWMValuetemp;

   //indicate invalid data in weldbuffer
  WeldBuffer[WeldBufferIndex].logValid = ValidFlag;

   CurrentDCPStatus.WeldStartFrequency = WeldStartFrequency;
   CurrentDCPStatus.WeldStopFrequency = WeldStopFrequency;
   CurrentDCPStatus.WeldMemoryFrequency = WeldMemoryFrequency;
   CurrentDCPStatus.WeldTime = WeldTime;
   CurrentDCPStatus.WeldEnergy = WeldEnergy;
   CurrentDCPStatus.WeldPower = WeldPower;
   CurrentDCPStatus.WeldAmplitude = WeldAmplitudeAvr;
   CurrentDCPStatus.PeakPower = TempPeakPower;
   CurrentDCPStatus.ErrorReason = ErrorReason;

   // Set Error Frequency
   FreqValue = WeldBuffer[WeldBufferIndex].Frequency;

   CurrentDCPStatus.ErrorFrequency = (((UINT32)FreqValue)*1000)/ F_Factor;

    // Set Error Power
   PowerValue = WeldBuffer[WeldBufferIndex].Power;

   CurrentDCPStatus.ErrorPower=PowerValue;

   // Set Error Current
   CurrentValue = WeldBuffer[WeldBufferIndex].Current;

   CurrentDCPStatus.ErrorCurrent = CurrentValue;

   //Set Actual amplitude
   IstAmplitudeVlue = WeldBuffer[WeldBufferIndex].ActualAmplitude;

   CurrentDCPStatus.ErrorAmplitude=IstAmplitudeVlue;

   // Set Error PWM amplitude
   PWMValuetemp = WeldBuffer[WeldBufferIndex].PWMValue;

   CurrentDCPStatus.ErrorPsv = PWMValuetemp;

   CurrentDCPStatus.ErrorTemperature = InputTemperature;
   CurrentDCPStatus.ErrorTime = WeldTimer;
	//To avoid multiple log entries in the case power supply goes off
 	//before DCP board we are checking that there should not be more than
 	//1 overloadBit high at the same time.
 	UINT8 skipLog = 0;
 	UINT8 indx = 0;
 	for(indx = 1; indx < 4; indx++)
 	{
 		if(ErrorReason & (1 << indx))
 			skipLog++;
 	}
 	if(skipLog <= 1)
 	{
   // Increment cycle and error counter
 		// CycleCounterIncrement++;
	 ManFErrorCycleCounterInc++;
 	}
	MFcycleResults.resultantFrequency = DDSFrequency;
	MFcycleResults.resultantPower = TempPeakPower;

//	pprintf(
//				"\nWR:\n WSF =\t%d,\n WStF =\t%d,\n WMF =\t%d,"
//					"\n WT =\t%d ms \n WE =\t%d \n WP =\t%d \n WAmAvr =\t%d \n"
//					"\n ErR =\t%d \n ErFr =\t%d,\n PP=\t%d",
//				WeldStartFrequency, WeldStopFrequency, WeldMemoryFrequency,
//				WeldTime, WeldEnergy, WeldPower, WeldAmplitudeAvr, ErrorReason,
//				CurrentDCPStatus.ErrorFrequency, TempPeakPower);
}

void Weld_Error(void)
/******************************************************************************/
/* Weld_Error(void)							     							               */
/* Description:-                                                            	*/
/* An error has occurred .The current amplitude and frequency values are      */
/* in error cells, and the amplitude is set to 0. 	                           */
/******************************************************************************/
{
	SonicsOff();
	//Check if test mode
	if (TestInputFlag)
	{
		SeekMemoryFrequency = 0;
		WeldMemoryFrequency = 0;
		CurrentDCPStatus.SeekMemoryFrequency = 0;
		CurrentDCPStatus.WeldMemoryFrequency = 0;
		MemoryReset = TRUE;
		MemoryValue = 0;
		CurrentPreset.MemoryFreq = MemoryValue;
		SavePreset(0);

		IntegralValue = 0;
		IntegralFactor = 0;
		if(OverLoadAlarm || AlarmFound)
		{
			if((ErrorReason & FPGA_ADC_ERR) || (ErrorReason & FPGA_FB_ERR) ||
					(ErrorReason & FPGA_STDDDS_ERR) || (ErrorReason & FPGA_SYNCDDS_ERR)||
					(ErrorReason & FPGA_HD_ERR1) || (ErrorReason & FPGA_HD_ERR2))
			{}//skip logging in case of FB or ADC error from FPGA
			else
			Log::WriteEventLog(TEST_OVERLOAD,0,0);
		}
	}

	Set_Weld_Status_Error();
	WebSite::ResetTestFlag();
   TestInputFlag = FALSE;
   SetFPGAFrequency();
   ScanInitiate = FALSE;

	if (LimitReadyFlag == TRUE)
		WeldErrorFinishFlag = TRUE;
	else
		ResetEnable = FALSE;


}/* Weld_Error() */

void ManFWeld_Error(void)
/******************************************************************************/
/* Weld_Error(void)							     							               */
/* Description:-                                                            	*/
/* An error has occurred .The current amplitude and frequency values are      */
/* in error cells, and the amplitude is set to 0. 	                           */
/******************************************************************************/
{
	SonicsOff();
//	//Check if overload
	if(OverLoadAlarm)
	{
		SeekMemoryFrequency = 0;
		WeldMemoryFrequency = 0;
		CurrentDCPStatus.SeekMemoryFrequency = 0;
		CurrentDCPStatus.WeldMemoryFrequency = 0;
		MemoryReset = TRUE;
		MemoryValue = 0;
		CurrentPreset.MemoryFreq = MemoryValue;
		SavePreset(0);
//		pprintf("\n MF clear memory ");
		IntegralValue = 0;
		IntegralFactor = 0;
	}
	Set_ManFWeld_Status_Error();
   TestInputFlag = FALSE;
   SetFPGAFrequency();
   ScanInitiate = FALSE;
   WeldErrorFinishFlag = TRUE;
}/* Weld_Error() */

void PS_Error(void)
/******************************************************************************/
/* PS_Error(void)															                  */
/* Description:-                                                            	*/
/* The PWMAmp is driven to zero. It is simply waited until the reset key      */
/* is pressed.                       										            */
/******************************************************************************/
{
	//FPGA Amplitude is ramp Down
	PWMAmp = 0;
	SonicsOff();
	SetFPGAFrequency();
	ScanInitiate = FALSE;

	if(EnableWebIO)
		AutoResetEnable = TRUE;
	else
	{
		if(!CheckForInputs())
			AutoResetEnable = TRUE;
	}
}

void ManFPS_Error(void)
/******************************************************************************/
/* PS_Error(void)															                  */
/* Description:-                                                            	*/
/* The PWMAmp is driven to zero. It is simply waited until the reset key      */
/* is pressed.                       										            */
/******************************************************************************/
{
	//FPGA Amplitude is ramp Down
	PWMAmp = 0;
	SonicsOff();
	SetFPGAFrequency();
	ScanInitiate = FALSE;
	if(CurrentCycleParam.stopOnAlarm == FALSE)
	{
		AlarmFlag = FALSE;
		UserIO::SetGenAlarm(FALSE);
		WebSite::ResetWebResetFlag();
		//Write Alarm Log
		//WriteAlarmLog();

		ErrorReason = 0;

		TimedSeekTimerTicks = 0;
		ResetEnable = TRUE;
	}
}

SINT16 Control(SINT32 FUNC)
/******************************************************************************/
/* Control(int Flag)														                  */
/* Description:-                                                            	*/
/* Here the functionality of the loop for amplitude and phase / frequency is  */
/* provided.If one of the frequency limits reached, then 1 is returned else   */
/* 0 is returned. 															               */
/* Input parameters:-                      					                     */
/* FUNC : Indicates either SEEK or WELD function										*/
/* returns RetVal : 0 if no error, else 1 if weld error.								*/
/******************************************************************************/
{
	SINT16 RetVal = NO_ERROR;

	if (!PhaseSign) {
		PhaseInput = -PhaseInput; /* negative Phase */
	}

	/*Write in weld buffer */
	IncWeldbufferIndex();

	WeldData TempWeldBuffer;
	TempWeldBuffer.Time = WeldBufferTime;
	TempWeldBuffer. Frequency = (UINT16) ((DDSFrequencyFpga * F_Factor) / 1000);
	TempWeldBuffer.PWMValue = (PWMAmp * SCALING_FACTOR) / MAX_PWM_AMP_HD;
	TempWeldBuffer.NominalAmplitude = FinalAmplitude;
	TempWeldBuffer.ActualAmplitude = (ActualAmplitude * SCALING_FACTOR) / ADC11BITVALUE;
	TempWeldBuffer.Current = (CurrentInput * SCALING_FACTOR) / ADC11BITVALUE;
	TempWeldBuffer.Power = (PowerInput * SCALING_FACTOR) / ADC11BITVALUE;
	TempWeldBuffer.Phase = PhaseInputFpga;
	TempWeldBuffer.logValid = TRUE;

	WeldBuffer[WeldBufferIndex] = TempWeldBuffer;
	WeldBufferTime++;
	WebWeldBufferTime++;
	WeldBuffer[WeldBufferIndex] = TempWeldBuffer;
	WebWeldBuffer[WebWeldBufferIndex] = TempWeldBuffer;

	if (!RunningPreset.UseTrueValues)
	{
		if ((AMPLITUDELOOPON) || (FUNC == SEEKFUNCTION)) //check condition of amplitude control
			AmplitudeControl();

		if (TempSuppressTime >= CONV_MILLI_MICROSEC(BlindTime))
			FrequencyControl(FUNC);
		else
			TempSuppressTime += DCPTIME;
	}
	else
	{
		if ((AMPLITUDELOOPON) || (FUNC == SEEKFUNCTION)) //check condition of amplitude control
			AmplitudeControlTrue();

		if (TempSuppressTime >= CONV_MILLI_MICROSEC(BlindTime))
			FrequencyControl(FUNC);
		else
			TempSuppressTime += DCPTIME;

		numFunctionCalls++;
	}

	if (FUNC == SEEKFUNCTION) { /*(Check Seek Limits)*/
		if (DDSFrequencyFpga > ddsFrequencyHighSeek)
		{
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_HIGHSEEKLIMIT;
			//			pprintf(
			//					"\n1\nPhaseValue %d\nFreq  %d\nPhaseI %d\nIntegralFactor %d\nIntegralValue %d\nCurrentInput %d\nddsFrequencyHighSeek %d\n",
			//					PhaseValueI, FrequencyValue, PhaseI, IntegralFactor,
			//					IntegralValue, CurrentInput, ddsFrequencyHighSeek);
			//			pprintf("CurrentPowerFactor %d\nPhaseInput%d\nDDSFrequency %d\n",
			//					CurrentPowerFactor, PhaseInput, DDSFrequency);
			RetVal = WELDERROR;
		}
		else if (DDSFrequencyFpga < ddsFrequencyLowSeek)
		{
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_LOWSEEKLIMIT;
			//			pprintf(
			//					"\n2\n PhaseValue %d, Freq  %d\nPhaseI %dnIntegralFactor %dnIntegralValue %d,CurrentInput %d\n",
			//					PhaseValueI, FrequencyValue, PhaseI, IntegralFactor,
			//					IntegralValue, CurrentInput);
			//			pprintf("CurrentPowerFactor %d\nPhaseInput%d\nDDSFrequency %d\n",
			//					CurrentPowerFactor, PhaseInput, DDSFrequency);
			RetVal = WELDERROR;
		}
		else
		{
			RetVal = NO_ERROR;/*(no error)*/
		}
	}

	if (FUNC == WELDFUNCTION)
	{ /*Check Weld Limits*/
		if (SGN(PhaseInput) > RunningPreset.WeldPhaseLimit)
			PhaseCount += DCPTIME;
		else
			PhaseCount = 0;
		if (PhaseCount > RunningPreset.WeldPhaseLimitTime) {
			AlarmFlag = TRUE;
			ErrorReason |= PHASELIMIT_TIMEERROR;
			//			pprintf("\n phase limit error");
			RetVal = WELDERROR;
		}
		//low mode 0 & 1

		if (CurrentPreset.HDParam.InitPara[0] == STDDDSAmplitudeLoopOn
				|| CurrentPreset.HDParam.InitPara[0] == STDDDSAmplitudeLoopOff) {
			if (DDSFrequencyFpga < ddsFrequencyLow)
			{
				AlarmFlag = TRUE;
				ErrorReason |= DDSFREQ_LOWWELDLIMIT;
				RetVal = WELDERROR;
			}
		}
		else if (CurrentPreset.HDParam.InitPara[0] == SYNCDDSAmplitudeLoopOn
				|| CurrentPreset.HDParam.InitPara[0] == SYNCDDSAmplitudeLoopOff)
		{
			if (DDSFrequencyFpga < ddsFrequencyLow)
				FrequencycountLow += DCPTIME;
			else
				FrequencycountLow = 0;

			if (FrequencycountLow > RunningPreset.FrequencyLimitTime)
			{
				AlarmFlag = TRUE;
				ErrorReason |= DDSFREQ_LOWWELDLIMIT;
				printf("\n ddsfreq low limit error in 2 and 3 ");
				RetVal = WELDERROR;
			}
		}
		//high freq
		//mode 0 & 1
		if (CurrentPreset.HDParam.InitPara[0] == STDDDSAmplitudeLoopOn
				|| CurrentPreset.HDParam.InitPara[0] == STDDDSAmplitudeLoopOff) {
			if (DDSFrequencyFpga > ddsFrequencyHigh) {
				AlarmFlag = TRUE;
				ErrorReason |= DDSFREQ_HIGHWELDLIMIT;
				RetVal = WELDERROR;
			}
		}
		else if (CurrentPreset.HDParam.InitPara[0] == SYNCDDSAmplitudeLoopOn
				|| CurrentPreset.HDParam.InitPara[0] == SYNCDDSAmplitudeLoopOff)
		{
			if (DDSFrequencyFpga > ddsFrequencyHigh)
				FrequencycountHigh += DCPTIME;
			else
				FrequencycountHigh = 0;

			if (FrequencycountHigh > RunningPreset.FrequencyLimitTime) {
				AlarmFlag = TRUE;
				ErrorReason |= DDSFREQ_HIGHWELDLIMIT;
				printf("\n ddsfreq high limit error in 2 and 3 ");
				RetVal = WELDERROR;
			}
		}

	}

	return RetVal;
}/* Control() */

void AmplitudeControl(void)
/******************************************************************************/
/* AmplitudeControl(void)															         */
/* Description:-                                                            	*/
/* The amplitude control loop is implemented here.                            */
/*                       										                        */
/******************************************************************************/
{
   SINT32 AmpDifference;
//   pprintf("\n WT%d,FA%d,AA%d,PM%d",WeldTimer,FinalAmplitude,ActualAmplitude,PWMAmp);
   /* Amplitude Loop */
   AmpDifference = FinalAmplitude - ActualAmplitude;
	TempPWMValue = PWMLastValue + ((AmplitudeGain * AmpDifference))
			-((AmplitudeIntegral * LastAmpDifference));
   PWMLastValue = TempPWMValue;
   LastAmpDifference = AmpDifference;

   if (TempPWMValue <= MIN_PWMAMP_VALUE)
   {
      TempPWMValue = MIN_PWMAMP_VALUE; /*(LastAmpDifference possibly treat)*/
      PWMLastValue = MIN_PWMAMP_VALUE;
   }
   if (TempPWMValue > MAX_PWMAMP_VALUE)
   {
      TempPWMValue = MAX_PWMAMP_VALUE;
      PWMLastValue = MAX_PWMAMP_VALUE;
   }
   PWMAmp = TempPWMValue >> 8;
}

void AmplitudeControlTrue(void)
/******************************************************************************
 * AmplitudeControlTrue(void)
 * Description:-
 * This function is an update to the original amplitude control that is currently
 * only used by advanced preset values tab from the website
 ******************************************************************************/
{
	DeltaA = FinalAmplitude - ActualAmplitude;
	DeltaAIntegral = ADILastValue + DeltaA;						// Integration of Amplitude Difference

	DeltaPWM = (AmplitudePropGainNew * DeltaA) + (AmplitudeIntGainNew * DeltaAIntegral);
	TempPWMValue = PWMLastValue + DeltaPWM;
	ADILastValue = DeltaAIntegral;
	PWMLastValue = TempPWMValue;

	PWMAmp = TempPWMValue >> 8;
}

void FrequencyControl(SINT32 FUNC)
/******************************************************************************/
/* FrequencyControl(void)															         */
/* Description:-                                                            	*/
/* The Frequency control loop is implemented here.                            */
/* Input parameters:-                      					                     */
/* FUNC : Indicates either SEEK , WELD function											*/
/******************************************************************************/
{
   /* Phase Loop */
  	if (CurrentPowerFactor == 0)
		CurrentPowerFactor = 1;
	if (PhaseI == 0)
		PhaseI = 1;

	PhaseValueI = PhaseInput * (((SINT32)(CurrentInput)/CurrentPowerFactor)+1);

	PhaseValueI = (PhaseValueI << 8);
	PhaseValueI /= PhaseI;

	IntegralFactor = IntegralFactor + (-PhaseValueI);
	IntegralValue = (IntegralFactor >> 8);

   if (FUNC == SEEKFUNCTION) /*(during seek)*/
      FrequencyValue = IntegralValue + SeekStartFreqOffset;

   if (FUNC == WELDFUNCTION) /*(during weld)*/
      FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;
   DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
}

void FrequencyControlTrue(SINT32 FUNC)
/******************************************************************************/
/* FrequencyControl(SINT32)													  */
/* Description:-                                                              */
/* The Frequency control loop using the true proportional and integral values */
/* is implemented here														  */
/* Input parameters:-                      					                  */
/* FUNC : Indicates either SEEK , WELD function								  */
/******************************************************************************/
{
	static UINT16 freqAverageIndex = 0;
	SINT32 SummedPhaseValue = 0;
	UINT16 NumTotals = RunningPreset.FreqWindowSize;

	if (CurrentPowerFactorNew == 0)
		CurrentPowerFactorNew = 1;
	if(NumTotals > MAX_NUM_PHASE_AVERAGES)
		NumTotals = MAX_NUM_PHASE_AVERAGES;
	else if(NumTotals < MIN_NUM_PHASE_AVERAGES)
		NumTotals = MIN_NUM_PHASE_AVERAGES;

	PhaseValueI = PhaseInput * (((SINT32)(CurrentInput * CURRENT_INPUT_GAIN) / CurrentPowerFactorNew) + 1);

	IntegralFactor -= PhaseValueI;

	PhasePNew = RunningPreset.WeldPhaseloopPNew;

	// create a sliding window average of phase values to be used
	PhaseValues[freqAverageIndex] = PhaseValueI;
	if(numFunctionCalls > NumTotals)			// check for if we have a filled array
	{
		numFunctionCalls = NumTotals;			// so that we never get a 32bit overflow
		if(freqAverageIndex >= NumTotals)			// check for array index overflow
			freqAverageIndex = 0;
		for(int i = 0; i < NumTotals; i++)		// add all contents of array together
		{
			SummedPhaseValue += PhaseValues[i];
		}
	}
	else
		PhasePNew = 0;									// ignore correction values at startup because system
														// is not stable yet, otherwise it will cause an error

	freqAverageIndex++;

	IntegralValue = (PhaseINew * IntegralFactor) - (PhasePNew * SummedPhaseValue);

	if(IntegralValue >= 0)
		IntegralValue >>= 16;
	else
	{
		IntegralValue = -IntegralValue;
		IntegralValue >>= 16;
		IntegralValue = -IntegralValue;
	}

	if (FUNC == SEEKFUNCTION) /*(during seek)*/
      FrequencyValue = IntegralValue + SeekStartFreqOffset;

    if (FUNC == WELDFUNCTION) /*(during weld)*/
      FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;

    DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
}

SINT32 ReadWeldBufferLog(SINT32 nLogs)
/******************************************************************************/
/* ReadWeldBufferLog(SINT32 nLogs)														   */
/* Description:-                                                            	*/
/* This function will read last 5 second weld data from FRAM.                 */
/* Input parameters:-                      					                     */
/* nLogs : Indicates number of entries to read											*/
/* returns NumOflogs: Indicates number of entries read								*/
/******************************************************************************/
{
	UINT16 FreqValue, PowerValue, CurrentValue, IstAmplitudeVlue, PWMValuetemp;
	SINT16 logIndex;
	int NumOflogs = 0;
	bool FillLogDataflag = TRUE;

	logIndex = WeldBufferIndex;

	//return 0 if index is not in the range of WeldBuffer array.
	if ((logIndex < 0) || (logIndex >= SIZE_WELDBUFFER))
	{
		nLogs = 0;
		NumOflogs = 0;
	}

//	pprintf("\n logIndex=%d", logIndex);
	SINT32 logStartIndx = logIndex;
	SINT32 indx;
	for (indx = 0; indx < nLogs; indx ++)
	{
		BOOLEAN ValidFlag = FALSE;
		ValidFlag = WeldBuffer[logIndex].logValid;

		if (!ValidFlag)
		{
			NumOflogs = indx;
			nLogs = 0;
			FillLogDataflag = FALSE;
		}
		if(FillLogDataflag)
		{
			FreqValue = WeldBuffer[logIndex].Frequency;
			PowerValue = WeldBuffer[logIndex].Power;
			CurrentValue = WeldBuffer[logIndex].Current;
			IstAmplitudeVlue = WeldBuffer[logIndex].ActualAmplitude;
			PWMValuetemp = WeldBuffer[logIndex].PWMValue;

			logIndex--;
			if (logIndex < 0)
				logIndex = SIZE_WELDBUFFER - 1;
			if (logIndex == logStartIndx)
			{
				NumOflogs = SIZE_WELDBUFFER;
				nLogs = 0;
			}
			else
			{
				NumOflogs = indx;
			}
		}
	}
	return NumOflogs;
}

void CheckAndAssignWeldCycleBoundaryValue()
/****************************************************************************/
/* void CheckAndAssignWeldCycleBoundaryValue()                              */
/* Description:-                                                            */
/* This function is responsible for Checking CurrentPreset against Minimum  */
/* and Maximum values,if boundary conditions are violated, then Minimum and */
/* maximum values are assigned.                                             */
/****************************************************************************/
{
	if(CurrentPreset.WeldAmplitude < Preset_Min.WeldAmplitude)
	{
		CurrentPreset.WeldAmplitude = Preset_Min.WeldAmplitude;
	}
	if(CurrentPreset.WeldAmplitude > Preset_Max.WeldAmplitude)
	{
		CurrentPreset.WeldAmplitude = Preset_Max.WeldAmplitude;
	}

	if(CurrentPreset.WeldAmplitudeLoopC1 < Preset_Min.WeldAmplitudeLoopC1)
	{
		CurrentPreset.WeldAmplitudeLoopC1 = Preset_Min.WeldAmplitudeLoopC1;
	}
	if(CurrentPreset.WeldAmplitudeLoopC1 > Preset_Max.WeldAmplitudeLoopC1)
	{
		CurrentPreset.WeldAmplitudeLoopC1 = Preset_Max.WeldAmplitudeLoopC1;
	}

	if(CurrentPreset.WeldAmplitudeLoopC2 < Preset_Min.WeldAmplitudeLoopC2)
	{
		CurrentPreset.WeldAmplitudeLoopC2 = Preset_Min.WeldAmplitudeLoopC2;
	}
	if(CurrentPreset.WeldAmplitudeLoopC2 > Preset_Max.WeldAmplitudeLoopC2)
	{
		CurrentPreset.WeldAmplitudeLoopC2 = Preset_Max.WeldAmplitudeLoopC2;
	}

	if(CurrentPreset.WeldRampTime < Preset_Min.WeldRampTime)
	{
		CurrentPreset.WeldRampTime = Preset_Min.WeldRampTime;
	}
	if(CurrentPreset.WeldRampTime > Preset_Max.WeldRampTime)
	{
		CurrentPreset.WeldRampTime = Preset_Max.WeldRampTime;
	}

	if(CurrentPreset.WeldPhaseLimit < Preset_Min.WeldPhaseLimit)
	{
		CurrentPreset.WeldPhaseLimit = Preset_Min.WeldPhaseLimit;
	}
	if(CurrentPreset.WeldPhaseLimit > Preset_Max.WeldPhaseLimit)
	{
		CurrentPreset.WeldPhaseLimit = Preset_Max.WeldPhaseLimit;
	}

	if(CurrentPreset.WeldPhaseLimitTime < Preset_Min.WeldPhaseLimitTime)
	{
		CurrentPreset.WeldPhaseLimitTime = Preset_Min.WeldPhaseLimitTime;
	}
	if(CurrentPreset.WeldPhaseLimitTime > Preset_Max.WeldPhaseLimitTime)
	{
		CurrentPreset.WeldPhaseLimitTime = Preset_Max.WeldPhaseLimitTime;
	}

	if(CurrentPreset.WeldPhaseloopI < Preset_Min.WeldPhaseloopI)
	{
		CurrentPreset.WeldPhaseloopI = Preset_Min.WeldPhaseloopI;
	}
	if(CurrentPreset.WeldPhaseloopI > Preset_Max.WeldPhaseloopI)
	{
		CurrentPreset.WeldPhaseloopI = Preset_Max.WeldPhaseloopI;
	}

	if(CurrentPreset.WeldPhaseloopCF < Preset_Min.WeldPhaseloopCF)
	{
		CurrentPreset.WeldPhaseloopCF = Preset_Min.WeldPhaseloopCF;
	}
	if(CurrentPreset.WeldPhaseloopCF > Preset_Max.WeldPhaseloopCF)
	{
		CurrentPreset.WeldPhaseloopCF = Preset_Max.WeldPhaseloopCF;
	}

	//HD Param check

	if(CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD ] < Preset_Min.HDParam.InitPara[HD_BLIND_TIME_WELD ])
	{
		CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_WELD ] = Preset_Min.HDParam.InitPara[HD_BLIND_TIME_WELD ];
	}
	if(CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] > Preset_Max.HDParam.InitPara[HD_PWM_PERCENTAGE])
	{
		CurrentPreset.HDParam.InitPara[HD_PWM_PERCENTAGE] = Preset_Max.HDParam.InitPara[HD_PWM_PERCENTAGE];
	}

	if(CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE ] > Preset_Max.HDParam.InitPara[HD_WELD_AMPLITUDE ])
	{
		CurrentPreset.HDParam.InitPara[HD_WELD_AMPLITUDE ] = Preset_Max.HDParam.InitPara[HD_WELD_AMPLITUDE ];
	}

	if(CurrentPreset.HDParam.InitPara[HD_MODE ] < Preset_Min.HDParam.InitPara[HD_MODE ])
	{
		CurrentPreset.HDParam.InitPara[HD_MODE ] = Preset_Min.HDParam.InitPara[HD_MODE ];
	}
	if(CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME ] > Preset_Max.HDParam.InitPara[HD_SWITCH_TIME ])
	{
		CurrentPreset.HDParam.InitPara[HD_SWITCH_TIME ] = Preset_Max.HDParam.InitPara[HD_SWITCH_TIME ];
	}
	SavePreset(0);
	UpdateRunningPreset();
}

void ManFWeld_Start(void)
/******************************************************************************/
/* Weld_Start(void)															               */
/* Description:-                                                            	*/
/* This function is called before preparing the Weld.The parameters for the   */
/* loop are initialized from preset structure.The Power and Energy are   	   */
/* initialized to zero.														               */
/******************************************************************************/
{
	if(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime) >= CurrentCycleParam.totalCycleTime)
	{
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if(!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	}
	else
	{
		Clear_Weld_Status();
		Reset_WeldBuffer();
		Reset_WebWeldBuffer();
		IntegralValue = MemoryValue;
		IntegralFactor =  MemoryValue << 8;
		FreqOffsetInputAtWeldStart = FreqOffsetInput;
		FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;
		DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
		WeldStartFrequency = DDSFrequency;

		PhaseI = RunningPreset.WeldPhaseloopI;
		AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1<<16)/AMPLOOP_C1FACTOR;
		AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2<<16)
				/AMPLOOP_C2FACTOR;
		CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;

		PWMLastValue = 0;
		LastAmpDifference = 0;

		RampTime = RunningPreset.WeldRampTime;
		if (RampTime==0)
		{
			RampTime= 1; /* LowerLimit=1	*/
		}
		RunningPreset.WeldAmplitude = CurrentCycleParam.Amplitude;

		if(InitParamUpdateRequired)
		  	   UpdateFPGAInitParameters();

	    ddsMode =  RunningPreset.HDParam.InitPara[HD_MODE ];

	    FPGA->st.stWUsOff.i32Para[HD_MODE ] = ddsMode;
	    fpga_CtrlHdX = FPGA_HDCTRL_WELD;
		TempSuppressTime = 0;
		BlindTime = RunningPreset.HDParam.InitPara[HD_BLIND_TIME_WELD ];

		Power = 0;
		PowerNumber = 0;

		WRPower = 0;
		WRPowerNumber = 0;
		WeldAmpAverage = 0;
		WeldAmpAverageNumber = 0;
		Energy = 0;
		TempPeakPower = 0;
		WeldTimer = 0; /* (Timer for weld set) */

		MFcycleResults.nCycles++;
		WeldStartFlag = TRUE;
	}
}/* Weld_Start() */

void ManFWeld_Ramp(void)
/******************************************************************************/
/* Weld_Ramp(void)														                  */
/* Description:-                                                            	*/
/* Once the parameters have been registered .The ramp is started based upon   */
/* WeldTimer.In each run, the nominal amplitude is calculated and regulated.  */
/* The Weld Ramp state is exited if	there is an weld error or run input 	   */
/* is no longer available.													               */
/******************************************************************************/
{

	UINT32 TempRampStepTimer = 0;
	UINT32 TempRampTimeinMilliseconds = 0;
	/********* Transition Check ******************/
   if(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime) >= CurrentCycleParam.totalCycleTime)
	{
		MFCycleFlag = FALSE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if(!EnableWebIO)
				IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	}
   else
   {
	StartSonics();
	CycleStarted = TRUE;
   /* Ramp in 12bit ADC steps  */
		if(AMPLITUDELOOPON)
			FinalAmplitude =( ((WeldTimer+1) * ADC11BITVALUE)/RampTime);
		else
		{
			TempRampStepTimer = CONVMICRO_MILLI (WeldTimer);
			TempRampTimeinMilliseconds = CONVMICRO_MILLI (RampTime);


			pwmAmpValueAmpLoopOff = (((TempRampStepTimer+1) * ((UINT32)MAX_PWM_AMP_HD*RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]/100))/TempRampTimeinMilliseconds);
		}
		WeldError = Control(WELDFUNCTION);


	   if(AMPLITUDELOOPON){}
	   else
		PWMAmp = pwmAmpValueAmpLoopOff;
	   /* (Loop for frequency and amplitude) */
	   if (WeldError)
	   {
		 //Indicates there is currently at least one alarm.
		  UserIO::SetGenAlarm(TRUE);

		  AlarmFound = TRUE;
	   }

	   WeldTimer+= DCPTIME; /* (Timer increment) */
	   Energy += PowerInput;

	   WeldEnergyResult += PowerInput;
	   if(AMPLITUDELOOPON)
		   WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
	   else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384



		   WeldAmplitudeLimit = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;//(((RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*RunningPreset.WeldAmplitude)/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
	   }
   // Check if final amplitude is reached
	   if(AMPLITUDELOOPON)
	   {
		   if (FinalAmplitude >= WeldAmplitudeLimit)
		   {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		   }
	   }
	   else
	   {
		  if(pwmAmpValueAmpLoopOff > WeldAmplitudeLimit)
		  {
				WeldRampFlag = TRUE;
				PWMAmp = WeldAmplitudeLimit;
		  }
	   }

	   if (SCALEDTIMETOWEBSITE(SonicsOnCounter) >= TempSonicsOnTimePerWeldCycle)
	   {
			SonicsOnCounter = 0;
			WeldRunComplete = TRUE;
	   }

		//Transition check
		if ((MFCycleFlag == FALSE) || (StartMFCycle== FALSE))
		{
			NoInput = TRUE;
				WeldStopFrequency = DDSFrequencyFpga;
			WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

			// Check if store at weldend option is enabled
			if (RunningPreset.SWDip.Flags.EndOfWeldStore)
			{
				/*store at weldend*/
				MemoryValue = WeldMemoryFrequency;

				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
			}
		}
		MFcycleResults.resultantFrequency = DDSFrequency;
		MFcycleResults.resultantPower = TempPeakPower;
   }
}/* Weld_Ramp() */

void ManFWeldSonicOn_Run(void)
/*******************************************************************************/
/* Weld_Run(void)															                   */
/* Description:-                                                            	 */
/* Here amplitude has reached constant value.The Loop for frequency and  	    */
/* amplitude are run through.The welding cycle is stopped if there is an       */
/* weld error or Run Input is no longer available. The memory is stored at     */
/* weld end if SW Dip switch is enabled.             						          */
/*******************************************************************************/
{
	if(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime) >= CurrentCycleParam.totalCycleTime)
	{
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if(!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	}
	else
	{
      StartSonics();

      if(AMPLITUDELOOPON)
      {
      FinalAmplitude = (RunningPreset.WeldAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
      }
      else{
			//calc: WeldAmp*PWMPara*0xFFFF/(100*100) == ExtAmp * PWMPara * ((0XFFFF*128/100)*(128/100))/((100*128/100)(100*128/100))
			//== WeldAmp * PWMPara * 107372/16384
      		pwmAmpValueAmpLoopOff = (((UINT32)((UINT32)RunningPreset.WeldAmplitude*(UINT32)RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*107372))>>14) ;//(((RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE ]*RunningPreset.WeldAmplitude)/SCALING_FACTOR) * MAX_PWM_AMP) / SCALING_FACTOR;
      		PWMAmp = pwmAmpValueAmpLoopOff;
      	}
      WeldError = Control(WELDFUNCTION);
      /* (Loop for frequency and amplitude)*/
      if (WeldError)
      {
         AlarmFound = TRUE;
         //Indicates there is currently at least one alarm.
         UserIO::SetGenAlarm(TRUE);
      }

      Energy += PowerInput;
      WeldEnergyResult += PowerInput;

      // Calculate Peak power
      if (PowerInput > TempPeakPower)
        TempPeakPower = PowerInput;

      Power += PowerInput;
      WRPower += PowerInput;
      PowerNumber++;
      WRPowerNumber++;

      WeldAmpAverage += ActualAmplitude;
      WeldAmpAverageNumber++;
      WeldTimer+= DCPTIME;

      if (SCALEDTIMETOWEBSITE(SonicsOnCounter) >= TempSonicsOnTimePerWeldCycle)
      {
         //pprintf("\n sonic on timer expires");
         SonicsOnCounter = 0;
         WeldRunComplete = TRUE;
      }
      /********* Transition Check ******************/

      // Check if Run input is still active.
      if ((MFCycleFlag == FALSE) || (StartMFCycle == FALSE))
      {
         WeldRunComplete = TRUE;

         WeldStopFrequency = DDSFrequencyFpga;
         WeldMemoryFrequency = DDSFrequencyFpga - RunningPreset.DigitalTune;

         if (RunningPreset.SWDip.Flags.EndOfWeldStore)
         {
            /*store at weldend*/
            MemoryValue = WeldMemoryFrequency;
            CurrentPreset.MemoryFreq = WeldMemoryFrequency;
            SavePreset(0);
         }
      }
      MFcycleResults.resultantFrequency = DDSFrequency;
      MFcycleResults.resultantPower = TempPeakPower;
	}
}/* Weld_Run() */

void ManFWeldSonicOff_Run(void)
/*******************************************************************************/
/* Weld_Run(void)															                   */
/* Description:-                                                            	 */
/* Here amplitude has reached constant value.The Loop for frequency and  	    */
/* amplitude are run through.The welding cycle is stopped if there is an       */
/* weld error or Run Input is no longer available. The memory is stored at     */
/* weld end if SW Dip switch is enabled.             						          */
/*******************************************************************************/
{
	if(SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime) >= CurrentCycleParam.totalCycleTime)
	{
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if(!EnableWebIO)
				IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*)&CurrentCycleParam, sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	}
	else
	{
		// Indicates that ultrasonics are off for weld or test mode.
		UserIO::SetSonicsActive(FALSE);

		SonicsOff();
		WeldTimer+= DCPTIME;

		if (SCALEDTIMETOWEBSITE(SonicsOffCounter) >= CurrentCycleParam.sonicsOffTimePerWeldCycle)
      {
         //pprintf("\n sonic Off timer expires");
         SonicsOffCounter = 0;
         //pprintf("\n StartMFCycle2 =%d",StartMFCycle);
         StartMFCycle = FALSE;
         WeldRunComplete = TRUE;
      }
	/********* Transition Check ******************/

      // Check if Run input is still active.
      if ((MFCycleFlag == FALSE) || (StartMFCycle == FALSE))
      {
         WeldRunComplete = TRUE;

         WeldStopFrequency = DDSFrequency;
         WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;

            if (RunningPreset.SWDip.Flags.EndOfWeldStore)
            {
               /*store at weldend*/
               MemoryValue = WeldMemoryFrequency;
               CurrentPreset.MemoryFreq = MemoryValue;
               SavePreset(0);
            }
      }
	}
}/* Weld_Run() */

void ManFWeld_Stop(void)
/******************************************************************************/
/* Weld_Stop(void)															  */
/* Description:-                                                           	  */
/* The PWM amplitude is reset to zero. The Average power and amplitude is     */
/* calculated  and WeldStatus is changed to waiting.                          */
/******************************************************************************/
{
	//FPGA Amplitude is ramp Down
	PWMAmp = 0;

	// Calculate Weld Energy
	Energy += PowerInput;
   WeldEnergy = Energy;
   WeldEnergyResult += PowerInput;
	// Calculate Weld Power
	if (PowerNumber!=0)
	{
		WeldPower = Power/PowerNumber; //Averaging about constant phase
	}

	// Calculate Average amplitude
	if (WeldAmpAverageNumber!=0)
	{
		WeldAmplitudeAvr = (WeldAmpAverage/WeldAmpAverageNumber)+10; //Averaging about constant phase
	}

	// Update weld timer
	WeldTime = CONVMICRO_MILLI(WeldTimer);

	// Set weld status
	if(MFCycleFlag)
		StartMFCycle = TRUE;
	else
		StartMFCycle = FALSE;



   WeldComplete = TRUE;
}

/*********************************************************************************/
 /*  void CalculateRampStep(void) */
 /*    This function compares target amplitude with set amplitude. If set amplitude changes during weld process, then Amplitude ramping*/
 /*  shall be done. For this state is changed to weld ramp. If set amplitude is greater than target amplitude, then ramp up process shall*/
 /* be done. In the same way if set amplitude is less than target amplitude, then ramp down process shall be done. This function is called*/
 /* by WeldRun() function.*/
void CalculateRampStep(void)
{
	UINT32 TempRampTimeinMilliseconds;
	TempRampTimeinMilliseconds = CONVMICRO_MILLI(RampTime);

   if(RampTime > 0){
	   if(AMPLITUDELOOPON)
	   {
		  if(FinalAmplitude > WeldAmplitudeLimit)
			 RampDown = TRUE;
		  else
			 RampDown = FALSE;

		  RampStepTimer = ((FinalAmplitude * RampTime) / ADC11BITVALUE) + DCPTIME;
	   }
	   else
	   {
		   if(PWMAmp > WeldAmplitudeLimit)
			  RampDown = TRUE;
		   else
			  RampDown = FALSE;
		   RampStepTimer = (PWMAmp * TempRampTimeinMilliseconds)/ ((UINT32)MAX_PWM_AMP_HD * RunningPreset.HDParam.InitPara[HD_PWM_PERCENTAGE]/100);
		   RampStepTimer = CONV_MILLI_MICROSEC(RampStepTimer);
		   RampStepTimer+=DCPTIME;
	   }
      RampRequired = TRUE;
   }
   else
   {
	   if(AMPLITUDELOOPON)
		   FinalAmplitude = WeldAmplitudeLimit;
	   else
		   PWMAmp = WeldAmplitudeLimit;
   }
}
