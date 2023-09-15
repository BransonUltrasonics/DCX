/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Alarms.cpp_v   1.1   24 Dec 2015 02:38:52   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Alarms.cpp_v  $
 * 
 *    Rev 1.1   24 Dec 2015 02:38:52   sesharma
 * Fix for DEF01259: CreateOverloadAlarm() modified to raise "Weld Overload - Generic" alarm as default.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:40   tayars
 * Initial revision.
 * 
 *    Rev 1.19   13 Mar 2014 15:53:14   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.18   08 Oct 2013 06:18:56   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.17   03 Sep 2013 12:19:34   ygupta
 * Moved to tip from 1.15.1.18
 * 
 *    Rev 1.15.1.18   03 Sep 2013 11:13:08   amaurya
 * Tracker fixes and Synchronization fixes for User Outputs.
 * 
 *    Rev 1.15.1.17   24 Aug 2013 17:13:10   amaurya
 * Code review and tracker issue fixes.
 *
 *    Rev 1.27.1.23   02 Aug 2013 10:55:58   amaurya
 * Coverty fixes.
 *
 *    Rev 1.27.1.22   18 Jul 2013 12:21:28   amaurya
 * Frequency output fixed.
 *
 *    Rev 1.27.1.21   17 Jul 2013 06:23:18   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 *
 *    Rev 1.27.1.20   10 Jul 2013 07:53:18   amaurya
 * Fixed powerup DCP Event Log issue.
 *
 *    Rev 1.27.1.19   08 Jul 2013 08:57:12   amaurya
 * Fixed tracker issue for Ver3.0y.
 *
 *    Rev 1.27.1.18   27 Jun 2013 05:58:24   amaurya
 * System crash fixes.
 *
 *    Rev 1.27.1.17   14 Jun 2013 11:21:18   amaurya
 * MH1 Round fixes
 *
 *    Rev 1.27.1.16   11 Jun 2013 09:53:24   amaurya
 * MH round I testing fixes.
 *
 *    Rev 1.27.1.15   07 Jun 2013 10:21:06   amaurya
 * MH testing round I fixes.
 *
 *    Rev 1.27.1.14   31 May 2013 11:51:52   amaurya
 * Fixed Login Issue.
 *
 *    Rev 1.27.1.12   28 May 2013 07:45:12   amaurya
 * Fixed issues of Level II testing.
 *
 *    Rev 1.27.1.11   24 May 2013 10:18:26   ygupta
 * Fixed Tracker Issue.
 *
 *    Rev 1.27.1.10   22 May 2013 10:41:50   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 *
 *    Rev 1.27.1.9   21 May 2013 12:26:12   amaurya
 * Code review fixes.
 *
 *    Rev 1.27.1.8   07 May 2013 10:16:00   amaurya
 * Code review fixed.
 *
 *    Rev 1.27.1.7   06 May 2013 09:22:44   amaurya
 * Changes to use actual frequency.
 *
 *    Rev 1.27.1.6   30 Apr 2013 11:23:34   amaurya
 * Changes related to State machine optimization.
 *
 *    Rev 1.27.1.5   26 Apr 2013 11:04:52   amaurya
 * Fixed traker issue for Ver2.0U.
 *
 *    Rev 1.27.1.4   23 Apr 2013 10:26:02   ygupta
 * Fixed issue of System Test page on website.
 *
 *    Rev 1.27.1.3   22 Apr 2013 11:34:26   amaurya
 * Fixed tracker issue of Ver2.0S
 *
 *    Rev 1.27.1.2   16 Apr 2013 11:21:02   amaurya
 * Fixed tracker issue of Ver2.0R.
 *
 *    Rev 1.27.1.1   08 Apr 2013 08:26:26   amaurya
 * Fixed Tracker issue of Ver2.0P.
 *
 *    Rev 1.27.1.0   20 Mar 2013 09:40:12   ygupta
 * RF Switch Changes, Requirement Changes
 *
 *    Rev 1.28   20 Mar 2013 08:20:04   ygupta
 * RF Switch Changes, Requirement Changes
 *
 *    Rev 1.27   15 Mar 2013 10:54:38   ygupta
 * Issues Resolved, Requirement Changes
 *
 *    Rev 1.26   11 Mar 2013 02:30:44   ygupta
 * Issues Fixed & Requirement Changes
 *
 *    Rev 1.25   18 Feb 2013 03:12:52   ygupta
 * FieldBus Changes
 *
 *    Rev 1.24   29 Jan 2013 02:25:02   ygupta
 * WebPages CleanUp, Code Review and CleanUp
 *
 *    Rev 1.23   22 Jan 2013 10:38:46   ygupta
 * Profibus Bug Fixes
 *
 *    Rev 1.22   17 Jan 2013 07:27:30   ygupta
 * Profibus Related changes
 *
 *    Rev 1.21   16 Jan 2013 06:31:24   ygupta
 * Level2 Code with Profibus
 *
 *    Rev 1.20   11 Jan 2013 02:56:12   ygupta
 * Level2 Requirement Changes
 *
 *    Rev 1.19   11 Jan 2013 01:52:08   ygupta
 * Stroke length and Amplitude Default Changed
 *
 *    Rev 1.18   03 Jan 2013 07:19:56   ygupta
 * Modbus Comm problem Resolved
 *
 *    Rev 1.17   27 Dec 2012 06:46:54   ygupta
 * Changes to fix Issues
 *
 *    Rev 1.16   18 Dec 2012 07:40:06   ygupta
 * Changes for Multiple Overload alarm in case of 15V lost
 *
 *    Rev 1.14   17 Dec 2012 02:19:52   ygupta
 * Level 2 passwords and Alarm(WC to DCP Comm) chnged
 *
 *    Rev 1.13   14 Dec 2012 05:29:54   ygupta
 * Alarms Implemented
 *
 *    Rev 1.6   05 Nov 2012 09:00:58   amaurya
 * Initial revision.
 *
 */

