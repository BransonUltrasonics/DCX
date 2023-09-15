/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WeldResultHistory.cpp_v   1.1   12 Apr 2016 16:18:24   hasanchez  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WeldResultHistory.cpp_v  $
 * 
 *    Rev 1.1   12 Apr 2016 16:18:24   hasanchez
 * Saved the correct temperature on the Weld history.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:44   tayars
 * Initial revision.
 * 
 *    Rev 1.21   13 Mar 2014 15:55:36   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.20   24 Aug 2013 17:14:28   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.19   24 May 2013 10:18:46   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.18   21 May 2013 12:26:14   amaurya
 * Code review fixes.
 * 
 *    Rev 1.17   30 Apr 2013 11:23:54   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.16   22 Apr 2013 11:34:56   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.15   08 Apr 2013 08:26:50   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.14   15 Mar 2013 10:55:08   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.13   11 Mar 2013 02:31:26   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.12   18 Feb 2013 03:13:20   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.11   29 Jan 2013 02:25:38   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 * 
 *    Rev 1.10   16 Jan 2013 06:31:46   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.9   27 Dec 2012 06:47:16   ygupta
 * Changes to fix Issues
 * 
 *    Rev 1.8   14 Dec 2012 05:30:12   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.7   13 Dec 2012 00:14:54   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.6   24 Nov 2012 08:08:18   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   29 Oct 2012 02:58:50   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.4   26 Oct 2012 02:42:50   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   21 Sep 2012 01:35:24   rjamloki
 * Added State machine
 * 
 *    Rev 1.2   10 Sep 2012 03:13:46   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.1   29 Aug 2012 13:28:20   rjamloki
 * WC Modbus checkin and related changes
 */

#include "WeldResultHistory.h"
#include "RTClock.h"
#include "GLOBALS.h"
#include "SysConfig.h"
#include "DcxManagerHandler.h"

#define pprintf
#define printf

#define NAAMP 200//Define for a value never expected in amplitude field. Need to be filled to show NA on web pages

extern DcpWeldResult WeldHistoryData;
extern SINT32 WeldStopFrequency;

/*	 void WriteWeldHistory(void)
 *
 *  Purpose :
 *   	To store the weld result information in weldresultObjs array in FRAM.
 *   	Keep track  of the index of weldresultObjs array where weld result data is to be copied.
 *  	In case index goes out of  array boundary rolls back to the initial index and override the previous entries.
 *  	This function is called from WC Application code.
 *
 *  Entry Condition: WeldresultObj - Structure that contains data to be copied.
 *
 *  Exit Condition: None.
 */
