/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/GlobalPreset.h_v   1.1   24 Aug 2013 17:14:02   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/GlobalPreset.h_v  $
 * 
 *    Rev 1.1   24 Aug 2013 17:14:02   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.0   05 Nov 2012 09:06:10   amaurya
 * Initial revision.
 *
 */


#ifndef GLOBALPRESET_H_
#define GLOBALPRESET_H_

#include "Preset.h"


enum FORCESTART_RAMP
{
   FORCESTART_RAMP_OFF
};

enum HOLDFORCE_RAMP
{
   HOLDFORCE_RAMP_OFF
};

enum RETURN_FORCE
{
   RETURN_FORCE_OFF
};

enum STEPFORCE_RAMP
{
   STEPFORCE_RAMP_OFF
};

enum WELDFORCE_RAMP
{
   WELDFORCE_RAMP_OFF
};

enum FORCESTEP_COND
{
	FORCESTEP_COND_OFF
};
//Common preset definition

struct ACT
{				// OPTIONAL
	SINT32		AbsCutoffDist;		// Absolute cutoff distance
	BOOLEAN		AbsCutoffFlag;		// Absolute cutoff flag
	SINT32		AbsDistCP;		// Absolute mode control value
	SINT32		ActClearDist;		// Actuator clear distance
	BOOLEAN		ActClearFlag;		// Indicates actuator has returned to a certain point
	SINT32		ColCutoffDist;		// Collapse distance at which ultrasonics will be shut off
	BOOLEAN		ColCutoffFlag;		// Indicates if collapse cutoff should be used
	SINT32		ColDistCP;		// Collapse mode control value
	UINT16		Downspeed1;		// Initial down speed
	UINT16		Downspeed2;		// Down speed after rapid traverse distance is reached
	UINT16		Force1;			// Initial weld force
	UINT16		Force2;			// Weld force after force step
	enum FORCESTART_RAMP ForceStartRamp;	// Ramp to change from down speed to initial force
	UINT16		GaugePressure;		// Pressure setting for aef actuator
	UINT16		HoldForce;		// Hold force for an aef actuator
	enum HOLDFORCE_RAMP	HoldForceRamp;	// Ramp to change from weld force to hold force
	SINT32		MissingPartMin;	// The minimum distance before trigger is allowed
	SINT32		MissingPartMax;	// The maximum distance allowed for trigger
	BOOLEAN		MissingPartFlag;	// Indicates if missing part function is enabled
	BOOLEAN		MissingPartMinFlag;	// Indicates if missing minimum option is enabled
	BOOLEAN		MissingPartMaxFlag;	// Indicates if missing maximum option is enabled
	enum RETURN_FORCE	ReturnForceRamp;	// Ramp to change from weld force to horn return
	UINT16		SetPressure;		// Set pressure on aef actuator
	UINT16		SetRTComp;		// Set rapid traverse comp?
	enum STEPFORCE_RAMP	StepForceRamp;	// Ramp between force 1 and force 2
	UINT16		TriggerDelay;		// Indicates ultrasonics will be delayed after trigger occurs
	BOOLEAN		TriggerDelayFlag;	// Indicates trigger delay is enabled
	UINT16		TriggerForce;		// Force needed before trigger will occur
	UINT16		TriggerTimer;		// Time needed before trigger occurs regardless or TRS
	UINT16		WeldForce;		// Required weld force for aef actuator
	enum WELDFORCE_RAMP	WeldForceRamp;	// Ramp to go from down speed to weld force
	UINT8		ZZSpare[28];		// some spare locations
};

