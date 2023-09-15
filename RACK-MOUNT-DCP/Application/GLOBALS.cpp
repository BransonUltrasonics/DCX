/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/GLOBALS.cpp_v   1.6   22 Sep 2016 12:04:10   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/GLOBALS.cpp_v  $
 * 
 *    Rev 1.6   22 Sep 2016 12:04:10   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.5   15 Sep 2016 17:01:16   EGUAJARDO
 * Added variables for Manager Basic app.
 * 
 *    Rev 1.4   06 Jun 2016 14:08:08   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.3   19 Mar 2015 06:48:30   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.2   16 Oct 2014 03:47:58   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.1   17 Sep 2014 08:50:32   amaurya
 * Code cleanup
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"

BOOLEAN PowerUpSeek;
BOOLEAN PowerUpSeekStarted;
BOOLEAN TestInputFlag; /* (whether the test input has been set)*/
BOOLEAN UpdateRPreset;

UINT8 OutputStatusToFPGA;
SINT8 PhaseSign;
BOOLEAN ScanInitiate; /* (if set, scan was initiated) */
BOOLEAN WRSendFlag;
UINT16 WeldBufferTime;
UINT16 PowerUpDelay;
UINT16 TotalPowerUpDelay;
UINT16 WaitDelay;

SINT16 PWMAmp; /*(Computed amplitude value) */
SINT16 PhaseI; /* (Parameters of the phase control loop) */
SINT32 PhaseINew;
SINT32 PhasePNew;

SINT32 DeltaA, DeltaAIntegral, DeltaPWM;
SINT16 PhaseInput; /* (Measured phase of the knife)*/
SINT16 WeldError;
SINT16 ExtAmplitudeInput; /* (analog input amplitude setpoint)*/
SINT16 CurrentInput; /* (ADC measured by output current)*/
UINT32 PowerInput; /* (ADC measured by output) */
SINT32 PhaseCount;
SINT16 WeldPeakCurrentResult; //To send to DCX Manager Basic app
STR  ErrorMessageResult[WR_ERRORMSG_SIZE]; // To send to DCX Manager Basic app

UINT32 PowerOnMilliCounterIncrement;
UINT32 SonicOnMilliCounterIncrement;
UINT32 TimedSeekTimerTicks;
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

SINT32 WeldEnergyResult;		//To send to DCX Manager Basic app

SINT32 WeldPower;
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
SINT32 FinalAmplitude; /* (MaxAmplitude Set point) */
SINT32 CurrentPowerFactor; /* (Power factor of the phase control loop)*/
SINT32 CurrentPowerFactorNew;
SINT32 AmplitudeGain, AmplitudeIntegral; /* (MaxAmplitude parameters of the rule circle)*/
SINT32 AmplitudePropGainNew, AmplitudeIntGainNew;
SINT32 SeekTimer; /* (Seek the clock run)*/
SINT32 WeldTimer; /* (Clock Welding)*/
SINT32 ddsFrequencyHighSeek, ddsFrequencyLowSeek;
SINT32 ddsFrequencyHigh, ddsFrequencyLow; /* (Frequency limits of the permitted work area) */
SINT32 ActualAmplitude;
SINT32 FreqOffsetInput, FreqOffsetInputAtWeldStart; /* (Freq value of the analog input offset) */
SINT32 PWMLastValue, LastAmpDifference, TempPWMValue;
SINT32 BoardTestFrequency;
UINT8 BoardTestOutput;
BOOLEAN TimedSeekEnabled;
CycleParam TempCurrentCycleParam;
/*******************STRUCT *******************/
Status CurrentDCPStatus;
Parameter RunningPreset;
Parameter CurrentPreset;

Status StatusSlot[20];

/*****************State machine variables********************/
BOOLEAN PowerUpScan;
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
BOOLEAN WeldErrorFinish;
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
BOOLEAN OverLoadAlarm;
BOOLEAN MFTimeoutEnable;
BOOLEAN AutoResetEnable;
BOOLEAN IsInputNotPresent;
BOOLEAN IsIOConfigured;
BOOLEAN PresetVerifiedRequired;
UINT16 SkipIndex;
SINT32 Midband;
UINT32 F_Factor;
BOOLEAN MemoryReset;
BOOLEAN EnableWebIO;
SINT32 MainParallelResonanceFreq, MainSeriesResonanceFreq;
UINT8 fpga_input;
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
EEPROMData EPromData;
SINT32 EnergyNeeded;
BOOLEAN WebSeek = FALSE;
BOOLEAN RampRequired = FALSE;