#include "Alarms.h"
#include "LOG.h"
#include "GLOBALS.h"
#include "StateTable.h"
#include "FieldBus.h"

#define pprintf

Alarms AlarmsArray[MAX_MULTIPLE_ALARMS];
UINT8 AlarmIndex = 0;
UINT8 AlarmLogIndx = 0;
UINT32 FBAlarms[ALARM_TYPES * (ALARM_PERGROUP / 32)];
BOOLEAN ResetReqFlag = FALSE, CycleCountFlag = TRUE;
extern AlarmLogData TempAlarmDataObj;

//Definition of alarms description along with IDs
AlarmsDescription AlarmDescription[] =
{
   // Weld Overload(0x0000)
   {"Weld Overload - Phase", OL01},
   {"Weld Overload - Current", OL02},
   {"Weld Overload - Frequency", OL03},
   {"Weld Overload - Power", OL04},
   {"Weld Overload - Voltage", OL05},
   {"Weld Overload - Temperature", OL06},

   //Energy Braking Overload
   {"Energy brake Overload - Phase", OL11},
   {"Energy brake Overload - Current", OL12},
   {"Energy brake Overload - Frequency", OL13},
   {"Energy brake Overload - Power", OL14},
   {"Energy brake Overload - Voltage", OL15},
   {"Energy brake Overload - Temperature", OL16},
   {"Weld Overload - Generic", OL20},

   //Cut-Off(0x0100)
   {"Ground Detect Cutoff", CU01},
   {"Energy Cutoff", CU02},
   {"Power Cutoff", CU03},
   {"Custom Input1 Cutoff", CU04},
   {"Time Cutoff (maximum time-out)", CU05},
   {"Frequency Low Cutoff", CU06},
   {"Frequency High Cutoff", CU07},
   {"Custom Input2 Cutoff", CU08},

   //SetUp(0x0200)
   {"Mode Setup Error - Generic", SE01},
   {"Setup Error - Generic", SE02},
   {"Invalid Preset", SE03},
   {"Parameter Out of Range", SE04},
   {"Verify Preset Failure", SE05},
   {"Data Entry Error", SE06},

   //CYCLEMODIFIED(0x0300)
   {"Trigger Lost During Weld or Hold", CM01},
   {"Cycle Aborted via user I/O", CM02},

   //Warnings(0x0400)
   {"Continuous Power Limit Reached", WA01},
   {"Warning 1 - Generic", WA02},
   {"Warning 2 - Generic", WA03},
   {"Amplitude Step not Reached", WA04},
   {"Sonics Disabled via user I/O", WA05},
   {"Afterburst Overload - Phase", WA11},
   {"Afterburst Overload - Current", WA12},
   {"Afterburst Overload - Frequency",WA13},
   {"Afterburst Overload - Power", WA14},
   {"Afterburst Overload - Voltage", WA15},
   {"Afterburst Overload - Temperature", WA16},

   //LimitAlarms(0x0500)
   {"Frequency High Limit", LM01},
   {"Frequency Low Limit",  LM02},
   {"Power - Minus Limit", LM03},
   {"Power - Plus Limit", LM04},
   {"Time - Minus Limit", LM05},
   {"Time - Plus Limit", LM06},
   {"Energy - Minus Limit", LM07},
   {"Energy - Plus Limit", LM08},

   //Equipment(0x0600)
   {"Start Input is active", EQ01},
   {"Trigger Active while ULS Active", EQ02},
   {"Trigger Active in Ready", EQ03},
   {"ULS Not Active in Ready", EQ04},
   {"Ground Detect Active in Ready", EQ05},
   {"Reserved EQ06", EQ06},
   {"Cable Failure - User I/O", EQ07},
   {"Field bus Removed",EQ08},
   {"Start Input lost", EQ09},
   {"Cycle Abort in Ready", EQ10},
   {"ULS time-out", EQ11},
   {"ULS Active during Weld", EQ12},

   //NOCYCLE(0x0700)
   {"ULS Time-out (start of cycle)", NC01},
   {"Trigger Time-out", NC02},
   {"External Sonics Delay Time-out", NC03},
   {"Interlock Not In Place (user I/O)", NC04},
   {"RF Switch Feedback Failure", NC05},
   {"Part Not in place", NC06},
   {"Stack Number not Valid for RF switching", NC07},

   //Communication Failure Events(0x0800)
   {"Modbus Communication Failure", CF01},
   {"LCD Communication Failure", CF02},
   {"Fieldbus Communication Failure", CF03},

   //TempProbe Alarms(0x0900)
   {"Temperature Probe Disconnected", TP01},
   {"Temperature > Maximum 1", TP02},
   {"Temperature > Maximum 2", TP03},
   {"Temperature < Minimum 1", TP04},
   {"Temperature < Minimum 2", TP05},

   //Hardware Group(0x0A00)
   {"LCD NOVRAM Failure", HW01},
   {"FRAM or NOVRAM Failure", HW02},
   {"SD Ram Failure", HW03},
   {"Connection Failure - WC to LCD", HW04},
   {"Cable Failure - WC to DCP", HW05},
   {"AC Line Voltage Lost", HW06}, //{"Multiple Overload Alarms", HW06},

   //Seek Overload(0x0B00)
   {"Seek Overload - Phase", NO01},
   {"Seek Overload - Current",  NO02},
   {"Seek Overload - Frequency", NO03},
   {"Seek Overload - Power", NO04},
   {"Seek Overload - Voltage", NO05},
   {"Seek Overload - Temperature", NO06},

   //Test Overload
   {"Test Overload - Phase", NO11},
   {"Test Overload - Current", NO12},
   {"Test Overload - Frequency", NO13},
   {"Test Overload - Power", NO14},
   {"Test Overload - Voltage", NO15},
   {"Test Overload - Temperature", NO16},
   {"Seek Overload - Generic", NO21},
   {"Test Overload - Generic", NO22},
   {0, (Alarms)0}
};

