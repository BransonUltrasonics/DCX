/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/Alarms.h_v   1.11   10 Sep 2013 03:46:02   ygupta  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation = (OVERLOAD | BIT(0)); 1995 = (OVERLOAD | BIT(0));96 = (OVERLOAD | BIT(0)); 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden = (OVERLOAD | BIT(0)); without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/Alarms.h_v  $
 * 
 *    Rev 1.11   10 Sep 2013 03:46:02   ygupta
 * WC revision 6.A.
 * 
 *    Rev 1.9   03 Sep 2013 12:21:48   ygupta
 * Moved to tip from 1.8.1.6
 * 
 *    Rev 1.8.1.6   31 May 2013 10:36:52   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.8.1.5   28 May 2013 07:41:16   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.8.1.4   24 May 2013 10:13:50   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.8.1.3   21 May 2013 12:25:24   amaurya
 * Code review fixes.
 * 
 *    Rev 1.8.1.2   30 Apr 2013 11:17:52   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.8.1.1   16 Apr 2013 11:15:38   amaurya
 * Fixed tracker issues in Ver2.0R
 * 
 *    Rev 1.8.1.0   20 Mar 2013 09:36:18   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.9   20 Mar 2013 08:16:44   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.8   15 Mar 2013 10:47:38   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.7   16 Jan 2013 06:23:38   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.6   18 Dec 2012 07:36:44   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 * 
 *    Rev 1.5   14 Dec 2012 05:25:44   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.4   13 Dec 2012 00:08:32   ygupta
 * Changes related to Issues
 * 
 *    Rev 1.3   29 Nov 2012 08:43:48   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.2   24 Nov 2012 07:59:26   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.1   09 Nov 2012 00:34:16   rjamloki
 * State Machine and website changes
 * 
 *    Rev 1.0   05 Nov 2012 09:06:04   amaurya
 * Initial revision.
 * Alarms.h
 *
 *  Created on: Oct 29 = (OVERLOAD | BIT(0)); 2012
 *      Author: seema.sharma
 */

#ifndef ALARMS_H_
#define ALARMS_H_

#include "portable.h"
#include "DEF_DEF.H"

//#define NONCYCLEABORTINGOVERLOAD  0x0000
#define  OL  0x0000    // OVERLOAD(001-0FF)
#define  CU  0x0100    // CUTOFFS(101-1FF)
#define  SE  0x0200    // SETUP(201-2FF)
#define  CM  0x0300    // CYCLEMODIFIED(301-3FF)
#define  WA  0x0400    // WARNINGS(401-4FF)
#define  LM  0x0500    // PROCESSLIMITS(501-5FF)
#define  EQ  0x0600    // EQUIPMENT(601-6FF)
#define  NC  0x0700    // NOCYCLE(701-7FF)
#define  CF  0x0800    // COMMUNICATION_FAILURE(801-8FF)
#define  TP  0x0900    // TEMPERATURE_PROBE(901-9FF)
#define  HW  0x0A00    // HARDWAER_FAILURE(A01-AFF)
#define  NO  0x0B00    // NONCYCLE_ABORT(B01-BFF)

#define ALARMTYPE_MASK  0x0F00
#define ALARMCODE_MASK  0x00FF

#define ALARMTYPE(Alarm)   ((Alarm & ALARMTYPE_MASK) >> 8)
#define ALARMCODE(Alarm)   (Alarm & ALARMCODE_MASK)   // maybe not used

#define MAX_MULTIPLE_ALARMS 10
#define MAX_ALARM_ENTRIES  100
//The maximum length of Event Description string
#define MAX_LOG_LENGTH  50
#define ALARM_PERGROUP  128

enum AlarmType
{
   OVERLOADS,
   CUTOFFS,
   SETUP,
   CYCLE_MODIFIED,
   WARNINGS,
   PROCESS_LIMITS,
   EQUIPMENT_FAILURES,
   NOCYCLE,
   COMM_FAILURES,
   TEMP,
   HARDWARE_FAILURE,
   NON_CYCLE_OVERLOADS,
   ALARM_TYPES
};

enum Alarms{
   //Weld Overload
   OL01 = (OL + 1),
   OL02,
   OL03,
   OL04,
   OL05,
   OL06,

   //Energy Braking Overload
   OL11 = (OL + 0x11),
   OL12,
   OL13,
   OL14,
   OL15,
   OL16,
   OL20 = (OL + 0x20),   //FOR SFX

   //CutOffAlarms
   CU01 = (CU + 1),
   CU02,
   CU03,
   CU04,
   CU05, //maximum timeout
   CU06,
   CU07,  //FOR SFX
   CU08,

   //SetUp
   SE01 = (SE + 1),  //SFX
   SE02,  //SFX
   SE03,
   SE04,
   SE05,
   SE06,  //SFX

   //CycleModifiedAlarms
   CM01 = (CM + 1),
   CM02,

   //Warnings
   WA01 = (WA + 1),
   WA02,  //SFX
   WA03,  //SFX
   WA04,
   WA05,
   WA11 = (WA + 0x11),
   WA12,
   WA13,
   WA14,
   WA15,
   WA16,

   //LimitAlarms
   LM01 = (LM + 1),
   LM02,
   LM03,
   LM04,
   LM05,
   LM06,
   LM07,
   LM08,

   //Equipment
   EQ01 = (EQ + 1),
   EQ02,
   EQ03,
   EQ04,
   EQ05,
   EQ06,
   EQ07,
   EQ08,
   EQ09,
   EQ10 = (EQ + 0x10),
   EQ11,
   EQ12,

   //NoCycle{
   NC01 = (NC + 1), //(START OF CYCLE)
   NC02,
   NC03, // (USER I/O)
   NC04,    // (USER I/O)
   NC05,
   NC06,    // (USER I/O)
   NC07,

   //CommFailureEvents
   CF01 = (CF + 1),  // (DCP TO WC)
   CF02,
   CF03,

   //Temperature ProbeAlarms
   TP01 = (TP + 1),  // SFX
   TP02,  // SFX
   TP03,  // SFX
   TP04,  // SFX
   TP05,  // SFX

   //Hardware failure
   HW01 = (HW + 1),
   HW02,
   HW03,
   HW04,
   HW05,
   HW06,

   //Seek Overload
   NO01 = (NO + 1),
   NO02,
   NO03,
   NO04,
   NO05,
   NO06,

   //TEST Overload
   NO11 = (NO + 0x11),   //(OL + 0x28)
   NO12,
   NO13,
   NO14,
   NO15,
   NO16,
   NO21,
   NO22,
};

struct AlarmsDescription
{
   SINT8  * AlarmStr; //alarm description in string
   Alarms Alarmid;        //alarm id
};

extern Alarms AlarmsArray[MAX_MULTIPLE_ALARMS];
extern UINT8 AlarmIndex, AlarmLogIndx;
extern BOOLEAN ResetReqFlag, CycleCountFlag;
SINT8 * GetAlarmStringFromID(Alarms AlarmId);
void CreateOverloadAlarm(UINT32 AlarmReason);
void CreateAlarm(void);
void CreateDCPGeneralAlarm(UINT32 AlarmReason);
void AlarmQueueWrite(Alarms AlarmID);
//void CheckForACPowerDownAlarm();

extern UINT32 FBAlarms[ALARM_TYPES * (ALARM_PERGROUP / 32)];
#endif /* ALARMS_H_ */