struct DCP
{				// REQUIRED
	SINT32		AmplitudeLoopC1;	// Amplitude control loop variable C1
	SINT32		AmplitudeLoopC2;	// Amplitude control loop variable C2
	SINT32		WeldAmplitudeLoopC1New;//New AmplitudeLoopC1;
	SINT32		WeldAmplitudeLoopC2New;//new AmplitudeLoopC2;
//	UINT16		AmpStartRamp;		// Amplitude start ramp for welding
	BOOLEAN		MemoryStoreFlag;	// Indicates if the frequency should be stored at end of weld
	SINT32		PhaseLimit;		// Phase limit control loop variable
	SINT32		PhaseLimitTime;	// Phase time control loop variable
	SINT32		PhaseLimitNew;		// Phase limit control loop variable
	SINT32		PhaseLimitTimeNew;	// Phase time control loop variable
	SINT32		PhaseLoop;		// Phase control loop variable
	SINT32		PhaseLoopCF;		// Phase loop CF variable
	UINT16	    FreqWindowSize;
	BOOLEAN     UseTrueValues;
	SINT32      WeldPhaseloopCFNew;//SINT32	PF of PCL ;
	SINT32  	WeldPhaseloopINew;//	Phase Integral Gain ;
	SINT32  	WeldPhaseloopPNew;//	Phase Proportional Gain
	BOOLEAN		TimedSeekFlag;		// Indicates timed seek is enabled
	SINT32		WeldFreqHigh;		// Frequency control variable
	SINT32		WeldFreqLow;		// Frequency control variable
	SINT32		WeldFreqHighNew;		// Frequency control variable
	SINT32		WeldFreqLowNew;		// Frequency control variable
	UINT16		WeldRampTime;	// Ramp time for weld
	BOOLEAN		WeldStatusFlag;	// Indicates additional digital information from DUPS
	UINT8		ZZSpare[48];		// some spare locations
};

struct INFO
{				// REQUIRED
	UINT8		CreatedDate;		// Date preset was created or cold started
	UINT8		CreatedHour;		// Hour preset was created or cold started
	UINT8		CreatedMinute;		// Minute preset was created or cold started
	UINT8		CreatedMonth;		// Month preset was created or cold started
	UINT8		CreatedYear;		// Year preset was created or cold started
	UINT32		HardwareConfig;	// Indicates the configuration of hardware when preset saved
	UINT16		HornPresetNumber;	// Horn preset/horn number associated with weld preset
	UINT8		ModifedDate;		// Date preset was last saved
	UINT8		ModifiedHour;		// Hour preset was last saved
	UINT8		ModifiedMinute;	// Minute preset was last saved
	UINT8		ModifiedMonth;	// Month preset was last saved
	UINT8		ModifiedYear;		// Year preset was last saved
	UINT32		OperatorID;		// Operator ID who saved preset
	enum FREQUENCY PowerSupplyFreq;	// Power supply frequency
	UINT8		PowerSupplySN[12];	// Serial number of power supply preset was created on
	UINT16		PowerSupplyWatts;	// Power supply rating in watts
	UINT16		PresetID[12];		// Ascii or unicode string for name of preset
	enum PRESET_STATUS	PresetStatus;		// Indicates Verified, Modified or Saved status of preset
	UINT32		WeldCount;		// Number of cycles run or number of cycles requested
	BOOLEAN		WeldCountFlag;		// Indicates if counting weld cycles up or down
	UINT16		WriteInField1[12];	// User write in field 1 – ascii or unicode
	UINT16		WriteInField2[12];	// User write in field 2 – ascii or unicode
	UINT8		ZZSpare[42];		// some spare locations
};

struct LIMITS {				// OPTIONAL
	SINT32		AbsMLR;		// Absolute minus reject limit
	SINT32		AbsMLS;		// Absolute minus suspect limit
	SINT32		AbsPLR;			// Absolute plus reject limit
	SINT32		AbsPLS;			// Absolute plus suspect limit
	SINT32		ColMLR;		// Collapse minus reject limit
	SINT32		ColMLS;		// Collapse minus suspect limit
	SINT32		ColPLR;			// Collapse plus reject limit
	SINT32		ColPLS;			// Collapse plus suspect limit
	UINT16		ControlLimits;		// Globally enable/disable of control limits
	UINT32		EnergyMLR;		// Energy minus reject limit
	UINT32		EnergyMLS;		// Energy minus suspect limit
	UINT32		EnergyPLR;		// Energy plus reject limit
	UINT32		EnergyPLS;		// Energy plus suspect limit
	UINT16		FreqMLR;		// Frequency minus reject limit
	UINT16		FreqMLS;		// Frequency minus suspect limit
	UINT16		FreqPLR;		// Frequency plus reject limit
	UINT16		FreqPLS;		// Frequency plus suspect limit
	UINT16		PowerMLR;		// Power minus reject limit
	UINT16		PowerMLS;		// Power minus suspect limit
	UINT16		PowerPLR;		// Power plus reject limit
	UINT16		PowerPLS;		// Power plus suspect limit
	UINT32		RejectLimitFlags;	// Bits to indicate which reject limits are enabled
	UINT32		SuspectLimitFlags;	// Bits to indicate which suspect limits are enabled
	SINT32		TrsDistMLR;		//Trigger distance minus reject limit
	SINT32		TrsDistMLS;		// Trigger distance minus suspect limit
	SINT32		TrsDistPLR;		// Trigger distance plus reject limit
	SINT32		TrsDistPLS;		// Trigger distance plus suspect limit
	UINT16		WeldForceMLR;		// Weld force minus reject limit
	UINT16		WeldForceMLS;		// Weld force minus suspect limit
	UINT16		WeldForcePLR;		// Weld force plus reject limit
	UINT16		WeldForcePLS;		// Weld force plus suspect limit
	UINT16		WeldTimeMLR;		// Weld time minus reject limit
	UINT16		WeldTimeMLS;		// Weld time minus suspect limit
	UINT16		WeldTimePLR;		// Weld time plus reject limit
	UINT16		WeldTimePLS;		// Weld time plus suspect limit
	UINT8		ZZSpare[20];		// some spare location
};