/* void PrepareAlarmData(void)
 *
 * Purpose:
 *   This function is used to Prepare Alarm data for 1st Alarm recorded in AlarmsArray.
 *   This data will remain same for all other alarms recorded at same point of time. This function
 *   is called from AlarmQueueWrite().
 *
 * Entry Condition : None.
 *
 * Exit Condition : None.
 */
void PrepareAlarmData(void)
{
   TempAlarmDataObj.Temperature = InputTemperature;
   TempAlarmDataObj.ErrorFrequency = RtReceiveData.FreqOut;
   TempAlarmDataObj.ErrorPower = RtReceiveData.PowerOut;
   TempAlarmDataObj.ErrorAmplitude = RtReceiveData.AmplitudeOut;
   TempAlarmDataObj.ErrorCurrent = RtReceiveData.Current;
   TempAlarmDataObj.CycleCounter = ParameterSet[Sysconfig->ActivePreset].InfoParam.WeldCount + 1;
   TempAlarmDataObj.Amplitude1 = RunningPreset.WcParam.Amplitude1;
   TempAlarmDataObj.Amplitude2 = RunningPreset.WcParam.Amplitude2;
   TempAlarmDataObj.CustomAIn1 = UserIO::GetCustomAIn1();
   TempAlarmDataObj.CustomAIn2 = UserIO::GetCustomAIn2();
}

