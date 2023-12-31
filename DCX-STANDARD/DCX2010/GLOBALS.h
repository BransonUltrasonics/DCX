/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/GLOBALS.h_v   1.12.1.3   13 Jun 2014 08:44:10   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/GLOBALS.h_v  $
 * 
 *    Rev 1.12.1.3   13 Jun 2014 08:44:10   amaurya
 * Changed Error bits to match FPGA error bit.
 * 
 *    Rev 1.12.1.2   01 May 2014 10:52:24   amaurya
 * Removed GlobalTuneFreq
 * 
 *    Rev 1.12.1.1   12 Mar 2014 07:52:28   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.12.1.0   09 Sep 2013 08:59:52   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.12   15 Jan 2013 04:40:32   amaurya
 * Energy Special.
 * 
 *    Rev 1.9.1.10.1.0.1.3   09 Nov 2012 01:19:48   amaurya
 * Fixed Bar Graph wrap issue.
 * 
 *    Rev 1.9.1.10.1.0.1.2   12 Oct 2012 10:04:46   ygupta
 * Fixed Reset Overload Clear Issue.
 * 
 *    Rev 1.9.1.10.1.0.1.1   09 Oct 2012 09:58:20   ygupta
 * Fixed Scaling Issue.
 * 
 *    Rev 1.9.1.10.1.0.1.0   23 Aug 2012 11:34:42   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.9.1.10.1.0   28 Mar 2012 11:13:28   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.9.1.10   16 Sep 2011 08:24:10   RatneshA
 * commented autosonics related variables
 * 
 *    Rev 1.9.1.9   01 Sep 2011 13:05:58   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.9.1.8   16 Aug 2011 08:23:48   pdwivedi
 * code cleanup
 * 
 *    Rev 1.9.1.7   05 Aug 2011 10:32:22   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.9.1.6   02 Aug 2011 07:37:40   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.9.1.5   22 Jul 2011 16:53:46   tayars
 * Added New Control Loops
 * 
 *    Rev 1.9.1.4   15 Jul 2011 09:45:38   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.9.1.3   21 Jun 2011 11:05:54   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.9.1.2   09 May 2011 08:11:46   ygupta
 * Manufacturing counters error fixed
 * 
 *    Rev 1.9.1.0   27 Apr 2011 10:47:12   PDwivedi
 * minor bug fixes, system info page GUI updated for max serail number,Debug re enabled
 * 
 *    Rev 1.9   20 Apr 2011 11:37:56   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.8   19 Apr 2011 10:47:30   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   04 Apr 2011 10:58:38   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.6   02 Mar 2011 08:23:16   PDwivedi
 * Added variables for board test.
 * 
 *    Rev 1.5   14 Jan 2011 07:55:42   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.4   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
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
#include "UserIO.h"
#include "WebSite.h"
#include "StateMachineDrv.h"

#ifndef GLOBALS_H_
#define GLOBALS_H_

//#define printf
//#define pprintf
#define SCANFREQFACTOR    	10

#define FLASHCODE_PROGARM 1
#define FLASHTAR_PROGRAM  2
#define REDLED_STATE 1
#define GREENLED_STATE 2
enum ErrorType{
  NO_ERROR = 0,
  POWER_OVERLOAD = 0x02,
  WELDERROR = 0x01,
  CURRENT_OVERLOAD = 0x08 ,
  VOLTAGE_OVERLOAD = 0x04,
  TEMP_OVERLOAD = 0x01,
  DDSFREQ_LOWSEEKLIMIT = 0x10,
  DDSFREQ_HIGHSEEKLIMIT = 0x20,
  DDSFREQ_LOWWELDLIMIT = 0x40,
  DDSFREQ_HIGHWELDLIMIT = 0x80,
  PHASELIMIT_TIMEERROR = 0x0100,
  OVERLOAD_MASK = (POWER_OVERLOAD | CURRENT_OVERLOAD | VOLTAGE_OVERLOAD | TEMP_OVERLOAD),
  ALL_ERROR_MASK = (OVERLOAD_MASK | DDSFREQ_LOWSEEKLIMIT | DDSFREQ_HIGHSEEKLIMIT | DDSFREQ_LOWWELDLIMIT | DDSFREQ_HIGHWELDLIMIT | PHASELIMIT_TIMEERROR)
};