struct PMC {				// OPTIONAL
	UINT32		PMCCycleCount;	// For PMC for 2000X
	UINT8		PMCData[402];		// For PMC for 2000X
	UINT32		PMCFlags;		// Indicates when PMC function is enabled
	UINT16		PMCHighLimit;		// For PMC for 2000X
	UINT16		PMCLowLimit;		// For PMC for 2000X
	UINT16		PMCTimeout;		// For PMC for 2000X
	UINT8		ZZSpare[98];		// spme spare locations
};

struct SBLVQS
{				// OPTIONAL
	UINT16		SBLEnableMax;		// SBL for 2000X
	UINT16		SBLEnableMin;		// SBL for 2000X
	UINT32		SBLFreqFlags;		// SBL for 2000X
	UINT16		SBLFreqMax;		// SBL for 2000X
	UINT16		SBLFreqMin;		// SBL for 2000X
	UINT16		SBLFreqMaxLimit;	// SBL for 2000X
	UINT16		SBLFreqMinLimit;	// SBL for 2000X
	UINT16		VQSCycleTimeMax;	// 2000X
	UINT16		VQSCycleTimeMin;	// 2000X
	UINT32		VQSEnergyMax;		// 2000X
	UINT32		VQSEnergyMin;		// 2000X
	UINT32		VQSFlags;		// 2000X
	UINT16		VQSPowerMax;		// 2000X
	UINT16		VQSPowerMin;		// 2000X
	SINT32		VQSTotalAbsMax;	// 2000X
	SINT32		VQSTotalAbsMin;	// 2000X
	SINT32		VQSTotalColMax;	// 2000X
	SINT32		VQSTotalColMin;	// 2000X
	SINT32		VQSWeldColMax;	// 2000X
	SINT32		VQSWeldColMin;	// 2000X
	UINT16		VQSWeldForceMax;	// 2000X
	UINT16		VQSWeldForceMin;	// 2000X
	UINT16		VQSWeldTimeMax;	// 2000X
	UINT16		VQSWeldTimeMin;	// 2000X
	UINT16		ZZSpare[30];		// some spare locations
};

