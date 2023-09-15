/*
 $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/DcxManagerHandler.cpp_v   1.2   21 Aug 2015 05:49:14   RJamloki  $
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
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/DcxManagerHandler.cpp_v  $
 * 
 *    Rev 1.2   21 Aug 2015 05:49:14   RJamloki
 * Fixes for coverity CID 12578, 11235 and 12579
 * 
 *    Rev 1.1   24 Jul 2015 08:34:28   sesharma
 * Added int argument in JSONHandler function to pass content length of post .
 * 
 *    Rev 1.0   09 Jun 2015 09:10:36   tayars
 * Initial revision.
 * 
 *    Rev 1.15   13 Mar 2014 15:53:54   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 *
 */
#include "DcxManagerHandler.h"
#include "stdio.h"
#include "FIFO.h"
#include "RTOS.h"
#include "WeldResultHistory.h"
#include "Verifypreset.h"
#include "UserIO.h"
#include "Alarms.h"
#include "GLOBALS.h"
#include "CyGOS.h"
#include "Preset.h"
#include "FUNC.h"
#include "FieldBus.h"

#define pprintf
#define printf
#define MAX_TS_SIZE 24//maximum Time stamp string length. This does not include html files
#define MAX_KEY_SIZE 5 //maximum KeyID size
#define DEFAULT_SAMPLE_RATE 100

extern UINT16 WeldBufferTime;
extern DcxPreset RunningPreset;
PtrList<JSON> JsonDataList;
extern SINT8 * Response;
extern SINT16  CurrentIndex;
ip_addr ConnectedDcxmAddr;
UINT32 JSonServiceTimeOut = 0;
SINT8 **KeyWeldData ;
SINT8 **KeyPreset;
SINT8 **KeyWeldResult;
BOOLEAN EnableJSONLogging = FALSE;
UINT32 SampleRate = DEFAULT_SAMPLE_RATE;
BOOLEAN SendPresetToDCXM;
UINT32 JsonWeldBuffTime;
UINT32 LastCycleSent, LastPsCycleSent;
BOOLEAN  MultipleCyleAdded, ObjAdded;
extern BOOLEAN ClearJSONObjects;
BOOLEAN UpdateLastCycleSent = FALSE;
UINT32 ActPresetCycleCount = 0;
int ResLen = 0;
#define JSON_IDVALUESEPERATOR  		 ','
#define JSON_STARTOFARRAY           '['
#define JSON_ENDOFARRAY             ']'
#define JSON_STARTOFOBJECT          '{'
#define JSON_ENDOFOBJECT            '}'
#define JSON_OBJECTSEPERATOR        ','

const SINT8 * KeyWeldDataUnassignedAIN[] = {"51", "52", "0"};
SINT8 * EndWeldData;


//Array of GET service name and corresponding handler
JSONGetHandler JsonGetCmdHandlers[] =
{
		{"/GetDcxData", DcxMangerHandler::JSONGetHandler},
		{"/StopDcxData", DcxMangerHandler::JSONStopHandler},
		{0, 0}
};

//Array of POST service name and corresponding handler
//Post services are currently not required between DCXManager and DCX unit.

JSONSetHandler JsonSetCmdHandlers[]  =
{
		{"/SetDcxData", DcxMangerHandler::JSONSetHandler},
		{0, 0}
};

/* void DcxMangerHandler::InitDcxMangerHandler(SINT32 Intf, SINT32 SrvrPort)
 *
 *  Purpose:
 *    This function assigns the handler functions for JSON.This Function should be called once at powerup.
 *
 *  Entry condition: Intf- The index of Ethernet module of processor
 *                   SrvrPort-The HTTP port this website belongs to.
 *
 *  Exit condition: None.
 */
void DcxMangerHandler::InitDcxMangerHandler(SINT32 Intf, SINT32 SrvrPort)
{
	//Add JSON handler
	CyGOS::AddJSONHandler(Intf, SrvrPort, JSONHandler);
	if(Sysconfig->ActivePreset < MAX_TOTAL_PRESETS)
		LastPsCycleSent = ActPresetCycleCount = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;
	InitJsonObjsKeys();
}

/* SINT8 * DcxMangerHandler::JSONGetHandler(ip_addr PeerAddr, SINT8 * ServiceArgs)
 * Purpose:
 * This is a handler function for GetDcxdata service. Using JsonObj pointer of JSON type we are traversing the ptrlist.
 * Depending on JsonObj type corresponding CreateJsonObj() will be called.
 *  Entry condition:
 *		PeerAddr : Address of the Client requesting data through JSON URI
 *		ServiceArgs: Extra data received with service
 *  Exit condition:
 *    The pointer to buffer containing the Response in JSON format
 */

