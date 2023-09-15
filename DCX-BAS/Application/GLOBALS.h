/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/GLOBALS.h_v   1.42   Feb 09 2017 08:58:56   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/GLOBALS.h_v  $
 * 
 *    Rev 1.42   Feb 09 2017 08:58:56   EGUAJARDO
 * Changes for the AC line restore sequence.
 * 
 *    Rev 1.41   24 Mar 2014 14:06:32   rjamloki
 * Removed redundant ram buffers.
 * 
 *    Rev 1.40   13 Mar 2014 15:54:38   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.39   06 Nov 2013 06:20:30   rjamloki
 * Going to preready on user input configuration change to prevent sonics.
 * 
 *    Rev 1.38   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.37   20 Sep 2013 07:00:12   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.36   03 Sep 2013 12:39:26   ygupta
 * Moved to tip from 1.34.1.0
 * 
 *    Rev 1.34.1.0   24 Aug 2013 17:14:02   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.34   02 Aug 2013 10:55:46   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.32   10 Jul 2013 07:50:28   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.31   27 Jun 2013 05:55:02   amaurya
 * System crash fixes.
 * 
 *    Rev 1.30   05 Jun 2013 03:09:44   amaurya
 * Fixed Passcode and Limits issue.
 * 
 *    Rev 1.29   24 May 2013 10:15:46   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.28   21 May 2013 12:25:52   amaurya
 * Code review fixes.
 * 
 *    Rev 1.27   07 May 2013 10:12:52   amaurya
 * Code review fixed.
 * 
 *    Rev 1.26   06 May 2013 09:16:06   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.25   30 Apr 2013 11:19:28   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.24   22 Apr 2013 11:30:14   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.23   15 Mar 2013 10:49:16   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.22   11 Mar 2013 02:25:34   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.21   18 Feb 2013 03:06:32   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.20   29 Jan 2013 02:19:36   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.19   23 Jan 2013 06:57:02   ygupta
 * Profibus weld result bug fixed
 * 
 *    Rev 1.18   22 Jan 2013 10:38:18   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.17   16 Jan 2013 06:26:06   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.16   11 Jan 2013 02:51:02   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.15   27 Dec 2012 06:43:40   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.14   18 Dec 2012 07:37:24   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.13   14 Dec 2012 05:26:12   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.12   13 Dec 2012 00:09:32   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.11   29 Nov 2012 08:44:28   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.10   24 Nov 2012 08:00:58   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.9   05 Nov 2012 08:56:22   amaurya
 * State Machine changes
 * 
 *    Rev 1.8   29 Oct 2012 02:54:42   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.7   26 Oct 2012 02:35:10   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.5   21 Sep 2012 01:31:28   rjamloki
 * Added State machine
 * 
 *    Rev 1.4   29 Aug 2012 13:27:50   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.3   05 Jul 2012 04:45:36   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.2   02 Jul 2012 13:04:08   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.1   29 Jun 2012 12:14:08   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.0   21 Mar 2012 12:20:44   PDwivedi
 * Initial revision.
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
extern UINT32 SendCmdAfterACLineRestore;
extern BOOLEAN ACLineResetFlag;
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
#endif
