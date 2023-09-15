/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SEEK.cpp_v   1.5   09 Apr 2015 05:27:48   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SEEK.cpp_v  $
 * 
 *    Rev 1.5   09 Apr 2015 05:27:48   sesharma
 * Removed check for External Start from run function of Seek Stop state.
 * 
 *    Rev 1.4   19 Mar 2015 07:02:34   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   13 Jan 2015 06:02:10   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.2   07 Nov 2014 16:20:50   ewomack
 * Remove AlarmFlag.
 * 
 *    Rev 1.1   17 Oct 2014 09:42:14   sesharma
 * Bug fixed DEF00770 and modifications in state Machine.
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
#include "portable.h"
#include "stdio.h"
#include "UserIO.h"
#include "ReceiveCommandHandler.h"

/*-------------------------- Global  Variables ----------------------------*/
SINT32 SeekRampTime, SeekTime;
SINT32 SeekStartFreqOffset;

/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 LCDKeyStatus;
extern Parameter Preset_Min;
extern Parameter Preset_Max;
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
	ErrorReason = NO_ERROR;

	CurrentDCPStatus.SeekStartFrequency = 0;
	CurrentDCPStatus.SeekStopFrequency = 0;

	CurrentDCPStatus.WeldStartFrequency = 0;
	CurrentDCPStatus.WeldStopFrequency = 0;
	CurrentDCPStatus.WeldTime = 0;
	CurrentDCPStatus.WeldEnergy = 0;
	CurrentDCPStatus.WeldPower = 0;
	CurrentDCPStatus.PeakPower = 0;
	CurrentDCPStatus.WeldAmplitude = 0;