struct WC
{				// REQUIRED
	UINT16		ABAmp;		// After burst amplitude
	UINT16		ABDelay;		// After burst delay
	BOOLEAN		ABFlag;			// After burst flag
	UINT16		ABTime;		//After burst time
	UINT16		Amplitude1;		// Amplitude 1 of 2
	UINT16		Amplitude2;		// Amplitude 2 of 2
	enum AMPSTEP_COND	AmpStepCondition;	// Trigger to cause amplitude to switch to Amplitude 2
	SINT32		AmpStepAbs;		// Absolute distance at which amplitude will switch to Amplitude 2
	SINT32		AmpStepCol;		//Collapse distance at which amplitude will switch to Amplitude 2
	UINT32		AmpStepEnergy;	// Energy value at which amplitude will switch to Amplitude 2
	BOOLEAN		AmpStepExternal;	// Digital input which will switch between Amplitude 1 & Amplitude 2
	UINT16		AmpStepForce;		// Force value at which amplitude will switch to Amplitude 2
	UINT16		AmpStepPower;	// Power value at which amplitude will switch to Amplitude 2
	enum CYCLE_ABORT	CycleAborts;		// Enable / disable flags for each cycle abort
	UINT16		EBAmp;			// Energy braking target amplitude
	BOOLEAN		EBFlag;			// Indicates if energy braking function is enabled
	UINT16		EBTime;		// Energy braking time
	UINT32		EnergyCP;		// Energy mode control parameter
	UINT32		EnergyComp;		// Energy compensation value
	BOOLEAN		EnergyCompFlag;	// Indicates if energy compensation is enabled
	UINT32		EnergyCutoff;		// Energy cutoff value
	BOOLEAN		EnergyCutoffFlag;	// Indicates if energy cutoff is enabled
	BOOLEAN		ExtTrigDelayFlag;	// Indicates when external trigger delay is enabled
	UINT16		ExtTrigDelayTime;	// Delay time when external trigger delay is enabled
	SINT32		FreqOffset;		// Frequency offset when frequency offset is enabled
	BOOLEAN		FreqOffsetFlag;		// Indicates if frequency offset is enabled
	enum FORCESTEP_COND	ForceStepCondition;	// Trigger to cause force to switch to Force 2
	SINT32		ForceStepAbs;		// Absolute distance at which force will switch to Force 2
	SINT32		ForceStepCol;		// Collapse distance at which force will switch to Force 2
	UINT32		ForceStepEnergy;	// Energy value at which force will switch to Force 2
	BOOLEAN		ForceStepExt;		// Digital input which will switch between Force 1 & Force 2
	UINT16		ForceStepAmp;		// Amplitude value at which force will switch to Force 2
	UINT16		ForceStepPower;	// Power value at which force will switch to Force 2
	BOOLEAN		GeneralAlarmFlag;	// Indicates if reset is required to clear an alarm
	BOOLEAN		GrdDetectFlagCP;	// Indicates when ground detect mode is enabled
	UINT16		HoldTime;		// Ultrasonics off time with SV active
	BOOLEAN		HoldTimeFlag;		// Indicates if hold time function is enabled
	UINT16		MaxWeldTime;		// Maximum time for modes other than time
	UINT16		PeakPowerCP;		// Peak power control parameter
	UINT16		PeakPowerCutoff;	// Peak power cutoff value
	BOOLEAN		PeakPowerCutoffFlag;	// Indicates when peak power cutoff is enabled
	UINT16		PMCCycleCount;	// For PMC for 2000X
	BOOLEAN		PostWeldSeekFlag;	// Indicates when a post weld seek is required
	UINT16		PreTrigAmp;		// Pre-trigger amplitude value
	UINT16		PreTrigDelay;		// Delay time after ULS before pre-trigger starts
	SINT32		PreTrigDist;		// Distance after ULS when pre-trigger starts
	BOOLEAN		PreTrigDistFlag;		// Indicates when pre-trigger distance is enabled
	UINT16		ScrubAmp;		// Amplitude for scrub time on ground detect mode
	UINT16		ScrubTime;		// Time to remain in scrub mode
	BOOLEAN		ScrubTimeFlag;		// Indicates when scrub time is enabled
	enum WELD_MODE	WeldMode;		// Indicates which weld modes are enabled
	UINT16		WeldTimeCP;		// Weld time for time mode
	//Added-
	BOOLEAN		CutOffEnabled;
	BOOLEAN		PreTriggerEnabled;

	UINT8		ZZSpare[94];		// some spare locations
};

typedef struct preset {
	ACT 		ActParam;
	DCP			DcpParam;
	INFO 		InfoParam;
	LIMITS 		LimitsParam;
	WC 			WcParam;
	UINT32		CRCAct;			// CRC for actuator structure – 0 indicates structure not used
	UINT32		CRCDcp;			// CRC for DCP structure
	UINT32		CRCInfo;		// CRC for Information structure
	UINT32		CRCLimits;		// CRC for limits structure – 0 indicates structure not used
	UINT32		CRCPmc;			// CRC for PMC structure – 0 indicates structure not used
	UINT32		CRCSblVqs;		// CRC for SBL-VQS – 0 indicates structure not used
	UINT32		CRCWC;			// CRC for WC structure
	UINT32		ZZSpare[9];		// some spare locations
} PRESET;
#endif /* GLOBALPRESET_H_ */