/* void AlarmQueueWrite(Alarms AlarmID)
 *
 * Purpose :
 *   This function is used to record Alarms in AlarmsArray
 *   This function is called whenever an alarm need to be recorded.
 *
 * Entry Condition :
 *   AlarmID: Unique Alarm Id to be recorded.
 *
 * Exit Condition :
 *   None.
 */
void AlarmQueueWrite(Alarms AlarmID)
{
//	pprintf("\n %s %d ", __FUNCTION__, AlarmID);
   BOOLEAN Found = FALSE;
   UINT8 Indx = 0;
   UINT16 Alarmtype = 0, Alarm = 0;
   Alarmtype = (AlarmID & ALARMTYPE_MASK);
   if((CableFailure == FALSE) || (AlarmID == EQ07) || ((Alarmtype != NC) && (Alarmtype != EQ) && (Alarmtype != CM))){
		if(AlarmIndex < MAX_MULTIPLE_ALARMS){
		  for(Indx = 0; ((Found == FALSE) &&(Indx < AlarmIndex)); Indx++){
			 if(AlarmsArray[Indx] == AlarmID)
				Found = TRUE;
		  }
		  if(Found == FALSE){
			 Alarm = ALARMTYPE(AlarmID);
			 AlarmsArray[AlarmIndex++] = AlarmID;
			 FBStat.Error |= BIT((Alarmtype >> 8));
			 FBAlarms[(Alarmtype >> 8) * 4] |= BIT((AlarmID & ALARMCODE_MASK));
			 if((Alarmtype == OL) || (Alarmtype == CM)
				   || (Alarmtype == CF) || (Alarmtype == HW)
				   ||(Alarmtype == NC)||(Alarmtype == EQ))
				AlarmCycleAbort = TRUE;
			 else
				SonicsAbortFlag = TRUE;

			 if((Alarmconfig->AlarmAction[Alarm] & GENERAL_ALARM) == GENERAL_ALARM)
				UserIO::SetGenAlarm(TRUE);
			 if((Alarmconfig->AlarmAction[Alarm] & ALARM_COUNTER) == ALARM_COUNTER)
				 SysCounters->GeneralAlarmCount++;
			 if((Alarmconfig->AlarmAction[Alarm] & CYCLE_COUNTER) == CYCLE_COUNTER)
				CycleCountFlag = TRUE;
			 else
				CycleCountFlag = FALSE;
			 if((Alarmconfig->AlarmAction[Alarm] & RESET_REQUIRED) == RESET_REQUIRED){
				ResetReqFlag = TRUE;
			 }
			 if((Alarmconfig->AlarmAction[Alarm] & CUSTOM_ALARM_OUTPUT) == CUSTOM_ALARM_OUTPUT)
				UserIO::SetCustomAlarmOutput(TRUE);
			 if(AlarmIndex == 1)
				PrepareAlarmData();
		  }
	   }
   }
}

/* void CreateAlarm(void)
 *
 * Purpose :
 *   This function is used to log all recorded Alarms.
 *   This function is called from ErrorEntry function.
 *
 * Entry Condition :
 *   None
 *
 * Exit Condition :
 *   Alarms are logged
 */
void CreateAlarm(void)
{
   UINT16 Indx;
   AlarmPtr = &TempAlarmDataObj;
   for(Indx = AlarmLogIndx; Indx < AlarmIndex; Indx++){
      Log::WriteAlarmLog(Alarms(AlarmsArray[AlarmLogIndx++]));
   }
}

/* void CreateOverloadAlarm(UINT32 AlarmReason)
 *
 * Purpose :
 *   This function is used to Record Overload Alarms occurred.This function is called from CheckforError().
 *
 * Entry Condition :
 *   Alarm - State during which alarm occurred.
 *   AlarmReason - reason for alarm.On the basis of AlarmReason, a Unique Alarm Id is recorded in Alarms Array.
 *
 * Exit Condition :
 *   Alarm is recorded in AlarmsArray.
 */
