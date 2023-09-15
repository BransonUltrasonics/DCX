/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/TYP_DEF.h_v   1.1   24 Jul 2015 08:38:22   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/TYP_DEF.h_v  $
 * 
 *    Rev 1.1   24 Jul 2015 08:38:22   sesharma
 * Added enum WCBOARD.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.20   28 May 2013 07:44:06   amaurya
 * Fixed issues of Level II testing.
 * 
 */
#ifndef TYP_DEF_H_
#define TYP_DEF_H_

#include "portable.h"
#include "DEF_DEF.H"
#include "SysConfig.h"
#include "ModBusSendTask.h"

typedef struct HornScanGraphPointData
{
   SINT32  Frequency;
   SINT16  PWMValue;
   SINT16  Amplitude;
   SINT16  Current;
   SINT16  Power;
   SINT16  Phase;
   SINT16  Index;
}ScanData;

struct HornSeekPointData
{
   SINT32  SeekStartFrequencyAbs;
   SINT32  SeekStopFrequencyAbs;
};

typedef struct HornWeldPointData
{
   SINT16  Time;
   SINT32  Frequency;
   SINT16  PWMValue;
   SINT16  NominalAmplitude;
   SINT16  ActualAmplitude;
   SINT16  Current;
   SINT16  Power;
   SINT16  Phase;
   BOOLEAN logValid;
}WeldData;

/* The values for Scan Point status are in the following enumeration. */
enum HORNScanPointStatus
{
	HornScanPointValid    = 0,
	HornScanPointLast     = 1,
	HornScanPointNotValid = 2
};

/* Resonance Frequency Types*/
enum RESONANCE_FREQ_TYPE
{
	FREQ_NONE,
	FREQ_SERIES,
	FREQ_PARALLEL
};

//enum DUPS_FREQCONVERSION   /*The appropriate units are as follows:*/
//{
//   DUPS_15KHZ  =  3,   /*  15Khz Units = (3 * 0.0025) 0.075hz/bit*/
//   DUPS_20KHZ  =  4,   /*  20Khz Units = (4 * 0.025 ) 0.100hz/bit*/
//   DUPS_30KHZ  =  6,   /*  30Khz Units = (6 * 0.025 ) 0.150hz/bit*/
//   DUPS_40KHZ  =  8    /*  40hz  Units = (8 * 0.025 ) 0.200hz/bit*/
//};

//The system currently in which state
//require for live pages in website
enum CurrentState
{
	stateError,
	stateWait,
	stateSeek,
	stateRun,
	stateScan,
};

//Data to be saved in MAC chip RW area
struct MACCHIPRWData
{
   FREQUENCY Freq;			// to be set from manufacturing web page.
   UINT32 Power;			// to be set from manufacturing web page.
   SYSTEMTYPE CurrSystemType; // to be set from manufacturing web page.
   UINT8  SerialNo[SERIALNUMBER_LEN];// to be set from manufacturing web page.
   UINT32 CheckSum;//checksum of the data saved in chip
};

struct CycleParam
{
   UINT32  TotalCycleTime;	//in miliseconds
   UINT32  SonicsOnTimePerWeldCycle;	////in miliseconds
   UINT32  SonicsOffTimePerWeldCycle;	////in miliseconds
   UINT16  Amplitude;	//in percentage.
   BOOLEAN StopOnAlarm;
   BOOLEAN MfCycling;
   UINT32  CRC;
};

//structure declarations corresponding to manufacturing
enum CycleStatus
{
   STATENOTHING,
   STATECOMPLETED,
   STATECYCLING,
   STATESTOPPED,
};

struct CycleResults
{
   UINT32 CyclesCount;
   UINT32 CurrCycleTime;
   UINT32 SonicsOnTime;
   UINT32 AlarmsCount;
   UINT32 RemainingCycleTime;
   CycleStatus Cyclestatus;
   SINT32 ResultantFrequency;
   UINT32 ResultantPower;
};

