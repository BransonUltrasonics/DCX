/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/GLOBALS.cpp_v   1.16.1.6.1.5   Apr 05 2017 13:49:32   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/GLOBALS.cpp_v  $
 * 
 *    Rev 1.16.1.6.1.5   Apr 05 2017 13:49:32   hasanchez
 * Updated the variable name WeldErrorFinish to WeldErrorFinishFlag
 * 
 *    Rev 1.16.1.6.1.4   Mar 29 2017 11:40:06   hasanchez
 * Added LimitReady flag to delay the ready signal resistor limit has been reached.
 * 
 *    Rev 1.16.1.6.1.3   Mar 15 2017 12:07:26   hasanchez
 * Updated the data type of WRTimer to UINT32.
 * 
 *    Rev 1.16.1.6.1.2   Feb 09 2017 15:25:30   hasanchez
 * Addded delcaration related to the Manager Basic.
 * 
 *    Rev 1.16.1.6.1.1   29 Apr 2016 09:36:42   hasanchez
 * Coding standard applied
 * 
 *    Rev 1.16.1.6.1.0   17 Mar 2016 16:23:40   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.16.1.7   17 Mar 2016 16:07:20   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.16.1.6   02 Sep 2014 06:57:40   amaurya
 * Added RampRequired flag.
 * 
 *    Rev 1.16.1.5   01 Apr 2014 07:57:48   rjamloki
 * Added variables FrequencycountLow, FrequencycountHigh.
 * 
 *    Rev 1.16.1.4   13 Feb 2013 09:19:32   amaurya
 * Added Graphs, Energy special and rescaling special.
 * 
 *    Rev 1.18   15 Jan 2013 04:40:18   amaurya
 * Energy Special.
 * 
 *    Rev 1.16.1.0.1.1   12 Oct 2012 10:04:34   ygupta
 * Fixed Reset Overload Clear Issue.
 * 
 *    Rev 1.16.1.0.1.0   23 Aug 2012 11:34:42   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.16.1.0   28 Mar 2012 11:13:12   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.16   16 Sep 2011 08:24:08   RatneshA
 * commented autosonics related variables
 * 
 *    Rev 1.15   16 Aug 2011 08:23:30   pdwivedi
 * code cleanup
 * 
 *    Rev 1.14   05 Aug 2011 10:32:10   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.13   02 Aug 2011 07:37:28   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.12   22 Jul 2011 16:53:44   tayars
 * Added New Control Loops
 * 
 *    Rev 1.11   15 Jul 2011 09:45:26   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.10   21 Jun 2011 11:05:44   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.9   09 May 2011 08:11:34   ygupta
 * Manufacturing counters error fixed
 * 
 *    Rev 1.8   20 Apr 2011 11:37:42   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.7   19 Apr 2011 10:47:16   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.6   04 Apr 2011 10:58:34   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.5   02 Mar 2011 08:34:02   PDwivedi
 * Added variables for board test
 * 
 *    Rev 1.4   14 Jan 2011 07:55:42   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.3   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.2   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.1   22 Nov 2010 09:04:24   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

******************************************************************************/

#include "DEF_DEF.h"
#include "TYP_DEF.h"
BOOLEAN PWMAmpAdjustFlag;
BOOLEAN PWMAdjustONFlag;
BOOLEAN PowerUpSeek;
BOOLEAN PowerUpSeekStarted;
BOOLEAN TestInputFlag; 			/* (whether the test input has been set)*/
BOOLEAN UpdateRPreset;

UINT8 OutputStatusToFPGA;
SINT8 PhaseSign;
BOOLEAN ScanInitiate;	   	      /* (if set, scan was initiated) */

UINT16 WeldBufferTime;
UINT16 PowerUpDelay;
UINT16 TotalPowerUpDelay;
UINT16 WaitDelay;

SINT16 PhaseI;             	   /* (Parameters of the phase control loop) */
SINT32 PhaseINew;
SINT32 PhasePNew;

SINT32 DeltaA, DeltaAIntegral, DeltaPWM;
SINT16 PhaseInput;		         /* (Measured phase of the knife)*/
SINT16  WeldError;
SINT16 ExtAmplitudeInput;        /* (analog input amplitude setpoint)*/
//todo
SINT16 CurrentInput;	            /* (ADC measured by output current)*/
UINT32 PowerInput;		         /* (ADC measured by output) */
SINT32 PhaseCount;
SINT32 FrequencycountLow = 0;
SINT32 FrequencycountHigh = 0;


