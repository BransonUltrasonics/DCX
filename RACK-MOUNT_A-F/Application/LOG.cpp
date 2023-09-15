/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/LOG.cpp_v   1.2   21 Aug 2015 05:51:10   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/LOG.cpp_v  $
 * 
 *    Rev 1.2   21 Aug 2015 05:51:10   RJamloki
 * Fixes for coverity CID 11225, 11224 and 11169
 * 
 *    Rev 1.1   24 Jul 2015 08:36:44   sesharma
 * FRAM Chip select done is moved from CyGOS to Application.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:32   tayars
 * Initial revision.
 * 
 *    Rev 1.42   21 Mar 2014 09:24:16   rjamloki
 * Tracker issue fixes.
 * 
*/

#include "LOG.h"
#include "string.h"
#include "stdio.h"
#include "Board.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "RTClock.h"
#include "WebSite.h"
#include "PasscodeConfig.h"
#include "SysConfig.h"

#define pprintf
#define printf
SINT32 LogData2 = 0;

extern EventLog DCPEventobj[];
extern SINT16 DCPEventLogs, DCPEventIndex;
extern bool FramCsDone;
//Definition of events description along with IDs
EventDescription EventsDescription[] =
{
	//DCP Events
	{ "DCP Power On", EVENT_DCP_POWERON},
	{ "DCP Power On Diagnostc Started", EVENT_DCP_POWERONDIAGNOSTIC_START},
	{ "DCP Power On Diagnostc Passed", EVENT_DCP_POWERONDIAGNOSTIC_PASSED},
	{ "DCP CRC Error", EVENT_DCP_CRC_ERROR},
	{ "DCP Fault Error", EVENT_DCP_FAULT_ERROR},
	{ "DCP Watchdog Reset", EVENT_DCP_WATCHDOG_RESET},
	{ "DCP Version Change", EVENT_DCP_VERSION_CHANGE},
	{ "DCP CRC Change", EVENT_DCP_CRC_CHANGE},
	{ "DCP Clock Lost", EVENT_DCP_CLOCK_LOST},
	{ "DCP Program Firmware", EVENT_DCP_PROGRAM_FIRMWARE},
	{ "DCP Power Shutdown", EVENT_DCP_POWER_SHUTDOWN},
	{ "DCP Lost AC Line Voltage", EVENT_LOSTAC},
	{ "DCP Got AC Line Voltage", EVENT_GOTAC},
	{ "DCP Multiple Overload", EVENT_MULTIPLEOVERLOADS},
	{ "NRT Command not Supported on DCP", EVENT_DCP_NRT_CMD_NOT_SUPPORTED},
	{ "WC DCP Time Sync", EVENT_DCP_TIMESYNC},
	{ "DCP Crash Error", EVENT_DCP_CRASH_ERROR},
	{ "DCP Assert Error", EVENT_DCP_ASSERT_ERROR},
	//WC Events
	{ "Event Log Cleared", EVENT_LOG_CLEARED},
	{ "Entry Checksum Invalid", EVENT_ENTRY_INVALID},
	{ "CRC Error", EVENT_CRC_ERROR},
	{ "Power On", EVENT_POWERON},
	{ "Fieldbus Card Detected", EVENT_HILSCHERCARD_DETECTED},
	{ "Fieldbus Protocol Detected", EVENT_FIELDBUSPROTOCOL_DETECTED},
	{ "Fieldbus Error", EVENT_FIELDBUSERROR},
	{ "Error Read MAC Addr", EVENT_ERROR_READMACADDR},
	{ "Watchdog Reset", EVENT_WATCHDOG_RESET},
	{ "Program Firmware",EVENT_PROGRAMFIRMWARE},
	{ "Preset CRC error",EVENT_PRESETCRC_ERROR},
	{ "User Defined IO", EVENT_USERIO_DEFINED},
	{ "Horn Scan Started",	EVENT_HORN_SCAN_START},
	{ "Horn Scan Completed" ,EVENT_HORN_SCAN_COMPLETED},
	{ "Horn Scan Aborted"  ,EVENT_HORN_SCAN_ABORTED},
	{ "Horn Scan Failed"  ,EVENT_HORN_SCAN_FAILED},
	{ "Preset Recalled" ,EVENT_PRESET_RECALL},
	{ "Power On Diagnostic Started"  ,EVENT_POWERONDIAGNOSTIC_STARTED},
	{ "Power On Diagnostic Passed"   ,EVENT_POWERONDIAGNOSTIC_PASSED},
	{ "LCD Board Detected"  ,EVENT_LCD_BOARD_DETECTED},
	{ "Web Page Login"   ,EVENT_WEBPAGE_LOGIN},
	{ "Web Page Logout"  ,EVENT_WEBPAGE_LOGOUT},
	{ "Test Activated"   ,EVENT_TEST_ACTIVATED},
    { "Test Overload"    ,EVENT_TEST_OVERLOAD},
	{ "Parameter Changed"   ,EVENT_PARAMETER_CHANGED},
	{ "Version Changed"   	,EVENT_VERSION_CHANGED},
	{ "Power On Crc Changed"   ,EVENT_POWERON_CRC_CHANGED},
	{ "Clock Lost"   ,EVENT_CLOCK_LOST},
	{ "Big File Name Get"   ,EVENT_BIG_FILE_NAME_GET},
	{ "Big File Name Post"  ,EVENT_BIG_FILE_NAME_POST},
	{ "Power Shut Down"   	,EVENT_POWER_SHUT_DOWN},
	{ "Invalid Serial Input"    ,EVENT_INVALID_SERIAL_INPUT},
	{ "IO Cable Not Detected"   ,EVENT_IO_CABLE_NOTDETECTED},
	{ "IO Cable Detected"   ,EVENT_IO_CABLE_DETECTED},
	{ "Calibration Pass"   ,EVENT_CALIBRATION_PASS},
	{ "Calibration Fail"   ,EVENT_CALIBRATION_FAIL},
	{ "Board Replaced"   ,EVENT_BOARD_REPLACED},
	{ "System Cleaned"   ,EVENT_SYSTEM_CLEANED},
	{ "Active Low Accepted",	EVENT_ACTIVELOWDIGIN},
    { "FRAM Init Done"         ,EVENT_FRAM_INIT_DONE},
    { "Cold Start", 				EVENT_COLD_START},
    { "Alarm Log Cleared", EVENT_ALARM_LOG_CLEARED},
    { "History Cleared", EVENT_HISTORY_CLEARED},
    { "Restore Defaults", EVENT_RESTORE_DEFAULTS},
    { "Invalid Modbus Register Address", EVENT_MB_ENOREG},
	{ "Field Service Event1", EVENT_FIELD_SERVICE_ONE},
	{ "Field Service Event2", EVENT_FIELD_SERVICE_TWO},
	{ "Field Service Event3", EVENT_FIELD_SERVICE_THREE},
	{ "Field Service Event4", EVENT_FIELD_SERVICE_FOUR},
	{ "RTC Config or Battery Failure", EVENT_RTC_BATTERYFAILURE},
	{ "Verify Preset Error", EVENT_PSVERIFY_FAIL},
	{ "Seek", EVENT_SEEK},
	{ "Fieldbus Software Version", EVENT_FBSLAVECARDVERSION},
	{ "Modbus Communication Failure", EVENT_MODBUSCOMMFAILURE},
	{ "FRAM Reformatted", EVENT_FRAM_REFORMATTED},
	{0, 0},
};

