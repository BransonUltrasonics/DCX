/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SEEK.cpp_v   1.8.1.8.1.1   Jul 07 2017 12:25:36   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SEEK.cpp_v  $
 * 
 *    Rev 1.8.1.8.1.1   Jul 07 2017 12:25:36   hasanchez
 * Update to make the code more readable.
 * 
 *    Rev 1.8.1.8.1.0   17 Mar 2016 16:23:44   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.8.1.9   17 Mar 2016 16:07:24   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.8.1.8   02 Sep 2014 07:01:26   amaurya
 * Added SeekOffset
 * 
 *    Rev 1.8.1.7   01 Apr 2014 08:01:18   rjamloki
 * Added Reset_WebWeldBuffer() to reset the WebWeldBufferTime to zero at seek start.
 * 
 *    Rev 1.8.1.6   27 Jun 2012 03:58:12   ygupta
 * Changes for High Dynamic support.
 * 
 *    Rev 1.8.1.5   01 Sep 2011 13:06:12   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.8.1.4   16 Aug 2011 08:28:20   pdwivedi
 * code cleanup
 * 
 *    Rev 1.8.1.3   02 Aug 2011 10:53:52   pdwivedi
 * Added changes for Memory management and removing warnings.
 * 
 *    Rev 1.8.1.2   02 Aug 2011 07:39:22   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.8.1.1   11 May 2011 08:41:56   ygupta
 * sonic out fix during seek,scan and TEST
 * 
 *    Rev 1.8.1.0   27 Apr 2011 10:48:06   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.8   19 Apr 2011 10:48:52   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.6   09 Dec 2010 07:54:28   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.5   06 Dec 2010 09:56:56   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:48:32   nhahmed
 * Added LCDKeyStatus to support TEST through LCD.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:46   ygupta
 * Initial revision.

******************************************************************************/

#include "GLOBALS.h"
#include "FUNC.h"
#include "StateMachineDrv.h"
#include "portable.h"
#include "stdio.h"
#include "UserIO.h"
#include "ReceiveCommandHandler.h"
#include "preset.h"

/*-------------------------- Global  Variables ----------------------------*/
SINT32 SeekRampTime, SeekTime;
SINT32 SeekStartFreqOffset;

/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 LCDKeyStatus;
extern BOOLEAN AlarmFlag;
extern Parameter Preset_Min ;
extern Parameter Preset_Max ;
/*--------------------------------------------- ---------------------------*/

void Clear_Seek_Status(void)
/****************************************************************************/
/* Clear_Seek_Status(void)                                                  */
/* Description:-                                                            */
/* Function clears seek status before Seek Start state or after Error state.*/
/* Here the structure CurrentDCPStatus is initialized.						    */
/****************************************************************************/
{
   SeekStartFrequency = 0;
   SeekStopFrequency = 0;
   WeldStartFrequency = 0;
   WeldStopFrequency = 0;
   WeldTime = 0;
   WeldEnergy = 0;
   ErrorState = 0;
   ErrorReason = NO_ERROR ;

   CurrentDCPStatus.SeekStartFrequency = 0;
   CurrentDCPStatus.SeekStopFrequency = 0;

   CurrentDCPStatus.WeldStartFrequency = 0;
   CurrentDCPStatus.WeldStopFrequency = 0;
   CurrentDCPStatus.WeldTime = 0;
   CurrentDCPStatus.WeldEnergy = 0;
   CurrentDCPStatus.WeldPower = 0;
   CurrentDCPStatus.PeakPower = 0;
   CurrentDCPStatus.WeldAmplitude = 0;

   CurrentDCPStatus.ErrorReason = NO_ERROR ;
   CurrentDCPStatus.ErrorFrequency = 0;
   CurrentDCPStatus.ErrorPower = 0;
   CurrentDCPStatus.ErrorPsv = 0;
   CurrentDCPStatus.ErrorCurrent = 0;
   CurrentDCPStatus.ErrorAmplitude = 0;
   CurrentDCPStatus.ErrorTemperature = 0;
   CurrentDCPStatus.ErrorTime = 0;
}