#define COUNTER_CRC    0xABAB41FF
/////////////////////////state machine/////////////////
extern BOOLEAN PowerUpScan;
extern BOOLEAN PowerUpScanStarted;
//extern BOOLEAN AutoSonicsEnable;
//extern BOOLEAN AutoSonicsStarted;
extern BOOLEAN NoPowerUpSeekScan;
extern BOOLEAN ScanEnable;
extern BOOLEAN ScanStarted;
extern BOOLEAN ScanRun;
extern BOOLEAN ScanStop;
extern BOOLEAN SeekStartFlag;
extern BOOLEAN SeekRampFlag;
extern BOOLEAN AlarmFound;
extern BOOLEAN SeekRunFlag;
extern BOOLEAN SeekStopFlag;
extern BOOLEAN NoInput;
extern BOOLEAN SeekWeldFlag;
extern BOOLEAN WeldStartFlag;
extern BOOLEAN WeldStartEnerFlag;
extern BOOLEAN WeldRampFlag;
extern BOOLEAN WeldRunComplete;
extern BOOLEAN WeldComplete;
extern BOOLEAN ResetComplete;
extern BOOLEAN PowerUpCompleted;
extern BOOLEAN PowerUpRequired;
extern BOOLEAN WeldErrorFinish;
extern BOOLEAN WaitComplete;
extern BOOLEAN ScanCalculate;
extern BOOLEAN ScanComplete;
extern BOOLEAN ScanAbort;
extern BOOLEAN ScanAbortComplete;
extern UINT16  SkipIndex;
extern BOOLEAN PowerUpCompleted;
extern BOOLEAN SeekEnable;
extern BOOLEAN TestEnable;
extern BOOLEAN StartInputEnable;
extern BOOLEAN ResetEnable;
extern BOOLEAN  OverLoadAlarm;
extern BOOLEAN PowerUpSeek;
extern BOOLEAN PowerUpSeekStarted;
extern BOOLEAN TestInputFlag;
extern BOOLEAN UpdateRPreset;
extern BOOLEAN MFTimeoutEnable;
extern BOOLEAN AutoResetEnable;
extern BOOLEAN IsInputNotPresent;
extern BOOLEAN IsIOConfigured;
extern UINT8 OutputStatusToFPGA;

extern SINT8 PhaseSign;
extern BOOLEAN ScanInitiate;

extern SINT16 WeldBufferIndex;
extern SINT16 PWMAmp;
extern SINT16 PhaseI;
extern SINT16 PhaseInput;
extern SINT16  WeldError;
extern SINT16 ExtAmplitudeInput;
extern SINT16 CurrentInput;
extern UINT32 PowerInput;
extern SINT32 PhaseCount;

extern UINT16 WeldBufferTime;
extern UINT16 PowerUpDelay;
extern UINT16 TotalPowerUpDelay;
extern UINT16 WaitDelay;

extern UINT32 CycleCounterIncrement;
extern UINT32 ErrorCounterIncrement;
extern SINT32 SeekStartFrequency;
extern SINT32 SeekStopFrequency;
extern SINT32 SeekMemoryFrequency;
extern SINT32 WeldStartFrequency;
extern SINT32 WeldStopFrequency;
extern SINT32 WeldMemoryFrequency;
extern SINT32 WeldTime;
extern SINT32 WeldEnergy;
extern SINT32 WeldPower;
extern SINT32 WeldAmplitudeAvr;
extern SINT32 ErrorState;
extern SINT32 ErrorReason;
extern SINT32 InputTemperature;
extern SINT32 IntegralFactor;
extern SINT32 IntegralValue;
extern SINT32 FrequencyValue;
extern SINT32 DDSFrequency;
extern SINT32 MemoryValue;
extern SINT32 FinalAmplitude;
extern SINT32 CurrentPowerFactor;
extern SINT32 AmplitudeGain, AmplitudeIntegral;
extern SINT32 SeekTimer;
extern SINT32 WeldTimer;
extern SINT32 ddsFrequencyHighSeek, ddsFrequencyLowSeek;
extern SINT32 ddsFrequencyHigh, ddsFrequencyLow;
extern SINT32 ActualAmplitude;
extern SINT32 FreqOffsetInput, FreqOffsetInputAtWeldStart;
extern SINT32 PWMLastValue, LastAmpDifference, TempPWMValue;
extern SINT32 BoardTestFrequency;
extern UINT8  BoardTestOutput;
extern UINT32 PowerOnMilliCounterIncrement;
extern UINT32 SonicOnMilliCounterIncrement;
extern UINT32 TimedSeekTimerTicks;