SINT8 * DcxMangerHandler::JSONGetHandler(ip_addr PeerAddr, SINT8 * ServiceArgs)
{
	int MaxObjs = MAX_JSONOBJSPERRESPONSE;
	SINT8 * ResPtr = Response, *Temp;
	SINT32 ListCount = 0;
	JSON * HeadObj;
	MultipleCyleAdded = FALSE;
	EndWeldData = 0;
	UINT32 NewSampleRate = 0;
	ResLen = 0;
	ObjAdded = FALSE;
	UpdateLastCycleSent = TRUE;
	if((PeerAddr.addr == ConnectedDcxmAddr.addr) && (ClearJSONObjects == FALSE) && ServiceArgs){//Coverity CID: 12578
		 JSonServiceTimeOut = 0;
		 ListCount = JsonDataList.GetCount();
		 Temp = strstr(ServiceArgs, "SR=");
		 if(Temp){
			Temp = Temp + strlen("SR=");
			NewSampleRate = (UINT32)atoi(Temp);
			if((NewSampleRate > 0) && (SampleRate != NewSampleRate)){
				SampleRate = NewSampleRate;
				printf("\n New Sample rate %d", NewSampleRate);
			}
		 }
		 if(EnableJSONLogging == FALSE){
//			 printf("\n Enable Logging");
			 LastCycleSent = 0;
			 CycleDataSend(PeerAddr, &ListCount);
			 EnableJSONLogging = TRUE;
		 }
		if(ListCount == 0){
			 UpdateLastCycleSent = FALSE;
			 WriteAlarmForDCXM(&ListCount, TRUE);
			 WriteCycleCounterForDCXM(FALSE, TRUE, 0, FALSE, &ListCount, FALSE);
		}
		else{
			HeadObj = JsonDataList.GetHead();
			if((HeadObj) &&  (HeadObj->GetType() != JSON_CYCLENUMBERDATA)){
				if(LastCycleSent > 0)
					WriteCycleCounterForDCXM(TRUE , TRUE, LastCycleSent, FALSE, &ListCount, FALSE);
				else
					WriteCycleCounterForDCXM(FALSE, TRUE, 0, FALSE, &ListCount, FALSE);//No cycle alarm case
			}
		}

		POSITION pos = JsonDataList.GetHeadPosition();
		Response[ResLen++] = JSON_STARTOFARRAY;
		Response[ResLen++] = JSON_STARTOFOBJECT;
		Response[ResLen] = 0;
		while (pos && (MaxObjs > 0) && (ListCount > 0))
		{
			POSITION last = pos;
			JSON * JsonObj = JsonDataList.GetNext(pos);
			JsonObj->CreateJsonObj();
			JsonDataList.RemoveAt(last);
			delete JsonObj;
			--MaxObjs;
			--ListCount;
			ObjAdded = TRUE;
		}

		if(EndWeldData)
			*EndWeldData = JSON_ENDOFARRAY;

		Response[ResLen++] = JSON_ENDOFOBJECT;
		if(MultipleCyleAdded == TRUE)
			Response[ResLen++] = JSON_ENDOFARRAY;
		else
			ResPtr = Response + 1;

		Response[ResLen] = 0;
	}
	else
		strcpy(Response,"{}");
	return ResPtr;
}


/* SINT8 * DcxMangerHandler::JSONGetHandler()
 * Purpose:
 * This is a handler function for StopDcxdata service.
 *  Entry condition:
 *		PeerAddr : Address of the Client requesting stop accumulating the data and clearing the accumulated data
 *		ServiceArgs: Extra data received with service
 *  Exit condition:
 *    The pointer to buffer containing the Response in JSON format
 */
SINT8 * DcxMangerHandler::JSONStopHandler(ip_addr PeerAddr, SINT8 * ServiceArgs)
{
	SINT8 * ResPtr = Response;
	EnableJSONLogging = FALSE;//First Disable logging
	ClearJSONObjects = TRUE;//Invoke clearing objects from WelderSearch task
	strcpy(ResPtr, "{\"Status\":\"Stopped\"}");
	ServiceArgs = 0;
	return ResPtr;
}


/* SINT8 * DcxMangerHandler::JSONSetHandler(SINT8 * JsonStr, ip_addr PeerAddr)
 * Purpose:
 * JSON set data handlers for specific service.
 *  Entry condition:
 *  	PeerAddr:  Address of the Client requesting/posting data through JSON URI
 *		JsonStr :  Data from POST service in JSON format.
 *		ServiceArgs: Extra data received with service
 *  Exit condition:
 *    The pointer to buffer containing the Response in JSON format
 */
										//coverity CID: 11235
SINT8 * DcxMangerHandler::JSONSetHandler(ip_addr , SINT8 * JsonStr, SINT8 * ServiceArgs)
{
	SINT8 * ResPtr = Response;
	JsonStr = 0;
	ServiceArgs = 0;
	strcpy(ResPtr, "[\"Notfound\" , \"Notfound\"]");
	return ResPtr;
}

/* void CreateJsonObj(void)
 * Purpose:
 * This Function is called by JSONGetHandler().
 * It is used to create object for WeldData JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *    None
 */

void JSONWeldData::CreateJsonObj(void)
{
	if(EndWeldData == 0){
		strcat(Response + ResLen , ",\"WeldData\":[");
		ResLen += strlen(",\"WeldData\":[");
	}
	strcat(Response + ResLen, Jsonbuff);
	ResLen += strlen(Jsonbuff);
	strcat(Response + ResLen, ",");
	EndWeldData = Response +  ResLen;
	ResLen++;
}


/* void CreateJsonObj(void)
 * Purpose:
 * This Function is called by JSONGetHandler().
 * It is used to create object for Preset JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *    None
 */

void JSONPreset::CreateJsonObj(void)
{
	strcat(Response + ResLen, Jsonbuff);
	ResLen += strlen(Jsonbuff);
}
/* void JSONWeldResult::CreateJsonObj(void)
 * Purpose:
 * This Function is called by JSONGetHandler().
 * It is used to create object for WeldResult JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *    None
 */

void JSONWeldResult::CreateJsonObj(void)
{
	if(EndWeldData){
		*EndWeldData++ = JSON_ENDOFARRAY;
		*EndWeldData = 0;
		 EndWeldData = 0;
	}
	LastPsCycleSent = PsCycNum;
	strcat(Response + ResLen, Jsonbuff);
	ResLen += strlen(Jsonbuff);
}

