/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/LOG.cpp_v   1.3.1.6   25 Jun 2015 07:06:00   amaurya  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/LOG.cpp_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added HORNSCAN_FAILED alarm description
 *
 *    Rev 1.3.1.6   25 Jun 2015 07:06:00   amaurya
 * Added Event Restore Defaults to DCPEvents.
 * 
 *    Rev 1.3.1.5   04 Jun 2015 07:43:54   sesharma
 * AC power lost alarm added to DCPAlarms array. 
 * Data type of Data1 & Data2 parameters in WriteEventLog and WriteAlarmLog functions changed to SINT32.
 * Branson coding standard formatter applied.
 * 
 *    Rev 1.3.1.4   19 Mar 2015 06:54:16   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3.1.3   28 Jan 2015 05:31:54   sesharma
 * Data Types Corrected to remove compiler warnings
 * 
 *    Rev 1.3.1.2   22 Jan 2015 04:57:48   sesharma
 * Bug Fixing for Large Negative value of Alarm No. in Alarm log.
 * Date & Time changed to Days & Hours In Alarm & Event Log.
 * 
 *    Rev 1.3.1.1   16 Jan 2015 06:30:36   sesharma
 * Changed Date to Days and Time to Hours in Alarm log, also changed Power On hours to Power On Days on System Information page.
 * 
 *    Rev 1.3.1.0   13 Jan 2015 06:01:06   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.3   07 Nov 2014 15:35:44   ewomack
 * Alarms added for Power Window Limits and Timeout in Energy Mode.
 * 
 *    Rev 1.2   25 Sep 2014 03:34:42   amaurya
 * Corrected the frequency value in event log for 40Khz.
 * 
 *    Rev 1.1   17 Sep 2014 08:41:20   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#include "LOG.h"
#include "string.h"
#include "stdio.h"
#include "Board.h"
#include "GLOBALS.h"
#include "FUNC.h"

SINT32 MiliSecTimer = 0;
SINT32 SecTimerSincePowerUp = 0;

//Definition of DCP events description along with IDs
DCPEvent DCPEvents[] = {
	{ "Horn Scan Completed", EVENT_HORNSCAN_COMPLETED },
	{ "Horn Scan Started", EVENT_HORNSCAN_START },
	{ "Horn Scan Aborted", EVENT_HORNSCAN_ABORTED },
	{ "Horn Scan Failed", EVENT_HORNSCAN_FAILED },
	{ "Preset Recalled", EVENT_PRESET_RECALL },
	{ "Power On", EVENT_POWERON },
	{ "Power On Diagnostics Passed", EVENT_POWERONDIAGNOSTIC_PASSED },
	{ "Power On Diagnostics Started", EVENT_POWERONDIAGNOSTIC_START },
	{ "MI Board Detected", MI_BOARD_DETECTED },
	{ "LCD Board Detected", LCD_BOARD_DETECTED },
	{ "Webpage Login", WEBPAGE_LOGIN },
	{ "Webpage Logout", WEBPAGE_LOGOUT },
    { "User I/O Defined", USERIO_DEFINED },
    { "Test Activated", TEST_ACTIVATED },
    { "Parameter Changed", PARAM_CHANGED },
    { "Test Overload", TEST_OVERLOAD },
	{ "CRC Error", CRC_ERROR },
	{ "System Fault", FAULT_ERROR },
	{ "Watchdog Reset", WATCHDOG_RESET },
    { "Version Changed", VERSION_CHANGE },
	{ "Power Supply CRC Changed", CRC_CHANGE },
	{ "Clock Lost", CLOCK_LOST },
	{ "Big File Name Get", GET_FREQ },
	{ "Big File Name Post", POST_FREQ },
	{ "Unexpected Post Command", POST_INVALID },
	{ "Unexpected Get Command", GET_INVALID },
	{ "CRLF not found", CRLF_INVALID },
	{ "File Upload Aborted", UPLOAD_ABORT },
	{ "Invalid Length", LEN_INVALID },
    { "Power ShutDown", POWER_SHUTDOWN },
	{ "Flash Program", PROGRAM_FLASH },
    { "Bad Serial Input", BAD_SERIALFORMAT },
	{ "Active Low Accepted", EVENT_ACTIVELOWDIGIN },
	{ "I/O Cable Detected", EVENT_INPUTCABLEFOUND },
	{ "I/O Cable not Detected", EVENT_INPUTCABLELOST },
    { "Program Firmware", EVENT_PROGRAMFIRMWARE },
    { "Restore Defaults", EVENT_RESTORE_DEFAULTS},
	{ 0, 0 },
};
//Definition of DCP alarms description along with IDs
DCPEvent DCPAlarms[] = {
	{ "Overload - Phase", OVERLOAD_PHASE },
	{ "Overload - Frequency", OVERLOAD_FREQUENCY },
	{ "Overload - Current", OVERLOAD_CURRENT },
	{ "Overload - Voltage", OVERLOAD_VOLTAGE },
	{ "Overload - Temperature", OVERLOAD_TEMPRATURE },
	{ "Overload - Power Limit", OVERLOAD_POWERLIMIT },
	{ "Overload - Start Frequency Outside Bandwidth", STARTFREQ_OUTSIDE_BANDWIDTH },
	{ "Frequency - High Seek Limit", DDSFREQ_OUTSIDE_HIGHSEEKLIMIT },
	{ "Frequency - Low Seek Limit", DDSFREQ_OUTSIDE_LOWSEEKLIMIT },
	{ "Phase Limit Time Error", PHASELIMIT_TIMELIMIT_ERROR },
	{ "Frequency - High Weld Limit", DDSFREQ_OUTSIDE_HIGHWELDLIMIT },
	{ "Frequency - Low Weld Limit", DDSFREQ_OUTSIDE_LOWWELDLIMIT },
	{ "Peak Power - Low Limit", PEAKPOWER_MINUS_LIMIT },
    { "Peak Power - High Limit", PEAKPOWER_PLUS_LIMIT },
	{ "Energy - Timeout Reached", ENERGYREACHED_TIMEOUT },
	{ "AC Power Lost", ACPOWER_LOST },
	{ "Horn Scan Failed", HORNSCAN_FAILED },
	{ 0, 0 },
};

