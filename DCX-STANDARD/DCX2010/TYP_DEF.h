/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/TYP_DEF.h_v   1.16.1.3   23 May 2014 09:57:12   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/TYP_DEF.h_v  $
 * 
 *    Rev 1.16.1.3   23 May 2014 09:57:12   amaurya
 * Changed DigitalTune type to SINT32.
 * 
 *    Rev 1.16.1.2   01 May 2014 10:53:42   amaurya
 * Removed TuneFreq
 * 
 *    Rev 1.16.1.1   30 Apr 2014 06:19:54   amaurya
 * Added Digital Tune 
 * 
 *    Rev 1.16.1.0   09 Sep 2013 08:59:56   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.16   15 Jan 2013 04:42:34   amaurya
 * Energy Special.
 * 
 *    Rev 1.13   16 Aug 2011 08:29:42   pdwivedi
 * code cleanup
 * 
 *    Rev 1.12   22 Jul 2011 16:54:18   tayars
 * Added New Control Loops
 * 
 *    Rev 1.11   15 Jul 2011 09:48:54   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.10   13 Jun 2011 09:27:32   PDwivedi
 * Chnages due to frequency and power level combination.
 * 
 *    Rev 1.9   25 Apr 2011 08:45:20   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.8   19 Apr 2011 10:49:32   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   04 Apr 2011 10:58:26   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.6   02 Mar 2011 08:21:42   PDwivedi
 * Added Alarm Latch
 * 
 *    Rev 1.5   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.4   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.3   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:50:36   nhahmed
 * Removed typedef.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

#ifndef TYP_DEF_H_
#define TYP_DEF_H_

#include "portable.h"
#include "DEF_DEF.H"

enum  ABS_LIMIT
{
	ABS_LIMIT_OFF
};

enum AMPSTEP_COND
{
	AMPSTEP_COND_OFF
};

enum COL_LIMITS
{
	COL_LIMITS_OFF
};

enum CYCLE_ABORT
{
	CYCLE_ABORT_OFF
};

enum ENERGY_LIMIT
{
	ENERGY_LIMIT_OFF
};

enum FREQ_LIMIT
{
	FREQ_LIMIT_OFF
};

enum FORCESTART_RAMP
{
	FORCESTART_RAMP_OFF
};

enum FORCESTEP_COND
{
	FORCESTEP_COND_OFF
};

enum HARDWARE_CONFIG{
 HARDWARE_CONFIG_OFF
};

enum HOLDFORCE_RAMP
{
	HOLDFORCE_RAMP_OFF
};

enum POWER_LIMIT
{
	POWER_LIMIT_OFF
};

enum PRESET_STATUS
{
	NOTVERIFIED
};

enum REJECT_LIMIT
{
	REJECT_LIMIT_OFF
};

enum RETURN_FORCE
{
	RETURN_FORCE_OFF
};

enum STEPFORCE_RAMP
{
	STEPFORCE_RAMP_OFF
};

enum TRS_LIMIT
{
	TRS_LIMIT_OFF
};

enum WELDFORCE_LIMIT
{
	WELDFORCE_LIMIT_OFF
};

enum WELDFORCE_RAMP
{
	WELDFORCE_RAMP_OFF
};

enum WELDTIME_LIMIT
{
	WELDTIME_LIMIT_OFF
};

enum MODES
{                      /* All possible welding modes            */
	TIMEMODE = BIT(0),               /* TIMEMODE   = 0x0001                   */
	ENERGYMODE = BIT(1),             /* ENERGYMODE = 0x0002                   */
	PKPOWERMODE = BIT(2),            /* PKPOWERMODE = 0x0004                  */
	COLLAPSEMODE = BIT(3),           /* COLLAPSEMODE = 0x0008                 */
	ABSOLUTEMODE = BIT(4),           /* ABSOLUTEMODE = 0x0010                 */
	GRDDETECTMODE = BIT(5),          /* GRDDETECTMODE = 0x0020                */
	TECMODE = BIT(6),                /* TECMODE = 0x0040                      */
	CONTINUOUSMODE = BIT(7),         /* CONTINUOUSMODE = 0x0080               */
	ALLMODES = BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7)
};

