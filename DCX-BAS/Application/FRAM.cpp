/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/FRAM.cpp_v   1.34   02 Sep 2016 11:57:50   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/FRAM.cpp_v  $
 * 
 *    Rev 1.34   02 Sep 2016 11:57:50   EGUAJARDO
 * Added function to convert V4FRAM to V5FRAM.
 * 
 *    Rev 1.33   30 Jul 2015 08:08:48   RJamloki
 * Added initial value for SetDigTuneWithHornScan in conversion routine
 * 
 *    Rev 1.32   29 Jul 2015 07:53:42   RJamloki
 * Upgrade routine changes for FRAM version 2.
 * 
 *    Rev 1.31   08 Aug 2014 11:24:26   rjamloki
 * Fixed FRAM pointer initialization at compile time. Removed InitFRAMPointers function.
 * 
 *    Rev 1.30   24 Mar 2014 14:15:04   rjamloki
 * Corrected Diagnostic buffers copying.
 * 
 *    Rev 1.29   21 Mar 2014 09:23:44   rjamloki
 * Tracker issue fixes.
 * 
 *    Rev 1.28   13 Mar 2014 15:54:24   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.27   04 Mar 2014 07:49:22   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.26   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.25   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.24   24 Sep 2013 06:28:40   rjamloki
 * Watch dog removed.
 * 
 *    Rev 1.23   03 Sep 2013 12:37:52   ygupta
 * Moved to tip from 1.21.1.0
 * 
 *    Rev 1.21.1.0   24 Aug 2013 17:14:00   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.21   02 Aug 2013 10:55:44   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.19   08 Jul 2013 08:52:56   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.18   27 Jun 2013 05:54:50   amaurya
 * System crash fixes.
 * 
 *    Rev 1.17   14 Jun 2013 11:21:08   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.16   11 Jun 2013 09:49:16   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.15   31 May 2013 10:37:14   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.14   15 Mar 2013 10:48:40   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.13   11 Mar 2013 02:25:12   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.12   11 Jan 2013 02:50:36   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.11   18 Dec 2012 07:37:00   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.10   14 Dec 2012 05:26:00   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.9   24 Nov 2012 08:00:46   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.8   05 Nov 2012 08:55:52   amaurya
 * State Machine changes
 * 
 *    Rev 1.7   26 Oct 2012 02:34:32   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.6   21 Sep 2012 01:30:46   rjamloki
 * Added State machine
 * 
 *    Rev 1.5   29 Aug 2012 13:27:48   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.4   05 Jul 2012 04:45:12   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.3   02 Jul 2012 13:04:08   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.2   29 Jun 2012 12:13:34   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.1   21 Jun 2012 12:33:00   ygupta
 * Event and alarm Object Added
 * 
 *    Rev 1.0   21 Mar 2012 12:20:32   PDwivedi
 * Initial revision.
 *
 */

#include "DEF_DEF.h"
#include "TYP_DEF.h"
#include "SysConfig.h"
#include "LOG.h"
#include "UserIO.h"
#include "WeldResultHistory.h"
#include "preset.h"
#include "RTClock.h"
#include "PasscodeConfig.h"
#include "EthernetIP.h"
#include "FRAM.h"
#include "FUNC.h"
#include "GLOBALS.h"


//Fram version
FRAM_INFO * FramInfo = (FRAM_INFO *) FRAM_START_ADDRESS;
#if FRAM_VERSION < 1//just for reference to 6.0. Can be removed
//This was 6.0 Official release
UINT8 dummyPlaceHolder[1024] __attribute__((section (".FRAM")));

SINT8 WCSoftwareVersionLabel[SWVERSION_LEN] __attribute__((section (".FRAM")));
UINT8 dummyPlaceHolder1[40] __attribute__((section (".FRAM")));

//global system Configuration data stored in FRAM
SystemConfiguration Sysconfig __attribute__((section(".FRAM")));
UINT8 dummyPlaceHolder2[1024] __attribute__((section (".FRAM")));

AlarmConfig Alarmconfig __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder6[1024] __attribute__((section (".FRAM")));

//USERIO configuration information
USERIOConfiguration  USERIOConfig __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder9[1024] __attribute__((section (".FRAM")));

//The global passcode configuration saved in FRAM
PassCodeConfiguration PasscodeConfig __attribute__((section(".FRAM")));
UINT8 dummyPlaceHolder10[1024 - 80] __attribute__((section (".FRAM")));

//parameter Set and Horn specific parameter set objects in FRAM
DcxPreset ParameterSet[MAX_TOTAL_PRESETS]__attribute__((section (".FRAM")));
//changed ampstep energy size from 2 to 4
UINT8 dummyPlaceHolder11[2048 - (MAX_TOTAL_PRESETS * 4)] __attribute__((section (".FRAM")));