/* void JSONWeldResult::CreateJsonObj(void)
 * Purpose:
 * This Function is called by JSONGetHandler().
 * It is used to create object for Alarm JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *      None
 */
void JSONAlarm::CreateJsonObj(void)
{
	if(EndWeldData){
		*EndWeldData++ = JSON_ENDOFARRAY;
		*EndWeldData = 0;
		 EndWeldData = 0;
	}
	strcat(Response + ResLen, Jsonbuff);
	ResLen += strlen(Jsonbuff);
}

/* void JSONWeldResult::CreateJsonObj(void)
 * Purpose:
 * This Function is called by JSONGetHandler().
 * It is used to create object for CycleNumber JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *      None
 */

void JSONCycleNumber::CreateJsonObj(void)
{
	SINT8 *Cycend = 0, * PscycEnd = 0, *Temp =  strstr(Jsonbuff,"\"cycle\":"), *Temp1 = strstr(Jsonbuff,"\"pscycle\":");
	if(EndWeldData){
		*EndWeldData++ = JSON_ENDOFARRAY;
		*EndWeldData = 0;
		 EndWeldData = 0;
	}
	if(ObjAdded == TRUE){
		MultipleCyleAdded = TRUE;
		Response[ResLen++] = JSON_ENDOFOBJECT;
		Response[ResLen++] = JSON_OBJECTSEPERATOR;
		Response[ResLen++] = JSON_STARTOFOBJECT;
		Response[ResLen] = 0;
	}

	if(UpdateLastCycleSent){
		if(Temp){
			Cycend = strchr((Temp), ':');
			LastCycleSent = atoi(Cycend + 1);
			if(Temp1){
				PscycEnd = strchr((Temp1), ':');
				LastPsCycleSent = atoi(PscycEnd + 1);
			}
		}
	}
	//pprintf("LCS = %d ",LastCycleSent);
	strcat(Response + ResLen, Jsonbuff);
	ResLen += strlen(Jsonbuff);
}

/* void WriteCycleCounterForDCXM(BOOLEAN Cycle_flag, BOOLEAN AddToHead, SINT32 CycleNum)
 * Purpose:
 * This Function is called from Statefunction by ReadyPassACT() , ReadyPassNoACT() and CycleDataSend().
 * It is used to write the cycle number to the Jsonbuff buffer and this buffer is added to the JsonDataList list.
 *  Entry condition:
 *		Cycle_flag:Cycle_flag default value is TRUE.
 *		AddToHead: AddToHead default value is FALSE.This flag will be TRUE when Cycle Number will be write other than statemachine as we need cycle number at the list head all time.
 *		CycleNum:
 *  Exit condition:
 *      None
 */
void WriteCycleCounterForDCXM(BOOLEAN Cycle_flag, BOOLEAN AddToHead, SINT32 CycleNum, BOOLEAN ForceWrite,
		SINT32 * LstCnt, BOOLEAN IncPsCycCount)
{
	SINT32 CycNum = 0;
	if(EnableJSONLogging || ForceWrite){
		if(Cycle_flag == TRUE){
			if(CycleNum > 0)
				CycNum = CycleNum;
			else
				CycNum = SysCounters->LifeTimeCycleCount + 1;
		}
		 JSONCycleNumber *Cycleno = new JSONCycleNumber();
		 sprintf(Cycleno->Jsonbuff,"\"cycle\":%lu,\"pscycle\":%lu",CycNum,
				 IncPsCycCount?(ActPresetCycleCount + 1): LastPsCycleSent);
		  if(AddToHead){
			 JsonDataList.AddHead(Cycleno);
			 if(LstCnt)
			  *LstCnt += 1;
		 }
		 else{
			 JsonDataList.AddTail(Cycleno);
			 if(LstCnt)
			 	*LstCnt += 1;
		 }
	}
}

/* void WriteWeldResultForDCXM(WeldResult *WRes)
 * Purpose:
 * This Function is called by WriteWeldHistory() when SendtoDcxM flag is TRUE to write the weld result data to Jsonbuff buffer.
 * It is used to create object for CycleNumber JSONObjectType.
 *  Entry condition:
 *		WRes : Pointer to WeldResult structure.
 *  Exit condition:
 *      None
 */

