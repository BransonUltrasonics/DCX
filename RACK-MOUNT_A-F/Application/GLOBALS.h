/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/GLOBALS.h_v   1.3   06 Nov 2015 07:03:50   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/GLOBALS.h_v  $
 * 
 *    Rev 1.3   06 Nov 2015 07:03:50   sesharma
 * extern LoadPresetToDCP flag required in Website.cpp.
 * 
 *    Rev 1.2   24 Jul 2015 08:36:06   sesharma
 * Added declaration for function pointers used for common binary changes.
 * 
 *    Rev 1.1   24 Jun 2015 08:36:28   amaurya
 * Added System type BITS
 * 
 *    Rev 1.0   09 Jun 2015 09:10:32   tayars
 * Initial revision.
 * 
 *    Rev 1.41   24 Mar 2014 14:06:32   rjamloki
 * Removed redundant ram buffers.
 * 
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "UserIO.h"
#include "ModBusSendTask.h"
#include "WebSite.h"
#include "StateMachineDrv.h"
#include "preset.h"
#include "LOG.h"
#include "RTClock.h"
#include "WeldResultHistory.h"
#include "portable.h"
#include "preset.h"
#include "FRAM.h"

#ifndef GLOBALS_H_
#define GLOBALS_H_

#define SCANFREQFACTOR     10

#define FLASHCODE_PROGARM 1
#define FLASHTAR_PROGRAM  2
#define IsSet(A, B)		  (A & BIT(B))
#define REDLED_STATE    1
#define GREENLED_STATE  2

#define N 4	/*for 4bits shifting the frequency values coming from DCP as DCP calculations are
			done with 4 bit shifting of frequency to produce more resolution.*/
#define SCALE_UP_FOR_N_BITS(x) ((x) << N)
#define SCALE_DOWN_FOR_N_BITS(x) ((x) >> N)

#define DCP_NUMRESONANCES     6     /* Number of Horn Scan Resonance Freqs detected */
#define HS_NUMCOEFS           17    /* Coff required for filter to smooth out the scan phase data */

#define AMP1 FALSE // used as function argument.
#define AMP2 TRUE // used as function argument.
#define NAAMP 200//Define for a value never expected in amplitude field. Need to be filled to show NA on web pages
#define FRAMINITSTRLEN		100
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
#define NOTUSED					 0
#define DCXA 					 BIT1
#define DCXF					 BIT2
#define ALLENABLE				 (DCXA | DCXF)

enum ErrorType{
	//DCP Error
	NO_ERROR = 0,
	WELDERROR = 0x01,      //0
	TEMP_OVERLOAD = 0x02,
	POWER_OVERLOAD = 0x04,
	VOLTAGE_OVERLOAD = 0x08,
	CURRENT_OVERLOAD = 0x10,  //4
	DDSFREQ_LOWSEEKLIMIT  = 0x20,
	DDSFREQ_HIGHSEEKLIMIT = 0x40,
	DDSFREQ_LOWWELDLIMIT  = 0x80,
	DDSFREQ_HIGHWELDLIMIT = 0x0100,
	PHASELIMIT_TIMEERROR  = 0x0200,
	PEAKPOWER_CUTOFF = 0x0400,
	ENERGY_CUTOFF = 0x0800,
	TIME_CUTOFF = 0x1000,
    SCAN_TIMEOUT = 0x2000,
    MULTIPLE_OVERLOAD_OR_AC_LOST = 0x4000,
    FREQHIGH_CUTOFF   =       0x8000,
    FREQLOW_CUTOFF    =       0x10000,
	//Wc Error
//    LIMIT_ENERGY_LOW = 0x2000,
//    LIMIT_ENERGY_HIGH = 0x4000,
//    LIMIT_PEAKPOWER_LOW = 0x8000,
//    LIMIT_PEAKPOWER_HIGH = 0x10000,
//    LIMIT_TIME_LOW = 0x20000,
//    LIMIT_TIME_HIGH = 0x40000,

};

/////////////////////////state machine/////////////////
extern BOOLEAN ScanStarted,ScanAborted,ScanFailed;
extern BOOLEAN ScanDone;
extern UINT16 WeldBufferTime;
extern SINT32 MemoryValue;
extern SINT32 BoardTestFrequency;
extern UINT8  BoardTestOutput;
extern UINT32 TimedSeekTimerTicks;
extern UINT32 JsonWeldBuffTime;
/************ struct **********/
extern WeldData * WeldBuffer;
extern ScanData * ScanBuffer;
extern SINT16 WeldBufferIndex;

//new preset
extern AdvData * AdvanceConfigData;

extern BOOLEAN UpdateRPreset;
extern BOOLEAN LoadPresetToDCP;
extern PassCodeConfiguration * PasscodeConfig;