/*   void Log::Init(void)
 *
 *   Purpose : To Initialize the Log module. This function is called from
 *   	PowerupInitialization() function.
 *
 *   Entry Condition
 *	 	None.
 *
 *   Exit Condition
 *		None.
 */
void Log::Init(void)
{
	if((AlarmLogFram->LogInitialized) != LOGINITIALIZED_CHECKSUM){
	   LogData2 = (AlarmLogFram->LogInitialized);
	   ResetLog(CLEARALARM);
		//cleared all entries. Mark the checksum valid
	   AlarmLogFram->LogInitialized = LOGINITIALIZED_CHECKSUM;
	}
	if((EventLogFram->LogInitialized) != LOGINITIALIZED_CHECKSUM){
	   LogData2 = (EventLogFram->LogInitialized);
	   ResetLog(CLEAREVENT);
		//cleared all entries. Mark the checksum valid
	   EventLogFram->LogInitialized = LOGINITIALIZED_CHECKSUM;
	}
	CyGOS::RegisterSysLogCallback(WriteSystemEventLog);
}

/*   void Log::ResetLog(ClearLog Logtype)
 *
 *   Purpose : To clear the event and alarm log data in FRAM.
 *	 This function is called from Init() function of this class and ColdStart() function.
 *
 *   Entry Condition
 *	 	Logtype- to determine whether to clear only alarm entries or event entries or both.
 *
 *   Exit Condition
 *	 	None.
 */