void WriteWeldResultForDCXM(WeldResult *WRes, UINT32 PsCycNum)
{
	SINT32 Indx = 0;
	UINT8 AlarmIndexTemp;
	if(EnableJSONLogging){

		JSONWeldResult *ObjWeldResult = new JSONWeldResult();
		ObjWeldResult->PsCycNum = PsCycNum;
	    sprintf(ObjWeldResult->Jsonbuff,
                  ",\"Result\":"
                  "{\"%s\":%ld,"
                  "\"%s\":\"%02d-%02d-%02d %02d:%02d:%02d\","
                  "\"%s\":%ld,"
                  "\"%s\":%ld,"
                  "\"%s\":%d,"
                  "\"%s\":%d,"
                  "\"%s\":%d,"
                  "\"%s\":%d,"
                  "\"%s\":%ld,"
                  "\"%s\":%ld,"
                  "\"%s\":%ld,"
                  "\"%s\":%ld,"
                  "\"%s\":%d,"
                  "\"%s\":%d,"
                  "\"%s\":%d,"
                  "\"%s\":%d,",
						KeyWeldResult[WELDRESULT_CYCLECOUNTER],WRes->CycleCounter,
					    KeyWeldResult[WELDRESULT_TS],WRes->TS.Month,WRes->TS.Day,WRes->TS.Year,
						WRes->TS.Hour,WRes->TS.Minute,WRes->TS.Second,
						KeyWeldResult[WELDRESULT_WELDTIME], WRes->WeldTime,
						KeyWeldResult[WELDRESULT_WELDENERGY], WRes->WeldEnergy,
						KeyWeldResult[WELDRESULT_PEAKPOWER], WRes->PeakPower,
						KeyWeldResult[WELDRESULT_AMPLITUDE1] ,WRes->Amplitude1,
						KeyWeldResult[WELDRESULT_AMPLITUDE2], WRes->Amplitude2,
						KeyWeldResult[WELDRESULT_PRESETNUMBER],WRes->PresetNumber,
						KeyWeldResult[WELDRESULT_WELDSTARTFREQUENCY],WRes->WeldStartFrequency,
						KeyWeldResult[WELDRESULT_WELDSTOPFREQUENCY],WRes->WeldStopFrequency,
					    KeyWeldResult[WELDRESULT_FREQUENCYCHANGE],WRes->WeldStopFrequency - WRes->WeldStartFrequency,
						KeyWeldResult[WELDRESULT_ERRORPWM],WRes->ErrorPwm,
						KeyWeldResult[WELDRESULT_AVGAMPLITUDE1],WRes->AvgAmplitude1,
						KeyWeldResult[WELDRESULT_AVGAMPLITUDE2],WRes->AvgAmplitude2,
						KeyWeldResult[WELDRESULT_CUSTOMAIN1],WRes->CustomAIn1,
						KeyWeldResult[WELDRESULT_CUSTOMAIN2],WRes->CustomAIn2);
	    AlarmIndexTemp = AlarmIndex;
	    if(AlarmIndexTemp > 0){
	    	sprintf(ObjWeldResult->Jsonbuff+strlen(ObjWeldResult->Jsonbuff),"\"%s\":\"%04X",KeyWeldResult[WELDRESULT_ERRORREASON],AlarmsArray[Indx]);
	    	for(Indx = 1; Indx < AlarmIndexTemp; Indx++)
	   	    sprintf(ObjWeldResult->Jsonbuff+strlen(ObjWeldResult->Jsonbuff),"-%04X",AlarmsArray[Indx]);
	    }
	    else
	    	sprintf(ObjWeldResult->Jsonbuff+strlen(ObjWeldResult->Jsonbuff),"\"%s\":\"%04X",KeyWeldResult[WELDRESULT_ERRORREASON],0);

   	 strcat(ObjWeldResult->Jsonbuff, "\"}");
	    JsonDataList.AddTail(ObjWeldResult);
	}
	ActPresetCycleCount = PsCycNum;
}

/* void WritePresetForDCXM()
 * Purpose:
 * This Function is called by SonicsEntry() and WaitforULSTRSEntry(),CycleDataSend() to write the preset data to Jsonbuff buffer.
 * It is used to create object for CycleNumber JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *      None
 */
