/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TYP_DEF.h_v   1.10   06 Jun 2016 16:08:20   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TYP_DEF.h_v  $
 * 
 *    Rev 1.10   06 Jun 2016 16:08:20   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.9   30 Oct 2015 08:53:48   sesharma
 * Added flag HandHeldMode to SoftwareFlag
 * 
 *    Rev 1.8   01 Sep 2015 02:38:10   amaurya
 * Added ClrMemAtPwrUp and SetDigTuneWithScan to struch Flag.
 * 
 *    Rev 1.7   25 Jun 2015 07:07:16   amaurya
 * Added Time-Maintained Mode.
 * 
 *    Rev 1.6   04 Jun 2015 07:51:30   sesharma
 * Unused enum and structure removed.Branson coding standard formatter applied.
 * 
 *    Rev 1.5   09 Apr 2015 05:30:32   sesharma
 * Impulse Start removed from Preset, HzPERSEGMENT for 30Khz frequency changed to 76.
 * 
 *    Rev 1.4   19 Mar 2015 07:09:48   sesharma
 * Branson Coding Standard Applied and define for FREQ30KHz_HzPERSEGMENT changed from 76 to 75.
 * 
 *    Rev 1.3   07 Nov 2014 16:53:10   ewomack
 * Added SIGSpecial to EEPROM.
 * 
 *    Rev 1.2   16 Oct 2014 03:49:18   sesharma
 * Tracker Issues Resolved
 * 
 *    Rev 1.1   17 Sep 2014 09:03:00   amaurya
 * Added power limit parameter
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:06   amaurya
 * Initial revision.
 */

#ifndef TYP_DEF_H_
#define TYP_DEF_H_

#include "portable.h"
#include "DEF_DEF.H"

enum ABS_LIMIT {
   ABS_LIMIT_OFF
};

enum CYCLE_ABORT {
   CYCLE_ABORT_OFF
};

enum ENERGY_LIMIT {
   ENERGY_LIMIT_OFF
};

enum FREQ_LIMIT {
   FREQ_LIMIT_OFF
};

enum HARDWARE_CONFIG {
   HARDWARE_CONFIG_OFF
};

enum PRESET_STATUS {
   NOTVERIFIED
};

enum MODES {
   CONTINUOUSMODE = 0,
   TIMEMODE,//Time impulse for rack std
   ENERGYMODE,
   PKPOWERMODE,
   GRDDETECTMODE,
   ABSOLUTEMODE,
   TECMODE,
   TIMEMAINTAINEDMODE,//Time maintained for rack std
   ALLMODES
};
enum FREQUENCY {
   FREQ_20KHZ = 20, FREQ_30KHZ = 30, FREQ_40KHZ = 40,
};
enum POWER {
   P400W = 400,
   P750W = 750,
   P800W = 800,
   P1250W = 1250,
   P1500W = 1500,
   P2500W = 2500,
   P4000W = 4000
};

enum JP2 {
   JP3_23FREQ = 0, JP3_12AMP
};