void Log::ResetLog(ClearLog Logtype)
{
   if(Logtype == CLEAREVENT || Logtype == CLEARALL){
      memset(EventLogFram->EventLogObjs, 0, sizeof(EventLogFram->EventLogObjs));  // clear the eventlogObjs
      EventLogFram->CircularIndexEvent = -1;          // That is not pointing to any entry in eventlogObjs array
      Log::WriteEventLog(EVENT_LOG_CLEARED, EventLogFram->EventsNo, LogData2);
      EventLogFram->EventsNo = 1;                     // clear the number representing event log entries made so far.
   }
	if(Logtype == CLEARALARM || Logtype == CLEARALL){
		memset(AlarmLogFram->AlarmLogObjs, 0, sizeof(AlarmLogFram->AlarmLogObjs));  // clear the alarmlogObjs entries in FRAM
		AlarmLogFram->CircularIndexAlarm = -1;				// That is not pointing to any entry in alarmlogObjs array
		Log::WriteEventLog(EVENT_ALARM_LOG_CLEARED, AlarmLogFram->AlarmsNo, LogData2);
		AlarmLogFram->AlarmsNo = 0;							// clear the number representing alarm log entries made so far.
	}
	if(Logtype == CLEARHISTORY || Logtype == CLEARALL){
      memset(WeldResFram->WeldResultObjs, 0, sizeof(WeldResFram->WeldResultObjs));  // clear History
      WeldResFram->Weldresultinfo.CircularIndex = -1;          // That is not pointing to any entry in eventlogObjs array
      Log::WriteEventLog(EVENT_HISTORY_CLEARED, WeldResFram->Weldresultinfo.TotalHistoryCount, LogData2);
      WeldResFram->Weldresultinfo.TotalHistoryCount = 0;
   }
   if(Logtype == CLEARDCP){
   		RtSendData.CmdFlags |= MB_CLEAREVENTLOG;
    	DCPEventLogs = 0;
        memset(DCPEventobj, 0, sizeof(DCPEventobj[MAX_DCP_EVENT_ENTRIES]));
        DCPEventIndex = -1;
   }
	LogData2 = 0;
}

/*   void Log::WriteSystemEventLog(int EventId, short Data1, short Data2, unsigned int UserId, bool TimeStampingRequired)
 *
 *   Purpose : This function is used to Log System Event. This function is called from Log::Init() function.
 *
 *   Entry Condition :
 *    EventId - The unique ID of  an event
 *    Data1  - Diagnostic purpose data1
 *    Data2  – Diagnostic purpose data2
 *    UserId - additional Diagnostic data if parameter value is greater than zero. The reason for this  naming convention is that it has to be displayed in userID column on Log webpage.
 *    TimeStampingRequired- TRUE for WC events, False for DCP events.
 *
 *   Exit Condition
 *    None.
 */
void Log::WriteSystemEventLog(short EventId, int Data1, int Data2, unsigned int UserId, bool TimeStampingRequired)
{
	TimeStampingRequired = FALSE;
	if(FramCsDone)
		Log::WriteEventLog((EventID)EventId, Data1, Data2, UserId);
}

/*   void Log::WriteEventLog(EventID EventId, SINT32 Data1, SINT32 Data2, UINT32 UserId, TimeStamp * TStamp, BOOLEAN TimeStampingRequired)
 *
 *   Purpose :
 *   	To store the event information in eventlogObjects array in FRAM.
 *  	Keep tarck  of the index of eventlogObjects array where event data is to be copied.
 *  	In case index goes out of  array boundary rolls back to the initial index and override the previous entries.
 *  	This function is called from WC application code whenever an event needs to be logged.
 *    	DCP events are written by ModBus task with already calculated timestamp.
 *
 *   Entry Condition :
 *  	EventId - The unique ID of  an event
 *  	Data1  - Diagnostic purpose data1
 *  	Data2  – Diagnostic purpose data2
 *   	TStamp - Time of event occurance. For WC events it contains NULL. For DCP events it contains event occurance time
 *  	UserId - additional Diagnostic data if parameter value is greater than zero. The reason for this  naming convention is that it has to be displayed in userID column on Log webpage.
 *  	TimeStampingRequired- TRUE for WC events, False for DCP events.
 *
 *   Exit Condition
 *   	None.
 */
