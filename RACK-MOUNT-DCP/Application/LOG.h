/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/LOG.h_v   1.3.2.5   30 Oct 2015 08:52:28   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/LOG.h_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added HORNSCAN_FAILED alarm ID
 *
 *    Rev 1.3.2.5   30 Oct 2015 08:52:28   sesharma
 * Added Parameter change event for Handheld Mode
 * 
 *    Rev 1.3.2.4   01 Sep 2015 02:35:20   amaurya
 * Added SetDigTuneWithScan and ClrMemAtPwrUp to HSPresetElementIDs
 * 
 *    Rev 1.3.2.3   25 Jun 2015 07:06:08   amaurya
 * Added Event enum IDs for Weld Preset parameter.
 * 
 *    Rev 1.3.2.2   04 Jun 2015 07:47:26   sesharma
 * AC Power lost alarm added to DCPAlarmID enum.
 * Data type of Data1 & Data2 parameters changed to SINT32 in structures: DCPLog & LogData and Functions: WriteEventLog & WriteAlarmLog. Branson coding standard formatter applied.
 * 
 *    Rev 1.3.2.1   19 Mar 2015 06:55:00   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3.2.0   22 Jan 2015 04:59:14   sesharma
 * Bug Fixing for Large Negative value of Alarm No. in Alarm log.
 * 
 *    Rev 1.3   07 Nov 2014 15:43:44   ewomack
 * Alarms added for Power Window Limits and Timeout in Energy Mode.
 * 
 *    Rev 1.2   25 Sep 2014 03:34:40   amaurya
 * Corrected the frequency value in event log for 40Khz.
 * 
 *    Rev 1.1   17 Sep 2014 08:43:14   amaurya
 * Code cleanup, code review changes
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#ifndef LOG_H_
#define LOG_H_

#include "portable.h"

//This structure represents a time stamp
struct LogTime {
   //seconds in 24 hour clock
   SINT32 tm_sec;
   //minutes in 24 hour clock
   SINT32 tm_min;
   //hours in 24 hour clock
   SINT32 tm_hour;
   //The day of the month
   SINT32 tm_day;
   //The month of the year
   SINT32 tm_mon;
   //The year in Gregorian calendar systems
   SINT32 tm_year;
};

//Maximum number of events which can be saved in FRAM
#define MAX_EVENT_ENTRIES 200
//Maximum number of alarms which can be saved in FRAM
#define MAX_ALARM_ENTRIES 100