void Fix_DDSLimits(void)
/****************************************************************************/
/* Fix_DDSLimits(void)                                                      */
/* Description:-                                                            */
/* In this Function the successful Seek will be allowed in limits of    	 */
/* frequency of the generator set.											          */
/****************************************************************************/
{
   ddsFrequencyHigh = DDSFrequencyFpga + RunningPreset.WeldFrequencyLimitHigh;
   if(ddsFrequencyHigh >(Midband + FREQ_MAX))
   	ddsFrequencyHigh = (Midband + FREQ_MAX);
   ddsFrequencyLow = DDSFrequencyFpga - RunningPreset.WeldFrequencyLimitLow;
   if(ddsFrequencyLow < (Midband + FREQ_MIN))
   	ddsFrequencyLow =(Midband + FREQ_MIN);
}


void Set_Seek_Status(void)
/****************************************************************************/
/* Set_Seek_Status(void)                                                    */
/* Description:-                                                            */
/* Function sets seek status .Here the structure CurrentDCPStatus members   */
/* values are updated.															          */
/****************************************************************************/
{
  	CurrentDCPStatus.SeekStartFrequency = SeekStartFrequency;
  	CurrentDCPStatus.SeekStopFrequency = SeekStopFrequency;
  	CurrentDCPStatus.SeekMemoryFrequency = SeekMemoryFrequency;
  	CurrentDCPStatus.WeldMemoryFrequency = WeldMemoryFrequency;
}
	

void Set_Seek_Status_Error(void)
/****************************************************************************/
/* Set_Seek_Status_Error(void)                                              */
/* Description:-                                                            */
/* Function sets seek status error information.Here the structure 			 */
/* CurrentDCPStatus members values are updated.								    */
/****************************************************************************/
{

  	BOOLEAN ValidFlag = FALSE;
  	pprintf("\n Set_Seek_Status_Error");
  	// Write Invalid flag in Weld Buffer
  	WeldBuffer[WeldBufferIndex].logValid = ValidFlag;

  	CurrentDCPStatus.SeekStartFrequency = SeekStartFrequency;
  	CurrentDCPStatus.SeekStopFrequency = SeekStopFrequency;
  	CurrentDCPStatus.SeekMemoryFrequency = SeekMemoryFrequency;
  	CurrentDCPStatus.WeldMemoryFrequency = WeldMemoryFrequency;
  	
  	CurrentDCPStatus.ErrorReason = ErrorReason;
  	CurrentDCPStatus.ErrorFrequency = DDSFrequencyFpga;
  	CurrentDCPStatus.ErrorPower = PowerInput;
  	CurrentDCPStatus.ErrorCurrent = CurrentInput;
  	CurrentDCPStatus.ErrorAmplitude = ActualAmplitude;
  	CurrentDCPStatus.ErrorTemperature = InputTemperature;
  	CurrentDCPStatus.ErrorTime = SeekTimer;
  	pprintf("\nSeek Result:\nSSFr=%d,\nSStpFr=%d,\nSMFr=%d,\nWMFr=%d,\nMFr=%d,\nTFr=%d\n",SeekStartFrequency,SeekStopFrequency,SeekMemoryFrequency,WeldMemoryFrequency,RunningPreset.MemoryFreq,RunningPreset.DigitalTune);
}