typedef struct DPSPreset   //0x0080
{
      SINT32 AbAmp;
      SINT32 AbDelay;
      BOOLEAN AbFlag;
      SINT32 AbTime;
      SINT32 WeldAmplitude;   //int32 Amplitude1;
      SINT32 Amplitude2;
      SINT32 WeldAmplitudeLoopC1;   //int AmplitudeLoopC1;
      SINT32 WeldAmplitudeLoopC2;   //int AmplitudeLoopC2;
      SINT32 WeldAmplitudeLoopC1New;   //int AmplitudeLoopC1;
      SINT32 WeldAmplitudeLoopC2New;   //int AmplitudeLoopC2;
      SINT32 AmpStartRamp;
      SINT32 AmpStepCol;
      SINT32 AmpStepEnergy;
      BOOLEAN AmpStepExt;
      SINT32 AmpStepPower;
      SINT32 ScrubAmp;
      SINT32 ColMLR;
      SINT32 ColMLS;
      SINT32 ColPLR;
      SINT32 ColPLS;
      UINT32 ControlLimits;
      SINT32 CreatedDate;
      SINT32 CreatedHour;
      SINT32 CreatedMinute;
      SINT32 CreatedMonth;
      SINT32 CreatedYear;
      enum CYCLE_ABORT CycleAborts;
      SINT32 EBAmp;
      BOOLEAN EBFlag;
      SINT32 EBTime;
      UINT32 EnergyCP;
      SINT32 EnergyComp;
      BOOLEAN EnergyCompFlag;
      SINT32 EnergyCutoff;
      BOOLEAN EnergyCutoffFlag;
      enum ENERGY_LIMIT EnergyLimitFlags;
      SINT32 EnergyMLR;
      SINT32 EnergyMLS;
      SINT32 EnergyPLR;
      SINT32 EnergyPLS;
      BOOLEAN ExtTrigDelay;
      enum FREQ_LIMIT FreqLimitFlags;
      SINT32 FreqMLR;
      SINT32 FreqMLS;
      SINT32 FreqPLR;
      SINT32 FreqPLS;
      SINT32 FreqOffset;
      BOOLEAN FreqOffsetFlag;
      BOOLEAN GeneralAlarmFlag;
      BOOLEAN GrdDetectCP;
      enum HARDWARE_CONFIG HardwareConfig;
      SINT32 HoldTime;
      BOOLEAN HoldTimeFlag;
      UINT16 MaxWeldTime;
      BOOLEAN MemoryStoreFlag;
      SINT32 MemoryFreq;
      SINT32 ModifiedDate;
      SINT32 ModifiedHour;
      SINT32 ModifiedMinute;
      SINT32 ModifiedMonth;
      SINT32 ModifiedYear;
      UINT8 PeakPowerCP;
      SINT32 PeakPowerCutoff;
      BOOLEAN PeakPowerCutoffFlag;
      SINT32 WeldPhaseLimit;   //	SINT32	PhaseLimit;
      SINT32 WeldPhaseLimitNew;
      SINT32 WeldPhaseLimitTime;   //	SINT32	PhaseLimitTime;
      SINT32 WeldPhaseLimitTimeNew;
      SINT32 WeldPhaseloopI;   //	SINT32	PhaseLoop;
      SINT32 WeldPhaseloopCF;   //	SINT32	PhaseLoopCF;
      BOOLEAN UseTrueValues;
      SINT32 WeldPhaseloopINew;   //	SINT32	Phase Integral Gain ;
      SINT32 WeldPhaseloopPNew;   //	SINT32  Phase Proportional Gain
      UINT16 FreqWindowSize;
      SINT32 WeldPhaseloopCFNew;   //	SINT32	PF of PCL ;
      UINT32 PowerMLR;
      UINT32 PowerMLS;
      UINT32 PowerPLR;
      UINT32 PowerPLS;
      UINT8 PowerWindowLimitLow;
      UINT8 PowerWindowLimitHigh;
      UINT16 PresetID[12];
      enum PRESET_STATUS PresetStatus;
      /* DIAG GUI*/
      UINT16 ScrubTime;
      BOOLEAN ScrubTimeFlag;
      SINT32 SeekAmplitude;
      SINT32 SeekAmplitudeLoopC1;
      SINT32 SeekAmplitudeLoopC2;
      SINT32 SeekFrequLimitHigh;
      SINT32 SeekFrequLimitLow;
      SINT32 SeekPhaseloopI;
      SINT32 SeekPhaseloopCF;
      SINT32 SeekRampTime;
      SINT32 SeekTime;
      SINT32 SeekFreqOffset;
      UINT8 SIG;
      SINT32 TestAmp;
      BOOLEAN TimedSeekFlag;
      SINT32 DigitalTune;
      SINT32 WeldCount;
      SINT32 WeldFrequencyLimitHigh;
      SINT32 WeldFrequencyLimitHighNew;
      SINT32 WeldFrequencyLimitLow;
      SINT32 WeldFrequencyLimitLowNew;
      SINT32 WeldRampTime;
      BOOLEAN WeldStatusFlag;
      SINT32 WeldTime;
      enum MODES WeldMode;
      SINT32 WeldTimeMLR;
      SINT32 WeldTimeMLS;
      SINT32 WeldTimePLR;
      SINT32 WeldTimePLS;
      UINT16 WriteInField1[12];
      UINT16 WriteInField2[12];
      union {		//Size of this union is 6bytes.
            UINT32 SoftwareFlag;
            struct {
					UINT32 unused :23;
					UINT16 HandHeldMode :1;		// Option to Switch between Time Pulse and Handheld mode
					UINT16 ClearMemAtRst :1;
					UINT16 EndOfWeldStore :1;
					UINT16 ExtAmplitude :1;
					UINT16 ExtFrequency :1;
					UINT16 AutoSonics :1;
					UINT16 TimedSeek :1;
					UINT16 ClrMemAtPwrUp :1;	// Option to clear memory at power up
					UINT16 PowerLimitOn :1;
					UINT16 PlusLimitCheck :1;
					UINT16 MinusLimitCheck :1;
					UINT16 PowerUpOptions :2;   //0 Off, 1-Seek ,2-Scan,3-Seek/Auto sonics
					UINT16 ClrMemBfrSeek :1;   // 0 - Memory value not cleared before seek, 1- Memory value is cleared before seek
					UINT16 SetDigTuneWithScan :1;  // Option to set Digital tune value to parallel resonance frequency obtained from Horn Scan.
            } Flags;
      } SWDip;
      BOOLEAN AlarmResetReq;
      UINT8 CutOffs;
      UINT8 Limits;
      UINT8 SystemType;
      enum JP2 Jumper2;
      UINT32 CRC;
} Parameter;