void Log::WriteEventLog(EventID EventId, SINT32 Data1, SINT32 Data2, UINT32 UserId, TimeStamp * TStamp, BOOLEAN TimeStampingRequired)
{
	RTCTime TM;
	SINT8 * UID;
	EventLogFram->CircularIndexEvent = ((EventLogFram->CircularIndexEvent) + 1) % MAX_EVENT_ENTRIES;// correct index where the event log has to be written
	if(TimeStampingRequired == TRUE){
		RTClock::GetCurrentTime(&TM);	//Get the current time from RTC
		//Fill the time stamp information ts in eventlogObjs array element
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Year   = TM.Years;
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Month  = TM.Century_Month;
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Day    = TM.Day;
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Hour   = TM.Hour;
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Minute = TM.Minute;
		EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS.Second = TM.Second;
	}
	else{
		ASSERT(TStamp != NULL);	//assert if tstamp parameter is NULL.
		//Fill the time stamp information ts in eventlogObjs array element at index LOGInfo.circularIndexEvent from the tstamp parameter..
		memcpy(&EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].TS, TStamp, sizeof(TimeStamp));
	}
//	Assign parameters data1,data2 and eventID to the data1 ,data2 and eventID field of eventlogObjs array element at index LOGInfo.circularIndexEvent.
	EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].Data1   = Data1;
	EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].Data2   = Data2;
	EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].EventID = EventId;
	memset(EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].UserID, 0 , sizeof(EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].UserID));
	if(UserId > 0)
		sprintf(EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].UserID, "%lu" , UserId);	//	Assign userID to userpassCode field in eventlogObjs array element
	else{
		UID = WebSite::GetUserID();
		memcpy(EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].UserID , UID, strlen(UID));
	}
	EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent].LogChecksum = ByteCRC((UINT8 *)&EventLogFram->EventLogObjs[EventLogFram->CircularIndexEvent],
				sizeof(EventLog) - sizeof(UINT16));
	//if(LOGInfo.EventsNo < MAX_EVENT_ENTRIES)
	EventLogFram->EventsNo++;
}

/*   void Log::WriteAlarmLog(Alarms AlarmId, BOOLEAN TimeStampingRequired)
 *
 * 	 Purpose :
 * 		To store the alarm information in alarmlogObjects array in FRAM.
 * 		Keep track of the index of alarmlogObjects array where alarm data is to be copied.
 * 		In case index goes out of  array boundary rolls back to the initial index and override the
 * 		previous entries. This function is called from WC application code whenever an alarm need to be logged.
 *
 *   Entry Condition
 * 	 	AlarmId - The unique ID of alarm
 *		   TimeStampingRequired- To check whether timestamp to be calulated or alredy present in AlarmLogData parameter
 *		   TimeStampingRequired default value is TRUE. All alarms detected by WC(limit, window etc) should have this
 *		   parameter value TRUE. Alarms come from DCP over modbus already have timestamping information in Obj
 *		   parameter. For those alarms(e.g. overload) this parameter should be passed FALSE value.
 *
 *   Exit Condition
 *   	   None.
 */
void Log::WriteAlarmLog(Alarms AlarmId, BOOLEAN TimeStampingRequired)
{
   RTCTime TM;
   UINT16 AlarmType = ALARMTYPE(AlarmId);
   if((Alarmconfig->AlarmAction[AlarmType] & LOG_ALARM) == LOG_ALARM){
      //check if log action is enabled for the given alarm type
	  AlarmLogFram->CircularIndexAlarm = ((AlarmLogFram->CircularIndexAlarm) + 1) % MAX_ALARM_ENTRIES;   // determine the correct index where the alarm log has to be written
      memcpy(&AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData, AlarmPtr, sizeof(AlarmLogData));        // Assign parameter obj
      AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmID = AlarmId; // Assign alarmID to the alarmData

      if(TimeStampingRequired == TRUE){
         RTClock::GetCurrentTime(&TM); // Get the current time from RTC by in RTCTime object tm
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Year   = TM.Years;
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Month  = TM.Century_Month;
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Day    = TM.Day;
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Hour   = TM.Hour;
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Minute = TM.Minute;
         AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.TS.Second = TM.Second;
      }//Else time stamping information is already present in AlarmData.TS for alarm came from
      //DCP over modbus for all the function call from modbus task.. Don't need to do anything.
      AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.ErrorReason = AlarmId;
      AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.PresetNum = Sysconfig->ActivePreset;
      AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].AlarmData.WeldMode = RunningPreset.WcParam.WeldMode;
      AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm].LogChecksum = ByteCRC((UINT8*)&AlarmLogFram->AlarmLogObjs[AlarmLogFram->CircularIndexAlarm],
      sizeof(AlarmLog) - sizeof(UINT16));
      //if(LOGInfo.AlarmsNo < MAX_ALARM_ENTRIES)
      AlarmLogFram->AlarmsNo++;
   }
}