SINT32 SecondsSincePowerUp()
{
   return SecTimerSincePowerUp;
}
void Log::Init(void)
/******************************************************************************/
/*Function initializes the RTC module.
 * It checks for specific value (defined in LOGINITIALIZED_CHECKSUM) in
 * logInitialized variable.  If that value is not found it sets the
 * default values in event array eventlogObjs and alarmlogObjs array.
 * It also mark the circular indexes to not pointing any event by
 * assigning -1. The function must be called once at start up.
 */
/********************************************************************************/
{
   Board::InitRTC();
   if ((LOGInfo.logInitialized) != LOGINITIALIZED_CHECKSUM) {
      memset(alarmlogObjs, 0, sizeof(alarmlogObjs));
      memset(eventlogObjs, 0, sizeof(eventlogObjs));
      LOGInfo.logInitialized = LOGINITIALIZED_CHECKSUM;
      LOGInfo.circularIndexAlarm = -1;
      LOGInfo.circularIndexEvent = -1;
      LOGInfo.nEvents = 0;
      LOGInfo.nAlarms = 0;
      pprintf(" \n LOG Default params init");
   }
   pprintf(" \n LOG Init");
}

void Log::ResetLog(ClearLog type)
{
   if (type == clearAlarm) {
      memset(alarmlogObjs, 0, sizeof(alarmlogObjs));
      LOGInfo.circularIndexAlarm = -1;
      LOGInfo.nAlarms = 0;
   }
   else if (type == clearEvent) {
      memset(eventlogObjs, 0, sizeof(eventlogObjs));
      LOGInfo.circularIndexEvent = -1;
      LOGInfo.nEvents = 0;
   }
   else if (type == clearAll) {
      memset(alarmlogObjs, 0, sizeof(alarmlogObjs));
      memset(eventlogObjs, 0, sizeof(eventlogObjs));
      LOGInfo.logInitialized = LOGINITIALIZED_CHECKSUM;
      LOGInfo.circularIndexAlarm = -1;
      LOGInfo.circularIndexEvent = -1;
      LOGInfo.nEvents = 0;
      LOGInfo.nAlarms = 0;
   }
}
void Log::WriteEventLog(DCPEventID eventID, SINT32 Data1, SINT32 Data2,
      UINT32 userID, BOOLEAN data)