void WritePresetForDCXM(BOOLEAN ForceWrite)
{
	SINT8 *PresetNumberKeyID = "129";
	SINT8 *PowerupOptionKeyID = "130";

	if ((EnableJSONLogging && SendPresetToDCXM) || (ForceWrite == TRUE)) {
		JSONPreset *ObjPreset = new JSONPreset();
		sprintf(ObjPreset->Jsonbuff,",\"Preset\":"
			"{\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%d,"
			"\"%s\":%ld,"
			"\"%s\":%ld,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%ld,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%ld,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%d,"
			"\"%s\":%ld,"
			"\"%s\":%lu,"
			"\"%s\":%d,"
			"\"%s\":%d",
				KeyPreset[DcpParamWeldAmplitudeLoopC1], RunningPreset.DcpParam.WeldAmplitudeLoopC1,
				KeyPreset[DcpParamWeldAmplitudeLoopC2], RunningPreset.DcpParam.WeldAmplitudeLoopC2,
				KeyPreset[DcpParamWeldAmplitudeLoopC1new], RunningPreset.DcpParam.WeldAmplitudeLoopC1new,
				KeyPreset[DcpParamWeldAmplitudeLoopC2new], RunningPreset.DcpParam.WeldAmplitudeLoopC2new,
				KeyPreset[DcpParamWeldPhaseLimit], RunningPreset.DcpParam.WeldPhaseLimit,
				KeyPreset[DcpParamWeldPhaseLimitTime], RunningPreset.DcpParam.WeldPhaseLimitTime,
				KeyPreset[DcpParamWeldPhaseLoopI], RunningPreset.DcpParam.WeldPhaseLoopI,
				KeyPreset[DcpParamWeldPhaseLoopCF], RunningPreset.DcpParam.WeldPhaseLoopCF,
				KeyPreset[DcpParamWeldPhaseLoopInew], RunningPreset.DcpParam.WeldPhaseLoopInew,
				KeyPreset[DcpParamWeldPhaseLoopCFnew], RunningPreset.DcpParam.WeldPhaseLoopCFnew,
				KeyPreset[DcpParamWeldPhaseLoopP], RunningPreset.DcpParam.WeldPhaseLoopP,
				KeyPreset[DcpParamFreqWindowSize], RunningPreset.DcpParam.FreqWindowSize,
				KeyPreset[DcpParamWeldFreqHigh], RunningPreset.DcpParam.WeldFreqHigh,
				KeyPreset[DcpParamWeldFreqLow],	RunningPreset.DcpParam.WeldFreqLow,
				KeyPreset[DcpParamWeldRampTime],RunningPreset.DcpParam.WeldRampTime,
				KeyPreset[DcpParamWeldProfileRampTime],	RunningPreset.DcpParam.WeldProfileRampTime,
				KeyPreset[DcpParamMemoryStoreFlag],	RunningPreset.DcpParam.MemoryStoreFlag,
				KeyPreset[DcpParamUseTrueValues], RunningPreset.DcpParam.UseTrueValues,
				KeyPreset[DcpParamTimedSeekFlag], RunningPreset.DcpParam.TimedSeekFlag,
				KeyPreset[InfoParamCreatedDate], RunningPreset.InfoParam.CreatedDate,
				KeyPreset[InfoParamCreatedHour], RunningPreset.InfoParam.CreatedHour,
				KeyPreset[InfoParamCreatedMinute],	RunningPreset.InfoParam.CreatedMinute,
				KeyPreset[InfoParamCreatedMonth], RunningPreset.InfoParam.CreatedMonth,
				KeyPreset[InfoParamCreatedYear], RunningPreset.InfoParam.CreatedYear,
				KeyPreset[InfoParamHardwareConfig],	RunningPreset.InfoParam.HardwareConfig,
				KeyPreset[InfoParamHornPresetNumber], RunningPreset.InfoParam.HornPresetNumber,
				KeyPreset[InfoParamModifedDate], RunningPreset.InfoParam.ModifiedDate,
				KeyPreset[InfoParamModifiedHour], RunningPreset.InfoParam.ModifiedHour,
				KeyPreset[InfoParamModifiedMinute], RunningPreset.InfoParam.ModifiedMinute,
				KeyPreset[InfoParamModifiedMonth], RunningPreset.InfoParam.ModifiedMonth,
				KeyPreset[InfoParamModifiedYear], RunningPreset.InfoParam.ModifiedYear,
				KeyPreset[InfoParamPowerSupplyFreq], RunningPreset.InfoParam.PowerSupplyFreq,
				KeyPreset[InfoParamPowerSupplyWatts], RunningPreset.InfoParam.PowerSupplyWatts,
				KeyPreset[InfoParamPresetStatus], RunningPreset.InfoParam.PresetStatus,
				KeyPreset[InfoParamWeldCount], RunningPreset.InfoParam.WeldCount,
				KeyPreset[WcParamAmpStepCondition],	RunningPreset.WcParam.AmpStepCondition,
				KeyPreset[WcParamCycleAborts], RunningPreset.WcParam.CycleAborts,
				KeyPreset[WcParamGeneralAlarmFlag],	RunningPreset.WcParam.GeneralAlarmFlag,
				KeyPreset[WcParamGrdDetectFlagCP],	RunningPreset.WcParam.GrdDetectFlagCP,
				KeyPreset[WcParamMaxWeldTime], RunningPreset.WcParam.MaxWeldTime,
				KeyPreset[WcParamWeldMode], RunningPreset.WcParam.WeldMode,
				KeyPreset[WcParamFreqOffsetFlag],RunningPreset.WcParam.FreqOffsetFlag,
				KeyPreset[WcParamFreqOffset], RunningPreset.WcParam.FreqOffset,
				PresetNumberKeyID, Sysconfig->ActivePreset,
				KeyPreset[ParaSeekSeekTime],RunningHSPreset.ParaSeek.SeekTime,
				KeyPreset[ParaSeekSeekRampTime],RunningHSPreset.ParaSeek.SeekRampTime);

		if(RunningPreset.WcParam.WeldMode == TimeMode)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",KeyPreset[WcParamWeldTimeCP], RunningPreset.WcParam.WeldTimeCP);
		if(RunningPreset.WcParam.WeldMode == EnergyMode)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",KeyPreset[WcParamEnergyCP], RunningPreset.WcParam.EnergyCP);
		if(RunningPreset.WcParam.WeldMode == PeakpowerMode)
     		sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",KeyPreset[WcParamPeakPowerCP], RunningPreset.WcParam.PeakPowerCP);

		if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_OFF)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",KeyPreset[WcParamAmplitude1], RunningPreset.WcParam.Amplitude1);

		if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_TIME)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			   ",\"%s\":%d"
			   ",\"%s\":%d"
			   ",\"%s\":%d",
			   KeyPreset[WcParamAmpStepTime], RunningPreset.WcParam.AmpStepTime,
			   KeyPreset[WcParamAmplitude1],  RunningPreset.WcParam.Amplitude1,
			   KeyPreset[WcParamAmplitude2],RunningPreset.WcParam.Amplitude2);

		if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_ENERGY)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			   ",\"%s\":%ld"
			   ",\"%s\":%d"
			   ",\"%s\":%d",
			   KeyPreset[WcParamAmpStepEnergy], RunningPreset.WcParam.AmpStepEnergy,
			   KeyPreset[WcParamAmplitude1],	RunningPreset.WcParam.Amplitude1,
			   KeyPreset[WcParamAmplitude2],RunningPreset.WcParam.Amplitude2);

		if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_POWER)
            sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
               ",\"%s\":%d"
               ",\"%s\":%d"
               ",\"%s\":%d",
               KeyPreset[WcParamAmpStepPower], RunningPreset.WcParam.AmpStepPower,
               KeyPreset[WcParamAmplitude1], RunningPreset.WcParam.Amplitude1,
               KeyPreset[WcParamAmplitude2], RunningPreset.WcParam.Amplitude2);

		if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_EXTERNAL)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			    ",\"%s\":%d"
				 ",\"%s\":%d",
				 KeyPreset[WcParamAmplitude1],RunningPreset.WcParam.Amplitude1,
				 KeyPreset[WcParamAmplitude2],RunningPreset.WcParam.Amplitude2);


		if (RunningPreset.WcParam.CutOffEnabled == TRUE) {
			if ((CustomInput1Configured == TRUE) && (RunningPreset.ActParam.CustomAIn1CutoffFlag))
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",
						KeyPreset[ActParamCustomAIn1Cutoff], RunningPreset.ActParam.CustomAIn1Cutoff);
			if ((CustomInput2Configured == TRUE) && (RunningPreset.ActParam.CustomAIn2CutoffFlag))
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",
						KeyPreset[ActParamCustomAIn2Cutoff], RunningPreset.ActParam.CustomAIn2Cutoff);
			if (RunningPreset.WcParam.EnergyCutoffFlag == TRUE)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",
						KeyPreset[WcParamEnergyCutoff], RunningPreset.WcParam.EnergyCutoff);
			if (RunningPreset.WcParam.PeakPowerCutoffFlag == TRUE)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
						KeyPreset[WcParamPeakPowerCutoff], RunningPreset.WcParam.PeakPowerCutoff);
			if (RunningPreset.WcParam.TimeCutOffFlag == TRUE)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld", KeyPreset[WcParamTimeCutOff],
						RunningPreset.WcParam.TimeCutOff);
			if (RunningPreset.WcParam.WeldFreqHighCutOffFlag == TRUE)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld", KeyPreset[WcParamWeldFreqHighCutoff],
						RunningPreset.WcParam.WeldFreqHighCutoff);
			if (RunningPreset.WcParam.WeldFreqLowCutOffFlag == TRUE)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld", KeyPreset[WcParamWeldFreqLowCutoff],
						RunningPreset.WcParam.WeldFreqLowCutoff);
		}

		if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENABLE) == CONTROL_LIMITS_ENABLE) {
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYMLR) == CONTROL_LIMITS_ENERGYMLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",
						KeyPreset[LimitsParamEnergyMLR], RunningPreset.LimitsParam.EnergyMLR);
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_ENERGYPLR) == CONTROL_LIMITS_ENERGYPLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld",
						KeyPreset[LimitsParamEnergyPLR], RunningPreset.LimitsParam.EnergyPLR);
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_POWERMLR) == CONTROL_LIMITS_POWERMLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
						KeyPreset[LimitsParamPowerMLR], RunningPreset.LimitsParam.PowerMLR);
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_POWERPLR) == CONTROL_LIMITS_POWERPLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
						KeyPreset[LimitsParamPowerPLR], RunningPreset.LimitsParam.PowerPLR);
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEMLR) == CONTROL_LIMITS_TIMEMLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
						KeyPreset[LimitsParamWeldTimeMLR], RunningPreset.LimitsParam.WeldTimeMLR);
			if ((RunningPreset.LimitsParam.ControlLimits & CONTROL_LIMITS_TIMEPLR) == CONTROL_LIMITS_TIMEPLR)
				sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
						KeyPreset[LimitsParamWeldTimePLR], RunningPreset.LimitsParam.WeldTimePLR);
		}
		if (RunningPreset.WcParam.ABFlag == TRUE)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			   ",\"%s\":%d"
				",\"%s\":%d"
				",\"%s\":%d",
				KeyPreset[WcParamABTime], RunningPreset.WcParam.ABTime,
				KeyPreset[WcParamABAmp], RunningPreset.WcParam.ABAmp,
				KeyPreset[WcParamABDelay], RunningPreset.WcParam.ABDelay);

		if (RunningPreset.WcParam.EBFlag == TRUE)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			   ",\"%s\":%d"
				",\"%s\":%d",
				KeyPreset[WcParamEBAmp], RunningPreset.WcParam.EBAmp,
				KeyPreset[WcParamEBTime], RunningPreset.WcParam.EBTime);

		if (RunningPreset.WcParam.EnergyCompFlag == TRUE)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%ld", KeyPreset[WcParamEnergyComp],
					RunningPreset.WcParam.EnergyComp);

		if (RunningPreset.WcParam.ExtTrigDelayFlag == TRUE)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",
					KeyPreset[WcParamExtTrigDelayTime], RunningPreset.WcParam.ExtTrigDelayTime);

		if (RunningPreset.WcParam.HoldTimeFlag == TRUE)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d", KeyPreset[WcParamHoldTime],
					RunningPreset.WcParam.HoldTime);

		if (RunningPreset.WcParam.WeldMode == GroundDetectMode)
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff),
			   ",\"%s\":%d"
				",\"%s\":%d",
				KeyPreset[WcParamScrubTime], RunningPreset.WcParam.ScrubTime,
				KeyPreset[WcParamScrubAmp], RunningPreset.WcParam.ScrubAmp);

		if((Sysconfig->PowerupOption == POWERUP_SEEK) || (Sysconfig->PowerupOption == POWERUP_SCAN))
			sprintf(ObjPreset->Jsonbuff + strlen(ObjPreset->Jsonbuff), ",\"%s\":%d",PowerupOptionKeyID, Sysconfig->PowerupOption);

		strcat(ObjPreset->Jsonbuff, "}");
		JsonDataList.AddTail(ObjPreset);
		SendPresetToDCXM = FALSE;
	}
}