/*  SINT32 Log::ReadEventLog(WebEventData * LogData, SINT32 LogsCount)
 *
 *   Purpose :
 * 	 	To read the number of events passed in LogsCount parameter and copy to the buffer passed in logData parameter.
 * 		It starts filling entries in buffer from the latest occurred event in the decreasing order of their occurrence time.
 *    	This function is called from WebSite module.
 *
 *   Entry Condition :
 * 		LogData- The pointer to the buffer where Event log is to be copied. (It must point to the buffer where maximum
 * 				 possible event entries can be copied)
 * 		LogsCount –  Number of latest event entries to be read in chronological order.(max expected *MAX_EVENT_ENTRIES).
 *
 *   Exit Condition
 * 		Return the actual number of entries read and copied to passed logData buffer.
 */
UINT32 Log::ReadDCPEventLog(WebEventData * LogData, SINT32 LogsCount)
{
   UINT32  TotalEventsRead = 0;
   SINT32  ArrayIndexToReadFrom;
   BOOLEAN ExitFlag = FALSE;
   UINT16 EntryChecksum;

   if(LogsCount  > MAX_DCP_EVENT_ENTRIES)
      LogsCount = MAX_DCP_EVENT_ENTRIES;
   else
      LogsCount = DCPEventLogs;

   ArrayIndexToReadFrom = DCPEventIndex;//GetLatestEventEntryIndex();

   while((LogsCount > 0) && (ExitFlag == FALSE) && (ArrayIndexToReadFrom >= 0)){
      EntryChecksum = ByteCRC((UINT8*)&DCPEventobj[ArrayIndexToReadFrom], sizeof(EventLog) - sizeof(UINT16));
      if(DCPEventobj[ArrayIndexToReadFrom].LogChecksum == EntryChecksum){
         LogData -> Data1 = DCPEventobj[ArrayIndexToReadFrom].Data1;
         LogData -> Data2 = DCPEventobj[ArrayIndexToReadFrom].Data2;
         LogData -> TS = DCPEventobj[ArrayIndexToReadFrom].TS;
		 //coverity CID: 11225
         strncpy(LogData -> EventStr, GetEventStringFromID((EventID)DCPEventobj[ArrayIndexToReadFrom].EventID), MAX_LOG_LENGTH - 1);//coverity CID: 11225
         if(strcmp("Undefined", LogData -> EventStr) == 0)//Check for invalid fram content
         		LogData -> Data1 = DCPEventobj[ArrayIndexToReadFrom].EventID;
         LogData -> EventNo = (DCPEventLogs - TotalEventsRead);//GetEventNumber(ArrayIndexToReadFrom);
         memcpy(LogData -> UserID, DCPEventobj[ArrayIndexToReadFrom].UserID, sizeof(DCPEventobj[ArrayIndexToReadFrom].UserID));
         TotalEventsRead++;
         LogData++;
      }
      else{
         //we have more events occurred in the system but the check sum of those is not correct
         if((UINT32)DCPEventLogs > TotalEventsRead){
            DCPEventobj[ArrayIndexToReadFrom].EventID = EVENT_ENTRY_INVALID;
            memset(&DCPEventobj[ArrayIndexToReadFrom].TS, 0 , sizeof(DCPEventobj[ArrayIndexToReadFrom].TS));
            //make the invalid events to be displayed from next time.
            DCPEventobj[ArrayIndexToReadFrom].LogChecksum = ByteCRC((UINT8*)&DCPEventobj[ArrayIndexToReadFrom] ,
                  sizeof(EventLog) - sizeof(UINT16));
         }//exit because we have read the all events occurred in system and the checksum of
         //of others would obviously be incorrect
         else
            ExitFlag = TRUE;
      }
      LogsCount--;
      ArrayIndexToReadFrom--;
      if(ArrayIndexToReadFrom < 0)
         ArrayIndexToReadFrom = MAX_DCP_EVENT_ENTRIES - 1;
   }
   return TotalEventsRead;
}