/***************************************************************************************************************/
/*Function first determines the index where the Log information should
 * be written in the evebtlogObjs array. If index goes beyond
 * the maximum limit it rolls back to 0 and overwrites the previous entries.
 *  DCP Application code should use this static function
 *  to write the event log wherever necessary.
 @param eventID:  The unique ID of DCP Event.
 @param Data1: Diagnostic data 1.
 @param Data2: Diagnostic data 2.
 */
/***************************************************************************************************************/
{
   LOGInfo.circularIndexEvent = ((LOGInfo.circularIndexEvent) + 1)
         % MAX_EVENT_ENTRIES;
   eventlogObjs[LOGInfo.circularIndexEvent].timeStamp = ReadPowerOnSecCounter();   //SecondsSincePowerUp();//Board::GetRTCTime();
   eventlogObjs[LOGInfo.circularIndexEvent].data1 = Data1;
   eventlogObjs[LOGInfo.circularIndexEvent].data2 = Data2;
   if (data)
      eventlogObjs[LOGInfo.circularIndexEvent].userID = userID;
   else
      eventlogObjs[LOGInfo.circularIndexEvent].userID = WebSite::GetUserID();   //TODO: Find from sys config

   eventlogObjs[LOGInfo.circularIndexEvent].eventID = eventID;
   eventlogObjs[LOGInfo.circularIndexEvent].logValid = TRUE;
   (LOGInfo.nEvents)++;
}
void Log::WriteAlarmLog(DCPAlarmID alarmID, SINT32 Data1, SINT32 Data2,
      UINT32 userID)
/****************************************************************************************/
/*Function first determines the index where the Log information
 * should be written in the alarmlogObjs array. If index goes beyond
 * the maximum limit it rolls back to 0 and overwrites the previous
 * entries . DCP Application code should use this static function  to
 *  write the alarm log wherever necessary.
 @param alarmID:  The unique ID of Alarm Event.
 @param Data1: Diagnostic data 1.
 @param Data2: Diagnostic data 2.
 */
/***************************************************************************************/
{
   LOGInfo.circularIndexAlarm = ((LOGInfo.circularIndexAlarm) + 1)
         % MAX_ALARM_ENTRIES;
   alarmlogObjs[LOGInfo.circularIndexAlarm].timeStamp = ReadPowerOnSecCounter();   //SecondsSincePowerUp();//Board::GetRTCTime();
   alarmlogObjs[LOGInfo.circularIndexAlarm].data1 = Data1;
   alarmlogObjs[LOGInfo.circularIndexAlarm].data2 = Data2;
   userID = 0;   //Disable warning unused variable'
   alarmlogObjs[LOGInfo.circularIndexAlarm].userID = WebSite::GetUserID();   //TODO: Find from sys config
   alarmlogObjs[LOGInfo.circularIndexAlarm].eventID = alarmID;
   alarmlogObjs[LOGInfo.circularIndexAlarm].logValid = TRUE;
   (LOGInfo.nAlarms)++;
   if (LOGInfo.nAlarms > 0x7FFFFFFF) LOGInfo.nAlarms = 0;
}

SINT32 Log::GetLatestEventEntryIndex(void)
/***************************************************************************************/
/*Function returns the index of latest entry
 *  written in eventlogObjs Array.
 */
/***************************************************************************************/
{
   return (LOGInfo.circularIndexEvent);
}

SINT32 Log::GetLatestAlarmEntryIndex(void)
/***************************************************************************************/
/*
 * Function returns the index of latest
 * entry written in alarmlogObjs Array.
 */
/***************************************************************************************/
{
   return (LOGInfo.circularIndexAlarm);
}

SINT32 Log::ReadEventLog(LogData * logData, SINT32 logIndex, SINT32 nLogs)
/***************************************************************************************/
/*  Function returns the number of events successfully copied to array
 *  passed in parameter “logData”. For getting the latest N entries in
 *  chronological order the caller of this function should first determine
 *  the index of latest event using GetLatestEventEntryIndex function.
 *  Suppose the latest entry index comes out to be X then this function
 *  should be called like ReadEventLog(logData, X, N).
 @param logData: The pointer to an array of LogData objects where log information
 should be copied .
 @param logIndex: The index from where log should be read in chronological order.
 @param nLogs : The number of  events to be read.
 */
