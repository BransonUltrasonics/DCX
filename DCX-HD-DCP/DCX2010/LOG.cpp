/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/LOG.cpp_v   1.22.1.1   Mar 29 2017 11:45:26   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/LOG.cpp_v  $
 * 
 *    Rev 1.22.1.1   Mar 29 2017 11:45:26   hasanchez
 * New alarm created “Power Average Overload”
 * 
 *    Rev 1.22.1.0   17 Mar 2016 16:23:42   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.23   17 Mar 2016 16:07:22   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.22   19 Sep 2014 07:35:52   amaurya
 * Changes for complete.bin upload through website.
 * 
 *    Rev 1.21   02 Sep 2014 06:58:46   amaurya
 * Fixed Time Format.
 * 
 *    Rev 1.20   27 Jun 2012 03:56:38   ygupta
 * Changes for High Dynamic support.
 * 
 *    Rev 1.19   16 Dec 2011 15:09:08   tayars
 * Changed power down routine to accomodate the 814 board which is to be released. We now sense 15V directly to the processor.
 * 
 *    Rev 1.18   17 Oct 2011 07:49:08   RatneshA
 * Added events related to cable select signal
 * 
 *    Rev 1.17   16 Sep 2011 08:07:10   RatneshA
 * DDS removed from LOG description
 * 
 *    Rev 1.16   06 Sep 2011 14:04:52   ygupta
 * Called ReadPowerOnSecCounter to get poweron Hours.
 * 
 *    Rev 1.15   01 Sep 2011 13:06:04   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.14   29 Aug 2011 11:37:48   ygupta
 * Ver1.01P. Implemented Shut Down Process.
 * 
 *    Rev 1.13   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.12   16 Aug 2011 08:25:00   pdwivedi
 * code cleanup
 * 
 *    Rev 1.11   05 Aug 2011 10:32:58   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.10   03 Aug 2011 09:35:50   pdwivedi
 * Added changes for CRC Error.
 * 
 *    Rev 1.9   18 Jul 2011 10:37:34   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.8   15 Jul 2011 09:46:26   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.7   04 Apr 2011 11:06:18   PDwivedi
 * Some functions removed from LOG class and have been made global.
 * 
 *    Rev 1.6   14 Jan 2011 07:55:42   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.5   16 Dec 2010 08:17:42   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.4   14 Dec 2010 11:15:12   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:36   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:24   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   12 Nov 2010 10:43:06   nhahmed
 * Added extra Log entries and RestLOG function.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#include "LOG.h"
#include "string.h"
#include "stdio.h"
#include "Board.h"
#include "GLOBALS.h"
#include "FUNC.h"