//An object of this structure represents a DCP event
//This is the one needs to be saved in FRAM.
struct DCPLog {
   //number of seconds since the system is started.
   UINT32 timeStamp;
   //Unique identification of an event.
   short eventID;
   //Diagnostic purpose data 1 related of the event
   SINT32 data1;
   //Diagnostic purpose data 2 related of the event.
   SINT32 data2;
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
struct LogData {
   char logStr[MAX_LOG_LENGTH];
   char timeStamp[MAX_TIMESTAMP_LENGTH];
   SINT32 data1;
   SINT32 data2;
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
enum DCPEventID {
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
   WATCHDOG_RESET,
   VERSION_CHANGE,
   CRC_CHANGE,
   CLOCK_LOST,
   GET_FREQ,
   POST_FREQ,
   POST_INVALID,
   GET_INVALID,
   POWER_SHUTDOWN,
   PROGRAM_FLASH,
   BAD_SERIALFORMAT,
   EVENT_ACTIVELOWDIGIN,
   EVENT_INPUTCABLEFOUND,
   EVENT_INPUTCABLELOST,
   EVENT_PROGRAMFIRMWARE,
   EVENT_RESTORE_DEFAULTS,
   EVENT_LOGEND   //Any new Event must be added before this
};
//The IDs of alarms
enum DCPAlarmID {
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
   DDSFREQ_OUTSIDE_LOWWELDLIMIT,
   PEAKPOWER_MINUS_LIMIT,
   PEAKPOWER_PLUS_LIMIT,
   ENERGYREACHED_TIMEOUT,
   ACPOWER_LOST,
   HORNSCAN_FAILED
};
//An object of this structure keeps the event description
//in string format corresponding to an event ID
struct DCPEvent {
   char* eventStr;
   SINT16 eid;
};

enum ClearLog {
   clearAll, clearEvent, clearAlarm,
};
//enum DPSPresetId represent parameter no to log when corresponding parameter is changed either from Webpage or LCD.
enum PresetElementIDs
{
	ActParamCustomAIn1Cutoff = 1,
	ActParamCustomAIn1CutoffFlag,
    ActParamCustomAIn2Cutoff,
    ActParamCustomAIn2CutoffFlag,
	DcpParamWeldAmplitudeLoopC1,
	DcpParamWeldAmplitudeLoopC2,
	DcpParamWeldAmplitudeLoopC1new,
	DcpParamWeldAmplitudeLoopC2new,
	DcpParamMemoryStoreFlag,  //9
	DcpParamWeldPhaseLimit,
	DcpParamWeldPhaseLimitTime,
	DcpParamWeldPhaseLoopI,
	DcpParamWeldPhaseLoopCF,
	DcpParamWeldPhaseLoopInew,
	DcpParamWeldPhaseLoopCFnew,
	DcpParamUseTrueValues,    //16
	DcpParamWeldPhaseLoopP,
	DcpParamFreqWindowSize,
	DcpParamWeldFreqHigh,
	DcpParamWeldFreqLow,
	DcpParamTimedSeekFlag,
	DcpParamWeldRampTime,  //22
	DcpParamWeldProfileRampTime,//23
	//info
	InfoParamCreatedDate,
	InfoParamCreatedHour,
	InfoParamCreatedMinute,
	InfoParamCreatedMonth, //27
	InfoParamCreatedYear,
	InfoParamHardwareConfig,
	InfoParamHornPresetNumber,
	InfoParamModifedDate,
	InfoParamModifiedHour, //32
	InfoParamModifiedMinute,
	InfoParamModifiedMonth,
	InfoParamModifiedYear,
	InfoParamOperatorID,
	InfoParamPowerSupplyFreq, //37
	InfoParamPowerSupplyWatts,
	InfoParamPresetStatus,
	InfoParamWeldCount,
	//limit
	LimitsParamControlLimits,
	LimitsParamEnergyMLR,  //42
	LimitsParamEnergyPLR,
//	LimitsParamFreqMLR,
//	LimitsParamFreqPLR,
	LimitsParamPowerMLR,
	LimitsParamPowerPLR,   //45
	LimitsParamWeldTimeMLR,
	LimitsParamWeldTimePLR,
	//wc
	WcParamABAmp,
	WcParamABDelay,
	WcParamABFlag,   //50
	WcParamABTime,
	WcParamAmplitude1,
	WcParamAmplitude2,
	WcParamAmpStepCondition,
	WcParamAmpStepTime, //55
	WcParamAmpStepEnergy,
	WcParamAmpStepExternal,
	WcParamAmpStepPower,
	WcParamCycleAborts,
	WcParamEBAmp, //60
	WcParamEBFlag,
	WcParamEBTime,
	WcParamEnergyCP,
	WcParamEnergyComp,
	WcParamEnergyCompFlag, //65
	WcParamEnergyCutoff,
	WcParamEnergyCutoffFlag,
	WcParamExtTrigDelayFlag,
	WcParamExtTrigDelayTime,
	WcParamFreqOffset,  //70
	WcParamFreqOffsetFlag,
	WcParamGeneralAlarmFlag,
	WcParamGrdDetectFlagCP,
	WcParamHoldTime,
	WcParamHoldTimeFlag,   //75
	WcParamMaxWeldTime,
	WcParamPeakPowerCP,
	WcParamPeakPowerCutoff,
	WcParamPeakPowerCutoffFlag,
	WcParamPostWeldSeekFlag,  //80