void Seek_Start(void)
/****************************************************************************/
/* Seek_Start()																             */
/* Description:-                                                            */
/* This function prepares Seek cycle before seek is started.The seek  		 */
/* parameters are loaded and seek buffer is initialized.					       */
/* 																		                   */
/****************************************************************************/
{
	BOOLEAN PresetVerfiedSuccess = TRUE;
	if (PresetVerifiedRequired)
	{
		PresetVerifiedRequired = FALSE;
		PresetVerfiedSuccess = VerifyPreset();
	}
	if(!PresetVerfiedSuccess)
	{
		pprintf("\n preset verified failed, assigning boundary values");
		CheckAndAssignSeekCycleBoundaryValue();
	}
   Clear_Seek_Status();
   Reset_WeldBuffer();
   Reset_WebWeldBuffer();

   if(InitParamUpdateRequired)
	   UpdateFPGAInitParameters();

   ddsMode =  RunningPreset.HDParam.InitPara[HD_MODE];
   FinalAmplitude = (RunningPreset.SeekAmplitude * ADC11BITVALUE) / SCALING_FACTOR;

   /* (Determines the frequency of the starting seek)*/
   IntegralValue = MemoryValue;
   IntegralFactor = MemoryValue << 8;
   FrequencyValue = IntegralValue;
   SeekStartFreqOffset = ConvertFreqtoDCPformat(RunningPreset.SeekFreqOffset);
   DDSFrequency = FrequencyValue + SeekStartFreqOffset + RunningPreset.DigitalTune;
   SeekStartFrequency = DDSFrequency;
   //Initialize PID constants from preset
   PhaseI = RunningPreset.SeekPhaseloopI;
   AmplitudeGain = (RunningPreset.SeekAmplitudeLoopC1 << 16)/AMPLOOP_C1FACTOR;
   AmplitudeIntegral = (RunningPreset.SeekAmplitudeLoopC2 << 16)/AMPLOOP_C2FACTOR;
   CurrentPowerFactor = RunningPreset.SeekPhaseloopCF;

   /* (Set the permitted range Seek) */
   ddsFrequencyHighSeek = Midband + RunningPreset.SeekFrequLimitHigh;
   ddsFrequencyLowSeek = Midband - RunningPreset.SeekFrequLimitLow;

   PWMLastValue = 0;
   LastAmpDifference = 0;
   // Set seek ramptime from preset
   SeekRampTime = RunningPreset.SeekRampTime;
   if(SeekRampTime == 0)
   {
      SeekRampTime = CONV_MILLI_MICROSEC(1);	/*LowerLimit=1	*/
   }

  // Set seek time from preset
   SeekTime = RunningPreset.SeekTime;
   PWMAmp = 0;
   SeekTimer = 0;		/* (Seek timer value is intialized) */

   if(TestInputFlag)
   {
	   FPGA->st.stWUsOff.i32Para[HD_MODE] = ddsMode;
	   fpga_CtrlHdX = FPGA_HDCTRL_TEST_SEEKRAMP;
  }
   else
   {
	   fpga_CtrlHdX = FPGA_HDCTRL_SEEK;
	   FPGA->st.stWUsOff.i32Para[HD_MODE] = 0;
   }
   //HD
   TempSuppressTime = 0;
   BlindTime = RunningPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK ];
   SeekStartFlag = TRUE;
}/* Seek_Start() */


void Seek_Ramp(void)
/*****************************************************************************/
/* Seek_Ramp()																                 */
/* Description:-                                                             */
/* In this function , after the parameters have been registered the amplitude*/
/* will go through the ramp.In each run, the nominal amplitude is calculated */
/* and regulated.															                 */
/*****************************************************************************/
{

   StartSonics();
   /* Ramp in 12bit ADC steps */
   FinalAmplitude = (SeekTimer * ADC11BITVALUE) / SeekRampTime;

   /* Control Loop */
   WeldError = Control(SEEKFUNCTION);
   if(WeldError)
   {
      Set_Seek_Status_Error();

      //Indicates there is currently at least one alarm.
      UserIO::SetGenAlarm(TRUE);
      AlarmFound = TRUE;
   }

   SeekTimer+= DCPTIME;         /* Timer increment */
   /********* Transition Check ******************/
   if(FinalAmplitude >= ((RunningPreset.SeekAmplitude * ADC11BITVALUE) / SCALING_FACTOR))
   {
      FinalAmplitude = (RunningPreset.SeekAmplitude * ADC11BITVALUE) / SCALING_FACTOR;
      SeekRampFlag = TRUE;
   }
}/* Seek_Ramp() */