enum FREQUENCY
{
	FREQ_20KHZ = 20,
	FREQ_30KHZ = 30,
	FREQ_40KHZ = 40,
};
enum POWER
{
	P400W = 400,
	P750W = 750,
	P800W = 800,
	P1250W = 1250,
	P1500W = 1500,
	P2500W = 2500,
	P4000W = 4000
};

enum FrequencyPowerOption
{
	T20Khz1250W = 1,
	T20Khz2500W,
	T20Khz4000W,
	T30Khz1500W,
	T30Khz750W,
	T40Khz400W,
	T40Khz800W
};

struct PSSpecificatin
{
	FREQUENCY freq;
	POWER pwr;
	FrequencyPowerOption type;
};

typedef struct DPSPreset  //0x0080
{
	UINT32 	prsetName1;
	UINT32 	prsetName2;
	UINT32 	prsetName3;
	UINT32 	prsetName4;
	UINT32 	prsetName5;
	SINT32	AbAmp;
	SINT32	AbDelay;
	BOOLEAN	AbFlag;
	SINT32	AbTime;
	SINT32	ABSCutoffDist;
	BOOLEAN	ABSCutoffFlag;
	SINT32	AbsDistCP;
	enum	ABS_LIMIT AbsLimitFlags;
	SINT32	AbsMLR;
	SINT32	AbsMLS;
	SINT32	AbsPLR;
	SINT32	AbsPLS;
	SINT32	ActClearDist;
	BOOLEAN	ActClearFlag;
	SINT32	WeldAmplitude;//int32 Amplitude1;
	SINT32	Amplitude2;
	SINT32	WeldAmplitudeLoopC1;//int AmplitudeLoopC1;
	SINT32	WeldAmplitudeLoopC2;//int AmplitudeLoopC2;
	SINT32	WeldAmplitudeLoopC1New;//int AmplitudeLoopC1;
	SINT32	WeldAmplitudeLoopC2New;//int AmplitudeLoopC2;
	SINT32	AmpStartRamp;
	enum	AMPSTEP_COND AmpStepCondition;
	SINT32	AmpStepAbs;
	SINT32	AmpStepCol;
	SINT32	AmpStepEnergy;
	BOOLEAN	AmpStepExt;
	SINT32	AmpStepForce;
	SINT32	AmpStepPower;
	SINT32	ScrubAmp;
	SINT32	ColCutoffDist;
	BOOLEAN	ColCutoffFlag;
	SINT32	ColDistCP;
	enum	COL_LIMITS ColLimitFlags;
	SINT32	ColMLR;
	SINT32	ColMLS;
	SINT32	ColPLR;
	SINT32	ColPLS;
	UINT32	ControlLimits;
	SINT32	CreatedDate;
	SINT32	CreatedHour;
	SINT32	CreatedMinute;
	SINT32	CreatedMonth;
	SINT32	CreatedYear;
	enum	CYCLE_ABORT CycleAborts;
	SINT32	DownSpeed1;
	SINT32	DownSpeed2;
	SINT32	EBAmp;
	BOOLEAN	EBFlag;
	SINT32	EBTime;
	SINT32	EnergyCP;
	SINT32	EnergyComp;
	BOOLEAN	EnergyCompFlag;
	SINT32	EnergyCutoff;
	BOOLEAN	EnergyCutoffFlag;
	enum	ENERGY_LIMIT EnergyLimitFlags;
	SINT32	EnergyMLR;
	SINT32	EnergyMLS;
	SINT32	EnergyPLR;
	SINT32	EnergyPLS;
	BOOLEAN	ExtTrigDelay;
	enum	FREQ_LIMIT FreqLimitFlags;
	SINT32	FreqMLR;
	SINT32	FreqMLS;
	SINT32	FreqPLR;
	SINT32	FreqPLS;
	SINT32	FreqOffset;
	BOOLEAN	FreqOffsetFlag;
	SINT32	Force1;
	SINT32	Force2;
	enum	FORCESTART_RAMP ForceStartRamp;
	enum	FORCESTEP_COND ForceStepCondition;
	SINT32	ForceStepAbs;
	SINT32	ForceStepCol;
	SINT32	ForceStepEnergy;
	BOOLEAN	ForceStepExt;
	SINT32	ForceStepAmp;
	SINT32	ForceStepPower;
	SINT32	GaugePressure;
	BOOLEAN	GeneralAlarmFlag;
	BOOLEAN	GrdDetectCP;
	enum	HARDWARE_CONFIG HardwareConfig;
	SINT32	HoldForce;
	enum	HOLDFORCE_RAMP HoldRorceRamp;
	SINT32	HoldTime;
	BOOLEAN	HoldTimeFlag;
	SINT32	MaxWeldTime;
	BOOLEAN MemoryStoreFlag;
	SINT32   MemoryFreq;
	SINT32	MissingPartMin;
	SINT32	MissingPartMax;
	BOOLEAN	MissingPartFlag;
	BOOLEAN	MissingPartMinFlag;
	BOOLEAN	MissingPartMaxFlag;
	SINT32	ModifiedDate;
	SINT32	ModifiedHour;
	SINT32	ModifiedMinute;
	SINT32	ModifiedMonth;
	SINT32	ModifiedYear;
	SINT32	PeakPowerCP;
	SINT32	PeakPowerCutoff;
	BOOLEAN	PeakPowerCutoffFlag;
	SINT32  WeldPhaseLimit;//	SINT32	PhaseLimit;
	SINT32  WeldPhaseLimitNew;
	SINT32  WeldPhaseLimitTime;//	SINT32	PhaseLimitTime;
	SINT32  WeldPhaseLimitTimeNew;
	SINT32  WeldPhaseloopI;//	SINT32	PhaseLoop;
	SINT32  WeldPhaseloopCF;//	SINT32	PhaseLoopCF;
	BOOLEAN UseTrueValues;
	SINT32  WeldPhaseloopINew;//	SINT32	Phase Integral Gain ;
	SINT32  WeldPhaseloopPNew;//	SINT32  Phase Proportional Gain
	UINT16	FreqWindowSize;
	SINT32  WeldPhaseloopCFNew;//	SINT32	PF of PCL ;
	SINT32	PMCCycleCount;
	UINT8   PMCData [402] ;
	BOOLEAN	PMCEnabledFlag;
	SINT32	PMCHighLimit;
	SINT32	PMCLowLimit;
	BOOLEAN	PMCMinusBandFlag;
	BOOLEAN	PMCPlusBandFlag;
	SINT32	PMCTimeout;
	BOOLEAN	PostWeldSeekFlag;
	enum	POWER_LIMIT PowerLimitFlags;
	UINT32	PowerMLR;
	UINT32	PowerMLS;
	UINT32	PowerPLR;
	UINT32	PowerPLS;
	UINT16	PresetID[12];
	enum	PRESET_STATUS PresetStatus;
	SINT32	PreTrigAmp;
	SINT32	PreTrigDelay;
	BOOLEAN	PreTrigDelayFlag;
	SINT32	PreTrigDist;
	BOOLEAN	PretrigDistFlag;
	enum	REJECT_LIMIT RejectLimits;
	BOOLEAN	RejectLimitsFlag;
	BOOLEAN	RejResetRequiredFlag;
	enum	RETURN_FORCE ReturnForceRamp;
	/* DIAG GUI*/
	SINT32	SBLEnableMax;
	SINT32	SBLEnableMin;
	UINT32	SBLFreqFlags;
	SINT32	SBLFreqMax;
	SINT32	SBLFreqMin;
	SINT32	SBLFreqMaxLimit;
	SINT32	SBLFreqMinLimit;
	SINT32	ScrubTime;
	BOOLEAN	ScrubTimeFlag;
	SINT32	SeekAmplitude;
	SINT32	SeekAmplitudeLoopC1;
	SINT32	SeekAmplitudeLoopC2;
	SINT32	SeekFrequLimitHigh;
	SINT32	SeekFrequLimitLow;
	SINT32	SeekPhaseloopI;
	SINT32	SeekPhaseloopCF;
	SINT32	SeekRampTime;
	SINT32	SeekTime;
	SINT32  SeekFreqOffset;
	BOOLEAN	SetPressure;
	SINT32	SetRTComp;
	enum	STEPFORCE_RAMP StepForceRamp;
	UINT32	SuspectLimits;
	BOOLEAN	SuspectLimitsFlag;
	BOOLEAN	SusResetRequiredFlag;
	SINT32	TestAmp;
	BOOLEAN	TimedSeekFlag;
	BOOLEAN	TimeOnTrigFlag;
	SINT32	TriggerForce;
	SINT32	TriggerDelay;
	BOOLEAN	TriggerDelayFlag;
	enum	TRS_LIMIT TrsLimitFlags;
	SINT32	TrsDistMLR;
	SINT32	TrsDistMLS;
	SINT32	TrsDistPLR;
	SINT32	TrsDistPLS;
	SINT32  DigitalTune;
	SINT32	VQSCycleTimeMax;
	SINT32	VQSCycleTimeMin;
	SINT32	VQSEnergyMin;
	SINT32	VQSEnergyMax;
	UINT32	VQSLimit;
	SINT32	VQSPowerMin;
	SINT32	VQSPowerMax;
	SINT32	VQSTotalAbsMax;
	SINT32	VQSTotalAbsMin;
	SINT32	VQSTotalColMax;
	SINT32	VQSTotalColMin;
	SINT32	VQSWeldColMax;
	SINT32	VQSWeldColMin;
	SINT32	VQSWeldForceMax;
	SINT32	VQSWeldForceMin;
	SINT32	VQSWeldTimeMax;
	SINT32	VQSWeldTimeMin;
	SINT32	WeldCount;
	SINT32	WeldForce;
	enum	WELDFORCE_LIMIT WeldForceLimitFlags;
	SINT32	WeldForceMLR;
	SINT32	WeldForceMLS;
	SINT32	WeldForcePLR;
	SINT32	WeldForcePLS;
	enum	WELDFORCE_RAMP WeldForceRamp;
	SINT32	WeldFrequencyLimitHigh;
	SINT32	WeldFrequencyLimitHighNew;
	SINT32	WeldFrequencyLimitLow;
	SINT32	WeldFrequencyLimitLowNew;
	SINT32	WeldRampTime;
	BOOLEAN	WeldStatusFlag;
	SINT32	WeldTime;
	enum	WELDTIME_LIMIT WeldTimeLimitFlags;
	enum MODES WeldMode;
	SINT32	WeldTimeMLR;
	SINT32	WeldTimeMLS;
	SINT32	WeldTimePLR;
	SINT32	WeldTimePLS;
	UINT16	WriteInField1[12];
	UINT16	WriteInField2[12];
	union
	{
		UINT32 SoftwareFlag;
		struct
		{
			UINT32  unused: 24;
			UINT16  ClearMemAtRst: 1;
			UINT16  EndOfWeldStore: 1;
			UINT16  ExtAmplitude: 1;
			UINT16  ExtFrequency: 1;
			UINT16  AutoSonics: 1;
			UINT16  TimedSeek: 1;
			UINT16  PowerUpOptions: 2;//0 Off, 1-Seek ,2-Scan,3-Seek/Auto sonics
		}Flags;
	}SWDip;
	BOOLEAN AlarmResetReq;
	UINT32 	CRC;
}Parameter;

