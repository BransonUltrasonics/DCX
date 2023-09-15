/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/GLOBALS.h_v   1.5.1.12   22 Sep 2016 12:02:44   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/GLOBALS.h_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added HORN_SCAN_FAILED error type
 *
 *    Rev 1.5.1.12   22 Sep 2016 12:02:44   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.5.1.11   16 Sep 2016 09:04:46   EGUAJARDO
 * Added WeldPeakCurrentResult variable for Manager Basic app.
 * 
 *    Rev 1.5.1.9   25 Jul 2016 12:19:52   GButron
 * RACKMOUNT_BASIC changed to DCX_S
 * 
 *    Rev 1.5.1.8   06 Jun 2016 14:34:02   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.5.1.7   30 Oct 2015 08:52:18   sesharma
 * Changed FRAM version to 4
 * 
 *    Rev 1.5.1.6   15 Sep 2015 06:14:50   RJamloki
 * Removed define MAXADCRAWVALUE
 * 
 *    Rev 1.5.1.5   01 Sep 2015 02:33:36   amaurya
 * Added defines and changed FRAM version to 3.
 * 
 *    Rev 1.5.1.4   04 Jun 2015 07:39:14   sesharma
 * Defines for FRAM_VERSION & FRAM_VERISON_INVERTED modified and AC Power lost added to enum ErrorType.
 * 
 *    Rev 1.5.1.3   19 Mar 2015 06:49:14   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.5.1.2   09 Mar 2015 09:37:42   sesharma
 * Mask Added for Window Limit Alarms.
 * 
 *    Rev 1.5.1.1   26 Feb 2015 08:26:44   sesharma
 * Flag added to handle window low limit alarm when it should be raised irrespective of its state i.e. enabled/disabled.
 * 
 *    Rev 1.5.1.0   13 Jan 2015 06:00:56   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.5   07 Nov 2014 15:32:44   ewomack
 * MAX ADC reading define added.  Alarms added for Power Window Limits and Timeout in Energy Mode.
 * 
 *    Rev 1.4   28 Oct 2014 16:57:38   ewomack
 * Add FRAM version number.  Change default system type to be Rack S.
 * 
 *    Rev 1.3   16 Oct 2014 03:48:22   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.2   17 Sep 2014 08:44:54   amaurya
 * Added defines.
 * 
 *    Rev 1.1   03 Sep 2014 07:37:32   amaurya
 * Added Defines for ON and OFF.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "UserIO.h"
#include "WebSite.h"
#include "StateMachineDrv.h"

#ifndef GLOBALS_H_
#define GLOBALS_H_

#define FRAM_VERSION 	4		// FRAM version changed as HandHeldMode flag is added to SoftwareFlag of DCP preset 
#define INIT_VERSION	1
#define INIT_FRAM_INDEX	1
#define FRAM_VERISON_INVERTED 0xFFFFFFFC

#define BIT0                     0x1
#define BIT1                     0x2
#define BIT2                     0x4
#define BIT3                     0x8
#define BIT4                     0x10
#define BIT5                     0x20
#define BIT6                     0x40
#define BIT7                     0x80
#define BIT8                     0x100
#define BIT9                     0x200
#define BIT10                    0x400
#define BIT11                    0x800
#define BIT12                    0x1000
#define BIT13                    0x2000
#define BIT14                    0x4000
#define BIT15                    0x8000
#define SCANFREQFACTOR    			10
#define NOTUSED						0
#define FLASHCODE_PROGARM  		1
#define FLASHTAR_PROGRAM   		2
#define REDLED_STATE 				1
#define GREENLED_STATE 				2
#define OFF					        	0
#define ON					        	1
#define FLASHCODE_PROGARM 			1
#define FLASHTAR_PROGRAM  			2
#define REDLED_STATE 				1
#define GREENLED_STATE 				2
#define MINUS_PLUS_PEAKPOWER 		2
#define DEF_SYSTEM					RACKMOUNT_S
#define DEF_EDP						1