void Seek_Run(void)
/******************************************************************************/
/* Seek_Run()																                  */
/* Description:-                                                              */
/* In this function , for programmed time of the amplitude, the amplitude and */
/* and the phase loop are run. If seek is timed out or there is no test input,*/
/* then seek is stopped.													               */
/******************************************************************************/
{
   StartSonics();

   /* Control Loop */
   WeldError = Control(SEEKFUNCTION);
   if(WeldError)
   {
      Set_Seek_Status_Error();

      //Indicates there is currently at least one alarm.
      UserIO::SetGenAlarm(TRUE);

      AlarmFound = TRUE;
   }

   SeekTimer += DCPTIME;         /* Timer increment */

   //Wait for SeekTimer to expires or test Ext. test /LCD input to be active
   if((TestInputFlag && (!(UserIO::CheckForExternalTest())) && (!(LCDKeyStatus & KB_TEST)) && (!(WebSite::CheckForWebTestCommand())) && !PWMAmpAdjustFlag )||(SeekTimer >= SeekTime))
   {
      // Set SeekStop frequency
      SeekStopFrequency = DDSFrequencyFpga;
      Fix_DDSLimits();
      SeekRunFlag = TRUE;
   }
}/* Seek_Run() */


void store_seek_memo(void)
/*******************************************************************************/
/* store_seek_memo()														                   */
/* Description:-                                                               */
/* In this function , frequency information is stored in memory during 		    */
/* seek cycle.																                   */
/*																			                      */
/*******************************************************************************/
{
	MemoryValue = DDSFrequencyFpga - RunningPreset.DigitalTune;
 	SeekMemoryFrequency = MemoryValue;
	WeldMemoryFrequency = MemoryValue;
	CurrentPreset.MemoryFreq = MemoryValue;
	SavePreset(0);
}/* store_seek_memo() */


void Seek_Stop(void)
/******************************************************************************/
/* Seek_Stop()																                  */
/* Description:-                                                              */
/* In this function ,Seek is stopped.The function store_seek_memo is called	*/
/* to store frquency information in memory.	If there is no test input ,      */
/* then state is changed to WAIT_NO_INPUT state , if not state is changed to  */
/* SEEK_WELD.																                  */
/******************************************************************************/
{

	/* Check if test operation is required or not */
	if(!TestInputFlag)
	{
		//Indicates not in seek mode
		UserIO::SetSeekOut(FALSE);
		UserIO::SetSonicsActive(FALSE);
		//Turn the sonics off
		SonicsOff();
		/* if not in test mode */
		store_seek_memo();
		Set_Seek_Status();
		PWMAmp = 0;           /* (MaxAmplitude off) */
		SeekStopFlag = TRUE;
	}
	/* Testmode */
	else
	{
		/* (Check if Run input is active)  */
		if(UserIO::CheckForExternalTest() || UserIO::CheckForExternalStart()|| (LCDKeyStatus & KB_TEST) || WebSite::CheckForWebTestCommand() || PWMAmpAdjustFlag)
		{
			//Indicates not in seek mode
			if (PWMAmpAdjustFlag==FALSE)
					UserIO::SetSeekOut(FALSE);
			store_seek_memo();
			Set_Seek_Status();
		}
		else
		{
		   /* (Wait until signals taken back) */
			//Turn the sonics off
			SonicsOff();
			//Indicates not in seek mode
			UserIO::SetSeekOut(FALSE);
			UserIO::SetSonicsActive(FALSE);
			store_seek_memo();
			Set_Seek_Status();
			PWMAmp = 0;   /* (MaxAmplitude off) */
			SeekStopFlag = TRUE;
		}
	}
	ScanInitiate = FALSE;
}/* Seek_Stop */