typedef struct HornScanParams
{
	SINT32 FrequencyStart;
	SINT32 FrequencyStop;
	SINT32 FrequencyStep;
	SINT32 TimeDelay;
	UINT16 MaxAmplitude;
	UINT16 MaxCurrent;
	UINT32 CRC;
}ScanParameter;

typedef struct HornScanGraphPointData
{
	SINT32 Frequency;
	SINT16 PWMValue;
	SINT16 Amplitude;
	SINT16 Current;
	SINT16 Power;
	SINT16 Phase;
	SINT16 Status;
}ScanData;

typedef struct DPSStatus
{
	SINT32 SeekStartFrequency;
	SINT32 SeekStopFrequency;
	SINT32 SeekMemoryFrequency;
	SINT32 WeldStartFrequency;
	SINT32 WeldStopFrequency;
	SINT32 WeldMemoryFrequency;
	SINT32 WeldTime;
	SINT32 WeldEnergy;
	SINT32 Temperature;
	SINT32 WeldPower;
	SINT32 PeakPower;
	SINT32 WeldAmplitude;
	UINT32 CycleCounter;
	UINT32 HourCounter;
//	SINT32 ErrorState;
	UINT32 ErrorReason;
	SINT32 ErrorFrequency;
	SINT32 ErrorPower;
	SINT32 ErrorCurrent;
	SINT32 ErrorPsv;
	SINT32 ErrorAmplitude;
	SINT32 ErrorTemperature;
	SINT32 ErrorTime;
}Status;

