/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WeldResultHistory.h_v   1.3   14 Apr 2016 08:23:22   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WeldResultHistory.h_v  $
 * 
 *    Rev 1.3   14 Apr 2016 08:23:22   hasanchez
 * Changed data type to Temperature (previously the change was not updated)
 * 
 *    Rev 1.2   12 Apr 2016 16:20:48   hasanchez
 * Changed the data type of temperature.
 * 
 *    Rev 1.1   21 Mar 2016 16:19:42   hasanchez
 * Updated the structures to the Weld Results
 * 
 *    Rev 1.0   09 Jun 2015 09:10:44   tayars
 * Initial revision.
 * 
 *    Rev 1.9   13 Mar 2014 16:08:26   rjamloki
 * Added FramHistory Struct.
 * 
 *    Rev 1.8   08 Jul 2013 09:02:08   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.7   21 May 2013 12:26:14   amaurya
 * Code review fixes.
 * 
 *    Rev 1.6   15 Mar 2013 10:55:20   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.5   18 Feb 2013 03:13:32   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   14 Dec 2012 05:30:26   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.3   26 Oct 2012 06:13:28   rjamloki
 * Changes required for website
 * 
 *    Rev 1.2   21 Sep 2012 01:35:36   rjamloki
 * Added State machine
 * 
 *    Rev 1.1   29 Aug 2012 13:28:20   rjamloki
 * WC Modbus checkin and related changes
*/

#ifndef WELDRESULTHISTORY_H_
#define WELDRESULTHISTORY_H_

#include "portable.h"
#include "LOG.h"

#define MAX_WELD_RESULT_ENTRIES	50
//Data below should go into FRAM.
struct WeldResultInfo
{
	SINT32 CircularIndex;
	SINT32 TotalHistoryCount;
};


struct WeldResult
{
	UINT32 	CycleCounter;
	TimeStamp TS;
	UINT32 	WeldTime;
	UINT32 	WeldEnergy;
	UINT8 	PeakPower;
	UINT8    Amplitude1;
	UINT8    Amplitude2;
	UINT8    PresetNumber;
	UINT32   WeldStartFrequency;
	UINT32   WeldStopFrequency;
	SINT16   FrequencyChange;
	SINT32   ErrorPwm;
	UINT32   ErrorReason;
	UINT16   AvgAmplitude1;
	UINT16   AvgAmplitude2;
	UINT16   CustomAIn1;
	UINT16   CustomAIn2;
	UINT16	 Temperature;
	UINT8 	 WeldMode;
	BOOLEAN  LogValid;
};


struct DcpWeldResult
{
	UINT32 	CycleCounter;
	TimeStamp TS;
	UINT32 	WeldTime;
	UINT32 	WeldEnergy;
	UINT8 	PeakPower;
	UINT8    Amplitude1;
	UINT8    Amplitude2;
	UINT8    PresetNumber;
	UINT32   WeldStartFrequency;
	UINT32   WeldStopFrequency;
	SINT16   FrequencyChange;
	SINT32   ErrorPwm;
	UINT32   ErrorReason;
	UINT16   AvgAmplitude1;
	UINT16   AvgAmplitude2;
	UINT16   CustomAIn1;
	UINT16   CustomAIn2;
	UINT16	 Temperature;
	BOOLEAN  LogValid;
};

struct WebHistoryData
{
   UINT32    CycleCounter;
   TimeStamp TS;
   UINT32    WeldTime;
   UINT32    WeldEnergy;
   UINT8     PeakPower;
   UINT8    Amplitude1;
   UINT8    Amplitude2;
   UINT8    PresetNumber;
   UINT32   WeldStartFrequency;
   UINT32   WeldStopFrequency;
   SINT16   FrequencyChange;
   SINT32   ErrorPwm;
   UINT32   ErrorReason;
   UINT16   AvgAmplitude1;
   UINT16   AvgAmplitude2;
   UINT16   CustomAIn1;
   UINT16   CustomAIn2;
   UINT16	Temperature;
   UINT8 	WeldMode;
};

struct WeldResultFRAM
{
	WeldResultInfo Weldresultinfo;
	WeldResult WeldResultObjs[MAX_WELD_RESULT_ENTRIES];
};

void WriteWeldHistory(void);
SINT32 GetWeldHistory(WebHistoryData * WeldHistoryBuff, SINT32 Count);
#endif /* WELDRESULTHISTORY_H_ */
