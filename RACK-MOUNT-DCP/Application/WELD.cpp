/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WELD.cpp_v   1.21.1.2   Oct 21 2016 12:51:50   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WELD.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 10:50:34   RVITE
 * Fixed issue Alarm counter increases in steps of 500
 *
 *    Rev 1.21.1.2   Oct 21 2016 12:51:50   EGUAJARDO
 * Generate an alarm when energy timeout is reached.
 * 
 *    Rev 1.21.1.1   15 Sep 2016 16:10:22   EGUAJARDO
 * Added variables in weld funtions to send weld results to the Manager Basic app.
 * 
 *    Rev 1.21.1.0   06 Jun 2016 16:56:00   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.21   28 Sep 2015 06:50:24   RJamloki
 * redundant condition checking in Weld_EnerRamp function is fixed for DCX-S system
 * 
 *    Rev 1.20   21 Sep 2015 06:46:14   RJamloki
 * Added boundary conditions for PeakPower and WeldMode in CheckAndAssignWeldCycleBoundaryValue().
 * 
 *    Rev 1.19   15 Sep 2015 06:16:18   RJamloki
 * Replaced MAXADCRAWVALUE by ADC11BITVALUE in function Weld_Start().
 * 
 *    Rev 1.18   02 Sep 2015 06:27:14   sesharma
 * Fix to Clear window limit outputs when SIG special is enabled.
 * 
 *    Rev 1.17   25 Jun 2015 07:07:54   amaurya
 * Checked for Time maintained mode in CheckForSonicsComplete().
 * 
 *    Rev 1.16   13 May 2015 08:52:20   sesharma
 * Window limit flags are checked from running preset instead of current preset.so that the modified values are taken into account for the next cycles.
 * To reset Alarms with Ext. Start in Energy Mode when Alarms-Reset required is disabled from webpage. IsIOConfigured flag is cleared in PS_Error() when weld mode is energy mode.
 * 
 *    Rev 1.15   09 Apr 2015 05:35:26   sesharma
 * Impulse Start Removed, Cycle can be aborted by Reset Input. In case of window limit alarms, only the respective window limit signal will be active.
 * 
 *    Rev 1.14   19 Mar 2015 07:15:32   sesharma
 * Branson Coding Standard Applied and Changes done for Energy Special.
 * 
 *    Rev 1.13   09 Mar 2015 09:40:58   sesharma
 * When SIG Special is Enabled only Window limit alarms should be cleared not Overload.
 * 
 *    Rev 1.12   26 Feb 2015 08:35:32   sesharma
 * Changes done to handle Overload and Limits Alarms both if they occurr at the same time.
 * 
 *    Rev 1.11   18 Feb 2015 01:04:50   SaPawar
 * power window limit low alarm added in case of not completion of impulse time
 * 
 *    Rev 1.10   23 Jan 2015 02:54:00   sesharma
 * Window limits not considered in test mode.
 * 
 *    Rev 1.9   22 Jan 2015 04:50:24   sesharma
 * if Impulse Start is Disabled, External Start i/p should be maintained through out the weld(Time & Energy Mode Only).
 * 
 *    Rev 1.8   13 Jan 2015 06:03:34   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.7   27 Nov 2014 09:12:50   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.6   13 Nov 2014 15:31:38   ewomack
 * Fixed operator precedence bug.
 * 
 *    Rev 1.5   13 Nov 2014 12:24:42   ewomack
 * Fixed bug where Power Window Limit Alarms were enabled for Rack-S and DC-Basic
 * 
 *    Rev 1.4   07 Nov 2014 16:29:54   ewomack
 * Added Time and Energy Weld Modes.  Impulse start also added.
 * 
 *    Rev 1.3   17 Oct 2014 09:43:34   sesharma
 * Bug fixed DEF00770 and modifications in state Machine.
 * 
 *    Rev 1.2   16 Oct 2014 03:51:10   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.1   03 Sep 2014 07:46:10   amaurya
 * Changed Midband to digital tune.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:34   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:08   amaurya
 * Initial revision.
 */

#include "GLOBALS.h"
#include "FUNC.h"
#include "StateMachineDrv.h"
#include "stdio.h"
#include "UserIO.h"
#include "ReceiveCommandHandler.h"
#include "LOG.h"
#include "preset.h"

//#define pprintf
//#define printf
/*
 *
 ----------------------------- DEFINES ---------------------------------------
 */
#define SGN(x) ((x>=0)?(x):(-x))   
#define CURRENT_INPUT_GAIN		10
#define ENERGY_REACHED_TIMEOUT 6000
#define ENERGY_CALCULATION_FACTOR 10
#define SIGSPECIAL_TIMEOUT	500
/*--------------------------------------------- ---------------------------*/

/*-------------------------- Global  Variables ----------------------------*/
UINT32 TempPeakPower;
UINT32 AccumulatedEnergy; /* the power is integrated to energy */
UINT32 Energy; /* the power is integrated to energy */
UINT32 Power, PowerNumber;
UINT32 WeldAmpAverage, WeldAmpAverageNumber;
SINT32 RampTime, WeldAmplitudeLimit, TempWeldAmplitudeLimit;
SINT32 PhaseValueI = 0;
SINT32 PhaseValueP = 0;
SINT32 PhaseValues[MAX_NUM_PHASE_AVERAGES];
SINT32 IntegralValues[MAX_NUM_PHASE_AVERAGES];
UINT16 numFunctionCalls;
UINT32 EnergyRequired; /* This energy need to be achieved */
UINT32 RampStepTimer;
BOOLEAN RampDown = false;
BOOLEAN WeldStoppedbyReset = FALSE;	//If Weld is stopped by Reset Input. Alarm should be cleared by next reset only.
//BOOLEAN PrimaryParameterReached = FALSE;
/*--------------------------------------------- ---------------------------*/

/*-------------------------- External Variables ---------------------------*/
extern UINT16 LCDKeyStatus;
extern BOOLEAN AlarmFlag;
extern Parameter Preset_Min;
extern Parameter Preset_Max;
extern SINT32 SeekStartFreqOffset;
extern BOOLEAN StartActiveHigh;
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