void CreateOverloadAlarm(UINT32 AlarmReason)
{
   /* check for Multiple Overload Alarm */
   if((AlarmReason & MULTIPLE_OVERLOAD_OR_AC_LOST) == MULTIPLE_OVERLOAD_OR_AC_LOST){
      AlarmQueueWrite(HW06);
      FBStat.EmergencyStopActive = 1;
   }
   else{
      UserIO::SetOverloadAlarm(TRUE);
      switch(CurrentIndex){
         case ENERGY_BRAKING_IDX:
            if((AlarmReason & PHASELIMIT_TIMEERROR) == PHASELIMIT_TIMEERROR)
               AlarmQueueWrite(OL11);
            else if((AlarmReason & CURRENT_OVERLOAD) == CURRENT_OVERLOAD)
               AlarmQueueWrite(OL12);
            else if(((AlarmReason & DDSFREQ_LOWWELDLIMIT) == DDSFREQ_LOWWELDLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHWELDLIMIT) == DDSFREQ_HIGHWELDLIMIT))
               AlarmQueueWrite(OL13);
            else if((AlarmReason & POWER_OVERLOAD) == POWER_OVERLOAD)
               AlarmQueueWrite(OL14);
            else if((AlarmReason & VOLTAGE_OVERLOAD) == VOLTAGE_OVERLOAD)
               AlarmQueueWrite(OL15);
            else if((AlarmReason & TEMP_OVERLOAD) == TEMP_OVERLOAD)
               AlarmQueueWrite(OL16);
            else if(AlarmReason == 0)// It's a Generic Overload
            	AlarmQueueWrite(OL20);
         break;
         case SEEKSTATE_IDX:
            if((AlarmReason & PHASELIMIT_TIMEERROR) == PHASELIMIT_TIMEERROR)
               AlarmQueueWrite(NO01);
            else if((AlarmReason & CURRENT_OVERLOAD) == CURRENT_OVERLOAD)
               AlarmQueueWrite(NO02);
            else if(((AlarmReason & DDSFREQ_LOWSEEKLIMIT) == DDSFREQ_LOWSEEKLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHSEEKLIMIT) == DDSFREQ_HIGHSEEKLIMIT))
               AlarmQueueWrite(NO03);
            else if((AlarmReason & POWER_OVERLOAD) == POWER_OVERLOAD)
               AlarmQueueWrite(NO04);
            else if((AlarmReason & VOLTAGE_OVERLOAD) == VOLTAGE_OVERLOAD)
               AlarmQueueWrite(NO05);
            else if((AlarmReason & TEMP_OVERLOAD) == TEMP_OVERLOAD)
               AlarmQueueWrite(NO06);
            else if(AlarmReason == 0)// It's a Generic Overload
            	AlarmQueueWrite(OL20);
        break;
         case TESTSTATE_IDX:
            if((AlarmReason & PHASELIMIT_TIMEERROR) == PHASELIMIT_TIMEERROR)
               AlarmQueueWrite(NO11);
            else if((AlarmReason & CURRENT_OVERLOAD) == CURRENT_OVERLOAD)
               AlarmQueueWrite(NO12);
            else if(((AlarmReason & DDSFREQ_LOWWELDLIMIT) == DDSFREQ_LOWWELDLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHWELDLIMIT) == DDSFREQ_HIGHWELDLIMIT) ||
                  ((AlarmReason & DDSFREQ_LOWSEEKLIMIT) == DDSFREQ_LOWSEEKLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHSEEKLIMIT) == DDSFREQ_HIGHSEEKLIMIT))
               AlarmQueueWrite(NO13);
            else if((AlarmReason & POWER_OVERLOAD) == POWER_OVERLOAD)
               AlarmQueueWrite(NO14);
            else if((AlarmReason & VOLTAGE_OVERLOAD) == VOLTAGE_OVERLOAD)
               AlarmQueueWrite(NO15);
            else if((AlarmReason & TEMP_OVERLOAD) == TEMP_OVERLOAD)
               AlarmQueueWrite(NO16);
            else if(AlarmReason == 0)// It's a Generic Overload
            	AlarmQueueWrite(OL20);
        break;
         case AFTER_BURST_IDX:
            if((AlarmReason & PHASELIMIT_TIMEERROR) == PHASELIMIT_TIMEERROR)
               AlarmQueueWrite(WA11);
            else if((AlarmReason & CURRENT_OVERLOAD) == CURRENT_OVERLOAD)
               AlarmQueueWrite(WA12);
            else if(((AlarmReason & DDSFREQ_LOWWELDLIMIT) == DDSFREQ_LOWWELDLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHWELDLIMIT) == DDSFREQ_HIGHWELDLIMIT))
               AlarmQueueWrite(WA13);
            else if((AlarmReason & POWER_OVERLOAD) == POWER_OVERLOAD)
               AlarmQueueWrite(WA14);
            else if((AlarmReason & VOLTAGE_OVERLOAD) == VOLTAGE_OVERLOAD)
               AlarmQueueWrite(WA15);
            else if((AlarmReason & TEMP_OVERLOAD) == TEMP_OVERLOAD)
               AlarmQueueWrite(WA16);
            else if(AlarmReason == 0)// It's a Generic Overload
            	AlarmQueueWrite(OL20);
        break;
         case SONICS_START_IDX:  default:
            if((AlarmReason & PHASELIMIT_TIMEERROR) == PHASELIMIT_TIMEERROR)
               AlarmQueueWrite(OL01);
            else if((AlarmReason & CURRENT_OVERLOAD) == CURRENT_OVERLOAD)
               AlarmQueueWrite(OL02);
            else if(((AlarmReason & DDSFREQ_LOWWELDLIMIT) == DDSFREQ_LOWWELDLIMIT) ||
                  ((AlarmReason & DDSFREQ_HIGHWELDLIMIT) == DDSFREQ_HIGHWELDLIMIT))
               AlarmQueueWrite(OL03);
            else if((AlarmReason & POWER_OVERLOAD) == POWER_OVERLOAD)
               AlarmQueueWrite(OL04);
            else if((AlarmReason & VOLTAGE_OVERLOAD) == VOLTAGE_OVERLOAD)
               AlarmQueueWrite(OL05);
            else if((AlarmReason & TEMP_OVERLOAD) == TEMP_OVERLOAD)
               AlarmQueueWrite(OL06);
            else if(AlarmReason == 0)// It's a Generic Overload
            	AlarmQueueWrite(OL20);
        break;
      }
   }
}