	WcParamScrubAmp = 85, //85
	WcParamScrubTime,
	WcParamScrubTimeFlag,
	WcParamTimeCutOff,
	WcParamTimeCutOffFlag,
	WcParamWeldMode, //90
	WcParamWeldTimeCP,
	WcParamCutOffEnabled,
	WcParamWeldFreqHighCutOffFlag,
	WcParamWeldFreqLowCutOffFlag,
	WcParamWeldFreqHighCutoff = 96,
	WcParamWeldFreqLowCutoff,//97
};

enum HSPresetElementIDs
{
	//hs seek
	ParaSeekSeekTime = WcParamWeldFreqLowCutoff + 1,//98
	ParaSeekSeekAmp,
	ParaSeekSeekRampTime,
	ParaSeekSeekAmpLoopC1,
	ParaSeekSeekAmpLoopC2,
	ParaSeekFreqOffset,
	ParaSeekSeekPhaseLoopI,
	ParaSeekSeekPhaseLoopCF,//105
	ParaSeekSeekFreqHigh,
	ParaSeekSeekFreqLow,
	//hs test
	ParaTestAmplitude1,
	ParaTestAmpStepCondition,
	ParaTestAmplitudeProfileTime,
	ParaTestAmplitude2,//111
	ParaTestAmplitudeStartRampTime,
	ParaTestAmplitudeProfileRampTime,
	ParaTestAmplitudeLoopC1,
	ParaTestAmplitudeLoopC2,//115
	ParaTestFreqOffset,
	ParaTestPhaseLoopI,
	ParaTestPhaseLoopCF,//118
	ParaTestWeldFreqHigh,
	ParaTestWeldFreqLow,
	ParaTestTimeErrorHigh,
	//hs scan
	ParaScanFrequencyStart,//122
	ParaScanFrequencyStop,
	ParaScanFrequencyStep,
	ParaScanTimeDelay,//125
	ParaScanMaxAmplitude,
	ParaScanMaxCurrent,
	ParaScanTimeErrorHigh,//128
	DigitalTune,//129
	PowerUpOptions,	//130
	AlarmResetReq,
	Jumper2,
	ClearMemAtRst,
	ClrMemBfrSeek,
	PowerWindowLimitLow,	//135
	PowerWindowLimitHigh,
	PlusLimitCheck,
	MinusLimitCheck,
	PowerLimitOn,
	SetDigTuneWithScan, // 140	To log parameter change event for Set with Horn Scan flag.
	ClrMemAtPwrUp,		// 141 To log parameter change event for Clear memory at power up flag.
	HandHeldMode	// 142 To log parameter change event for Switch between Time pulse and Hand held mode.
};


struct DCPLOGInfo {
   //A sort of checksum to determine that whether the LOG is ever been
   //written or not, should be stored here.
   UINT32 logInitialized;
   
   //Data below should go into in FRAM.
   //the index which moves between 0 and MAX_EVENT_ENTRIES - 1
   //and always points to the latest entry made in eventlogObjs array
   //defined below i.e. to the latest event occurred.
   SINT32 circularIndexEvent;
   
   //the index which moves between 0 and MAX_ALARM_ENTRIES - 1
   //and always points to the latest entry made in alarmlogObjs array
   //defined below i.e. to the latest alarm occurred.
   SINT32 circularIndexAlarm;
   
   //Number of Events to read
   UINT32 nEvents;
   
   //Number of Alarm Events to read
   UINT32 nAlarms;
};

//This is the interface class to access and store DCP events
class Log {
      static DCPLOGInfo LOGInfo;
      static char* GetEventStringFromID(DCPEventID eventID);
      static char* GetAlarmStringFromID(DCPAlarmID alarmID);
      static SINT32 GetEventNumber(int logIndx);
      static UINT32 GetAlarmNumber(int logIndx);
   public:
      static void Init(void);
      static void ResetLog(ClearLog type = clearAll);
      static void WriteEventLog(DCPEventID eventID, SINT32 Data1, SINT32 Data2,
            UINT32 userID = 0, BOOLEAN data = false);
      static void WriteAlarmLog(DCPAlarmID alarmID, SINT32 Data1, SINT32 Data2,
            UINT32 userID = 0);
      static SINT32 ReadEventLog(LogData * logData, SINT32 logIndex,
            SINT32 nLogs);
      static SINT32 ReadAlarmLog(LogData * logData, SINT32 logIndex,
            SINT32 nLogs);
      static SINT32 GetLatestEventEntryIndex();
      static SINT32 GetLatestAlarmEntryIndex();
      static DCPLog eventlogObjs[MAX_EVENT_ENTRIES];
      static DCPLog alarmlogObjs[MAX_ALARM_ENTRIES];
};
SINT32 SecondsSincePowerUp();
void CalculateTimeSincePowerUp(UINT32 time, struct LogTime *tm);
void CalculateDaysAndHoursSincePowerUp(UINT32 time, struct LogTime *tm);
#endif /* LOG_H_ */