typedef struct HornWeldPointData
{
	SINT16	Time;
	UINT16	Frequency;
	SINT16	PWMValue;
	SINT16	NominalAmplitude;
	SINT16	ActualAmplitude;
	SINT16	Current;
	SINT16	Power;
	SINT16	Phase;
	BOOLEAN	logValid;
}WeldData;

struct SystemCounters
{
	UINT32 CycleCounter;// total Cycle counter
	UINT32 ErrorCounter;// total Error cycle counter
	UINT32 PowerOnSecond;
	UINT32 PowerOnMilliseconds;    //Total Milliseconds of power on.
	UINT32 SecondsOfSonic;         //Total seconds of sonic run.
	UINT32 MillisecondOfSonic;     //Total Millisecond of sonic run.
	UINT32 	CRC;
};

/* The values for Scan Point status are in the following enumeration. */
enum HORNScanPointStatus
{  HornScanPointValid    = 0,
   HornScanPointLast     = 1,
   HornScanPointNotValid = 2
};

/* Resonance Frequency Types*/
enum RESONANCE_FREQ_TYPE
{   FREQ_NONE,
    FREQ_SERIES,
    FREQ_PARALLEL
};

enum DUPS_FREQCONVERSION   /*The appropriate units are as follows:*/
{   DUPS_15KHZ = 3,   /*  15Khz Units = (3 * 0.0025) 0.075hz/bit*/
    DUPS_20KHZ = 4,   /*  20Khz Units = (4 * 0.025 ) 0.100hz/bit*/
    DUPS_30KHZ = 6,   /*  30Khz Units = (6 * 0.025 ) 0.150hz/bit*/
    DUPS_40KHZ = 8    /*  40hz  Units = (8 * 0.025 ) 0.200hz/bit*/
};

