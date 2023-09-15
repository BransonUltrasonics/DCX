/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FRAM.cpp_v   1.9   06 Apr 2016 15:10:50   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FRAM.cpp_v  $
 * 
 *    Rev 1.9   06 Apr 2016 15:10:50   hasanchez
 * Improvements on the Function ConvertV4FRAM_to_V5FRAM
 * 
 *    Rev 1.8   06 Apr 2016 09:53:06   hasanchez
 * Fix to avoid lock the memory
 * 
 *    Rev 1.7   04 Apr 2016 13:21:42   hasanchez
 * updated function to improve the header, release memory, and variable types.
 * 
 *    Rev 1.5   21 Mar 2016 16:16:54   hasanchez
 * Added the function to update the result version 4 to result version 5. (added Temperature to Weld history) 
 * 
 *    Rev 1.4   03 Feb 2016 00:56:56   ygupta
 * ConvertV3FRAM_to_V4FRAM() modified to store Default value for recalled preset parameter; Global assignments handled for FRAM_VERSION 4
 * 
 *    Rev 1.3   28 Jan 2016 01:52:48   ygupta
 * ConvertV2FRAM_to_V3FRAM() modified to store Default unit for power results; Global assignments handled for FRAM_VERSION 3
 * 
 *    Rev 1.2   21 Aug 2015 05:50:40   RJamloki
 * Fix For coverity CID 11341
 * 
 *    Rev 1.1   10 Aug 2015 09:40:18   RJamloki
 * Upgrade routine changes for FRAM version 2
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.31   08 Aug 2014 11:24:26   rjamloki
 * Fixed FRAM pointer initialization at compile time. Removed InitFRAMPointers function.
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
#define TWO_BYTES 2
#define ONE_BYTE 1
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

#if FRAM_VERSION == 1 || FRAM_VERSION == 2 || FRAM_VERSION == 3 || FRAM_VERSION == 4 || FRAM_VERSION == 5
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

#if FRAM_VERSION == 1 || FRAM_VERSION == 2 || FRAM_VERSION == 3 || FRAM_VERSION == 4 || FRAM_VERSION == 5
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
	delete[] SdramPtr;//Free sdram //coverity CID:11341
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
	Sysconfig->ClearMemoryAtPowerUp = FALSE;
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
	Sysconfig->PowerResUnit = PowerInWatts;
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
	Sysconfig->RecalledPreset = CurrPreset;
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 3, 4);
}


/*  void ConvertV4FRAM_to_V5FRAM(void)
 *
 *  Purpose:
 *    Convert Ver4 FRAM to Version 5 FRAM.
 *    Adding the temperature to the weld history.
 *
 *  Entry condition:
 *    The old FRAM has Ver 4
 *
 *  Exit condition:
 *    Fram has been updated to new version and Temperature has been set to zero for all existing records.
 */
void ConvertV4FRAM_to_V5FRAM(void)
{
	UINT8 * SdramPtr = new UINT8[FRAM_SIZE_APP];
	SINT32 Indx;
	UINT32 SizeOfFirstBlock;
	memcpy(SdramPtr, (UINT8*)V1_WELDRES_START, V1_WELDRES_SIZE);//Copy FRAM to SDRAM
	memset((UINT8*)(V1_WELDRES_START), 0, V1_WELDRES_SIZE);//set all zeroes in FRAM

	memcpy(&WeldResFram->Weldresultinfo, SdramPtr , sizeof(WeldResultInfo));


	SizeOfFirstBlock = sizeof(WeldResult) - TWO_BYTES ; // The size of the old results
	//SdramPtr+=sizeof(WeldResultInfo);   SdramPtr+sizeof(WeldResultInfo)

	for(Indx = 0; Indx < MAX_WELD_RESULT_ENTRIES; Indx++)
	{
		memcpy(&WeldResFram->WeldResultObjs[Indx], (SdramPtr+sizeof(WeldResultInfo)) + (SizeOfFirstBlock * Indx) , SizeOfFirstBlock);  // the full all results anyway the last 4 values will be re-write
		WeldResFram->WeldResultObjs[Indx].Temperature=0; // As on the previous version there was not temperature, it is set to zero.
		memcpy(&WeldResFram->WeldResultObjs[Indx].WeldMode, (SdramPtr+sizeof(WeldResultInfo)) + (SizeOfFirstBlock * Indx)  + SizeOfFirstBlock-TWO_BYTES  , 1); //The location were this value was on the old revision
		memcpy(&WeldResFram->WeldResultObjs[Indx].LogValid, (SdramPtr+sizeof(WeldResultInfo)) + (SizeOfFirstBlock * Indx)  + SizeOfFirstBlock-ONE_BYTE  , 1); //The location were this value was on the old revision
	}
	Log::WriteEventLog(EVENT_FRAM_REFORMATTED, 4, 5);
	delete[] SdramPtr;
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