HSPRESET HsParameterSet[MAX_TOTAL_HSPRESETS]__attribute__((section (".FRAM")));
UINT8 dummyPlaceHolder12[2048] __attribute__((section (".FRAM")));

//cycle run
CycleParam CurrentCycleParam __attribute__((section (".FRAM")));
UINT8 dummyPlaceHolder13[1024] __attribute__((section (".FRAM")));

CycleResults MFcycleResults __attribute__((section (".FRAM")));
UINT8 dummyPlaceHolder14[1024] __attribute__((section (".FRAM")));

//counters
//SystemCounters DCPCounter __attribute__((section (".FRAM")));
//UINT8 dummyPlaceHolder15[12] __attribute__((section (".FRAM")));

CounterSt Counter  __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder16[1024] __attribute__((section (".FRAM")));

AdvData AdvanceConfigData __attribute__((section (".FRAM")));
UINT8 dummyPlaceHolder17[1024] __attribute__((section (".FRAM")));

//Weld result history information object
WeldResultInfo Weldresultinfo __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder7[1024] __attribute__((section (".FRAM")));

//Array of Event objects.
EventLog Log::EventLogObjs[MAX_EVENT_ENTRIES] __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder3[4000] __attribute__((section (".FRAM")));

//Array of Alarm Event objects.
AlarmLog Log::AlarmLogObjs[MAX_ALARM_ENTRIES] __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder4[1400] __attribute__((section (".FRAM")));

//Log information object
LOGInformation Log::LOGInfo __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder5[1024] __attribute__((section (".FRAM")));

//Array of weld result objects.
WeldResult WeldResultObjs[MAX_WELD_RESULT_ENTRIES] __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder8[924] __attribute__((section (".FRAM")));

//Memory frequencies in absolute format
SINT32 StoredFrequencyAbs[MAX_HSSTATUS_SET + 1] __attribute__ ((section (".FRAM")));
UINT8 dummyPlaceHolder20[1024] __attribute__((section (".FRAM")));

TestPoints TPoints __attribute__((section (".FRAM")));

unsigned int CrashValid __attribute__((section (".FRAM")));
char TaskName[TASKNAMELEN]__attribute__((section (".FRAM")));
unsigned int CrashBuffer[4096 + 35] __attribute__((section (".FRAM")));

unsigned int CurrStackPtr __attribute__((section (".FRAM")));
unsigned int Stackbegin __attribute__((section (".FRAM")));
unsigned int StackEnd __attribute__((section (".FRAM")));
unsigned int CrashBuffPrintLen __attribute__((section (".FRAM")));
unsigned int MMUAR __attribute__((section (".FRAM")));

unsigned char FRAMCrashFBRecvBuffer[64] __attribute__((section (".FRAM")));
unsigned char FRAMCrashMailBoxbuff[1596] __attribute__((section (".FRAM")));
UINT8 FRAMCrashSerialRecBuff[512] __attribute__((section (".FRAM")));
UINT8 FRAMCrashMBRcvPktbuf[MB_UDP_BUF_SIZE] __attribute__((section (".FRAM")));

//Ethernet IP non volatile data should go here. It can be part of System Information but have less time to make all changes and test them
AddrConflictDetection LastAcd __attribute__((section (".FRAM")));
InterfaceSpeedControl IntControl __attribute__((section (".FRAM")));
QOS QosVals __attribute__((section (".FRAM")));

UINT8 dummyPlaceHolderRes[38556 - sizeof(CrashBuffer) - sizeof(CrashValid) - sizeof(TaskName)
                          - sizeof(CurrStackPtr) - sizeof(Stackbegin) - sizeof(StackEnd)
                          - sizeof(CrashBuffPrintLen) - sizeof(MMUAR)
                          - (sizeof(TestPoints) - sizeof(TestPointsOld)) -
                          sizeof(FRAMCrashFBRecvBuffer) - sizeof(FRAMCrashMailBoxbuff)
                          - sizeof(FRAMCrashSerialRecBuff) - sizeof(FRAMCrashMBRcvPktbuf) - sizeof(AddrConflictDetection)
                          - sizeof(InterfaceSpeedControl) - sizeof(QOS)] __attribute__((section (".FRAM")));

SINT8 FRAMInitDone[100] __attribute__ ((section (".FRAM")));
#endif

