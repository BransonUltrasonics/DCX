/*
 $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/DcxManagerHandler.h_v   1.5   25 Oct 2013 11:59:12   ygupta  $
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
 $Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/DcxManagerHandler.h_v  $
 * 
 *    Rev 1.5   25 Oct 2013 11:59:12   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.4   24 Aug 2013 17:13:58   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.3   10 Jul 2013 07:49:34   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.2   08 Jul 2013 08:52:28   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.1   14 Jun 2013 11:21:06   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.0   21 May 2013 12:30:54   amaurya
 * Initial revision.
 */

#ifndef DCXMANAGERHANDLER_H_
#define DCXMANAGERHANDLER_H_
#include "TYP_DEF.h"
#include "LOG.h"
#include "WeldResultHistory.h"

#define MAX_JSONOBJSPERRESPONSE		150
#define MAX_WELDDATA_JSONBUFFLEN    200
#define MAX_WELDRESULT_JSONBUFFLEN  400
#define MAX_PRESET_JSONBUFFLEN      1500
#define MAX_ALARM_JSONBUFFLEN       200
#define MAX_CYCLENUM_JSONBUFFLEN    100

enum WeldDataElementIDs{
   WELDDATA_TIME = 1,
   WELDDATA_FREQUENCY,
   WELDDATA_PWM ,
   WELDDATA_AMPLITUDE,
   WELDDATA_CURRENT,
   WELDDATA_POWER,
   WELDDATA_PHASE,
   WELDDATA_ANALOGIN1,
   WELDDATA_ANALOGIN2,
   WELDDATA_AMLITUDEIN,
   WELDDATA_FREQUENCYIN,
};

//struct DCXWeldData
//{
//	SINT32 Time;
//	SINT32 Frequency;
//	SINT32 PWM;
//	SINT32 Amplitude;
//	SINT32 Current;
//	SINT32 Power;
//	SINT32 Phase;
//	SINT32 Ain1;
//	SINT32 Ain2;
//};

enum WeldResultElementIDs{
   WELDRESULT_CYCLECOUNTER = 1,
   WELDRESULT_TS, //(TIME IN HHMMSS AND DATE IN MMDDYY FORMAT)
   WELDRESULT_WELDTIME,
   WELDRESULT_WELDENERGY,
   WELDRESULT_PEAKPOWER,
   WELDRESULT_AMPLITUDE1,
   WELDRESULT_AMPLITUDE2,
   WELDRESULT_PRESETNUMBER,
   WELDRESULT_WELDSTARTFREQUENCY,
   WELDRESULT_WELDSTOPFREQUENCY,
   WELDRESULT_FREQUENCYCHANGE,
   WELDRESULT_ERRORPWM,
   WELDRESULT_ERRORREASON,
   WELDRESULT_AVGAMPLITUDE1,
   WELDRESULT_AVGAMPLITUDE2,
   WELDRESULT_CUSTOMAIN1,
   WELDRESULT_CUSTOMAIN2,
};

//The possible JSON objects need to be sent to DCX Manager
enum JSONObjectType
{
	JSON_WELDDATA,
	JSON_PRESETDATA ,
	JSON_WELDRESULTDATA,
	JSON_ALARMDATA,
	JSON_CYCLENUMBERDATA,
};

//struct WeldResult

//PresetElementIDs and HSPresetElementIDs
//struct DcxPreset

typedef SINT8 * (*JsonGetCmdHandler)(ip_addr PeerAddr, SINT8 * ServiceArgs);
typedef SINT8 * (*JsonSetCmdHandler)(ip_addr PeerAddr, SINT8 * JsonStr, SINT8 * ServiceArgs);
struct JSONGetHandler
{
	SINT8 * ServiceName;
	JsonGetCmdHandler Handler;
};

struct JSONSetHandler
{
	SINT8 * ServiceName;
	JsonSetCmdHandler Handler;
};

class JSON
{
public:
	//CreateJsonObj is a pure virtual function which is implimented by derived classes
	virtual void CreateJsonObj(void) = 0;
	virtual JSONObjectType GetType() = 0;
};

class JSONWeldData: public JSON
{
public:
	char Jsonbuff[MAX_WELDDATA_JSONBUFFLEN];
	virtual void CreateJsonObj(void);
	JSONObjectType GetType(){
		return JSON_WELDDATA;
	}
};

class JSONPreset: public JSON
{
public:
	char Jsonbuff[MAX_PRESET_JSONBUFFLEN];
	virtual void CreateJsonObj(void);
	JSONObjectType GetType(){
		return JSON_PRESETDATA;
	}

};

class JSONWeldResult: public JSON
{
public:
	char Jsonbuff[MAX_WELDRESULT_JSONBUFFLEN];
	UINT32 PsCycNum;
	virtual void CreateJsonObj(void);
	JSONObjectType GetType(){
		return JSON_WELDRESULTDATA;
	}
};

class JSONAlarm: public JSON
{
public:
	char Jsonbuff[MAX_ALARM_JSONBUFFLEN];
	virtual void CreateJsonObj(void);
	JSONObjectType GetType(){
		return JSON_ALARMDATA;
	}
};

class JSONCycleNumber: public JSON
{
public:
	char Jsonbuff[MAX_CYCLENUM_JSONBUFFLEN];
	virtual void CreateJsonObj(void);
	JSONObjectType GetType(){
		return JSON_CYCLENUMBERDATA;
	}
};



class DcxMangerHandler
{
public:
	static void InitDcxMangerHandler(SINT32 Intf, SINT32 SrvrPort);
	//JSON Handlers for specific service
	static SINT8 * JSONGetHandler(ip_addr PeerAddr, SINT8 * ServiceArgs);
	static SINT8 * JSONStopHandler(ip_addr PeerAddr, SINT8 * ServiceArgs);
	//JSON set data handlers for specific service
	static SINT8 * JSONSetHandler(ip_addr PeerAddr, SINT8 * JsonStr, SINT8 * ServiceArgs);
};

BOOLEAN JSONHandler(SINT8 * ServiceURI, HTTPReplyHeader & Header, SINT8 * JsonStr, ip_addr PeerAddr);
extern JSONGetHandler JsonGetCmdHandlers[];
extern JSONSetHandler JsonSetCmdHandlers[];
extern PtrList<JSON> JsonDataList;
void WriteCycleCounterForDCXM(BOOLEAN Cycle_flag = TRUE, BOOLEAN AddToHead = FALSE, SINT32 CycleNum = 0, BOOLEAN ForceWrite = FALSE,
		SINT32 * LstCnt = 0, BOOLEAN IncPsCycCount = TRUE);
void WritePresetForDCXM(BOOLEAN ForceWrite = FALSE);
void WriteWeldResultForDCXM(WeldResult *Weldresult, UINT32 CurrActPsCycCount);
void WriteAlarmForDCXM(SINT32 * LstCnt = 0, BOOLEAN AddToHead = FALSE);
void WriteWeldDatForDCXM(WeldData * WData, UINT32 TimeSinceStartOfCycle);
void ClearJsonObjects();
void InitJsonObjsKeys();
void CycleDataSend(ip_addr Dcxmaddr, SINT32 * LstCnt = 0);
extern BOOLEAN EnableJSONLogging;
extern UINT32 SampleRate;
extern BOOLEAN SendPresetToDCXM;
extern SINT32 FreqIn, AmplitudeIn;
extern UINT32 JSonServiceTimeOut;
#endif /* DCXMANAGERHANDLER_H_ */