/*********** FRAM ************/
extern UINT16 AnalogInputs[NUM_AIN];
extern UINT32 DigitalInputs;

extern BOOLEAN PresetVerifiedRequired;
extern UINT32  Midband;
extern UINT32   MinFrequency;
extern UINT32   MaxFrequency;
extern SINT32  MaxOffset;
extern SINT16  NumScanPoints;
extern USERIOConfiguration * USERIOConfig;
extern USERIOConfiguration CustomDefaultUSERIOConfig;
extern BOOLEAN MemoryReset;
extern BOOLEAN SeekAlarmFlag;
extern RESONANCE_FREQ_TYPE ResonanceFreqType[DCP_NUMRESONANCES];
extern ModBusRTSendData RtSendData;
extern ModBusRTReceiveData RtReceiveData;
extern BOOLEAN WebIOEnabled;
extern SINT32 MainParallelResonanceFreq;

//manufacturing
extern CycleParam MinCycleParam;
extern CycleParam MaxCycleParam;
extern CycleParam * CurrentCycleParam;
extern CycleResults * MFcycleResults;
extern SystemCounters DCPCounter;
extern BOOLEAN NoInput;
extern AlarmConfig * Alarmconfig;
extern BOOLEAN PowerupCommunicationDone, ModBusErrorFlag;
extern BOOLEAN DigitalInputConfigured;
//
//Weld result history information object
extern WeldResultFRAM * WeldResFram;

extern DcxPreset * ParameterSet;
extern HSPRESET  * HsParameterSet;
extern DcxPreset RunningPreset;
extern HSPRESET  RunningHSPreset;
extern DCPPreset DcpParameterSet;
extern BOOLEAN   UpdateRPreset;
extern SINT32    PresetVerifyError;

extern SINT32 MinPSNum;
extern SINT32 MaxPSNum;
extern SINT32 MinHSPSNum;
extern SINT32 MaxHSPSNum;
extern MACCHIPRWData MacchipData;

extern HSSTATUS HsStatusSet[MAX_HSSTATUS_SET + 1][MAX_HS_FUNC];

extern TimeStamp SendRtc;
extern CycleParam TempCurrentCycleParam;
extern UINT32 SonicsOnCounter;
extern UINT32 SonicsOffCounter;
extern UINT32 TempSonicsOnTimePerWeldCycle;
extern struct HornSeekPointData SeekData;
extern BOOLEAN FirmwareFileSent;
extern SINT8 WCBootLoaderVersionLabel[BOOTSWVERSION_LEN];
extern UINT32 WCBootLoaderCRC;
extern BOOLEAN LCDDetect;
extern SINT8  FRAMInitDone[FRAMINITSTRLEN];
extern SINT32 InputTemperature;
extern DCXFUNC DCXFunc;
extern AlarmLogData * AlarmPtr;
extern CommIfc CommIfcAccessToken;
extern OperatingMode OpMode;
extern UINT16 CompressedWebPageCRC;
extern UINT8 AbortReason, CableDetectedPin;
extern SINT32 ResonanceFreqs[DCP_NUMRESONANCES];
extern UINT32 LastCmd, ControlVariable;
extern BOOLEAN ActuatorPresentAtPwrUp;
extern NrtQ NRTcmd;
extern SINT16  CurrentIndex;
extern BOOLEAN OverLoadAlarm;
extern UINT32 DCPTimeSyncCounter;
extern TestPoints TPoints;
extern TestPoints TPointsRam;

extern unsigned int CrashValid;
extern char TaskName[TASKNAMELEN];
extern unsigned int CrashBuffer[4131];
extern unsigned int CurrStackPtr;
extern unsigned int Stackbegin;
extern unsigned int StackEnd;
extern unsigned int CrashBufferRam[4131];
extern unsigned int CrashBuffPrintLen;
extern unsigned int MMUAR;
extern DEBUGSection * DebugSection;
extern char __TAR_START[];
extern char __TAR_SIZE[];
extern int __FRAM_START;
extern char __BOOTLOADER_SIZE[];
extern WCBOARD BoardType;

//Board specific functions
extern void (* InitExternalWatchDogTrigger)(void);
extern void (* ConfigDigitalOutputLatch)(void);
extern void (* ConfigureLCDRst)(void);
extern void (* SendResetToLCD)(void);
extern void (* ConfigureResetPHYandHilscher)(void);
extern void (* SendResetToPHYandHilscher)(void);
extern void (* InitBDMPins)(void);
extern void (* SetReset)(int);
extern void (* SetBkgd)(int);
extern int  (* GetBkgd)(void);
extern void (* SetBkgdOut)(void);
extern void (* SetBkgdIn)(void);
#endif