#if FRAM_VERSION == 1 || FRAM_VERSION == 2 || FRAM_VERSION == 3
//below pointers are assigned addresses based on FRAM version. These global assignments are necessary so that compiler would know the addreeses
//at compile time and the varibles mapped on Fieldbustable point to right addresses.
SystemConfiguration * Sysconfig = (SystemConfiguration *)V1_SYSINFO_START;
CycleResults * MFcycleResults = (CycleResults *)V1_MFCYCRES_START;
AlarmConfig * Alarmconfig = (AlarmConfig *)V1_ALARMCONFIG_START;
USERIOConfiguration * USERIOConfig = (USERIOConfiguration *)V1_USRIOCONFIG_START;
PassCodeConfiguration * PasscodeConfig = (PassCodeConfiguration *)V1_PASSCODE_START;
CycleParam * CurrentCycleParam = (CycleParam *)V1_MFPARA_START;
DcxPreset * ParameterSet = (DcxPreset *)V1_PRESET_START;
HSPRESET * HsParameterSet = (HSPRESET *)V1_HORN_PRESET_START;
AdvData * AdvanceConfigData = (AdvData *)V1_ADVCONFIG_START;
EventLogFRAM * EventLogFram = (EventLogFRAM *)V1_EVENT_START;
AlarmLogFRAM * AlarmLogFram = (AlarmLogFRAM *)V1_ALARM_START;
WeldResultFRAM * WeldResFram = (WeldResultFRAM *)V1_WELDRES_START;
EthIPNVData * EthipnvData = (EthIPNVData *)V1_ETHIP_START;
DEBUGSection * DebugSection = (DEBUGSection *)V1_DEBUG_START;
CounterSt * SysCounters = (CounterSt *)(V1_ALARMCONFIG_START + sizeof(Alarmconfig->AlarmAction) + sizeof(Alarmconfig->Spare));

SEGMENT_INFO Segment[NUM_FRAM_SEGMENTS] = {
	{V1_INFO_START, V1_INFO_SIZE, 0},
	{V1_SYSINFO_START, V1_SYSINFO_SIZE, 0},
	{V1_MFCYCRES_START, V1_MFCYCRES_SIZE, 0},
	{V1_ALARMCONFIG_START, V1_ALARMCONFIG_SIZE, 0},
	{V1_USRIOCONFIG_START, V1_USRIOCONFIG_SIZE, 0},
	{V1_PASSCODE_START, V1_PASSCODE_SIZE, 0},
	{V1_MFPARA_START, V1_MFPARA_SIZE, 0},
	{V1_PRESET_START, V1_PRESET_SIZE, 0},
	{V1_SPARE1_START, V1_SPARE1_SIZE, 0},
	{V1_HORN_PRESET_START, V1_HORN_PRESET_SIZE, 0},
	{V1_ADVCONFIG_START, V1_ADVCONFIG_SIZE, 0},
	{V1_SPARE2_START, V1_SPARE2_SIZE, 0},
	{V1_WELDRES_START, V1_WELDRES_SIZE, 0},
	{V1_EVENT_START, V1_EVENT_SIZE, 0},
	{V1_ALARM_START, V1_ALARM_SIZE, 0},
	{V1_SPARE3_START, V1_SPARE3_SIZE, 0},
	{V1_ETHIP_START, V1_ETHIP_SIZE, 0},
	{V1_DEBUG_START, V1_DEBUG_SIZE, 0},
};
#endif

FramPowerUpHandler ConversionPointers[] = {
		NULL,						// Location 0 not used
		InitFRAM,					// Version 1 should force an init
		ConvertV1FRAM_to_V2FRAM,	// Version 1 to version 2 conversion
		ConvertV2FRAM_to_V3FRAM,	// Version 2 to version 3 conversion
		ConvertV3FRAM_to_V4FRAM,	// Version 3 to version 4 conversion
		ConvertV4FRAM_to_V5FRAM,	// Version 4 to version 5 conversion
};