void InitializeWeldbuffer(void)
/****************************************************************************/
/* InitializeWeldbuffer(void)                                               */
/* Description:-                                                            */
/* Function Initializes WeldBuffer.											          */
/* 																			                */
/****************************************************************************/
{
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
}

/* In Time and Energy Mode, asserting the Reset signal during a weld cycle will
 * abort the cycle.  When a cycle is aborted in this way, a window low limit
 * alarm is declared which causes the LCD segment to blink (if enabled)
 * and the Window Low output to be driven low.
 */
BOOLEAN CheckforResetInput() {
	return (UserIO::CheckForExternalReset() || (LCDKeyStatus & KB_RESET)
	      || WebSite::CheckForWebReset());
}
void CheckForSonicsComplete(void) {
	if (TestInputFlag) {
		if (!(UserIO::CheckForExternalTest()) && !(LCDKeyStatus & KB_TEST)
		      && !(WebSite::CheckForWebTestCommand()))
			WeldRunComplete = TRUE;
	} else if (RunningPreset.WeldMode == CONTINUOUSMODE) {
		if (!UserIO::CheckForExternalStart())
			WeldRunComplete = TRUE;
	} else if ((TIMEMODE == RunningPreset.WeldMode)  || (TIMEMAINTAINEDMODE == RunningPreset.WeldMode)) {  //Checking for Time Mode
		if (CONVMICRO_MILLI(WeldTimer) >= RunningPreset.WeldTime)
			WeldRunComplete = TRUE;
		else if((TIMEMAINTAINEDMODE == RunningPreset.WeldMode) && (!UserIO::CheckForExternalStart())){
    	  ErrorReason |= PEAKPOWER_MINUSLIMIT;
          UserIO::SetPowerWindowLimitLow(TRUE);
          AlarmFound = TRUE;
          AlarmFlag = TRUE;
          WindowLimitAlarm = TRUE;
		}
		else if(CheckforResetInput()) {
			ErrorReason |= PEAKPOWER_MINUSLIMIT;
			UserIO::SetPowerWindowLimitLow(TRUE);
			AlarmFound = TRUE;
			AlarmFlag = TRUE;
			WeldStoppedbyReset = TRUE;
			WindowLimitAlarm = TRUE;
		}
	}
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
	if (PresetVerifiedRequired) {
		PresetVerifiedRequired = FALSE;
		PresetVerifiedSuccess = VerifyPreset();
	}
	if (!PresetVerifiedSuccess) {
		CheckAndAssignWeldCycleBoundaryValue();
	}
	Clear_Weld_Status();
	Reset_WeldBuffer();

	IntegralValue = MemoryValue;
	IntegralFactor = MemoryValue << 8;
	FreqOffsetInputAtWeldStart = FreqOffsetInput;

	FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;

	DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
	WeldStartFrequency = DDSFrequency;
	PhaseI = RunningPreset.WeldPhaseloopI;
	AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1 << 16) / AMPLOOP_C1FACTOR;
	AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2 << 16)
	      / AMPLOOP_C2FACTOR;
	CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;

	//******* New Test Values *********************************************
	PhaseINew = (RunningPreset.WeldPhaseloopINew << 16) / FREQLOOP_KI;
	PhasePNew = (RunningPreset.WeldPhaseloopPNew << 16) / FREQLOOP_KP;
	AmplitudePropGainNew = (RunningPreset.WeldAmplitudeLoopC1New << 16)
	      / AMPLOOP_C1FACTORNEW;
	AmplitudeIntGainNew = (RunningPreset.WeldAmplitudeLoopC2New << 16)
	      / AMPLOOP_C2FACTORNEW;
	CurrentPowerFactorNew = RunningPreset.WeldPhaseloopCFNew;
	numFunctionCalls = 1; // so that we can start a new running average in frequency control true
	FinalAmplitude = 0;
	PWMLastValue = 0;
	ADILastValue = 0;
	LastAmpDifference = 0;

	RampTime = RunningPreset.WeldRampTime;
	if (RampTime == 0) {
		RampTime = CONV_MILLI_MICROSEC(1); /* LowerLimit=1	*/
	}
	RampStepTimer = (FinalAmplitude * RampTime) / ADC11BITVALUE;
	if (RunningPreset.SWDip.Flags.ExtAmplitude)
		WeldAmplitudeLimit = ExtAmplitudeInput;
	else
		WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR;

	TempWeldAmplitudeLimit = WeldAmplitudeLimit;
	RampDown = FALSE;
	Power = 0;
	PowerNumber = 0;
	WeldAmpAverage = 0;
	WeldAmpAverageNumber = 0;
	AccumulatedEnergy = 0;
	Energy = 0;
	TempPeakPower = 0;
	WeldTimer = 0; /* (Timer for weld set) */
	// PrimaryParameterReached = FALSE;
	//Check if mode is energy mode which will be set if energy input is selected
	//on user input with a valid input voltage.
	if (RunningPreset.WeldMode == ENERGYMODE) {
		if (BIT(EPromData.Systemtype) == RACKMOUNT_STD) {
			//FIXME - Get appropriate energy
			EnergyNeeded = ((RunningPreset.EnergyCP)
			      * ((ENERGY_MULT_FAC * ADC11BITVALUE) / EPromData.PSpower))
			      / ENERGY_CALCULATION_FACTOR;
		}
		EnergyRequired = EnergyNeeded;
		WeldStartEnerFlag = TRUE;
	} else
		WeldStartFlag = TRUE;
	WeldStoppedbyReset = FALSE;
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

	StartSonics();
	/* Ramp in 12bit ADC steps  */
	if (RampDown == TRUE) {
		RampStepTimer -= DCPTIME;
		FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		// Check if final amplitude is reached
		if (FinalAmplitude <= WeldAmplitudeLimit) {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		}
	} else {
		RampStepTimer += DCPTIME;
		FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		// Check if final amplitude is reached
		if (FinalAmplitude >= WeldAmplitudeLimit) {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		}
	}
	WeldError = Control(WELDFUNCTION);
	/* (Loop for frequency and amplitude) */
	if (WeldError == TRUE) {
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
		AlarmFound = TRUE;
	} else {
		WeldTimer += DCPTIME; /* (Timer increment) */
		Energy += PowerInput;
		WeldEnergyResult 	+= PowerInput;			//To DCX Manager Basic app
		/********* Transition Check ******************/
		//Transition check
		if (WeldRunComplete == FALSE) {
			if (RunningPreset.SWDip.Flags.ExtAmplitude)
				WeldAmplitudeLimit = ExtAmplitudeInput;
			else
				WeldAmplitudeLimit = ((RunningPreset.WeldAmplitude * ADC11BITVALUE)
				      / SCALING_FACTOR);
			if (TempWeldAmplitudeLimit != WeldAmplitudeLimit) {
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
		CheckForSonicsComplete();
		if (WeldRunComplete == TRUE) {
			NoInput = TRUE;
			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;
			// Check if store at weldend option is enabled
			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend, and store the memory freq,only if external start*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
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

	StartSonics();
	/* Ramp in 12bit ADC steps  */
	if (RampDown == TRUE) {
		RampStepTimer -= DCPTIME;
		FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		// Check if final amplitude is reached
		if (FinalAmplitude <= WeldAmplitudeLimit) {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		}
	} else {
		RampStepTimer += DCPTIME;
		FinalAmplitude = ((RampStepTimer * ADC11BITVALUE) / RampTime);
		// Check if final amplitude is reached
		if (FinalAmplitude >= WeldAmplitudeLimit) {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		}
	}
	WeldError = Control(WELDFUNCTION);
	/* (Loop for frequency and amplitude) */
	if (WeldError == TRUE) {
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
		AlarmFound = TRUE;
	} else {
		WeldTimer += DCPTIME; /* (Timer increment) */
		AccumulatedEnergy += PowerInput;
		WeldEnergyResult += PowerInput; 	// To DCX Manager Basic
		/* Stop sonics, energy value reached*/
		//These conditions should be checked only for DC-Std system type; not for energy special
		if ((((!UserIO::CheckForExternalStart())) || CheckforResetInput())
				&& (BIT(EPromData.Systemtype) == RACKMOUNT_STD)){
			ErrorReason |= PEAKPOWER_MINUSLIMIT;
			UserIO::SetPowerWindowLimitLow(TRUE);
			AlarmFound = TRUE;
			AlarmFlag = TRUE;
			WindowLimitAlarm = TRUE;
			if (CheckforResetInput())
				WeldStoppedbyReset = TRUE;
		} else if ((AccumulatedEnergy >= EnergyRequired)
		      || ((BIT(EPromData.Systemtype) == RACKMOUNT_S)
		            && !(UserIO::CheckForExternalStart()))) {
			WeldRampFlag = TRUE;
			NoInput = TRUE;
			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;
			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend, and store the memory freq,only if external start*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
			}
		}
		/********* Transition Check ******************/
		if (WeldRampFlag == FALSE) {
			if (RunningPreset.SWDip.Flags.ExtAmplitude)
				WeldAmplitudeLimit = ExtAmplitudeInput;
			else
				WeldAmplitudeLimit = ((RunningPreset.WeldAmplitude * ADC11BITVALUE)
				      / SCALING_FACTOR);
			if (TempWeldAmplitudeLimit != WeldAmplitudeLimit) {
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
	}
}/* Weld_Ramp() */

void Weld_Run(void)
/*******************************************************************************/
/* Weld_Run(void)															   */
/* Description:-                                                               */
/* Here amplitude has reached constant value.The Loop for frequency and  	   */
/* amplitude are run through.The welding cycle is stopped if there is an       */
/* weld error or Run Input is no longer available. The memory is stored at     */
/* weld end if SW Dip switch is enabled.             						   */
/*******************************************************************************/
{
	StartSonics();
	/* (Check if external amplitude option is enabled) */

	if (RunningPreset.SWDip.Flags.ExtAmplitude)
		WeldAmplitudeLimit = ExtAmplitudeInput;
	else
		WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR;
	WeldError = Control(WELDFUNCTION);
	/* (Loop for frequency and amplitude)*/
	if (WeldError) {
		AlarmFound = TRUE;
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
	} else {

		/********* Transition Check ******************/
		if (WeldRunComplete == FALSE) {
			Energy += PowerInput;
			WeldEnergyResult +=PowerInput ; //To DCX Manager Basic


			// Calculate Peak power
			if (PowerInput > TempPeakPower)
				TempPeakPower = PowerInput;
			Power += PowerInput;
			PowerNumber++;
			WeldAmpAverage += ActualAmplitude;
			WeldAmpAverageNumber++;
			WeldTimer += DCPTIME;
			if (TempWeldAmplitudeLimit != WeldAmplitudeLimit) {
				TempWeldAmplitudeLimit = WeldAmplitudeLimit;
				CalculateRampStep();
			}
		}
		CheckForSonicsComplete();
		if (WeldRunComplete == TRUE) {
			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;
			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend, and store the memory freq,only if external start*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = WeldMemoryFrequency;
				SavePreset(0);
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
	/* (Check if external amplitude option is enabled) */
	if (RunningPreset.SWDip.Flags.ExtAmplitude)
		WeldAmplitudeLimit = ExtAmplitudeInput;
	else
		WeldAmplitudeLimit = ((RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR);
	WeldError = Control(WELDFUNCTION);
	/* (Loop for frequency and amplitude)*/
	if (WeldError) {
		AlarmFound = TRUE;
		//Indicates there is currently at least one alarm.
		UserIO::SetGenAlarm(TRUE);
	} else {
		/********* Transition Check ******************/
		// Check if Run input is still active.
		AccumulatedEnergy += PowerInput;
		WeldTimer += DCPTIME;
		WeldEnergyResult += PowerInput; // To DCX Manager Basic

		/* Stop sonics, energy value reached*/
		if (((!UserIO::CheckForExternalStart())
		      || ( CONVMICRO_MILLI(WeldTimer) >= ENERGY_REACHED_TIMEOUT)
		      || CheckforResetInput())
		      && (BIT(EPromData.Systemtype) == RACKMOUNT_STD)) {

				ErrorReason |= PEAKPOWER_MINUSLIMIT;
				UserIO::SetPowerWindowLimitLow(TRUE);

			AlarmFound = TRUE;
			AlarmFlag = TRUE;
			WindowLimitAlarm = TRUE;
			if (CheckforResetInput())
				WeldStoppedbyReset = TRUE;
		} else if ((AccumulatedEnergy >= EnergyRequired)
		      || ((BIT(EPromData.Systemtype) == RACKMOUNT_S)
		            && (!UserIO::CheckForExternalStart()))) {

					WeldRunComplete = TRUE;
					IsIOConfigured = TRUE;
					WeldStopFrequency = DDSFrequency;
					WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;
					CurrentPreset.MemoryFreq = WeldMemoryFrequency;
					if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
						/*store at weldend*/
						MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
						CurrentPreset.MemoryFreq = MemoryValue;
						SavePreset(0);
					}
		}else if(CONVMICRO_MILLI(WeldTimer) >= ENERGY_REACHED_TIMEOUT
					&& BIT(EPromData.Systemtype) == RACKMOUNT_S)
		{
			UserIO::SetGenAlarm(TRUE);
			ErrorReason |= ENERGY_REACHEDTIMEOUT;
			AlarmFound = TRUE;
			AlarmFlag = TRUE;
			if (CheckforResetInput())
					WeldStoppedbyReset = TRUE;
		}
		// Calculate Peak power
		if (PowerInput > TempPeakPower)
			TempPeakPower = PowerInput;

		Power += PowerInput;
		PowerNumber++;
		WeldAmpAverage += ActualAmplitude;
		WeldAmpAverageNumber++;

		if (TempWeldAmplitudeLimit != WeldAmplitudeLimit) {
			TempWeldAmplitudeLimit = WeldAmplitudeLimit;
			CalculateRampStep();
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
//	UserIO::SetSonicsActive(FALSE);
	if (!OverLoadAlarm) {
		SonicsOff();

		//FPGA Amplitude is ramp Down
		PWMAmp = 0;

		// Calculate Weld Energy
		AccumulatedEnergy += PowerInput;
		Energy += PowerInput;
		WeldEnergy = Energy;
		WeldEnergyResult += PowerInput; // To DCX Manager Basic
		// Calculate Weld Power
		if (PowerNumber != 0) {
			WeldPower = Power / PowerNumber; //Averaging about constant phase
		}

		// Calculate Average amplitude
		if (WeldAmpAverageNumber != 0) {
			WeldAmplitudeAvr = (WeldAmpAverage / WeldAmpAverageNumber) + 10; //Averaging about constant phase
		}

		// Update weld timer
		WeldTime = CONVMICRO_MILLI(WeldTimer);

		// Set weld status
		Set_Weld_Status();
		WeldStoppedbyReset = FALSE;
	}

	if (!TestInputFlag
	      && (((BIT(EPromData.Systemtype) & RACKMOUNT_STD) == RACKMOUNT_STD)
	            && RunningPreset.SWDip.Flags.PowerLimitOn)) {
		// for low level limit check of peak power
		if ((RunningPreset.SWDip.Flags.MinusLimitCheck == TRUE)
		      && (((TempPeakPower * 100) / ADC11BITVALUE)
		            < RunningPreset.PowerWindowLimitLow)) {
			AlarmFlag = TRUE;
			ErrorReason |= PEAKPOWER_MINUSLIMIT;
			WindowLimitAlarm = TRUE;
			UserIO::SetPowerWindowLimitLow(TRUE);
		}

		// for high level limit check of peak power
		else if ((RunningPreset.SWDip.Flags.PlusLimitCheck)
		      && (((TempPeakPower * 100) / ADC11BITVALUE)
		            > RunningPreset.PowerWindowLimitHigh)) {
			AlarmFlag = TRUE;
			ErrorReason |= PEAKPOWER_PLUSLIMIT;
			WindowLimitAlarm = TRUE;
			UserIO::SetPowerWindowLimitHigh(TRUE);
		}
		if (!AlarmFlag)
			WeldComplete = TRUE;
	} else
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
	StartSonics();
	UserIO::SetSonicsActive(TRUE); //We dont need it really
	// Clear weld status
	Clear_Weld_Status();
	FreqOffsetInputAtWeldStart = SeekStartFreqOffset;
	FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;
	DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
	WeldStartFrequency = DDSFrequency;

	// PID constants are intialized from preset
	PhaseI = RunningPreset.WeldPhaseloopI;
	AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1 << 16) / AMPLOOP_C1FACTOR;
	AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2 << 16)
	      / AMPLOOP_C2FACTOR;
	CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;
	RampTime = RunningPreset.WeldRampTime; ////////////commented

	// Intialized global variables
	Energy = 0;
	Power = 0;
	TempPeakPower = 0;
	PowerNumber = 0;
	WeldAmpAverage = 0;
	WeldAmpAverageNumber = 0;
	RampTime = RunningPreset.WeldRampTime;
	if (RampTime == 0) {
		RampTime = CONV_MILLI_MICROSEC(1); /* LowerLimit=1	*/
	}
	RampStepTimer = ((ActualAmplitude * RampTime) / ADC11BITVALUE); // + DCPTIME;
	if (RunningPreset.SWDip.Flags.ExtAmplitude)
		WeldAmplitudeLimit = ExtAmplitudeInput;
	else
		WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR; //don't use macro here
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
	WindowLimitAlarm = FALSE;
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

	CurrentDCPStatus.ErrorFrequency = (((UINT32) FreqValue) * 1000) / F_Factor;

	// Set Error Power
	PowerValue = WeldBuffer[WeldBufferIndex].Power;

	CurrentDCPStatus.ErrorPower = PowerValue;

	// Set Error Current
	CurrentValue = WeldBuffer[WeldBufferIndex].Current;

	CurrentDCPStatus.ErrorCurrent = CurrentValue;

	//Set Actual amplitude
	IstAmplitudeVlue = WeldBuffer[WeldBufferIndex].ActualAmplitude;

	CurrentDCPStatus.ErrorAmplitude = IstAmplitudeVlue;

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
	for (indx = 1; indx < 4; indx++) {
		if (ErrorReason & (1 << indx))
			skipLog++;
	}
	if (skipLog <= 1) {
		//Increment cycle and error counter
		if (MFCycleFlag)
			ManFErrorCycleCounterInc++;
		else
			if ((ErrorReason & WINDOW_LIMITS_MASK) && RunningPreset.SIG && EPromData.SIGSpecial)
			{	if ((SigSplTick+1) >= SIGSPECIAL_TIMEOUT)
				ErrorCounterIncrement++;
			}
			else
				ErrorCounterIncrement++;
	}
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

	CurrentDCPStatus.ErrorFrequency = (((UINT32) FreqValue) * 1000) / F_Factor;

	// Set Error Power
	PowerValue = WeldBuffer[WeldBufferIndex].Power;

	CurrentDCPStatus.ErrorPower = PowerValue;

	// Set Error Current
	CurrentValue = WeldBuffer[WeldBufferIndex].Current;

	CurrentDCPStatus.ErrorCurrent = CurrentValue;

	//Set Actual amplitude
	IstAmplitudeVlue = WeldBuffer[WeldBufferIndex].ActualAmplitude;

	CurrentDCPStatus.ErrorAmplitude = IstAmplitudeVlue;

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
	for (indx = 1; indx < 4; indx++) {
		if (ErrorReason & (1 << indx))
			skipLog++;
	}
	if (skipLog <= 1) {
		// Increment cycle and error counter
		// CycleCounterIncrement++;
		ManFErrorCycleCounterInc++;
	}
	MFcycleResults.resultantFrequency = DDSFrequency;
	MFcycleResults.resultantPower = TempPeakPower;
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
	if (TestInputFlag) {
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
		if (OverLoadAlarm || AlarmFound) {
			Log::WriteEventLog(TEST_OVERLOAD, 0, 0);
		}
	}
	Set_Weld_Status_Error();
	WebSite::ResetTestFlag();
	TestInputFlag = FALSE;
	SetFPGAFrequency();
	ScanInitiate = FALSE;
	if (!UserIO::CheckForExternalStart()) {
		StartActiveHigh = FALSE;
	}
	if ((ErrorReason & WINDOW_LIMITS_MASK) && RunningPreset.SIG
	      && EPromData.SIGSpecial) {
		SigSplTick++;
		if ((SigSplTick >= (SIGSPECIAL_TIMEOUT))
		      || (RunningPreset.AlarmResetReq && CheckforResetInput())
		      || (!StartActiveHigh && !RunningPreset.AlarmResetReq
		            && UserIO::CheckForExternalStart())) {
		   if (!(ErrorReason & ~WINDOW_LIMITS_MASK)){	//Only window limit alarm occurs, Reset alarm.
		      AlarmFlag = FALSE;
		      ResetEnable = TRUE;
		   }
		   ErrorReason &= ~WINDOW_LIMITS_MASK;
		   ClearWindowLimitAlarm();
		}
	} else
		WeldErrorFinish = TRUE;

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
	if (OverLoadAlarm) {
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
	WeldErrorFinish = TRUE;
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

	if (EnableWebIO)
		AutoResetEnable = TRUE;
	else {
		if (!CheckForInputs()) {
			AutoResetEnable = TRUE;
			StartActiveHigh = FALSE;
			if (RunningPreset.WeldMode == ENERGYMODE)
				IsIOConfigured = FALSE;
		}
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
	if (CurrentCycleParam.stopOnAlarm == FALSE) {
		AlarmFlag = FALSE;
		UserIO::SetGenAlarm(FALSE);
		UserIO::SetPowerWindowLimitLow(FALSE);
		UserIO::SetPowerWindowLimitHigh(FALSE);
		WindowLimitAlarm = FALSE;
		WebSite::ResetWebResetFlag();
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
	TempWeldBuffer.Frequency = (UINT16) ((DDSFrequency * F_Factor) / 1000);
	TempWeldBuffer.PWMValue = (PWMAmp * SCALING_FACTOR) / MAX_PWM_AMP;
	TempWeldBuffer.NominalAmplitude = FinalAmplitude;
	TempWeldBuffer.ActualAmplitude = (ActualAmplitude * SCALING_FACTOR)
	      / ADC11BITVALUE;
	TempWeldBuffer.Current = (CurrentInput * SCALING_FACTOR) / ADC11BITVALUE;
	TempWeldBuffer.Power = (PowerInput * SCALING_FACTOR) / ADC11BITVALUE;
	TempWeldBuffer.Phase = PhaseInput;
	TempWeldBuffer.logValid = TRUE;

	WeldBuffer[WeldBufferIndex] = TempWeldBuffer;
	WeldBufferTime++;

	if (!RunningPreset.UseTrueValues) {
		AmplitudeControl();
		FrequencyControl(FUNC);
	} else {
		AmplitudeControlTrue();
		FrequencyControlTrue(FUNC);
		numFunctionCalls++;
	}

	if (FUNC == SEEKFUNCTION) { /*(Check Seek Limits)*/
		if (DDSFrequency > ddsFrequencyHighSeek) {
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_HIGHSEEKLIMIT;
			RetVal = WELDERROR;
		} else if (DDSFrequency < ddsFrequencyLowSeek) {
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_LOWSEEKLIMIT;
			RetVal = WELDERROR;
		} else {
			RetVal = NO_ERROR;/*(no error)*/
		}
	}

	if (FUNC == WELDFUNCTION) { /*Check Weld Limits*/
		if (SGN(PhaseInput) > RunningPreset.WeldPhaseLimit)
			PhaseCount += DCPTIME;
		else
			PhaseCount = 0;
		if (PhaseCount > RunningPreset.WeldPhaseLimitTime) {
			AlarmFlag = TRUE;
			ErrorReason |= PHASELIMIT_TIMEERROR;
//			pprintf("\n phase limit error");
			RetVal = WELDERROR;
		} else if (DDSFrequency > ddsFrequencyHigh) {
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_HIGHWELDLIMIT;
//			pprintf("\n ddsfreq high limit error ");
			RetVal = WELDERROR;
		} else if (DDSFrequency < ddsFrequencyLow) {
			AlarmFlag = TRUE;
			ErrorReason |= DDSFREQ_LOWWELDLIMIT;
//			pprintf("\n ddsfreq low limit error");
			RetVal = WELDERROR;
		} else {
			RetVal = NO_ERROR;
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
	/* Amplitude Loop */
	AmpDifference = FinalAmplitude - ActualAmplitude;
	TempPWMValue = PWMLastValue + ((AmplitudeGain * AmpDifference))
	      - ((AmplitudeIntegral * LastAmpDifference));
	PWMLastValue = TempPWMValue;
	LastAmpDifference = AmpDifference;

	if (TempPWMValue <= MIN_PWMAMP_VALUE) {
		TempPWMValue = MIN_PWMAMP_VALUE; /*(LastAmpDifference possibly treat)*/
		PWMLastValue = MIN_PWMAMP_VALUE;
	}
	if (TempPWMValue > MAX_PWMAMP_VALUE) {
		TempPWMValue = MAX_PWMAMP_VALUE;
		PWMLastValue = MAX_PWMAMP_VALUE;
	}
	PWMAmp = TempPWMValue >> 16;
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
	DeltaAIntegral = ADILastValue + DeltaA;// Integration of Amplitude Difference

	DeltaPWM = (AmplitudePropGainNew * DeltaA)
	      + (AmplitudeIntGainNew * DeltaAIntegral);
	TempPWMValue = PWMLastValue + DeltaPWM;
	ADILastValue = DeltaAIntegral;
	PWMLastValue = TempPWMValue;

	PWMAmp = TempPWMValue >> 16;
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

	PhaseValueI = PhaseInput
	      * (((SINT32) (CurrentInput) / CurrentPowerFactor) + 1);

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
	if (NumTotals > MAX_NUM_PHASE_AVERAGES)
		NumTotals = MAX_NUM_PHASE_AVERAGES;
	else if (NumTotals < MIN_NUM_PHASE_AVERAGES)
		NumTotals = MIN_NUM_PHASE_AVERAGES;

	PhaseValueI = PhaseInput
	      * (((SINT32) (CurrentInput * CURRENT_INPUT_GAIN)
	            / CurrentPowerFactorNew) + 1);

	IntegralFactor -= PhaseValueI;

	PhasePNew = RunningPreset.WeldPhaseloopPNew;

	// create a sliding window average of phase values to be used
	PhaseValues[freqAverageIndex] = PhaseValueI;
	if (numFunctionCalls > NumTotals)		// check for if we have a filled array
	      {
		numFunctionCalls = NumTotals;		// so that we never get a 32bit overflow
		if (freqAverageIndex >= NumTotals)		// check for array index overflow
			freqAverageIndex = 0;
		for (int i = 0; i < NumTotals; i++)	// add all contents of array together
		      {
			SummedPhaseValue += PhaseValues[i];
		}
	} else
		PhasePNew = 0;			// ignore correction values at startup because system
	// is not stable yet, otherwise it will cause an error

	freqAverageIndex++;

	IntegralValue = (PhaseINew * IntegralFactor)
	      - (PhasePNew * SummedPhaseValue);

	if (IntegralValue >= 0)
		IntegralValue >>= 16;
	else {
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
	if ((logIndex < 0) || (logIndex >= SIZE_WELDBUFFER)) {
		nLogs = 0;
		NumOflogs = 0;
	}

//	pprintf("\n logIndex=%d", logIndex);
	SINT32 logStartIndx = logIndex;
	SINT32 indx;
	for (indx = 0; indx < nLogs; indx++) {
		BOOLEAN ValidFlag = FALSE;
		ValidFlag = WeldBuffer[logIndex].logValid;

		if (!ValidFlag) {
			NumOflogs = indx;
			nLogs = 0;
			FillLogDataflag = FALSE;
		}
		if (FillLogDataflag) {
			FreqValue = WeldBuffer[logIndex].Frequency;
			PowerValue = WeldBuffer[logIndex].Power;
			CurrentValue = WeldBuffer[logIndex].Current;
			IstAmplitudeVlue = WeldBuffer[logIndex].ActualAmplitude;
			PWMValuetemp = WeldBuffer[logIndex].PWMValue;

			logIndex--;
			if (logIndex < 0)
				logIndex = SIZE_WELDBUFFER - 1;
			if (logIndex == logStartIndx) {
				NumOflogs = SIZE_WELDBUFFER;
				nLogs = 0;
			} else {
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
	if (CurrentPreset.WeldAmplitude < Preset_Min.WeldAmplitude) {
		CurrentPreset.WeldAmplitude = Preset_Min.WeldAmplitude;
	}
	if (CurrentPreset.WeldAmplitude > Preset_Max.WeldAmplitude) {
		CurrentPreset.WeldAmplitude = Preset_Max.WeldAmplitude;
	}

	if (CurrentPreset.WeldAmplitudeLoopC1 < Preset_Min.WeldAmplitudeLoopC1) {
		CurrentPreset.WeldAmplitudeLoopC1 = Preset_Min.WeldAmplitudeLoopC1;
	}
	if (CurrentPreset.WeldAmplitudeLoopC1 > Preset_Max.WeldAmplitudeLoopC1) {
		CurrentPreset.WeldAmplitudeLoopC1 = Preset_Max.WeldAmplitudeLoopC1;
	}

	if (CurrentPreset.WeldAmplitudeLoopC2 < Preset_Min.WeldAmplitudeLoopC2) {
		CurrentPreset.WeldAmplitudeLoopC2 = Preset_Min.WeldAmplitudeLoopC2;
	}
	if (CurrentPreset.WeldAmplitudeLoopC2 > Preset_Max.WeldAmplitudeLoopC2) {
		CurrentPreset.WeldAmplitudeLoopC2 = Preset_Max.WeldAmplitudeLoopC2;
	}

	if (CurrentPreset.WeldRampTime < Preset_Min.WeldRampTime) {
		CurrentPreset.WeldRampTime = Preset_Min.WeldRampTime;
	}
	if (CurrentPreset.WeldRampTime > Preset_Max.WeldRampTime) {
		CurrentPreset.WeldRampTime = Preset_Max.WeldRampTime;
	}

	if (CurrentPreset.WeldPhaseLimit < Preset_Min.WeldPhaseLimit) {
		CurrentPreset.WeldPhaseLimit = Preset_Min.WeldPhaseLimit;
	}
	if (CurrentPreset.WeldPhaseLimit > Preset_Max.WeldPhaseLimit) {
		CurrentPreset.WeldPhaseLimit = Preset_Max.WeldPhaseLimit;
	}

	if (CurrentPreset.WeldPhaseLimitTime < Preset_Min.WeldPhaseLimitTime) {
		CurrentPreset.WeldPhaseLimitTime = Preset_Min.WeldPhaseLimitTime;
	}
	if (CurrentPreset.WeldPhaseLimitTime > Preset_Max.WeldPhaseLimitTime) {
		CurrentPreset.WeldPhaseLimitTime = Preset_Max.WeldPhaseLimitTime;
	}

	if (CurrentPreset.WeldPhaseloopI < Preset_Min.WeldPhaseloopI) {
		CurrentPreset.WeldPhaseloopI = Preset_Min.WeldPhaseloopI;
	}
	if (CurrentPreset.WeldPhaseloopI > Preset_Max.WeldPhaseloopI) {
		CurrentPreset.WeldPhaseloopI = Preset_Max.WeldPhaseloopI;
	}

	if (CurrentPreset.WeldPhaseloopCF < Preset_Min.WeldPhaseloopCF) {
		CurrentPreset.WeldPhaseloopCF = Preset_Min.WeldPhaseloopCF;
	}
	if (CurrentPreset.WeldPhaseloopCF > Preset_Max.WeldPhaseloopCF) {
		CurrentPreset.WeldPhaseloopCF = Preset_Max.WeldPhaseloopCF;
	}
	//Added EnergyCp, MaxWeldTime, ScrubTime Preset Limit checks
	if (CurrentPreset.EnergyCP < Preset_Min.EnergyCP) {
		CurrentPreset.EnergyCP = Preset_Min.EnergyCP;
	}
	if (CurrentPreset.EnergyCP > Preset_Max.EnergyCP) {
		CurrentPreset.EnergyCP = Preset_Max.EnergyCP;
	}
	if (CurrentPreset.PeakPowerCP < Preset_Min.PeakPowerCP) {
		CurrentPreset.PeakPowerCP = Preset_Min.PeakPowerCP;
	}
	if (CurrentPreset.PeakPowerCP > Preset_Max.PeakPowerCP) {
		CurrentPreset.PeakPowerCP = Preset_Max.PeakPowerCP;
	}
	if (CurrentPreset.MaxWeldTime < Preset_Min.MaxWeldTime) {
		CurrentPreset.MaxWeldTime = Preset_Min.MaxWeldTime;
	}
	if (CurrentPreset.MaxWeldTime > Preset_Max.MaxWeldTime) {
		CurrentPreset.MaxWeldTime = Preset_Max.MaxWeldTime;
	}
	if (CurrentPreset.ScrubTime < Preset_Min.ScrubTime) {
		CurrentPreset.ScrubTime = Preset_Min.ScrubTime;
	}
	if (CurrentPreset.ScrubTime > Preset_Max.ScrubTime) {
		CurrentPreset.ScrubTime = Preset_Max.ScrubTime;
	}
	if (CurrentPreset.WeldMode < Preset_Min.WeldMode) {
		CurrentPreset.WeldMode = Preset_Min.WeldMode;
	}
	if (CurrentPreset.WeldMode > Preset_Max.WeldMode && CurrentPreset.WeldMode != TIMEMAINTAINEDMODE) {

		if((CurrentPreset.WeldMode != ENERGYMODE) ||(EPromData.EnergyPowerSpecial == FALSE))				//When the Energy special software is enabled
			CurrentPreset.WeldMode = Preset_Max.WeldMode;																//The current weld mode stays being Energy mode
																																		//Even if the current maximum weldmode is less than Energy mode
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
	if (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)
	      >= CurrentCycleParam.totalCycleTime) {
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if (!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
		      sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	} else {
		Clear_Weld_Status();
		Reset_WeldBuffer();

		IntegralValue = MemoryValue;
		IntegralFactor = MemoryValue << 8;
		FrequencyValue = IntegralValue + FreqOffsetInputAtWeldStart;
		DDSFrequency = FrequencyValue + RunningPreset.DigitalTune;
		WeldStartFrequency = DDSFrequency;

		PhaseI = RunningPreset.WeldPhaseloopI;
		AmplitudeGain = (RunningPreset.WeldAmplitudeLoopC1 << 16)
		      / AMPLOOP_C1FACTOR;
		AmplitudeIntegral = (RunningPreset.WeldAmplitudeLoopC2 << 16)
		      / AMPLOOP_C2FACTOR;
		CurrentPowerFactor = RunningPreset.WeldPhaseloopCF;

		PWMLastValue = 0;
		LastAmpDifference = 0;

		RampTime = RunningPreset.WeldRampTime;
		if (RampTime == 0) {
			RampTime = CONV_MILLI_MICROSEC(1); /* LowerLimit=1	*/
		}
		RunningPreset.WeldAmplitude = CurrentCycleParam.Amplitude;
		Power = 0;
		PowerNumber = 0;
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
	/********* Transition Check ******************/
	if (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)
	      >= CurrentCycleParam.totalCycleTime) {
		MFCycleFlag = FALSE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if (!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
		      sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	} else {
		StartSonics();
		/* Ramp in 12bit ADC steps  */
		FinalAmplitude = (((WeldTimer + 1) * ADC11BITVALUE) / RampTime);
		WeldError = Control(WELDFUNCTION);
		/* (Loop for frequency and amplitude) */
		if (WeldError) {
			//Indicates there is currently at least one alarm.
			UserIO::SetGenAlarm(TRUE);

			AlarmFound = TRUE;
		}

		WeldTimer += DCPTIME; /* (Timer increment) */
		Energy += PowerInput;

		WeldEnergyResult 	+= PowerInput;			//To DCX Manager Basic app

		WeldAmplitudeLimit = (RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR;

		// Check if final amplitude is reached
		if (FinalAmplitude >= WeldAmplitudeLimit) {
			WeldRampFlag = TRUE;
			FinalAmplitude = WeldAmplitudeLimit;
		}

		if (SCALEDTIMETOWEBSITE(SonicsOnCounter)
		      >= TempSonicsOnTimePerWeldCycle) {
			SonicsOnCounter = 0;
			WeldRunComplete = TRUE;
		}

		//Transition check
		if ((MFCycleFlag == FALSE) || (StartMFCycle == FALSE)) {
			NoInput = TRUE;
			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;

			// Check if store at weldend option is enabled
			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
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
	if (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)
	      >= CurrentCycleParam.totalCycleTime) {
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if (!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
		      sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	} else {
		StartSonics();

		FinalAmplitude = (RunningPreset.WeldAmplitude * ADC11BITVALUE)
		      / SCALING_FACTOR;

		WeldError = Control(WELDFUNCTION);
		/* (Loop for frequency and amplitude)*/
		if (WeldError) {
			AlarmFound = TRUE;
			//Indicates there is currently at least one alarm.
			UserIO::SetGenAlarm(TRUE);
		}

		Energy += PowerInput;
		WeldEnergyResult 	+= PowerInput;			//To DCX Manager Basic app
		// Calculate Peak power
		if (PowerInput > TempPeakPower)
			TempPeakPower = PowerInput;

		Power += PowerInput;
		PowerNumber++;
		WeldAmpAverage += ActualAmplitude;
		WeldAmpAverageNumber++;
		WeldTimer += DCPTIME;

		if (SCALEDTIMETOWEBSITE(SonicsOnCounter)
		      >= TempSonicsOnTimePerWeldCycle) {
			//pprintf("\n sonic on timer expires");
			SonicsOnCounter = 0;
			WeldRunComplete = TRUE;
		}
		/********* Transition Check ******************/

		// Check if Run input is still active.
		if ((MFCycleFlag == FALSE) || (StartMFCycle == FALSE)) {
			WeldRunComplete = TRUE;

			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;

			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;
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
	if (SCALEDTIMETOWEBSITE(MFcycleResults.currCycleTime)
	      >= CurrentCycleParam.totalCycleTime) {
		MFCycleFlag = FALSE;
		EnableWebIO = WebSite::CheckForManufacturingFlag();
		if (!EnableWebIO)
			IsIOConfigured = TRUE;
		regAmplitudeUpdateFlag = TRUE;
//		pprintf("\n timeout");
		StartMFCycle = FALSE;
		MFcycleResults.cycleStatus = statCompleted;
		CurrentCycleParam.MfTest = FALSE;
		SonicsOff();
		SonicsOnCounter = 0;
		SonicsOffCounter = 0;
		CurrentCycleParam.CRC = ByteCRC((UINT8*) &CurrentCycleParam,
		      sizeof(CurrentCycleParam) - sizeof(CurrentCycleParam.CRC));
		MFTimeoutEnable = TRUE;
	} else {
		// Indicates that ultrasonics are off for weld or test mode.
		UserIO::SetSonicsActive(FALSE);

		SonicsOff();
		WeldTimer += DCPTIME;

		if (SCALEDTIMETOWEBSITE(SonicsOffCounter)
		      >= CurrentCycleParam.sonicsOffTimePerWeldCycle) {
			//pprintf("\n sonic Off timer expires");
			SonicsOffCounter = 0;
			//pprintf("\n StartMFCycle2 =%d",StartMFCycle);
			StartMFCycle = FALSE;
			WeldRunComplete = TRUE;
		}
		/********* Transition Check ******************/

		// Check if Run input is still active.
		if ((MFCycleFlag == FALSE) || (StartMFCycle == FALSE)) {
			WeldRunComplete = TRUE;

			WeldStopFrequency = DDSFrequency;
			WeldMemoryFrequency = DDSFrequency - RunningPreset.DigitalTune;

			if (RunningPreset.SWDip.Flags.EndOfWeldStore) {
				/*store at weldend*/
				MemoryValue = DDSFrequency - RunningPreset.DigitalTune;
				CurrentPreset.MemoryFreq = MemoryValue;
				SavePreset(0);
			}
		}
	}
}/* Weld_Run() */

void ManFWeld_Stop(void)
/******************************************************************************/
/* Weld_Stop(void)															               */
/* Description:-                                                            	*/
/* The PWM amplitude is reset to zero. The Average power and amplitude is     */
/* calculated  and WeldStatus is changed to waiting.                          */
/******************************************************************************/
{
	//FPGA Amplitude is ramp Down
	PWMAmp = 0;

	// Calculate Weld Energy
	Energy += PowerInput;
	WeldEnergy = Energy;
	WeldEnergyResult += PowerInput; // To DCX Manager Basic
	// Calculate Weld Power
	if (PowerNumber != 0) {
		WeldPower = Power / PowerNumber; //Averaging about constant phase
	}

	// Calculate Average amplitude
	if (WeldAmpAverageNumber != 0) {
		WeldAmplitudeAvr = (WeldAmpAverage / WeldAmpAverageNumber) + 10; //Averaging about constant phase
	}

	// Update weld timer
	WeldTime = CONVMICRO_MILLI(WeldTimer);

	// Set weld status
	if (MFCycleFlag)
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
void CalculateRampStep(void) {
	if (RampTime > 0) {
		if (FinalAmplitude > WeldAmplitudeLimit)
			RampDown = TRUE;
		else
			RampDown = FALSE;

		RampStepTimer = ((FinalAmplitude * RampTime) / ADC11BITVALUE);
		RampRequired = TRUE;
	} else
		FinalAmplitude = WeldAmplitudeLimit;
}