UINT32 Log::ReadWCEventLog(WebEventData * LogData, SINT32 LogsCount)
{
	UINT32  TotalEventsRead = 0;
	SINT32  ArrayIndexToReadFrom;
	BOOLEAN ExitFlag = FALSE;
	UINT16 EntryChecksum;

	if(LogsCount  > MAX_EVENT_ENTRIES)
		LogsCount = MAX_EVENT_ENTRIES;

	if(LogsCount  > EventLogFram->EventsNo)
		LogsCount = EventLogFram->EventsNo;

	ArrayIndexToReadFrom = EventLogFram->CircularIndexEvent;//GetLatestEventEntryIndex();

	while((LogsCount > 0) && (ExitFlag == FALSE) && (ArrayIndexToReadFrom >= 0)){
		EntryChecksum = ByteCRC((UINT8*)&EventLogFram->EventLogObjs[ArrayIndexToReadFrom] , sizeof(EventLog) - sizeof(UINT16));
		if(EventLogFram->EventLogObjs[ArrayIndexToReadFrom].LogChecksum == EntryChecksum){
			LogData -> Data1 = EventLogFram->EventLogObjs[ArrayIndexToReadFrom].Data1;
			LogData -> Data2 = EventLogFram->EventLogObjs[ArrayIndexToReadFrom].Data2;
			LogData -> TS = EventLogFram->EventLogObjs[ArrayIndexToReadFrom].TS;
			//coverity CID: 11224
			strncpy(LogData -> EventStr, GetEventStringFromID((EventID)EventLogFram->EventLogObjs[ArrayIndexToReadFrom].EventID), MAX_LOG_LENGTH - 1);
			if(strcmp("Undefined", LogData -> EventStr) == 0)//Check for invalid fram content
				LogData -> Data1 = EventLogFram->EventLogObjs[ArrayIndexToReadFrom].EventID;
			LogData -> EventNo = (EventLogFram->EventsNo - TotalEventsRead);//GetEventNumber(ArrayIndexToReadFrom);
			memcpy(LogData -> UserID, EventLogFram->EventLogObjs[ArrayIndexToReadFrom].UserID, sizeof(EventLogFram->EventLogObjs[ArrayIndexToReadFrom].UserID));
         TotalEventsRead++;
         LogData++;
		}
		else{
			//we have more events occurred in the system but the check sum of those is not correct
			if(EventLogFram->EventsNo > TotalEventsRead){
				EventLogFram->EventLogObjs[ArrayIndexToReadFrom].EventID = EVENT_ENTRY_INVALID;
				memset(&EventLogFram->EventLogObjs[ArrayIndexToReadFrom].TS, 0 , sizeof(EventLogFram->EventLogObjs[ArrayIndexToReadFrom].TS));
				//make the invalid events to be displayed from next time.
				EventLogFram->EventLogObjs[ArrayIndexToReadFrom].LogChecksum = ByteCRC((UINT8*)&EventLogFram->EventLogObjs[ArrayIndexToReadFrom] ,
						sizeof(EventLog) - sizeof(UINT16));
			}//exit because we have read the all events occurred in system and the checksum of
			//of others would obviously be incorrect
			else
				ExitFlag = TRUE;
		}
		LogsCount--;
		ArrayIndexToReadFrom--;
		if (ArrayIndexToReadFrom < 0)
			ArrayIndexToReadFrom = MAX_EVENT_ENTRIES - 1;
	}
	return TotalEventsRead;
}

/*  UINT32 Log::ReadAlarmLog(WebAlarmData * LogData, SINT32 LogsCount)
 *
 *   Purpose :
 * 		To read the number of alarms passed in LogsCount Parameter and copy to the buffer passed in logData parameter.
 *      It starts filling entries in buffer from the latest occurred alarm in the decreasing order of there occurrence time.
 *      This function is called from website module.
 *
 *   Entry Condition :
 *   	LogData - The pointer to the buffer where alarm log is to be copied(it must
 *                point to the buffer where maximum possible alarm entries can be copied)
 *    	LogsCount -   Number of latest alarm entries to be read in chronological order
 *
 *   Exit Condition :
 *		Return : The actual no of entries read and copied to passed logData buffer.
 */