int MiliSecTimer = 0;
int SecTimerSincePowerUp = 0;
//Array index represents a month and value at array index
//defines the day in that month. This array is used in calculation
//of time stamp since reference.
//static const unsigned char days_in_month[] =
//{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//Definition of DCP events description along with IDs
DCPEvent DCPEvents[] =
{
		{"Horn Scan Completed", EVENT_HORNSCAN_COMPLETED},
		{"Horn Scan Started", EVENT_HORNSCAN_START},
		{"Horn Scan Aborted", EVENT_HORNSCAN_ABORTED},
		{"Horn Scan Failed", EVENT_HORNSCAN_FAILED},
		{"Preset Recalled", EVENT_PRESET_RECALL},
		{"Power On", EVENT_POWERON},
		{"Power On Diagnostics Passed", EVENT_POWERONDIAGNOSTIC_PASSED},
		{"Power On Diagnostics Started", EVENT_POWERONDIAGNOSTIC_START},
		{"MI Board Detected", MI_BOARD_DETECTED},
		{"LCD Board Detected", LCD_BOARD_DETECTED},
		{"Webpage Login", WEBPAGE_LOGIN},
		{"Webpage Logout", WEBPAGE_LOGOUT},
		{"User I/O Defined", USERIO_DEFINED},
		{"Test Activated", TEST_ACTIVATED},
		{"Parameter Changed", PARAM_CHANGED},
		{"Test Overload", TEST_OVERLOAD},
		{"CRC Error", CRC_ERROR},
		{"System Fault", FAULT_ERROR},
		{"Watchdog Reset", WATCHDOG_RESET},
		{"Version Changed", VERSION_CHANGE},
		{"Power Supply CRC Changed",CRC_CHANGE},
		{"Clock Lost", CLOCK_LOST},
		{"Big File Name Get" ,GET_FREQ},
		{"Big File Name Post" ,POST_FREQ},
		{"Unexpected Post Command", POST_INVALID},
		{"Unexpected Get Command", GET_INVALID},
		{"CRLF not found", CRLF_INVALID},
		{"File Upload Aborted", UPLOAD_ABORT},
		{"Invalid Length", LEN_INVALID},
		{"15 Volts Lost",POWER_SHUTDOWN},
		{"Flash Program", PROGRAM_FLASH},
		{"Bad Serial Input", BAD_SERIALFORMAT},
		{"Active Low Accepted", EVENT_ACTIVELOWDIGIN},
		{"I/O Cable Detected", EVENT_INPUTCABLEFOUND},
		{"I/O Cable not Detected", EVENT_INPUTCABLELOST},
		{"Fpga Startup Test", EVENT_FPGASTARTUPTEST},
		{ "Program Firmware",EVENT_PROGRAMFIRMWARE},
		{0, 0},
};
//Definition of DCP alarms description along with IDs
DCPEvent DCPAlarms[] =
{
		{"Overload - Phase", OVERLOAD_PHASE},
		{"Overload - Frequency", OVERLOAD_FREQUENCY},
		{"Overload - Current", OVERLOAD_CURRENT},
		{"Overload - Voltage", OVERLOAD_VOLTAGE},
		{"Overload - Temperature", OVERLOAD_TEMPRATURE},
		{"Overload - Power Limit", OVERLOAD_POWERLIMIT },
		{"Overload - Start Frequency Outside Bandwidth", STARTFREQ_OUTSIDE_BANDWIDTH},
		{"Frequency - High Seek Limit",	DDSFREQ_OUTSIDE_HIGHSEEKLIMIT},
		{"Frequency - Low Seek Limit",	DDSFREQ_OUTSIDE_LOWSEEKLIMIT},
		{"Phase Limit Time Error",	PHASELIMIT_TIMELIMIT_ERROR},
		{"Frequency - High Weld Limit",	DDSFREQ_OUTSIDE_HIGHWELDLIMIT},
		{"Frequency - Low Weld Limit",	DDSFREQ_OUTSIDE_LOWWELDLIMIT},
		{"Fpga Adc Error", FPGA_ADC_ERROR},
		{"Fpga FlexBus Error", FPGA_FB_ERROR},
		{"Fpga Std DDS Error ",FPGA_STDDDS_ERROR},
		{"Fpga Sync DDS Error",	FPGA_SYNCDDS_ERROR},
		{"Fpga HD Error1",	FPGA_HD_ERROR1},
		{"FPGA HD Error2",	FPGA_HD_ERROR2},
		{"Overload - Average Power", OVERLOAD_POWER_AVERAGE},
		{0,0},
};

int SecondsSincePowerUp()
{
	return SecTimerSincePowerUp;
}

/*Function initializes the RTC module.
 * It checks for specific value (defined in LOGINITIALIZED_CHECKSUM) in
 * logInitialized variable.  If that value is not found it sets the
 * default values in event array eventlogObjs and alarmlogObjs array.
 * It also mark the circular indexes to not pointing any event by
 * assigning -1. The function must be called once at start up.
 */