/*  void CheckFRAMSections(void)
 *
 *  Purpose:
 *    Dummy Function to fail the compilation if FRAM Section boundary exceeds. Purpose is to
 *    1- catch the boundary exceeding conditions at compile time if somebody unknowingly add variable to structures saved in FRAM.
 *    2- let the programmer know that he/she needs to write a conversion routine and change the FRAM version.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void CheckFRAMSections(void){

#if FRAM_VERSION == 1 || FRAM_VERSION == 2 || FRAM_VERSION == 3
	//fail for if condition returns false
	COMPILE_TIME_ASSERT((sizeof(SystemConfiguration)) <=  (V1_SYSINFO_SIZE));
	COMPILE_TIME_ASSERT((sizeof(CycleResults)) <=  (V1_MFCYCRES_SIZE));
	COMPILE_TIME_ASSERT((sizeof(PassCodeConfiguration)) <=  (V1_PASSCODE_SIZE));
	COMPILE_TIME_ASSERT((sizeof(USERIOConfiguration)) <=  (V1_USRIOCONFIG_SIZE));
	COMPILE_TIME_ASSERT((sizeof(CycleParam)) <=  (V1_MFPARA_SIZE));
	COMPILE_TIME_ASSERT((sizeof(DcxPreset) * MAX_TOTAL_PRESETS) <= (V1_PRESET_SIZE));
	COMPILE_TIME_ASSERT((sizeof(EventLogFRAM)) <=  (V1_EVENT_SIZE));
	COMPILE_TIME_ASSERT((sizeof(HSPRESET) * MAX_TOTAL_HSPRESETS) <=  (V1_HORN_PRESET_SIZE));
	COMPILE_TIME_ASSERT((sizeof(AlarmLogFRAM)) <=  (V1_ALARM_SIZE));
	COMPILE_TIME_ASSERT((sizeof(AdvData)) <=  (V1_ADVCONFIG_SIZE));
	COMPILE_TIME_ASSERT((sizeof(WeldResultFRAM)) <=  (V1_WELDRES_SIZE));
	COMPILE_TIME_ASSERT((sizeof(EthIPNVData)) <=  (V1_ETHIP_SIZE));
	COMPILE_TIME_ASSERT((sizeof(AlarmConfig))  <=  (V1_ALARMCONFIG_SIZE));
	COMPILE_TIME_ASSERT((sizeof(DEBUGSection))  <=  (V1_DEBUG_SIZE));
#endif
}


/*  void ConvertVer6RlsFRAM_to_V1FRAM(void)
 *
 *  Purpose:
 *    Convert Ver6.0 release FRAM to Version 1 FRAM.
 *
 *  Entry condition:
 *    The old FRAM has Ver 6.0
 *
 *  Exit condition:
 *    None.
 */