/***************************************************************************************/
{
   SINT32 NumOfEventlogs = 0;
   bool FillLogDataflag = TRUE;

   if ((logIndex < 0) || (logIndex >= MAX_EVENT_ENTRIES)) {
      nLogs = 0;
      NumOfEventlogs = 0;
   }

   SINT32 logStartIndx = logIndex;
   SINT32 indx;
   for (indx = 0; indx < nLogs; indx++) {
      BOOLEAN ValidFlag = FALSE;
      ValidFlag = eventlogObjs[logIndex].logValid;
      if (!ValidFlag) {
         NumOfEventlogs = indx;
         nLogs = 0;
         FillLogDataflag = FALSE;
      }
      if (FillLogDataflag) {

         strcpy(logData[indx].logStr,
               GetEventStringFromID(
                     (DCPEventID) eventlogObjs[logIndex].eventID));
         LogTime rTm;
         CalculateDaysAndHoursSincePowerUp(eventlogObjs[logIndex].timeStamp,
               &rTm);
         //format mm-dd-yyyy HH:MM:SS
         sprintf(logData[indx].timeStamp, "%02d %02d:%02d:%02d",
               (int) rTm.tm_day, (int) rTm.tm_hour, (int) rTm.tm_min,
               (int) rTm.tm_sec);
         logData[indx].data1 = eventlogObjs[logIndex].data1;
         logData[indx].data2 = eventlogObjs[logIndex].data2;
         logData[indx].userID = eventlogObjs[logIndex].userID;
         logData[indx].eventNo = GetEventNumber(logIndex);
         logIndex--;
         if (logIndex < 0) logIndex = MAX_EVENT_ENTRIES - 1;
         if (logIndex == logStartIndx) {
            NumOfEventlogs = MAX_EVENT_ENTRIES;
            nLogs = 0;
         }
         else
            NumOfEventlogs = indx;
      }
   }
   return NumOfEventlogs;
}

SINT32 Log::ReadAlarmLog(LogData * logData, SINT32 logIndex, SINT32 nLogs)
/****************************************************************************/
/*Function returns the number of events successfully copied to array
 * passed in parameter “logData”. For getting the latest N entries
 * in chronological order the caller of this function should first
 * determine the index of latest event using GetLatestAlarmEntryIndex function.
 * Suppose the latest entry index comes out to be X then this function should
 * be called like ReadAlarmLog(logData, X, N).
 @param logData: The pointer to an array of LogData objects where log information
 should be copied .
 @param logIndex: The index from where log should be read in chronological order.
 @param nLogs : The number of  events to be read.
 *******************************************************************************/
{
   SINT32 NumOfAlarmlogs = 0;
   BOOLEAN FillLogDataflag = TRUE;

   SINT32 logStartIndx = logIndex;
   SINT32 indx;
   LogTime rTm;
   if ((logIndex < 0) || (logIndex >= MAX_ALARM_ENTRIES)) {
      nLogs = 0;
      NumOfAlarmlogs = 0;
   }

   for (indx = 0; indx < nLogs; indx++) {
      BOOLEAN ValidFlag = FALSE;
      ValidFlag = alarmlogObjs[logIndex].logValid;
      if (!ValidFlag) {
         NumOfAlarmlogs = indx;
         nLogs = 0;
         FillLogDataflag = FALSE;
      }
      if (FillLogDataflag) {
         strcpy(logData[indx].logStr,
               GetAlarmStringFromID(
                     (DCPAlarmID) alarmlogObjs[logIndex].eventID));
         CalculateDaysAndHoursSincePowerUp(alarmlogObjs[logIndex].timeStamp,
               &rTm);
         //format mm-dd-yyyy HH:MM:SS
         sprintf(logData[indx].timeStamp, "%02d %02d:%02d:%02d",
               (int) rTm.tm_day, (int) rTm.tm_hour, (int) rTm.tm_min,
               (int) rTm.tm_sec);
         logData[indx].data1 = alarmlogObjs[logIndex].data1;
         logData[indx].data2 = alarmlogObjs[logIndex].data2;
         logData[indx].userID = alarmlogObjs[logIndex].userID;
         logData[indx].eventNo = GetAlarmNumber(logIndex);
         logIndex--;
         if (logIndex < 0) logIndex = MAX_ALARM_ENTRIES - 1;
         if (logIndex == logStartIndx) {
            NumOfAlarmlogs = MAX_ALARM_ENTRIES;
            nLogs = 0;
         }
         else
            NumOfAlarmlogs = indx;
      }
   }
   return NumOfAlarmlogs;
}
void CalculateTimeSincePowerUp(UINT32 time, struct LogTime *tm)
/************************************************************************************/
/* Function Convert seconds in time parameter to Gregorian date. */
/************************************************************************************/
{
   tm->tm_year = time / NSECSINYEAR;
   time -= (tm->tm_year * NSECSINYEAR);
   tm->tm_mon = time / NSECSINMONTH;
   time -= (tm->tm_mon * NSECSINMONTH);
   tm->tm_day = time / NSECSINDAY;
   time -= (tm->tm_day * NSECSINDAY);
   tm->tm_hour = time / NSECSINHOUR;
   time -= (tm->tm_hour * NSECSINHOUR);
   tm->tm_min = time / NSECSINMINUTE;
   tm->tm_sec = time - (tm->tm_min * NSECSINMINUTE);
}