/* void WriteWeldDatForDCXM(WeldData * WData, UINT32 TimeSinceStartOfCycle)
 * Purpose:
 * This Function is called by StoreWeldData() at sample rate, requested by DcxManager to write the weld data to Jsonbuff buffer.
 * It is used to create object for CycleNumber JSONObjectType.
 *  Entry condition:
 *		WData : pointer to WeldData structure.
 *		WData TimeSinceStartOfCycle :
 *  Exit condition:
 *      None
 */
void WriteWeldDatForDCXM(WeldData * WData, UINT32 TimeSinceStartOfCycle)
{
	SINT8 * AinId[2];
	SINT32 Ain1Val[2];
	int i;
	if(EnableJSONLogging){
		JSONWeldData * ObjWeldData = new JSONWeldData();
		if(IsManualFunctionAvailable() == TRUE){
			for(i = 1; i < NUM_AIN; i++){
				switch(USERIOConfig->AnalogInUse[i])
				{
				case FREQ_CONTROL:
					AinId[i - 1] = KeyWeldData[WELDDATA_FREQUENCYIN];
					Ain1Val[i - 1] = UserIO::GetExternalFrequency() / FREQ_SCALING_FACTOR;
					break;
				case AMPL_CONTROL:
					AinId[i - 1] =KeyWeldData[WELDDATA_AMLITUDEIN];
					Ain1Val[i - 1] = UserIO::GetExternalAmplitude();
					break;
				case CUSTOM_INPUT1:
					AinId[i - 1] =KeyWeldData[WELDDATA_ANALOGIN1];
					Ain1Val[i - 1] =UserIO::GetCustomAIn1();
					break;
				case CUSTOM_INPUT2:
					AinId[i - 1] =KeyWeldData[WELDDATA_ANALOGIN2];
					Ain1Val[i - 1] =UserIO::GetCustomAIn2();
					break;
				default:
					AinId[i - 1] = (SINT8*)KeyWeldDataUnassignedAIN[i - 1];
					Ain1Val[i - 1] = 0;
					break;
				}
			}
		}
		else{
			i = 0;
			AinId[i] = KeyWeldData[WELDDATA_FREQUENCYIN];
			if(FieldBusType == HW_ASSEMBLY_ETHERNETIP)
				Ain1Val[i] = (FieldBus::GetExternalFrequencyOffset() / FREQ_SCALING_FACTOR);
			else
				Ain1Val[i] = 0;
			i++;
			AinId[i] = KeyWeldData[WELDDATA_AMLITUDEIN];
			Ain1Val[i] = FieldBus::GetExternalAmplitude();
		}

		sprintf(ObjWeldData->Jsonbuff,
         "{\"%s\":%lu,"
         "\"%s\":%ld,"
         "\"%s\":%d,"
         "\"%s\":%d,"
         "\"%s\":%d,"
         "\"%s\":%d,"
         "\"%s\":%d,"
         "\"%s\":%ld,"
         "\"%s\":%ld}",
         KeyWeldData[WELDDATA_TIME], TimeSinceStartOfCycle,
         KeyWeldData[WELDDATA_FREQUENCY], WData->Frequency,
         KeyWeldData[WELDDATA_PWM], WData->PWMValue,
         KeyWeldData[WELDDATA_AMPLITUDE], WData->ActualAmplitude,
         KeyWeldData[WELDDATA_CURRENT], WData->Current,
         KeyWeldData[WELDDATA_POWER] ,WData->Power,
         KeyWeldData[WELDDATA_PHASE], WData->Phase,
         AinId[0],Ain1Val[0],
         AinId[1],Ain1Val[1]);

		JsonDataList.AddTail(ObjWeldData);
	}
}
/* void WriteAlarmForDCXM()
 * Purpose:
 * This Function is called from ErrorEntry() to write the weld data to Jsonbuff buffer.
 * It is used to create object for CycleNumber JSONObjectType.
 *  Entry condition:
 *		None
 *  Exit condition:
 *      None
 */