void ConvertVer6RlsFRAM_to_V1FRAM(void){
	SINT32 Indx;
	UINT16 CrcVal;
	UINT8 * SdramPtr = new UINT8[FRAM_SIZE_APP];
	AlarmConfig * OldConfig;
	CounterSt * OldCounters;
	LOGInformationRls6 * OldLogInfo;
	EventLogRls6 * OldEvntLog;
	HspresetRls6 * OldHsPs;
	SINT32 * StoreFreqOld;
	memcpy(SdramPtr, (UINT8*)FRAM_START_APP, FRAM_SIZE_APP);//Copy FRAM to SDRAM
	memset((UINT8*)(FRAM_START_APP), 0, FRAM_SIZE_APP);//set all zeroes in FRAM
	//start copying data from SD ram to current FRAM Structures
	//FRAM Info
	memcpy(FramInfo->WCSoftwareVersionLabel, SdramPtr + (ADDR_WC_SOFT_VERSION - ADDR_VER6RLS_FRAMSTART), SWVERSION_LEN);
	//system info
	memcpy(Sysconfig, SdramPtr + (ADDR_SYSCONFIG - ADDR_VER6RLS_FRAMSTART), sizeof(SystemConfiguration));
	//Manufacturing Cycle Results
	memcpy(MFcycleResults, SdramPtr + (ADDR_MF_CYCLE_RESULTS - ADDR_VER6RLS_FRAMSTART), sizeof(CycleResults));
	//Alarm Configurations
	OldConfig = (AlarmConfig*)(SdramPtr + (ADDR_ALARM_CONFIG - ADDR_VER6RLS_FRAMSTART));
	memcpy(Alarmconfig->AlarmAction, OldConfig->AlarmAction, sizeof(OldConfig->AlarmAction));
	Alarmconfig->Spare = OldConfig->Spare;
	//we have added counters also in this alarm config section
	OldCounters = (CounterSt*)(SdramPtr + (ADDR_COUNTERS - ADDR_VER6RLS_FRAMSTART));
	SysCounters->GeneralAlarmCount = OldCounters->GeneralAlarmCount;
	SysCounters->LifeTimeCycleCount = OldCounters->LifeTimeCycleCount;
	SysCounters->PowerOnHours = OldCounters->PowerOnHours;
	SysCounters->SonicsOnHours = OldCounters->SonicsOnHours;
	//User IO config
	memcpy(USERIOConfig, SdramPtr + (ADDR_USER_IO_CONFIG - ADDR_VER6RLS_FRAMSTART), sizeof(USERIOConfiguration));
	//pass codes
	memcpy(PasscodeConfig, SdramPtr + (ADDR_PASSCODE_CONFIG - ADDR_VER6RLS_FRAMSTART), sizeof(PassCodeConfiguration));
	//Current Cycle Parameter
	memcpy(CurrentCycleParam, SdramPtr + (ADDR_MF_CYCLE_PARAM - ADDR_VER6RLS_FRAMSTART), sizeof(CycleParam));
	//preset copy
	memcpy(ParameterSet, SdramPtr + (ADDR_DCX_PRESET - ADDR_VER6RLS_FRAMSTART), sizeof(DcxPreset) * MAX_TOTAL_PRESETS);
	//Horn Presets
	OldHsPs = (HspresetRls6 *)(SdramPtr + (ADDR_DCX_HSPRESET - ADDR_VER6RLS_FRAMSTART));
	StoreFreqOld = (SINT32 *) (SdramPtr + (ADDR_STORED_FREQ - ADDR_VER6RLS_FRAMSTART));//point to absolute mem frequencies
	for(Indx = 0; Indx < MAX_TOTAL_HSPRESETS; Indx++)
	{
		  memcpy(&HsParameterSet[Indx] , OldHsPs, sizeof(HspresetRls6) - sizeof(OldHsPs->Reserved));
		  HsParameterSet[Indx].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(Midband);
		  HsParameterSet[Indx].MemoryOffsetScaledUP = *StoreFreqOld - HsParameterSet[Indx].DigitalTuneScaledUP;
		  HsParameterSet[Indx].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[Indx]), HSPSCRCDATA_SIZE);
		  OldHsPs++;
		  StoreFreqOld++;
	}
	//Fix min, max , tweak ad current again
	HsParameterSet[CurrHsPreset].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(Midband);
	HsParameterSet[CurrHsPreset].MemoryOffsetScaledUP = HsParameterSet[ParameterSet[CurrPreset].InfoParam.HornPresetNumber].MemoryOffsetScaledUP;
	HsParameterSet[CurrHsPreset].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[CurrHsPreset]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_MIN].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(MinFrequency);
	HsParameterSet[HsPreset_MIN].MemoryOffsetScaledUP = SCALE_UP_FOR_N_BITS(-MaxOffset);
	HsParameterSet[HsPreset_MIN].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_MIN]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_MAX].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(MaxFrequency);
	HsParameterSet[HsPreset_MAX].MemoryOffsetScaledUP = SCALE_UP_FOR_N_BITS(MaxOffset);
	HsParameterSet[HsPreset_MAX].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_MAX]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_TweakMIN].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(MinFrequency);
	HsParameterSet[HsPreset_TweakMIN].MemoryOffsetScaledUP = SCALE_UP_FOR_N_BITS(-MaxOffset);
	HsParameterSet[HsPreset_TweakMIN].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_TweakMIN]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_TweakMAX].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(MaxFrequency);
	HsParameterSet[HsPreset_TweakMAX].MemoryOffsetScaledUP = SCALE_UP_FOR_N_BITS(MaxOffset);
	HsParameterSet[HsPreset_TweakMAX].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_TweakMAX]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_Default].DigitalTuneScaledUP = SCALE_UP_FOR_N_BITS(Midband);
	HsParameterSet[HsPreset_Default].MemoryOffsetScaledUP = SCALE_UP_FOR_N_BITS(0);
	HsParameterSet[HsPreset_Default].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_Default]), HSPSCRCDATA_SIZE);

	//Advance Config
	memcpy(AdvanceConfigData, SdramPtr + (ADDR_ADVCONFIG_DATA - ADDR_VER6RLS_FRAMSTART), sizeof(AdvData));
	//Weld result section
	memcpy(&WeldResFram->Weldresultinfo, SdramPtr + (ADDR_WELDRESULT_INFO - ADDR_VER6RLS_FRAMSTART), sizeof(WeldResultInfo));
	memcpy(WeldResFram->WeldResultObjs, SdramPtr + (ADDR_WELD_RESULTS - ADDR_VER6RLS_FRAMSTART), sizeof(WeldResFram->WeldResultObjs));

	OldEvntLog = (EventLogRls6 *)(SdramPtr + (ADDR_EVENT_LOG - ADDR_VER6RLS_FRAMSTART));
	//event log
	for(Indx = 0; Indx < MAX_EVENT_ENTRIES; Indx++)
	{
		CrcVal = ByteCRC((UINT8*)OldEvntLog, sizeof(EventLogRls6) - sizeof(UINT16));
		if(CrcVal == OldEvntLog->LogChecksum)
		{
			EventLogFram->EventLogObjs[Indx].Data1 = OldEvntLog->Data1;
			EventLogFram->EventLogObjs[Indx].Data2 = OldEvntLog->Data2;
			EventLogFram->EventLogObjs[Indx].EventID = OldEvntLog->EventID;
			EventLogFram->EventLogObjs[Indx].TS = OldEvntLog->TS;
			memcpy(EventLogFram->EventLogObjs[Indx].UserID , OldEvntLog->UserID, sizeof(EventLogFram->EventLogObjs[Indx].UserID));
			//fix the new checksum
			EventLogFram->EventLogObjs[Indx].LogChecksum =
					ByteCRC((UINT8*)&EventLogFram->EventLogObjs[Indx], sizeof(EventLog) - sizeof(UINT16));
		}
		OldEvntLog++;
	}
	OldLogInfo = (LOGInformationRls6 *)(SdramPtr + (ADDR_LOG_INFO - ADDR_VER6RLS_FRAMSTART));

	EventLogFram->CircularIndexEvent = OldLogInfo->CircularIndexEvent;
	EventLogFram->EventsNo = OldLogInfo->EventsNo;
	//Rls 6 was checking 32 bit value with 16 bit checksum????
	if(OldLogInfo->LogInitialized == (UINT16)LOGINITIALIZED_CHECKSUM)
		EventLogFram->LogInitialized = LOGINITIALIZED_CHECKSUM;
	else
		EventLogFram->LogInitialized = OldLogInfo->LogInitialized;

	//Alarm
	AlarmLogRls6 * AlarmLogOld = (AlarmLogRls6 *)(SdramPtr + (ADDR_ALARM_LOG - ADDR_VER6RLS_FRAMSTART));
	for(Indx = 0; Indx < MAX_ALARM_ENTRIES; Indx++)
	{
		CrcVal = ByteCRC((UINT8*)AlarmLogOld, sizeof(AlarmLogRls6) - sizeof(UINT16));
		if(CrcVal == AlarmLogOld->LogChecksum)//Ver 6 has valid alarm entry
		{
			AlarmLogFram->AlarmLogObjs[Indx].AlarmID = AlarmLogOld->AlarmID;
			memcpy(&AlarmLogFram->AlarmLogObjs[Indx].AlarmData , &AlarmLogOld->AlarmData, sizeof(AlarmLogDataRls6));
			AlarmLogFram->AlarmLogObjs[Indx].AlarmData.WeldMode = 0;//Set continuous mode for now.
			AlarmLogFram->AlarmLogObjs[Indx].AlarmData.PresetNum = 0;//Set current preset for now
			//fix the new checksum
			AlarmLogFram->AlarmLogObjs[Indx].LogChecksum =
					ByteCRC((UINT8*)&AlarmLogFram->AlarmLogObjs[Indx], sizeof(AlarmLog) - sizeof(UINT16));
		}
		AlarmLogOld++;
	}
	AlarmLogFram->AlarmsNo = OldLogInfo->AlarmsNo;
	AlarmLogFram->CircularIndexAlarm = OldLogInfo->CircularIndexAlarm;
	//Rls 6 was checking 32 bit value with 16 bit checksum????
	if(OldLogInfo->LogInitialized == (UINT16)LOGINITIALIZED_CHECKSUM)
		AlarmLogFram->LogInitialized = LOGINITIALIZED_CHECKSUM;
	else
		AlarmLogFram->LogInitialized = OldLogInfo->LogInitialized;

	//ether net ip section
	memcpy(&EthipnvData->LastAcd, SdramPtr + (ADDR_LASTACD - ADDR_VER6RLS_FRAMSTART), sizeof(EthipnvData->LastAcd));
	memcpy(&EthipnvData->IntControl, SdramPtr + (ADDR_INTSPEEDCONTROL - ADDR_VER6RLS_FRAMSTART), sizeof(EthipnvData->IntControl));
	memcpy(&EthipnvData->QosVals, SdramPtr + (ADDR_QOSVALS - ADDR_VER6RLS_FRAMSTART), sizeof(EthipnvData->QosVals));

	//debug section copy
	memcpy(&DebugSection->TPoints, SdramPtr + (ADDR_TPOINTS - ADDR_VER6RLS_FRAMSTART), sizeof(TPoints));
	memcpy(&DebugSection->CrashValid, SdramPtr + (ADDR_CRASHVALID - ADDR_VER6RLS_FRAMSTART), sizeof(CrashValid));
	memcpy(DebugSection->TaskName, SdramPtr + (ADDR_CRASHTASK - ADDR_VER6RLS_FRAMSTART), sizeof(TaskName));
	memcpy(DebugSection->CrashBuffer, SdramPtr + (ADDR_CRASHBUFFER - ADDR_VER6RLS_FRAMSTART), sizeof(CrashBuffer));
	memcpy(&DebugSection->CurrStackPtr, SdramPtr + (ADDR_CRASH_CURRSTACKPTR - ADDR_VER6RLS_FRAMSTART), sizeof(CurrStackPtr));
	memcpy(&DebugSection->Stackbegin, SdramPtr + (ADDR_CRASH_CURRSTACKBEGIN - ADDR_VER6RLS_FRAMSTART), sizeof(Stackbegin));
	memcpy(&DebugSection->StackEnd, SdramPtr + (ADDR_CRASH_ENDSTACKPTR - ADDR_VER6RLS_FRAMSTART), sizeof(StackEnd));
	memcpy(&DebugSection->CrashBuffPrintLen, SdramPtr + (ADDR_CRASH_BUFFPRINTLEN - ADDR_VER6RLS_FRAMSTART), sizeof(CrashBuffPrintLen));
	memcpy(&DebugSection->MMUAR, SdramPtr + (ADDR_CRASH_MMUAR - ADDR_VER6RLS_FRAMSTART), sizeof(MMUAR));
	memcpy(DebugSection->FRAMCrashFBRecvBuffer, SdramPtr + (ADDR_CRASH_FBRECVBUFF - ADDR_VER6RLS_FRAMSTART), sizeof(DebugSection->FRAMCrashFBRecvBuffer));
	memcpy(DebugSection->FRAMCrashMailBoxbuff, SdramPtr + (ADDR_CRASH_FBRECVMAILBOX - ADDR_VER6RLS_FRAMSTART), sizeof(DebugSection->FRAMCrashMailBoxbuff));
	memcpy(DebugSection->FRAMCrashSerialRecBuff, SdramPtr + (ADDR_CRASH_SERRECVBUFF - ADDR_VER6RLS_FRAMSTART), sizeof(DebugSection->FRAMCrashSerialRecBuff));
	memcpy(DebugSection->FRAMCrashMBRcvPktbuf, SdramPtr + (ADDR_CRASH_MBRECVBUFF - ADDR_VER6RLS_FRAMSTART), sizeof(DebugSection->FRAMCrashMBRcvPktbuf));
	delete SdramPtr;//Free sdram
}