void WriteWeldHistory()
{
	static RTCTime TM = {0,0,0,0,0,0,0};
	// correct index where the event log has to be written
	//TODO:WC need to fill its data here.
	WeldResFram->Weldresultinfo.CircularIndex = ((WeldResFram->Weldresultinfo.CircularIndex) + 1) % MAX_WELD_RESULT_ENTRIES;
	if(WeldResFram->Weldresultinfo.TotalHistoryCount < MAX_WELD_RESULT_ENTRIES)
		WeldResFram->Weldresultinfo.TotalHistoryCount++;
  	RTClock::GetCurrentTime(&TM);	//Get the current time from RTC
  	WeldHistoryData.TS.Year = TM.Years;
  	WeldHistoryData.TS.Month = TM.Century_Month;
  	WeldHistoryData.TS.Day = TM.Day;
  	WeldHistoryData.TS.Hour = TM.Hour;
  	WeldHistoryData.TS.Minute = TM.Minute;
  	WeldHistoryData.TS.Second = TM.Second;
  	if(WeldStopFrequency > 0){
  		WeldHistoryData.WeldStopFrequency = WeldStopFrequency;
  		WeldHistoryData.FrequencyChange = WeldHistoryData.WeldStopFrequency - WeldHistoryData.WeldStartFrequency;
  	}
  	memcpy(&WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex] , &WeldHistoryData, sizeof(WeldHistoryData));	// Copy data into FRAM
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Temperature = InputTemperature; 
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude1 = RunningPreset.WcParam.Amplitude1;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude2 = RunningPreset.WcParam.Amplitude2;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].WeldMode = RunningPreset.WcParam.WeldMode;
	if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_EXTERNALAIN){
		WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude1 = NAAMP;
		WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude2 = NAAMP;
	}
	else if(RunningPreset.WcParam.AmpStepCondition == AMPSTEP_COND_OFF)
		WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].Amplitude2 = NAAMP;

  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].LogValid = TRUE;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude1 = WeldHistoryData.AvgAmplitude1;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].AvgAmplitude2 = WeldHistoryData.AvgAmplitude2;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].PresetNumber = Sysconfig->ActivePreset;

  	if(CycleCountFlag == TRUE)
  		ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount++;
  	CalculateAndSavePresetCRC((SAVECRC_CMD)SAVECRC_INFO, (PRESET_NUMBER)Sysconfig->ActivePreset);
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].CycleCounter = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount;//Counter.LifeTimeCycleCount;
  	 	   SysCounters->LifeTimeCycleCount++;
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].CustomAIn1 = UserIO::GetCustomAIn1();
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].CustomAIn2 = UserIO::GetCustomAIn2();
  	WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorReason = 0;
   if(AlarmIndex > 1)
      WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorReason = ((AlarmsArray[1] << 16) | AlarmsArray[0]);
   else if(AlarmIndex == 1)
      WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex].ErrorReason = AlarmsArray[0];
   SaveWeldStatusSet();
   WriteWeldResultForDCXM(&WeldResFram->WeldResultObjs[WeldResFram->Weldresultinfo.CircularIndex], ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount);
}

/* SINT32 GetWeldHistory(WebHistoryData * WeldHistoryBuff, SINT32 Count)
 *
 *  Purpose:
 *   This function is used to get history data from WeldHistory Buffer.
 *   This function gets called from History Data Get handler.
 *
 *  Entry condition:
 *    WeldHistoryBuff- Pointer to WeldHistoryBuff array.
 *    Count- No. of History count to be copied.
 *
 *  Exit condition:
 *    Returns No. of history data Copied.
 */
SINT32 GetWeldHistory(WebHistoryData * WeldHistoryBuff, SINT32 Count)
{
   SINT32 TotalReadCount = 0;
   SINT32 ArrayIndexToReadFrom = 0;
   BOOLEAN ExitFlag = FALSE;
   if(Count > MAX_WELD_RESULT_ENTRIES)
      Count = MAX_WELD_RESULT_ENTRIES;

   ArrayIndexToReadFrom = WeldResFram->Weldresultinfo.CircularIndex;
   while((Count > 0) && (ExitFlag == FALSE) && (ArrayIndexToReadFrom >= 0)){
      if(WeldResFram->WeldResultObjs[ArrayIndexToReadFrom].LogValid == TRUE){
         memcpy(WeldHistoryBuff, &WeldResFram->WeldResultObjs[ArrayIndexToReadFrom], sizeof(WebHistoryData));
         TotalReadCount++;
         WeldHistoryBuff++;
      }
      else{
         if(WeldResFram->Weldresultinfo.TotalHistoryCount > TotalReadCount)
            memset(&WeldResFram->WeldResultObjs[ArrayIndexToReadFrom].TS, 0, sizeof(WeldResFram->WeldResultObjs[ArrayIndexToReadFrom].TS));
         else
            ExitFlag = TRUE;
      }
      Count--;
      ArrayIndexToReadFrom--;
      if(ArrayIndexToReadFrom < 0)
         ArrayIndexToReadFrom = MAX_WELD_RESULT_ENTRIES - 1;
   }
   return TotalReadCount;
}