//   CurrentDCPStatus.ErrorState = 0;
	CurrentDCPStatus.ErrorReason = NO_ERROR;
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
	ddsFrequencyHigh = DDSFrequency + RunningPreset.WeldFrequencyLimitHigh;
	if (ddsFrequencyHigh > (Midband + FREQ_MAX))
		ddsFrequencyHigh = (Midband + FREQ_MAX);
	ddsFrequencyLow = DDSFrequency - RunningPreset.WeldFrequencyLimitLow;
	if (ddsFrequencyLow < (Midband + FREQ_MIN))
		ddsFrequencyLow = (Midband + FREQ_MIN);
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
	CurrentDCPStatus.ErrorFrequency = DDSFrequency;
	CurrentDCPStatus.ErrorPower = PowerInput;
	CurrentDCPStatus.ErrorCurrent = CurrentInput;
	CurrentDCPStatus.ErrorAmplitude = ActualAmplitude;
	CurrentDCPStatus.ErrorTemperature = InputTemperature;
	CurrentDCPStatus.ErrorTime = SeekTimer;
	pprintf(
	      "\nSeek Result:\nSSFr=%d,\nSStpFr=%d,\nSMFr=%d,\nWMFr=%d,\nMFr=%d,\nTFr=%d\n",
	      SeekStartFrequency, SeekStopFrequency, SeekMemoryFrequency,
	      WeldMemoryFrequency, RunningPreset.MemoryFreq,
	      RunningPreset.DigitalTune);
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
	if (PresetVerifiedRequired) {
		PresetVerifiedRequired = FALSE;
		PresetVerfiedSuccess = VerifyPreset();
	}
	if (!PresetVerfiedSuccess) {
		pprintf("\n preset verified failed, assigning boundary values");
		CheckAndAssignSeekCycleBoundaryValue();
	}
	Clear_Seek_Status();
	Reset_WeldBuffer();

	FinalAmplitude = (RunningPreset.SeekAmplitude * ADC11BITVALUE)
	      / SCALING_FACTOR;

	/* (Determines the frequency of the starting seek)*/
	IntegralValue = MemoryValue;
	IntegralFactor = MemoryValue << 8;
	FrequencyValue = IntegralValue;
	SeekStartFreqOffset = ConvertFreqtoDCPformat(RunningPreset.SeekFreqOffset);
	DDSFrequency = FrequencyValue + SeekStartFreqOffset
	      + RunningPreset.DigitalTune;
	SeekStartFrequency = DDSFrequency;
	//Initialize PID constants from preset
	PhaseI = RunningPreset.SeekPhaseloopI;
	AmplitudeGain = (RunningPreset.SeekAmplitudeLoopC1 << 16) / AMPLOOP_C1FACTOR;
	AmplitudeIntegral = (RunningPreset.SeekAmplitudeLoopC2 << 16)
	      / AMPLOOP_C2FACTOR;
	CurrentPowerFactor = RunningPreset.SeekPhaseloopCF;

	/* (Set the permitted range Seek) */
	ddsFrequencyHighSeek = Midband + RunningPreset.SeekFrequLimitHigh;
	ddsFrequencyLowSeek = Midband - RunningPreset.SeekFrequLimitLow;

	PWMLastValue = 0;
	LastAmpDifference = 0;
	// Set seek ramptime from preset
	SeekRampTime = RunningPreset.SeekRampTime;
	if (SeekRampTime == 0) {
		SeekRampTime = CONV_MILLI_MICROSEC(1); /*LowerLimit=1	*/
	}

	// Set seek time from preset
	SeekTime = RunningPreset.SeekTime;
	PWMAmp = 0;
	SeekTimer = 0; /* (Seek timer value is intialized) */

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
	if (WeldError) {
		Set_Seek_Status_Error();

		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
		AlarmFound = TRUE;
	}

	SeekTimer += DCPTIME; /* Timer increment */
	/********* Transition Check ******************/
	if (FinalAmplitude
	      >= ((RunningPreset.SeekAmplitude * ADC11BITVALUE) / SCALING_FACTOR)) {
		FinalAmplitude = (RunningPreset.SeekAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR;
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
	if (WeldError) {
		Set_Seek_Status_Error();
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
		AlarmFound = TRUE;
	}

	SeekTimer += DCPTIME; /* Timer increment */

	//Wait for SeekTimer to expires or test Ext. test /LCD input to be active
	if ((TestInputFlag && (!(UserIO::CheckForExternalTest()))
	      && (!(LCDKeyStatus & KB_TEST))
	      && (!(WebSite::CheckForWebTestCommand())))
	      || (SeekTimer >= SeekTime)) {
		SeekStopFrequency = DDSFrequency;
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
	MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
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
	if (!TestInputFlag) {
		//Indicates not in seek mode
//		UserIO::SetSeekOut(FALSE);
//		UserIO::SetSonicsActive(FALSE);
		//Turn the sonics off
		SonicsOff();
		/* if not in test mode */
		store_seek_memo();
		Set_Seek_Status();
		PWMAmp = 0; /* (MaxAmplitude off) */
		SeekStopFlag = TRUE;
	}
	/* Testmode */
	else {
		/* (Check if Run input is active)  */
		if (UserIO::CheckForExternalTest() || (LCDKeyStatus & KB_TEST)
		      || WebSite::CheckForWebTestCommand()) {
			//Indicates not in seek mode
			UserIO::SetSeekOut(FALSE);
			store_seek_memo();
			Set_Seek_Status();
		} else {
			/* (Wait until signals taken back) */
			//Turn the sonics off
			SonicsOff();
			//Indicates not in seek mode
			UserIO::SetSeekOut(FALSE);
			UserIO::SetSonicsActive(FALSE);
			store_seek_memo();
			Set_Seek_Status();
			PWMAmp = 0; /* (MaxAmplitude off) */
			SeekStopFlag = TRUE;
		}
	}
	SetFPGAFrequency();
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
	if (CurrentPreset.SeekAmplitude < Preset_Min.SeekAmplitude) {
		CurrentPreset.SeekAmplitude = Preset_Min.SeekAmplitude;
	}
	if (CurrentPreset.SeekAmplitude > Preset_Max.SeekAmplitude) {
		CurrentPreset.SeekAmplitude = Preset_Max.SeekAmplitude;
	}

	if (CurrentPreset.SeekAmplitudeLoopC1 < Preset_Min.SeekAmplitudeLoopC1) {
		CurrentPreset.SeekAmplitudeLoopC1 = Preset_Min.SeekAmplitudeLoopC1;
	}
	if (CurrentPreset.SeekAmplitudeLoopC1 > Preset_Max.SeekAmplitudeLoopC1) {
		CurrentPreset.SeekAmplitudeLoopC1 = Preset_Max.SeekAmplitudeLoopC1;
	}

	if (CurrentPreset.SeekAmplitudeLoopC2 < Preset_Min.SeekAmplitudeLoopC2) {
		CurrentPreset.SeekAmplitudeLoopC2 = Preset_Min.SeekAmplitudeLoopC2;
	}
	if (CurrentPreset.SeekAmplitudeLoopC2 > Preset_Max.SeekAmplitudeLoopC2) {
		CurrentPreset.SeekAmplitudeLoopC2 = Preset_Max.SeekAmplitudeLoopC2;
	}

	if (CurrentPreset.SeekPhaseloopI < Preset_Min.SeekPhaseloopI) {
		CurrentPreset.SeekPhaseloopI = Preset_Min.SeekPhaseloopI;
	}
	if (CurrentPreset.SeekPhaseloopI > Preset_Max.SeekPhaseloopI) {
		CurrentPreset.SeekPhaseloopI = Preset_Max.SeekPhaseloopI;
	}

	if (CurrentPreset.SeekPhaseloopCF < Preset_Min.SeekPhaseloopCF) {
		CurrentPreset.SeekPhaseloopCF = Preset_Min.SeekPhaseloopCF;
	}
	if (CurrentPreset.SeekPhaseloopCF > Preset_Max.SeekPhaseloopCF) {
		CurrentPreset.SeekPhaseloopCF = Preset_Max.SeekPhaseloopCF;
	}

	if (CurrentPreset.SeekRampTime < Preset_Min.SeekRampTime) {
		CurrentPreset.SeekRampTime = Preset_Min.SeekRampTime;

	}
	if (CurrentPreset.SeekRampTime > Preset_Max.SeekRampTime) {
		CurrentPreset.SeekRampTime = Preset_Max.SeekRampTime;
	}

	if (CurrentPreset.SeekTime < Preset_Min.SeekTime) {
		CurrentPreset.SeekTime = Preset_Min.SeekTime;
	}
	if (CurrentPreset.SeekTime > Preset_Max.SeekTime) {
		CurrentPreset.SeekTime = Preset_Max.SeekTime;
	}
	SavePreset(0);
	UpdateRunningPreset();
}