/*  void ConvertV1FRAM_to_V2FRAM(void)
 *
 *  Purpose:
 *    Convert Ver1 FRAM to Version 2 FRAM.
 *
 *  Entry condition:
 *    The old FRAM has Ver 1
 *
 *  Exit condition:
 *    None.
 */
void ConvertV1FRAM_to_V2FRAM(void)
{
	//Only change between V1 and V2 FRAM is added variables in system configuration structure without affecting the size of
	//any section in FRAM. This routine just put the default values for added parameters.
	Sysconfig->ClearMemoryBeforeSeek = TRUE;
	Sysconfig->ClearMemoryWithReset = FALSE;
	Sysconfig->SetDigTuneWithHornScan = FALSE;
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 1, 2);
}

/*  void ConvertV2FRAM_to_V3FRAM(void)
 *
 *  Purpose:
 *    Convert Ver2 FRAM to Version 3 FRAM.
 *
 *  Entry condition:
 *    The old FRAM has Ver 2
 *
 *  Exit condition:
 *    None.
 */
void ConvertV2FRAM_to_V3FRAM(void)
{
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 2, 3);
}

/*  void ConvertV3FRAM_to_V4FRAM(void)
 *
 *  Purpose:
 *    Convert Ver3 FRAM to Version 4 FRAM.
 *
 *  Entry condition:
 *    The old FRAM has Ver 3
 *
 *  Exit condition:
 *    None.
 */