void CheckAndAssignSeekCycleBoundaryValue()
/****************************************************************************/
/* Description:-                                                            */
/* This function is responsible for Checking CurrentPreset against Minimum  */
/* and Maximum values,if boundary conditions are violated, then Minimum and */
/* maximum values are assigned.                                             */
/****************************************************************************/
{
	if(CurrentPreset.SeekAmplitude < Preset_Min.SeekAmplitude)
	{
		CurrentPreset.SeekAmplitude = Preset_Min.SeekAmplitude;
	}
	if(CurrentPreset.SeekAmplitude > Preset_Max.SeekAmplitude)
	{
		CurrentPreset.SeekAmplitude = Preset_Max.SeekAmplitude;
	}

	if(CurrentPreset.SeekAmplitudeLoopC1 < Preset_Min.SeekAmplitudeLoopC1)
	{
		CurrentPreset.SeekAmplitudeLoopC1 = Preset_Min.SeekAmplitudeLoopC1;
	}
	if(CurrentPreset.SeekAmplitudeLoopC1 > Preset_Max.SeekAmplitudeLoopC1)
	{
		CurrentPreset.SeekAmplitudeLoopC1 = Preset_Max.SeekAmplitudeLoopC1;
	}

	if(CurrentPreset.SeekAmplitudeLoopC2 < Preset_Min.SeekAmplitudeLoopC2)
	{
		CurrentPreset.SeekAmplitudeLoopC2 = Preset_Min.SeekAmplitudeLoopC2;
	}
	if(CurrentPreset.SeekAmplitudeLoopC2 > Preset_Max.SeekAmplitudeLoopC2)
	{
		CurrentPreset.SeekAmplitudeLoopC2 = Preset_Max.SeekAmplitudeLoopC2;
	}

	if(CurrentPreset.SeekPhaseloopI < Preset_Min.SeekPhaseloopI)
	{
		CurrentPreset.SeekPhaseloopI = Preset_Min.SeekPhaseloopI;
	}
	if(CurrentPreset.SeekPhaseloopI > Preset_Max.SeekPhaseloopI)
	{
		CurrentPreset.SeekPhaseloopI = Preset_Max.SeekPhaseloopI;
	}

	if(CurrentPreset.SeekPhaseloopCF < Preset_Min.SeekPhaseloopCF)
	{
		CurrentPreset.SeekPhaseloopCF = Preset_Min.SeekPhaseloopCF;
	}
	if(CurrentPreset.SeekPhaseloopCF > Preset_Max.SeekPhaseloopCF)
	{
		CurrentPreset.SeekPhaseloopCF = Preset_Max.SeekPhaseloopCF;
	}

	if(CurrentPreset.SeekRampTime < Preset_Min.SeekRampTime)
	{
		CurrentPreset.SeekRampTime = Preset_Min.SeekRampTime;
	}
	if(CurrentPreset.SeekRampTime > Preset_Max.SeekRampTime)
	{
		CurrentPreset.SeekRampTime = Preset_Max.SeekRampTime;
	}

	if(CurrentPreset.SeekTime < Preset_Min.SeekTime)
	{
		CurrentPreset.SeekTime = Preset_Min.SeekTime;
	}
	if(CurrentPreset.SeekTime > Preset_Max.SeekTime)
	{
		CurrentPreset.SeekTime = Preset_Max.SeekTime;
	}
	//HD Param check

	if(CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK ] < Preset_Min.HDParam.InitPara[HD_BLIND_TIME_SEEK ])
	{
		CurrentPreset.HDParam.InitPara[HD_BLIND_TIME_SEEK ] = Preset_Min.HDParam.InitPara[HD_BLIND_TIME_SEEK ];
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
