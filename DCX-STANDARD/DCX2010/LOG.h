/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/LOG.h_v   1.15.2.0   12 Oct 2012 10:05:04   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/LOG.h_v  $
 * 
 *    Rev 1.15.2.0   12 Oct 2012 10:05:04   ygupta
 * Fixed Reset Overload Clear Issue.
 * 
 *    Rev 1.15   17 Oct 2011 07:49:26   RatneshA
 * Added events related to cable select signal
 * 
 *    Rev 1.14   06 Sep 2011 14:02:58   ygupta
 * Added event for BAD Serial Input.
 * 
 *    Rev 1.13   01 Sep 2011 13:06:06   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.12   29 Aug 2011 11:37:50   ygupta
 * Ver1.01P. Implemented Shut Down Process.
 * 
 *    Rev 1.11   28 Aug 2011 11:42:26   ygupta
 * Ver1.01M
 * 
 *    Rev 1.10   16 Aug 2011 08:25:16   pdwivedi
 * code cleanup
 * 
 *    Rev 1.9   03 Aug 2011 09:36:04   pdwivedi
 * Added changes for CRC Error.
 * 
 *    Rev 1.8   18 Jul 2011 10:37:46   pdwivedi
 * Added Exception Log entry changes. 
 * 
 *    Rev 1.7   15 Jul 2011 09:46:38   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.6   04 Apr 2011 11:06:20   PDwivedi
 * Some functions removed from LOG class and have been made global.
 * 
 *    Rev 1.5   14 Jan 2011 07:55:42   PDwivedi
 * Added changes for DCX bug and proposed changes.
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
 *    Rev 1.1   12 Nov 2010 10:43:04   nhahmed
 * Added extra Log entries and RestLOG function.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include "portable.h"

//Below are the defines, enums and data structures for LOG handling module

//This structure represents a time stamp
struct LogTime
{
	//seconds in 24 hour clock
	int tm_sec;
	//minutes in 24 hour clock
	int tm_min;
	//hours in 24 hour clock
	int tm_hour;
	//The day of the month
	int tm_day;
	//The month of the year
	int tm_mon;
	//The year in Gregorian calendar systems
	int tm_year;
};


//Maximum number of events which can be saved in FRAM
#define MAX_EVENT_ENTRIES 200
//Maximum number of alarms which can be saved in FRAM
#define MAX_ALARM_ENTRIES 100


//An object of this structure represents a DCP event
//This is the one needs to be saved in FRAM.
struct DCPLog
{
	//number of seconds since the system is started.
	UINT32 timeStamp;
	//Unique identification of an event.
	short eventID;
	//Diagnostic purpose data 1 related of the event
	short data1;
	//Diagnostic purpose data 2 related of the event.
	short data2;
	//The User ID set by website.
	unsigned int userID;
	//Flag to check whether the values in other fields
	//of this structure are valid or not.
	BOOLEAN logValid;
};
//The maximum length of Event Description string
#define MAX_LOG_LENGTH 50
//The maximum length of Event Timestamp in mm-dd-yyyy HH-MM-SS fromat.
#define MAX_TIMESTAMP_LENGTH 20


//This structure is used to keep the
//DCP event information in string format.
//No object of this is saved in FRAM. It is just used
//to change the information saved in DCPLog object
//to descriptive format.
struct LogData
{
	char logStr[MAX_LOG_LENGTH];
	char timeStamp[MAX_TIMESTAMP_LENGTH];
	short data1;
	short data2;
	unsigned int userID;
	unsigned int eventNo;
};
//The time stamp of a DCP event shall be calculated since first day
//and hour of reference year defined below.
//e.g. in case of 2010 the time stamp shall be calculated since
// 01-01-2010 00:00:00

//The value to determine that whether the
//LOG module ever been initialized.
#define LOGINITIALIZED_CHECKSUM 0xAAEEDDFC
#define NSECSINYEAR   31536000
#define NSECSINMONTH  2592000
#define NSECSINDAY    86400
#define NSECSINHOUR   3600
#define NSECSINMINUTE 60
#define NMSINHOUR     3600000
#define NMSINMINUTE   60000