extern ScanParameter ScanParameterDefault;

// TEST PARAMETERS
extern SINT32 AmplitudePropGainNew, AmplitudeIntGainNew;
extern SINT32 CurrentPowerFactorNew;
extern SINT32 ADILastValue;
extern SINT32 PhaseINew;
extern SINT32 PhasePNew;
extern SINT32 DeltaA, DeltaAIntegral, DeltaPWM;

/************ struct **********/
extern Status CurrentDCPStatus, StatusSlot[20];
extern Parameter RunningPreset;
extern Parameter CurrentPreset;
extern ScanParameter ParaScan;
extern WeldData WeldBuffer[SIZE_WELDBUFFER];
extern ScanData ScanBuffer[SIZE_SCANBUFFER];
extern Parameter Preset_Min;
extern Parameter Preset_Max;
extern ScanParameter ScanParamMin;
extern ScanParameter ScanParamMax;

/*********** FRAM ************/
extern SINT8 ParmName[21];
extern UINT8 CurrentWorkNumber;
extern SystemCounters DCPCounter;

extern UINT16 AnalogInputs[3];

extern SINT32 HornScanGraphPointCount;
extern UINT32 DigitalInputs;
extern Parameter storedPreset[MAX_WORK_NUMBER];
extern BOOLEAN TimedSeekEnabled;
extern BOOLEAN PresetVerifiedRequired;
extern SINT32 Midband;
extern UINT32 F_Factor;
extern UINT16   NumScanPoints;
extern USERIOConfiguration USERIOConfig;
extern SINT8 DCPSoftwareVersionLabel[MAXPARAMNAME_LEN];
extern BOOLEAN MemoryReset;
extern int Adc_MultFact_Phase;
extern int Adc_MultFact_Ampl;
extern int Adc_MultFact_ExtAmpl;
extern int Adc_MultFact_Curr;
extern int Adc_MultFact_Temp;
extern int Adc_MultFact_Power;

extern unsigned int DcxWebUserID;
extern BOOLEAN IOTaskStop;
extern SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
extern RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
extern int MiliSecTimer;
extern int SecTimerSincePowerUp;
extern BOOLEAN EnableWebIO;
extern SINT32 MainParallelResonanceFreq;
extern SINT32 MainSeriesResonanceFreq;
extern UINT8 fpga_input;
//manufacturing
extern CycleParam MinCycleParam;
extern CycleParam MaxCycleParam;
extern CycleParam CurrentCycleParam;
extern CycleResults MFcycleResults;

extern BOOLEAN MFCycleFlag;
extern UINT32 ManFCycleCounterInc;
extern UINT32 ManFErrorCycleCounterInc;
extern BOOLEAN StartMFCycle;
extern UINT32 SonicsOnCounter;
extern UINT32 SonicsOffCounter;
extern UINT32 TempSonicsOnTimePerWeldCycle;
extern CycleParam TempCurrentCycleParam;

extern BOOLEAN regUpdateCmdFlag;
extern BOOLEAN regAmplitudeUpdateFlag;
extern EEPROMData ePromData;
extern BOOLEAN MemoryClearAtRst;
extern UINT8 PercentScan;
extern SINT32 EnergyNeeded;
extern BOOLEAN RampRequired;
extern BOOLEAN WebSeek;
#endif