void WriteAlarmForDCXM(SINT32 * LstCnt, BOOLEAN AddToHead)
{
	SINT32 Indx;
	UINT8 AlarmIndexTemp;
	if(EnableJSONLogging){
	   AlarmIndexTemp = AlarmIndex;
		if(AlarmIndexTemp > 0){
			JSONAlarm * ObjAlarm = new JSONAlarm();
			strcpy(ObjAlarm->Jsonbuff, ",\"alarms\":[");
			for(Indx = 0; Indx < AlarmIndexTemp; Indx++){
				if(Indx < AlarmIndexTemp - 1)
					sprintf(ObjAlarm->Jsonbuff + strlen(ObjAlarm->Jsonbuff),"\"%04X\",", AlarmsArray[Indx]);
				else
					sprintf(ObjAlarm->Jsonbuff + strlen(ObjAlarm->Jsonbuff),"\"%04X\"]", AlarmsArray[Indx]);
			}
			if(AddToHead)
				JsonDataList.AddHead(ObjAlarm);
			else
				JsonDataList.AddTail(ObjAlarm);

			if(LstCnt)
				*LstCnt += 1;
		}
	}
}

/* void CycleDataSend(ip_addr Dcxmaddr)
 * Purpose:
 * This Function is called from WelderSearchTask::Run() to write the weld data to Jsonbuff buffer.
 *  Entry condition:
 *		Dcxmaddr : Address of the DcxManager.To send the Dcx Data to the connected DCX Manager IP Address.
 *  Exit condition:
 *      None
 */
 void CycleDataSend(ip_addr Dcxmaddr,  SINT32 * LstCnt){
	 SINT16 CurrIndex = CurrentIndex;
	 if(((CurrIndex == SONICS_START_IDX) ||
	    (CurrIndex == ENERGY_BRAKING_IDX) ||
	    (CurrIndex == HOLD_OFF_TIME_IDX)) && (Dcxmaddr.addr > 0)){
		 WriteCycleCounterForDCXM(TRUE, FALSE, 0, TRUE, 0, TRUE);
		 LastPsCycleSent = ActPresetCycleCount + 1;
		 WritePresetForDCXM(TRUE);
		 if(LstCnt)
			 *LstCnt += 2;
	 }
	 else
		 SendPresetToDCXM = TRUE;
	 ConnectedDcxmAddr = Dcxmaddr;
}

 /* void InitJsonObjsKeys(void)
  * Purpose:
  * This Function is called by InitDcxMangerHandler() to initialise the JsonObject KeyId.
  * It is used to create object for CycleNumber JSONObjectType.
  *  Entry condition:
  *		None
  *  Exit condition:
  *      None
  */