#define DCX_S      		         BIT0
#define RACKMOUNT_S         		BIT1
#define RACKMOUNT_STD       		BIT2
#define DCXA         				BIT3
#define DCXF         				BIT4
#define PC        					BIT5
#define NOTUSED  						0
#define ALLENABLE       			(DCX_S | RACKMOUNT_S | RACKMOUNT_STD | DCXA | DCXF | PC)
#define RMBASIC_S_STD   			(DCX_S | RACKMOUNT_S | RACKMOUNT_STD)
#define RMBASIC_S       			(DCX_S | RACKMOUNT_S)
#define DCXA_F_PC       			(DCXA | DCXF | PC)
#define DCXF_PC         			(DCXF | PC)
#define RM_STD_A_F_PC   			(RACKMOUNT_STD | DCXA | DCXF | PC)
#define RACK_ALL 						(DCX_S|RACKMOUNT_S|RACKMOUNT_STD)

enum ErrorType {
   NO_ERROR = 0,
   POWER_OVERLOAD = 0x02,
   WELDERROR = 0x01,
   CURRENT_OVERLOAD = 0x08,
   VOLTAGE_OVERLOAD = 0x04,
   TEMP_OVERLOAD = 0x01,
   DDSFREQ_LOWSEEKLIMIT = 0x10,
   DDSFREQ_HIGHSEEKLIMIT = 0x20,
   DDSFREQ_LOWWELDLIMIT = 0x40,
   DDSFREQ_HIGHWELDLIMIT = 0x80,
   PHASELIMIT_TIMEERROR = 0x0100,
   PEAKPOWER_MINUSLIMIT = 0X0200,
   PEAKPOWER_PLUSLIMIT = 0X0400,
   ENERGY_REACHEDTIMEOUT = 0X0800,
   AC_POWER_LOST = 0x1000,
   HORN_SCAN_FAILED = 0x2000,
   OVERLOAD_MASK = (POWER_OVERLOAD | CURRENT_OVERLOAD | VOLTAGE_OVERLOAD
         | TEMP_OVERLOAD),
   ALL_ERROR_MASK = (OVERLOAD_MASK | DDSFREQ_LOWSEEKLIMIT
         | DDSFREQ_HIGHSEEKLIMIT | DDSFREQ_LOWWELDLIMIT | DDSFREQ_HIGHWELDLIMIT
         | PHASELIMIT_TIMEERROR | PEAKPOWER_MINUSLIMIT | PEAKPOWER_PLUSLIMIT
         | ENERGY_REACHEDTIMEOUT | HORN_SCAN_FAILED),

   WINDOW_LIMITS_MASK = (PEAKPOWER_MINUSLIMIT | PEAKPOWER_PLUSLIMIT)
};

#define COUNTER_CRC    				0xABAB41FF
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
extern UINT16 SkipIndex;
extern BOOLEAN PowerUpCompleted;
extern BOOLEAN SeekEnable;
extern BOOLEAN TestEnable;
extern BOOLEAN StartInputEnable;
extern BOOLEAN ResetEnable;
extern BOOLEAN OverLoadAlarm;
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
extern BOOLEAN WRSendFlag;
extern SINT16 WeldBufferIndex;
extern SINT16 PWMAmp;
extern SINT16 PhaseI;
extern SINT16 PhaseInput;
extern SINT16 WeldError;
extern SINT16 ExtAmplitudeInput;
extern SINT16 CurrentInput;
extern UINT32 PowerInput;
extern SINT32 PhaseCount;
extern SINT16 WeldPeakCurrentResult;
extern STR  ErrorMessageResult[WR_ERRORMSG_SIZE];

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

extern UINT32 WeldEnergyResult;

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
extern UINT8 BoardTestOutput;
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
extern UINT16 NumScanPoints;
extern USERIOConfiguration USERIOConfig;
extern SINT8 DCPSoftwareVersionLabel[MAXPARAMNAME_LEN];
extern BOOLEAN MemoryReset;

extern unsigned int DcxWebUserID;
extern BOOLEAN IOTaskStop;
extern SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
extern RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
extern SINT32 MiliSecTimer;
extern SINT32 SecTimerSincePowerUp;
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

extern BOOLEAN RegUpdateCmdFlag;
extern BOOLEAN RegMaxMinUpdateFlag;
extern BOOLEAN regAmplitudeUpdateFlag;
extern EEPROMData EPromData;
extern UINT8 PercentScan;
extern SINT32 EnergyNeeded;
extern BOOLEAN RampRequired;
extern BOOLEAN WebSeek;
extern BOOLEAN WindowLimitAlarm;
#endif