UINT8  TempMode=0;
UINT8  TempAmp=0;
UINT32 PowerOnMilliCounterIncrement;
UINT32 SonicOnMilliCounterIncrement;
UINT32 TimedSeekTimerTicks;
UINT32 TimedPWMTimerTicks = 0;
UINT32 TimedTestTimerTicks = 0 ;
SINT32 HornScanGraphPointCount;
UINT32 CycleCounterIncrement;
UINT32 ErrorCounterIncrement;
SINT32 SeekStartFrequency;
SINT32 SeekStopFrequency;
SINT32 SeekMemoryFrequency;
SINT32 WeldStartFrequency;
SINT32 WeldStopFrequency;
SINT32 WeldMemoryFrequency;
SINT32 WeldTime;
SINT32 WeldEnergy;
SINT32 WeldPower;
SINT32 WRPower;
UINT32 PowerNumber;
UINT32 WRPowerNumber;
UINT32 WeldAmpAverage;
UINT32 WeldAmpAverageNumber;
UINT32 Power;
SINT32 WeldAmplitudeAvr;
SINT32 ErrorState;
SINT32 ErrorReason;
SINT32 InputTemperature;
SINT32 IntegralFactor;
SINT32 IntegralValue;
SINT32 FrequencyValue;
SINT32 DDSFrequency;
SINT32 MemoryValue;
SINT32 ADILastValue;
SINT32 FinalAmplitude;		              /* (MaxAmplitude Set point) */
SINT32 CurrentPowerFactor;			        /* (Power factor of the phase control loop)*/
SINT32 CurrentPowerFactorNew;
SINT32 AmplitudeGain, AmplitudeIntegral; /* (MaxAmplitude parameters of the rule circle)*/
SINT32 AmplitudePropGainNew, AmplitudeIntGainNew;
SINT32 SeekTimer;		                    /* (Seek the clock run)*/
SINT32 WeldTimer;		                    /* (Clock Welding)*/
SINT32 ddsFrequencyHighSeek, ddsFrequencyLowSeek;
SINT32 ddsFrequencyHigh, ddsFrequencyLow; /* (Frequency limits of the permitted work area) */
SINT32 ActualAmplitude;
SINT32 FreqOffsetInput, FreqOffsetInputAtWeldStart;		               /* (Freq value of the analog input offset) */
SINT32 PWMLastValue, LastAmpDifference, TempPWMValue;
SINT32 BoardTestFrequency;
UINT16  BoardTestOutput;
BOOLEAN TimedSeekEnabled;
CycleParam TempCurrentCycleParam;
/*******************STRUCT *******************/
Status CurrentDCPStatus;
Parameter RunningPreset;
Parameter CurrentPreset;

Status StatusSlot[20];

/*****************State machine variables********************/
BOOLEAN PowerUpPWMAmpFlag;
BOOLEAN PowerUpScan;
BOOLEAN PowerUpTestStartedFlag;
BOOLEAN PowerUpScanStarted;
//BOOLEAN AutoSonicsStarted;
BOOLEAN NoPowerUpSeekScan;
BOOLEAN ScanEnable;
//BOOLEAN AutoSonicsEnable;
BOOLEAN ScanStarted;
BOOLEAN ScanRun;
BOOLEAN ScanStop;
BOOLEAN SeekStartFlag;
BOOLEAN SeekRampFlag;
BOOLEAN AlarmFound;
BOOLEAN SeekRunFlag;
BOOLEAN SeekStopFlag;
BOOLEAN NoInput;
BOOLEAN SeekWeldFlag;
BOOLEAN WeldStartFlag;
BOOLEAN WeldStartEnerFlag;
BOOLEAN WeldRampFlag;
BOOLEAN WeldRunComplete;
BOOLEAN WeldComplete;
BOOLEAN ResetComplete;
BOOLEAN PowerUpCompleted;
BOOLEAN WeldErrorFinishFlag;
BOOLEAN WaitComplete;
BOOLEAN ScanCalculate;
BOOLEAN ScanComplete;
BOOLEAN ScanAbort;
BOOLEAN ScanAbortComplete;
BOOLEAN PowerUpRequired;
BOOLEAN SeekEnable;
BOOLEAN TestEnable;
BOOLEAN StartInputEnable;
BOOLEAN ResetEnable;
BOOLEAN  OverLoadAlarm;
BOOLEAN MFTimeoutEnable;
BOOLEAN AutoResetEnable;
BOOLEAN IsInputNotPresent;
BOOLEAN IsIOConfigured;
BOOLEAN PresetVerifiedRequired;
UINT16  SkipIndex;
SINT32 Midband;
UINT32 F_Factor;
BOOLEAN MemoryReset;
int Adc_MultFact_Phase;
int Adc_MultFact_Ampl;
int Adc_MultFact_ExtAmpl;
int Adc_MultFact_Curr;
int Adc_MultFact_Temp;
int Adc_MultFact_Power;
BOOLEAN EnableWebIO;
SINT32 MainParallelResonanceFreq,MainSeriesResonanceFreq;
//UINT8 fpga_input;
////////////////Manufacturing///////////////////
CycleParam MinCycleParam;
CycleParam MaxCycleParam;

BOOLEAN MFCycleFlag;
UINT32 ManFCycleCounterInc;
UINT32 ManFErrorCycleCounterInc;
BOOLEAN StartMFCycle;
UINT32 SonicsOnCounter;
UINT32 SonicsOffCounter;
UINT32 TempSonicsOnTimePerWeldCycle;
/*********************************************************/

//EEPROM Data
EEPROMData ePromData;
BOOLEAN MemoryClearAtRst;
SINT32 EnergyNeeded;

 //HD
UINT32  ddsMode;
UINT32 TempSuppressTime;
UINT32 BlindTime;
UINT32 pwmAmpValueAmpLoopOff;
UINT8 fpga_input;
UINT16 fpga_status;
UINT16 fpga_hdstaus;
BOOLEAN InitParamUpdateRequired;
SINT32 DDSFrequencyFpga;
UINT16 PWMAmp;				         /*(Computed amplitude value) */
SINT16 PhaseInputFpga;		         /* (Measured phase of the knife)*/
UINT16 fpga_CtrlHdX;
WeldData WeldBuffer[SIZE_WELDBUFFER];
BOOLEAN WebSeek = FALSE;		// Seek from website
BOOLEAN RampRequired = FALSE;


BOOLEAN CycleStarted = FALSE;
BOOLEAN WRSendFlag;
UINT32 WRTimer = 0;
SINT16 WeldPeakCurrentResult; //To send to DCX Manager Basic app
STR  ErrorMessageResult[WR_ERRORMSG_SIZE]; // To send to DCX Manager Basic app
SINT32 WeldEnergyResult;		//To send to DCX Manager Basic app
BOOLEAN LimitReadyFlag = TRUE;