void Log::Init(void)
{
	Board::InitRTC();
	if((LOGInfo.logInitialized) != LOGINITIALIZED_CHECKSUM)
	{
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
	if(type == clearAlarm)
	{
		memset(alarmlogObjs, 0, sizeof(alarmlogObjs));
		LOGInfo.circularIndexAlarm = -1;
		LOGInfo.nAlarms = 0;
	}
	else if(type == clearEvent)
	{
		memset(eventlogObjs, 0, sizeof(eventlogObjs));
		LOGInfo.circularIndexEvent = -1;
		LOGInfo.nEvents = 0;
	}
	else if(type == clearAll)
	{
		memset(alarmlogObjs, 0, sizeof(alarmlogObjs));
		memset(eventlogObjs, 0, sizeof(eventlogObjs));
		LOGInfo.logInitialized = LOGINITIALIZED_CHECKSUM;
		LOGInfo.circularIndexAlarm = -1;
		LOGInfo.circularIndexEvent = -1;
		LOGInfo.nEvents = 0;
		LOGInfo.nAlarms = 0;
	}
}

/*Function first determines the index where the Log information should
 * be written in the evebtlogObjs array. If index goes beyond
 * the maximum limit it rolls back to 0 and overwrites the previous entries.
 *  DCP Application code should use this static function
 *  to write the event log wherever necessary.
@param eventID:  The unique ID of DCP Event.
@param Data1: Diagnostic data 1.
@param Data2: Diagnostic data 2.
*/
void Log::WriteEventLog(DCPEventID eventID, short Data1, short Data2, unsigned int userID, bool data)
{
	LOGInfo.circularIndexEvent = ((LOGInfo.circularIndexEvent) + 1) % MAX_EVENT_ENTRIES;
	eventlogObjs[LOGInfo.circularIndexEvent].timeStamp = ReadPowerOnSecCounter();//SecondsSincePowerUp();//Board::GetRTCTime();
	eventlogObjs[LOGInfo.circularIndexEvent].data1 = Data1;
	eventlogObjs[LOGInfo.circularIndexEvent].data2 = Data2;
	if(data)
		eventlogObjs[LOGInfo.circularIndexEvent].userID = userID;
	else
		eventlogObjs[LOGInfo.circularIndexEvent].userID = WebSite::GetUserID();//TODO: Find from sys config

	eventlogObjs[LOGInfo.circularIndexEvent].eventID = eventID;
	eventlogObjs[LOGInfo.circularIndexEvent].logValid = TRUE;
	(LOGInfo.nEvents)++;
}

/*Function first determines the index where the Log information
 * should be written in the alarmlogObjs array. If index goes beyond
 * the maximum limit it rolls back to 0 and overwrites the previous
 * entries . DCP Application code should use this static function  to
 *  write the alarm log wherever necessary.
@param alarmID:  The unique ID of Alarm Event.
@param Data1: Diagnostic data 1.
@param Data2: Diagnostic data 2.
*/
void Log::WriteAlarmLog(DCPAlarmID alarmID, short Data1, short Data2, unsigned int userID)
{
	LOGInfo.circularIndexAlarm = ((LOGInfo.circularIndexAlarm) + 1) % MAX_ALARM_ENTRIES;
	alarmlogObjs[LOGInfo.circularIndexAlarm].timeStamp = ReadPowerOnSecCounter();//SecondsSincePowerUp();//Board::GetRTCTime();
	alarmlogObjs[LOGInfo.circularIndexAlarm].data1 = Data1;
	alarmlogObjs[LOGInfo.circularIndexAlarm].data2 = Data2;
	userID = 0;//Disable warning unused variable'
	alarmlogObjs[LOGInfo.circularIndexEvent].userID = WebSite::GetUserID();//TODO: Find from sys config
	alarmlogObjs[LOGInfo.circularIndexAlarm].eventID = alarmID;
	alarmlogObjs[LOGInfo.circularIndexAlarm].logValid = TRUE;
	(LOGInfo.nAlarms)++;
}

/*Function returns the index of latest entry
 *  written in eventlogObjs Array.
 */
int Log::GetLatestEventEntryIndex(void)
{
	return (LOGInfo.circularIndexEvent);
}

/*
 * Function returns the index of latest
 * entry written in alarmlogObjs Array.
 */
int Log:: GetLatestAlarmEntryIndex (void)
{
	return (LOGInfo.circularIndexAlarm);
}
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
int Log::ReadEventLog(LogData * logData, int logIndex, int nLogs)
{
	int NumOfEventlogs = 0;
	bool FillLogDataflag = TRUE;

	if((logIndex < 0) || (logIndex >= MAX_EVENT_ENTRIES))
	{
		nLogs = 0;
		NumOfEventlogs = 0;
	}

	int logStartIndx = logIndex;
	int indx;
	for(indx = 0; indx < nLogs; indx ++)
	{
		BOOLEAN ValidFlag = FALSE;
		ValidFlag = eventlogObjs[logIndex].logValid;
		if(!ValidFlag)
		{
			NumOfEventlogs = indx;
			nLogs = 0;
			FillLogDataflag = FALSE;
		}
		if(FillLogDataflag)
		{

			strcpy(logData[indx].logStr, GetEventStringFromID((DCPEventID)eventlogObjs[logIndex].eventID));
			LogTime rTm;
			CalculateTimeSincePowerUp(eventlogObjs[logIndex].timeStamp, &rTm);
			//format mm-dd-yyyy HH:MM:SS
			sprintf(logData[indx].timeStamp, "%02d-%02d-%04d %02d:%02d:%02d", rTm.tm_mon, rTm.tm_day,
							rTm.tm_year, rTm.tm_hour, rTm.tm_min, rTm.tm_sec);
			logData[indx].data1 = eventlogObjs[logIndex].data1;
			logData[indx].data2 = eventlogObjs[logIndex].data2;
			logData[indx].userID = eventlogObjs[logIndex].userID;
			logData[indx].eventNo = GetEventNumber(logIndex);
			logIndex--;
			if(logIndex < 0)
				logIndex = MAX_EVENT_ENTRIES - 1;
			if(logIndex == logStartIndx)
			{
				NumOfEventlogs = MAX_EVENT_ENTRIES;
				nLogs = 0;
			}
			else
				NumOfEventlogs = indx;
		}
	}
	return NumOfEventlogs;
}


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
*/
int Log::ReadAlarmLog(LogData * logData, int logIndex, int nLogs)
{
	int NumOfAlarmlogs = 0;
	bool FillLogDataflag = TRUE;

	int logStartIndx = logIndex;
	int indx;
	LogTime rTm;
	if((logIndex < 0) || (logIndex >= MAX_ALARM_ENTRIES))
	{
		nLogs = 0;
		NumOfAlarmlogs = 0;
	}

	for(indx = 0; indx < nLogs; indx ++)
	{
		BOOLEAN ValidFlag = FALSE;
		ValidFlag = alarmlogObjs[logIndex].logValid;
		if(!ValidFlag)
		{
			NumOfAlarmlogs = indx;
			nLogs = 0;
			FillLogDataflag = FALSE;
		}
		if(FillLogDataflag)
		{
			strcpy(logData[indx].logStr, GetAlarmStringFromID((DCPAlarmID)alarmlogObjs[logIndex].eventID));
			CalculateTimeSincePowerUp(alarmlogObjs[logIndex].timeStamp, &rTm);
			//format mm-dd-yyyy HH:MM:SS
			sprintf(logData[indx].timeStamp, "%02d-%02d-%04d %02d:%02d:%02d", rTm.tm_mon, rTm.tm_day,
							rTm.tm_year, rTm.tm_hour, rTm.tm_min, rTm.tm_sec);
			logData[indx].data1 = alarmlogObjs[logIndex].data1;
			logData[indx].data2 = alarmlogObjs[logIndex].data2;
			logData[indx].userID = alarmlogObjs[logIndex].userID;
			logData[indx].eventNo = GetAlarmNumber(logIndex);
			logIndex--;
			if(logIndex < 0)
				logIndex = MAX_ALARM_ENTRIES - 1;
			if(logIndex == logStartIndx)
			{
				NumOfAlarmlogs = MAX_ALARM_ENTRIES;
				nLogs = 0;
			}
			else
				NumOfAlarmlogs = indx;
		}
	}
	return NumOfAlarmlogs;
}

//Convert seconds in time parameter to Gregorian date.
void CalculateTimeSincePowerUp(unsigned long time, struct LogTime *tm)
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

/*Function finds the event ID passed in param
 * “eventID” in the DCPEvents array. If the ID is
 * found it returns the corresponding event description.
 * Otherwise it reruns NULL.
@param eventID: A unique ID of a DCP event.
*/
char * Log::GetEventStringFromID(DCPEventID eventID)
{
	int indx = 0;
	char * EventString = NULL;
	BOOLEAN ExitFlag = FALSE;
	while((DCPEvents[indx].eventStr) && (ExitFlag == FALSE))
	{
		if(eventID == DCPEvents[indx].eid)
		{
			ExitFlag = TRUE;
			EventString = (DCPEvents[indx].eventStr);
		}
		indx++;
	}
	return EventString;
}

/*Function finds the event ID passed in param “eventID” in the
 * DCPAlarm array. If the ID is found it returns the corresponding
 *  Alarm description. Otherwise it reruns NULL.
@param eventID: A unique ID of a DCP event.
*/
char * Log::GetAlarmStringFromID(DCPAlarmID eventID)
{
	int indx = 0;
	char * EventString = NULL;
	BOOLEAN ExitFlag = FALSE;
	while((DCPAlarms[indx].eventStr)&& (ExitFlag == FALSE))
	{
		if(eventID == DCPAlarms[indx].eid)
		{
			ExitFlag = TRUE;
			EventString = (DCPAlarms[indx].eventStr);
		}
		indx++;
	}
	return EventString;
}

int Log::GetEventNumber(int logIndx)
{
	int eventNo = LOGInfo.nEvents;
	int temp = (logIndx - (LOGInfo.circularIndexEvent));
	if(temp < 0)
		eventNo = (LOGInfo.nEvents + temp);
	if(temp > 0)
		eventNo = (LOGInfo.nEvents - (MAX_EVENT_ENTRIES - logIndx) - (LOGInfo.circularIndexEvent));

	return eventNo;
}

int Log::GetAlarmNumber(int logIndx)
{
	int alarmNo = (LOGInfo.nAlarms);
	int temp = (logIndx - (LOGInfo.circularIndexAlarm));
	if(temp < 0)
		alarmNo = (LOGInfo.nAlarms + temp);
	if(temp > 0)
		alarmNo = (LOGInfo.nAlarms - (MAX_ALARM_ENTRIES - logIndx) - (LOGInfo.circularIndexAlarm));

	return alarmNo;
}