/* void CreateDCPGeneralAlarm(UINT32 AlarmReason)
 *
 * Purpose :
 *   This function is used to Record all General Alarms. This function is called from CheckforError().
 *
 * Entry Condition :
 *   AlarmReason - Reason for alarm to occur.On the basis of which a Unique Alarm Id is recorded in Alarms Array.
 *
 * Exit Condition :
 *   Alarm is recorded in AlarmsArray.
 */
void CreateDCPGeneralAlarm(UINT32 AlarmReason)
{
   if((AlarmReason & ENERGY_CUTOFF) == ENERGY_CUTOFF)
      AlarmQueueWrite(CU02);
   else if((AlarmReason & PEAKPOWER_CUTOFF) == PEAKPOWER_CUTOFF)
      AlarmQueueWrite(CU03);
   else if((AlarmReason & TIME_CUTOFF) == TIME_CUTOFF)
      AlarmQueueWrite(CU05);
   else if((AlarmReason & FREQHIGH_CUTOFF) == FREQHIGH_CUTOFF)
	      AlarmQueueWrite(CU07);
   else if((AlarmReason & FREQLOW_CUTOFF) == FREQLOW_CUTOFF)
  	      AlarmQueueWrite(CU06);
}

/*  SINT8 * Log::GetAlarmStringFromID(AlarmID AlarmId)
 *
 *  Purpose :
 *    To find alarm string corresponding to an alarmID by searching the global alarms description array.
 *    This function is called from ReadAlarmLog() function of this class.
 *    The implementation of function doesn't care about the order the entries are made in global array
 *    as it traverse through the whole array
 *
 *  Entry Condition :
 *    AlarmId - The unique ID of an alarm
 *
 *  Exit Condition :
 *    The alarm description if found else returns "Undefined"
 */
SINT8 * GetAlarmStringFromID(Alarms AlarmId)
{
   static SINT8 * TmpStr = "Undefined";
   //making static will ensure that local string pointer in stack is not returned
   //to the calling function in case the description is not found assigning
   //a value ensures we don't return null pointer for further processing.
   TmpStr = "Undefined";
   int Indx = 0;
   BOOLEAN ExitFlag = FALSE;
   while((AlarmDescription[Indx].AlarmStr) && (ExitFlag == FALSE)){
      if(AlarmId == AlarmDescription[Indx].Alarmid){
         ExitFlag = TRUE;
         TmpStr = (AlarmDescription[Indx].AlarmStr);
      }
      Indx++;
   }
   return TmpStr;
}