//The IDs of event
enum DCPEventID
{
	EVENT_POWERON,
 	EVENT_POWERONDIAGNOSTIC_START,
	EVENT_POWERONDIAGNOSTIC_PASSED,
	EVENT_PRESET_RECALL,
	EVENT_HORNSCAN_START,
	EVENT_HORNSCAN_COMPLETED,
	EVENT_HORNSCAN_ABORTED,
	EVENT_HORNSCAN_FAILED,
	MI_BOARD_DETECTED,
	LCD_BOARD_DETECTED,
	WEBPAGE_LOGIN,
	WEBPAGE_LOGOUT,
	USERIO_DEFINED,
	TEST_ACTIVATED,
	PARAM_CHANGED,
	TEST_OVERLOAD,
	CRC_ERROR,
	FAULT_ERROR,
	WATCHDOG_RESET,
	VERSION_CHANGE,
	CRC_CHANGE,
	CLOCK_LOST,
	GET_FREQ,
	POST_FREQ,
	POST_INVALID,
	GET_INVALID,
	CRLF_INVALID,
	UPLOAD_ABORT,
	LEN_INVALID,
	POWER_SHUTDOWN,
	PROGRAM_FLASH,
	BAD_SERIALFORMAT,
	EVENT_ACTIVELOWDIGIN,
	EVENT_INPUTCABLEFOUND,
	EVENT_INPUTCABLELOST,
	EVENT_LOGEND,//Any new Event must be added before this
};
//The IDs of alarms
enum DCPAlarmID
{
	OVERLOAD_PHASE,
	OVERLOAD_FREQUENCY,
	OVERLOAD_CURRENT,
	OVERLOAD_VOLTAGE,
	OVERLOAD_TEMPRATURE,
	OVERLOAD_POWERLIMIT,
	STARTFREQ_OUTSIDE_BANDWIDTH,
	DDSFREQ_OUTSIDE_HIGHSEEKLIMIT,
	DDSFREQ_OUTSIDE_LOWSEEKLIMIT,
	PHASELIMIT_TIMELIMIT_ERROR,
	DDSFREQ_OUTSIDE_HIGHWELDLIMIT,
	DDSFREQ_OUTSIDE_LOWWELDLIMIT
};
//An object of this structure keeps the event description
//in string format corresponding to an event ID
struct DCPEvent
{
	char * eventStr;
	int eid;
};

enum ClearLog
{
	clearAll,
	clearEvent,
	clearAlarm,
};

struct DCPLOGInfo
{
	//A sort of checksum to determine that whether the LOG is ever been
	//written or not, should be stored here.
	unsigned int logInitialized;

	//Data below should go into in FRAM.
	//the index which moves between 0 and MAX_EVENT_ENTRIES - 1
	//and always points to the latest entry made in eventlogObjs array
	//defined below i.e. to the latest event occurred.
	int circularIndexEvent;

	//the index which moves between 0 and MAX_ALARM_ENTRIES - 1
	//and always points to the latest entry made in alarmlogObjs array
	//defined below i.e. to the latest alarm occurred.
	int circularIndexAlarm;

	//Number of Events to read
	unsigned int nEvents;

	//Number of Alarm Events to read
	unsigned int nAlarms;
};

//This is the interface class to access and store DCP events
class Log
{
	static DCPLOGInfo LOGInfo;
	static char * GetEventStringFromID(DCPEventID eventID);
	static char * GetAlarmStringFromID(DCPAlarmID alarmID);
	static int GetEventNumber(int logIndx);
	static int GetAlarmNumber(int logIndx);
public:
	static void Init(void);
	static void ResetLog(ClearLog type = clearAll);
	static void WriteEventLog(DCPEventID eventID, short Data1, short Data2, unsigned int userID = 0, bool data = false);
	static void WriteAlarmLog(DCPAlarmID alarmID, short Data1, short Data2, unsigned int userID = 0);
	static int ReadEventLog(LogData * logData, int logIndex, int nLogs);
	static int ReadAlarmLog(LogData * logData, int logIndex, int nLogs);
	static int GetLatestEventEntryIndex();
	static int GetLatestAlarmEntryIndex();
	static DCPLog eventlogObjs[MAX_EVENT_ENTRIES];
	static DCPLog alarmlogObjs[MAX_ALARM_ENTRIES];
};
int SecondsSincePowerUp();
void CalculateTimeSincePowerUp(unsigned long time, struct LogTime *tm);
#endif /* LOG_H_ */