//for website compilation --needs cleanup
enum FrequencyOffsetType
{
	InternalOffset,
	ExternalOffset
};

//The system currently in which state
//require for live pages in website
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

extern MACCHIPRWData MacchipData;

struct SystemCounters
{
	UINT32 CycleCounter;// total Cycle counter
	UINT32 ErrorCounter;// total Error cycle counter
	UINT32 PowerOnSecond;
	UINT32 PowerOnMilliseconds;    //Total Milliseconds of power on.
	UINT32 SecondsOfSonic;         //Total seconds of sonic run.
	UINT32 MillisecondOfSonic;     //Total Millisecond of sonic run.
	UINT32 CRC;
};
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
struct PSSpecificatin
{
	FREQUENCY freq;
	POWER pwr;
	FrequencyPowerOption type;
};
enum Freq_Mode
{
	Delaytime,
	AutomaticSwitch
};
enum SWITCH_MODE
{
	NoRF,
	RFviaProfibus,
	RFviaDigitalIO
};

enum Test_Status
{
   Runing,
   Passed,
   Failed
};
typedef struct
{
	struct AdvRnd                   // for Advanced RnD webpage
	{
		UINT32 DCPTimerRate;
		UINT32 DCPStateMachineTime;
		UINT32 DCPControlLoopTime;
		UINT32 DCPModBusTime;
		UINT32 DCPADCResolution;
		UINT32 DCPBlindTime;
		UINT32 Gp2Value1;
		UINT32 Gp2Value2;
		UINT32 Gp2Value3;
		UINT32 Gp2Value4;
		UINT32 Gp2Value5;
		UINT32 Gp2Value6;
		BOOLEAN RevDigitalOP;
		BOOLEAN Flag2;
		BOOLEAN Flag3;
		BOOLEAN Flag4;
		BOOLEAN Flag5;
		BOOLEAN Flag6;
	}Advrnd;
 }AdvData;



 struct DCXFUNC{
    BOOLEAN  WeldStart;
    BOOLEAN  CycleStart;
    BOOLEAN  TestStart;
    BOOLEAN  SeekStart;
    BOOLEAN  ScanStart;
    BOOLEAN  MFCycleStart;  //
 };



//Available Communication Interfaces
enum CommIfc
{
   COMM_IFC_NO_IFC,  //No interface
   //Communication Interfaces which can't fetch the token
   COMM_IFC_WEB,
   COMM_IFC_LCD,
   COMM_IFC_RES,//Touch Screen
   COMM_IFC_USERIO,
   //Communication Interfaces which can fetch the token
   COMM_IFC_FBB,     //field bus
   COMM_IFC_PB = 5,//PB = 5 for data logging purpose
   COMM_IFC_ETHIP = 6,//ETHIP = 6 for data logging purpose
   COMM_IFC_DCXMANAGER //dcx manager
};

//Preset update required for LCD and Filedbus table in case of preset change from special preset page.
enum PSUPDATESPECIAL
{
	UPDATENOSPECIALPS = 0,
	UPDATEMINPS = BIT(0),
	UPDATEMAXPS = BIT(1),
	UPDATETWEAKMINPS = BIT(2),
	UPDATETWEAKMAXPS = BIT(3),
	UPDATEDEFAULTPS = BIT(4),
	UPDATEMIN = BIT(5),
	UPDATEMAX = BIT(6),
	UPDATEMINMAX = (UPDATEMIN | UPDATEMAX),
};

//Operating mode
enum OperatingMode
{
   NOMODE, //mode set at power up
   MANUAL, //horn and weld functions are allowed from other interfaces
   AUTO,   //horn and weld functions are only allowed from fieldbus
};

// WC board type
enum WCBOARD
{
	WC_NONE,
	WC_991_BOARD,
	WC_1169_BOARD,
	SC_1054_BOARD,
};
#endif   /* TYP_DEF_H_ */