//The system currently in during TEST from website
enum CurrentState
{
	stateError,
	stateWait,
	stateSeek,
	stateRun,
	stateScan,
};

#define FREQ20KHz_START          19475
#define FREQ20KHz_HzPERSEGMENT   50

#define FREQ30KHz_START          29278
#define FREQ30KHz_HzPERSEGMENT   76

#define FREQ40KHz_START          38950
#define FREQ40KHz_HzPERSEGMENT   100

//structure declarations corresponding to manufacturing
enum CycleStatus
{
	statNothing,
	statCompleted,
	statCycling,
	statStopped,
};

struct CycleParam
{
	UINT32 totalCycleTime;//in miliseconds
	UINT32 sonicsOnTimePerWeldCycle;////in miliseconds
	UINT32 sonicsOffTimePerWeldCycle;////in miliseconds
	UINT16 Amplitude;//in percentage.
	BOOLEAN stopOnAlarm;
	BOOLEAN MfTest;
	UINT32 CRC;
};

struct CycleResults
{
	UINT32 nCycles;
	UINT32 currCycleTime;
	UINT32 sonicsOnTime;
	UINT32 nAlarms;
	UINT32 remainingCycleTime;
	CycleStatus cycleStatus;
	SINT32 resultantFrequency;
	UINT32 resultantPower;
};

//Data to be saved in I2C chip 24AA025E48
struct EEPROMData
{
	UINT32 PSfrequency;
	UINT32 PSpower;
	UINT8 SerialNo[SERAILNUMBER_LEN];
	BOOLEAN EnergyPowerSpecial;
	UINT32 checkSum;
};

#endif   /* TYP_DEF_H_ */