void InitJsonObjsKeys(void)
{

	int WeldData_Count = WELDDATA_FREQUENCYIN + 1;
	int Preset_Count = ParaSeekSeekRampTime + 1;
	int WeldResult_Count = WELDRESULT_CUSTOMAIN2 + 1;
	int I, J;
	KeyWeldData = new SINT8 *[WeldData_Count];
	for (I = 0; I < WeldData_Count; ++I)
		KeyWeldData[I] = new SINT8[MAX_KEY_SIZE];

	KeyPreset = new SINT8*[Preset_Count];
	for (I = 0; I< Preset_Count; ++I)
		KeyPreset[I] = new SINT8[MAX_KEY_SIZE];

	KeyWeldResult = new SINT8*[WeldResult_Count];
	for (I = 0; I < WeldResult_Count; ++I)
		KeyWeldResult[I] = new SINT8[MAX_KEY_SIZE];


	for (J = WELDDATA_TIME; J <=  WELDDATA_FREQUENCYIN; J++) {
		sprintf(KeyWeldData[J], "%d", J );
	}

	for (J = ActParamCustomAIn1Cutoff; J <= ParaSeekSeekRampTime; J++) {
		sprintf(KeyPreset[J], "%d", J);
	}

	for (J = WELDRESULT_CYCLECOUNTER; J <=  WELDRESULT_CUSTOMAIN2;J++) {
		sprintf(KeyWeldResult[J], "%d",J);
	}
}

/* void ClearJsonObjects(void)
 * Purpose:
 * This Function is called from InputTask.
 * It is used to clear the JsonDataList List.
 *  Entry condition:
 *		None
 *  Exit condition:
 *      None
 */
void ClearJsonObjects(void)
{
	POSITION pos = JsonDataList.GetHeadPosition();
//	printf("\n Cleared Num Objs %d", JsonDataList.GetCount());
	while (pos)
	{
		POSITION last = pos;
		JSON * JsonObj = JsonDataList.GetNext(pos);
		JsonDataList.RemoveAt(last);
		delete JsonObj;
	}
	LastPsCycleSent = ActPresetCycleCount;
}
/* void BOOLEAN  JSONHandler(SINT8 * ServiceURI, HTTPReplyHeader & Header, SINT8 * JsonStr, ip_addr PeerAddr)
 * Purpose:
* 		JSON Handler callback. Function is called when JSON URI is received by the server.
 *  Entry condition:
 *		ServiceURI- The JSON URI based on which different services will be invoked.
 *		Header- Object which will be filled with the response length,response type etc.
 *		JsonStr- Pointer to the data sent with JSON URI in case of POST request
 *		PeerAddr- Address of the Client requesting/posting data through JSON URI
 *  Exit condition:
 *      Returns TRUE in case of valid request otherwise returns FALSE
 */
BOOLEAN  JSONHandler(SINT8 * ServiceURI, HTTPReplyHeader & Header, SINT8 * JsonStr, ip_addr PeerAddr, int)
{
	BOOLEAN RetVal = FALSE;
	SINT8 * Temp = 0, * ServiceEnd = 0, * ServiceName = 0, *ServiceArgs = 0;
	BOOLEAN HandlerFound = FALSE;
	SINT32 Indx = 0;
	Header.contentType = typeJSON;
	strcpy(Response, "[\"Notfound\" , \"Notfound\"]");
	Header.data = Response;
	if(PeerAddr.addr == ConnectedDcxmAddr.addr){
		Temp = strstr(ServiceURI, "Services");
		if (Temp){
			Temp = Temp + strlen("Services");//   /Services/GetDcxData/SR=100 /Services/StopDcxData
			ServiceName = Temp;
			if(Temp)//Coverity CID:12579
			{
				ServiceEnd = strchr((Temp + 1), '/');
				if(ServiceEnd){
					ServiceArgs = ServiceEnd + 1;
					*ServiceEnd = '\0';
				}
			}
			//printf("\n Service Name %s Service Args %s", ServiceName, ServiceArgs);
			if(ServiceName){
				if(JsonStr){
					while((JsonSetCmdHandlers[Indx].ServiceName != 0) && (HandlerFound == FALSE)){
						//TODO: Encode JSonStr
						if(stricmp(JsonSetCmdHandlers[Indx].ServiceName, ServiceName) == 0){
							Header.data = JsonSetCmdHandlers[Indx].Handler(PeerAddr, JsonStr, ServiceArgs);
							Header.statusCode = httpOk;
							RetVal = HandlerFound = TRUE;
						}
						Indx++;
					}
				}
				else {
					while((JsonGetCmdHandlers[Indx].ServiceName != 0) && (HandlerFound == FALSE)){
						if(stricmp(JsonGetCmdHandlers[Indx].ServiceName, ServiceName) == 0){
							Header.data = JsonGetCmdHandlers[Indx].Handler(PeerAddr, ServiceArgs);
							Header.statusCode = httpOk;
							RetVal = HandlerFound = TRUE;
						}
						Indx++;
					}
				}
			}
		}
	}
	Header.contentLength = strlen(Header.data);
	return RetVal;
}