typedef struct HornScanParams {
      SINT32 FrequencyStart;
      SINT32 FrequencyStop;
      SINT32 FrequencyStep;
      SINT32 TimeDelay;
      UINT16 MaxAmplitude;
      UINT16 MaxCurrent;
      UINT32 CRC;
} ScanParameter;

typedef struct HornScanGraphPointData {
      SINT32 Frequency;
      SINT16 PWMValue;
      SINT16 Amplitude;
      SINT16 Current;
      SINT16 Power;
      SINT16 Phase;
      SINT16 Status;
} ScanData;

typedef struct DPSStatus {
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
} Status;

typedef struct HornWeldPointData {
      SINT16 Time;
      UINT16 Frequency;
      SINT16 PWMValue;
      SINT16 NominalAmplitude;
      SINT16 ActualAmplitude;
      SINT16 Current;
      SINT16 Power;
      SINT16 Phase;
      BOOLEAN logValid;
} WeldData;

struct SystemCounters {
      UINT32 CycleCounter;   // total Cycle counter
      UINT32 ErrorCounter;   // total Error cycle counter
      UINT32 PowerOnSecond;
      UINT32 PowerOnMilliseconds;    //Total Milliseconds of power on.
      UINT32 SecondsOfSonic;         //Total seconds of sonic run.
      UINT32 MillisecondOfSonic;     //Total Millisecond of sonic run.
      UINT32 CRC;
};

/* The values for Scan Point status are in the following enumeration. */
enum HORNScanPointStatus {
   HornScanPointValid = 0, HornScanPointLast = 1, HornScanPointNotValid = 2
};

/* Resonance Frequency Types*/
enum RESONANCE_FREQ_TYPE {
   FREQ_NONE, FREQ_SERIES, FREQ_PARALLEL
};

enum DUPS_FREQCONVERSION /*The appropriate units are as follows:*/
{
   DUPS_15KHZ = 3, /*  15Khz Units = (3 * 0.0025) 0.075hz/bit*/
   DUPS_20KHZ = 4, /*  20Khz Units = (4 * 0.025 ) 0.100hz/bit*/
   DUPS_30KHZ = 6, /*  30Khz Units = (6 * 0.025 ) 0.150hz/bit*/
   DUPS_40KHZ = 8 /*  40hz  Units = (8 * 0.025 ) 0.200hz/bit*/
};

//The system currently in during TEST from website
enum CurrentState {
   stateError, stateWait, stateSeek, stateRun, stateScan,
};

#define FREQ20KHz_START          19450	//19475
#define FREQ20KHz_HzPERSEGMENT   50

#define FREQ30KHz_START          29250	//29278
#define FREQ30KHz_HzPERSEGMENT   76

#define FREQ40KHz_START          38900	//38950
#define FREQ40KHz_HzPERSEGMENT   100

//structure declarations corresponding to manufacturing
enum CycleStatus {
statNothing, statCompleted, statCycling, statStopped,
};

struct CycleParam {
   UINT32 totalCycleTime;     //in miliseconds
   UINT32 sonicsOnTimePerWeldCycle;     ////in miliseconds
   UINT32 sonicsOffTimePerWeldCycle;     ////in miliseconds
   UINT16 Amplitude;     //in percentage.
   BOOLEAN stopOnAlarm;
   BOOLEAN MfTest;
   UINT32 CRC;
};

struct CycleResults {
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
struct EEPROMData {
   UINT32 PSfrequency;
   UINT32 PSpower;
   UINT8 SerialNo[SERAILNUMBER_LEN];
   BOOLEAN EnergyPowerSpecial;
   UINT8 EDP;
   UINT32 Systemtype;
   UINT32 checkSum;
   BOOLEAN SIGSpecial;
};

#endif   /* TYP_DEF_H_ */