void ConvertV3FRAM_to_V4FRAM(void)
{
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 3, 4);
}

/*  void ConvertV4FRAM_to_V5FRAM(void)
 *
 *  Purpose:
 *    Convert Ver4 FRAM to Version 5 FRAM.
 *
 *  Entry condition:
 *    The old FRAM has Ver 4
 *
 *  Exit condition:
 *    None.
 */
void ConvertV4FRAM_to_V5FRAM(void)
{

	SINT32 Indx;
	UINT8 * SdramPtr = new UINT8[V1_HORN_PRESET_START];
	HspresetV4 * OldHSPs;


	memcpy(SdramPtr, (UINT8*)V1_HORN_PRESET_START, V1_HORN_PRESET_SIZE);//Copy FRAM to SDRAM
	memset((UINT8*)(V1_HORN_PRESET_START), 0, V1_HORN_PRESET_SIZE);//set zeroes in FRAM
	OldHSPs = (HspresetV4*)(SdramPtr);

	for(Indx = 0; Indx < MAX_TOTAL_HSPRESETS; Indx++)
	{
		memcpy(&HsParameterSet[Indx] , OldHSPs,  sizeof(HspresetV4));
		HsParameterSet[Indx].TestSelection= (TESTAMP_COND)TEST_SELECTION_DEFAULT;
		HsParameterSet[Indx].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[Indx]), HSPSCRCDATA_SIZE);

	}

	HsParameterSet[CurrHsPreset].TestSelection= (TESTAMP_COND)TEST_SELECTION_DEFAULT;
	HsParameterSet[CurrHsPreset].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[CurrHsPreset]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_MIN].TestSelection= (TESTAMP_COND)TEST_SELECTION_MIN;
	HsParameterSet[HsPreset_MIN].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_MIN]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_MAX].TestSelection= (TESTAMP_COND)TEST_SELECTION_MAX;
	HsParameterSet[HsPreset_MAX].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_MAX]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_TweakMIN].TestSelection= (TESTAMP_COND)TEST_SELECTION_MIN;
	HsParameterSet[HsPreset_TweakMIN].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_TweakMIN]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_TweakMAX].TestSelection= (TESTAMP_COND)TEST_SELECTION_MAX;
	HsParameterSet[HsPreset_TweakMAX].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_TweakMAX]), HSPSCRCDATA_SIZE);

	HsParameterSet[HsPreset_Default].TestSelection= (TESTAMP_COND)TEST_SELECTION_DEFAULT;
	HsParameterSet[HsPreset_Default].CRChsp = ByteCRC((UINT8 *)(&HsParameterSet[HsPreset_Default]), HSPSCRCDATA_SIZE);


	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 4, 5);
	delete SdramPtr;//Free sdram
}