UINT32 Log::ReadAlarmLog(WebAlarmData * LogData, SINT32 LogsCount)
{
    UINT32  TotalAlarmsRead = 0;
    SINT32  ArrayIndexToReadFrom;
    BOOLEAN ExitFlag = FALSE;
    UINT16 EntryChecksum;
    if(LogsCount > MAX_ALARM_ENTRIES)
       LogsCount = MAX_ALARM_ENTRIES;

    if(LogsCount > AlarmLogFram->AlarmsNo)
	    LogsCount = AlarmLogFram->AlarmsNo;

    ArrayIndexToReadFrom = AlarmLogFram->CircularIndexAlarm;//GetLatestAlarmEntryIndex();

    while((LogsCount > 0) && (ExitFlag == FALSE) && (ArrayIndexToReadFrom >= 0)){
    	EntryChecksum = ByteCRC((UINT8*)&AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom], sizeof(AlarmLog) - sizeof(UINT16));
   		if(AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].LogChecksum == EntryChecksum){
   			memcpy(&LogData -> AlarmData, &AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].AlarmData, sizeof(LogData -> AlarmData));
   			memcpy(&LogData -> AlarmStr, GetAlarmStringFromID((Alarms)AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].AlarmID), sizeof(LogData -> AlarmStr));
   			LogData -> AlarmNo = (AlarmLogFram->AlarmsNo - TotalAlarmsRead);//GetAlarmNumber(ArrayIndexToReadFrom);
   			TotalAlarmsRead++;
   			LogData++;
    	}
   		else{
   			//we have more alarms occurred in the system but the check sum of those is not correct
			if(AlarmLogFram->AlarmsNo > TotalAlarmsRead){
				AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].AlarmID = 0;
				memset(&AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].AlarmData.TS, 0 , sizeof(AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].AlarmData.TS));
				//make the invalid alarms to be displayed from next time.
				AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom].LogChecksum = ByteCRC((UINT8*)&AlarmLogFram->AlarmLogObjs[ArrayIndexToReadFrom] ,
						sizeof(AlarmLog) - sizeof(UINT16));
			}//exit because we have read the all alarms occurred in system and the checksum of
			//of others would obviously be incorrect
			else
				ExitFlag = TRUE;
   		}
   		LogsCount--;
		ArrayIndexToReadFrom--;
		if(ArrayIndexToReadFrom < 0)
			ArrayIndexToReadFrom = MAX_ALARM_ENTRIES - 1;
    }
    return TotalAlarmsRead;
}

/*   SINT8 * Log::GetEventStringFromID(EventID EventId)
 *
 *   Purpose :
 *   	To find event string corresponding to an event ID by searching the global event description array.
 *      This function is called from ReadEventLog() function of this class.
 *		The implementation of function doesn't care about the order the entries are made in global array
 *		as it traverse through the whole array
 *   Entry Condition :
 *  	EventId - The unique ID of an event
 *
 *   Exit Condition :
 *   	The event description if found else returns "Undefined"
 */
SINT8 * Log::GetEventStringFromID(EventID EventId)
{
	static SINT8 TmpStr[MAX_LOG_LENGTH];
	//making static will ensure that local string pointer in stack is not returned
	//to the calling function in case the description is not found assigning
	//a value ensures we don't return null pointer for further processing.
	SINT32  Indx = 0;
	BOOLEAN ExitFlag = FALSE;
	strcpy(TmpStr , "Undefined");
	if((SINT32)EventId > (SINT32)SYSTEM_EVENT_START)
		strncpy(TmpStr, CyGOS::GetSysEventStrigFromID(EventId), MAX_LOG_LENGTH - 1);//coverity CID: 11169
	else{
      while((EventsDescription[Indx].EventStr) && (ExitFlag == FALSE)){
         if(EventId == EventsDescription[Indx].Eventid){
            ExitFlag = TRUE;
            if(strlen(EventsDescription[Indx].EventStr) < MAX_LOG_LENGTH)
            	strcpy(TmpStr, (EventsDescription[Indx].EventStr));
         }
         Indx++;
      }
	}
	return TmpStr;
}