void CalculateDaysAndHoursSincePowerUp(UINT32 time, struct LogTime *tm)
/************************************************************************************/
/* Function Convert seconds in time parameter to Gregorian date. */
/************************************************************************************/
{
   tm->tm_day = time / NSECSINDAY;
   time -= (tm->tm_day * NSECSINDAY);
   tm->tm_hour = time / NSECSINHOUR;
   time -= (tm->tm_hour * NSECSINHOUR);
   tm->tm_min = time / NSECSINMINUTE;
   tm->tm_sec = time - (tm->tm_min * NSECSINMINUTE);
}

char* Log::GetEventStringFromID(DCPEventID eventID)
/************************************************************************************/
/*Function finds the event ID passed in param
 * “eventID” in the DCPEvents array. If the ID is
 * found it returns the corresponding event description.
 * Otherwise it reruns NULL.
 * @param eventID: A unique ID of a DCP event.*/
/***************************************************************************************/
{
   SINT32 indx = 0;
   char* EventString = NULL;
   BOOLEAN ExitFlag = FALSE;
   while ((DCPEvents[indx].eventStr) && (ExitFlag == FALSE)) {
      if (eventID == DCPEvents[indx].eid) {
         ExitFlag = TRUE;
         EventString = (DCPEvents[indx].eventStr);
      }
      indx++;
   }
   return EventString;
}
char * Log::GetAlarmStringFromID(DCPAlarmID eventID)
/********************************************************************************/
/*Function finds the event ID passed in param “eventID” in the
 * DCPAlarm array. If the ID is found it returns the corresponding
 *  Alarm description. Otherwise it reruns NULL.
 *@param eventID: A unique ID of a DCP event. */
/***********************************************************************************/
{
   SINT32 indx = 0;
   char * EventString = NULL;
   BOOLEAN ExitFlag = FALSE;
   while ((DCPAlarms[indx].eventStr) && (ExitFlag == FALSE)) {
      if (eventID == DCPAlarms[indx].eid) {
         ExitFlag = TRUE;
         EventString = (DCPAlarms[indx].eventStr);
      }
      indx++;
   }
   return EventString;
}

SINT32 Log::GetEventNumber(int logIndx)
{
   SINT32 eventNo = LOGInfo.nEvents;
   SINT32 temp = (logIndx - (LOGInfo.circularIndexEvent));
   if (temp < 0) eventNo = (LOGInfo.nEvents + temp);
   if (temp > 0)
      eventNo = (LOGInfo.nEvents - (MAX_EVENT_ENTRIES - logIndx)
            - (LOGInfo.circularIndexEvent));

   return eventNo;
}

UINT32 Log::GetAlarmNumber(int logIndx)
{
   UINT32 alarmNo = (LOGInfo.nAlarms);
   SINT32 temp = (logIndx - (LOGInfo.circularIndexAlarm));
   if (temp < 0) alarmNo = (LOGInfo.nAlarms + temp);
   if (temp > 0)
      alarmNo = (LOGInfo.nAlarms - (MAX_ALARM_ENTRIES - logIndx)
            - (LOGInfo.circularIndexAlarm));

   return alarmNo;
}