/*void InitFRAM(void)
/  Purpose:
*      This function needs to be called to initialize the FRAM at least once when the system is turned on
*      for the very first time.  It also has to be called if the software is being downgraded ( was running
*      version 3 and now ram version 1 has been detected).  Since backwards compatibility is not
*      supported, the FRAM will need to be initialized.  The purpose of initializing FRAM is to:
*      create the FRAM data block which contains:
*      FRAM format number
*      Starting address and length of each segment
*/
void InitFRAM(void){
	//Only exceptional case is that try to restore Release 6.0 FRAM else
	//there will be a straight cold start
	UINT8 * SoftVer = (UINT8 *)ADDR_WC_SOFT_VERSION;
	UINT8 * FramInitDone = (UINT8 *)ADDR_FRAM_INIT_DONE;
	SINT32 Indx;
	if((strcmp((char *)SoftVer, "Ver6.0") == 0) && (strcmp((char *)FramInitDone, "FRAM Init Done!!!!") == 0)){
		ConvertVer6RlsFRAM_to_V1FRAM();
	}
	else
	{
		memset((UINT8 *)(FRAM_START_APP),0, FRAM_SIZE_APP);			//clear fram
		Sysconfig->DcpSpeedControl = SPEEDCONTROL_STANDARD;
		Log::ResetLog();
		ClearAllCounters();
		SetDefaultAlarms();
		SetFBStaticIPDefaults();
		SetDefaultAdvRnd();
		Sysconfig->RFSwitch.RFOutputType = CODED;
		SetColdStartPresetValues();
		SetDefaultPasscodeConfiguration();
		SetDefaultRFswitch();
		SetStaticIPDefaults();
		SetStaticIPDefaultsOnDCPInterface();
		SetDefaultCycleParam();
	}
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 0, 1);
	for(Indx = FRAMINFO; Indx < NUM_FRAM_SEGMENTS; Indx++){
		FramInfo->Seg[Indx].Addr  =  Segment[Indx].Addr;
		FramInfo->Seg[Indx].Size  =  Segment[Indx].Size;
		FramInfo->Seg[Indx].Spare =  Segment[Indx].Spare;
	}
}

/*void FramCheck(void)																		*/
/*	Purpose:
 * 		This function is called from PowerUpInitialization. It checks for the FRAMVersion stored
 * 		in FRAM.
 * 		If the stored FRAMVersion is invalid, InitFRAM will be called.
 * 		If the stored FRAMversion is less than powerup FRAMVersion needed, then there will be software
 * 		upgrade till the required version.
 * 		If the stored FRAMVersion is greater than powerup FRAMVersion needed, then there
 * 		will be software degrade, InitFRAM will be called.
 *********************************************************************************************/
void FramCheck(void)
{
	UINT16 ConversionArrayIndx;
	SINT32 Indx;
	if((FRAM_VERSION == 1) && (FramInfo->FRAMVersion != INIT_VERSION)){ //first time
		ConversionArrayIndx = INIT_FRAM_INDEX;
		ConversionPointers[ConversionArrayIndx]();
	}
	else if(FramInfo->FRAMVersion < FRAM_VERSION){//upgrade
		ConversionArrayIndx = FramInfo->FRAMVersion + 1;
		for(Indx = ConversionArrayIndx; Indx <= FRAM_VERSION; Indx++ )
			ConversionPointers[Indx]();
	}
	else if(FramInfo->FRAMVersion > FRAM_VERSION){//downgrade
		ConversionArrayIndx = INIT_FRAM_INDEX;
		ConversionPointers[ConversionArrayIndx]();
	}

	FramInfo->FRAMVersion = FRAM_VERSION;
}

